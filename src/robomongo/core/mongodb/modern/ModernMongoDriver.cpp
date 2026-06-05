#include "robomongo/core/mongodb/modern/ModernMongoDriver.h"

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/options/index.hpp>
#include <mongocxx/options/replace.hpp>
#include <mongocxx/uri.hpp>

#include <mongo/bson/bsonobjbuilder.h>

#include "robomongo/core/mongodb/modern/ModernConnectionUri.h"
#include "robomongo/core/mongodb/modern/ModernDriverRuntime.h"
#include "robomongo/shell/bson/json.h"

namespace
{
    using bsoncxx::builder::basic::kvp;

    double numberField(const bsoncxx::document::view &doc, const char *fieldName)
    {
        auto elem = doc[fieldName];
        if (!elem)
            return 0;

        switch (elem.type()) {
        case bsoncxx::type::k_double:
            return elem.get_double().value;
        case bsoncxx::type::k_int32:
            return elem.get_int32().value;
        case bsoncxx::type::k_int64:
            return static_cast<double>(elem.get_int64().value);
        default:
            return 0;
        }
    }

    std::vector<std::pair<std::string, double>> indexSizesFrom(const bsoncxx::document::view &stats)
    {
        std::vector<std::pair<std::string, double>> result;
        auto elem = stats["indexSizes"];

        if (!elem || elem.type() != bsoncxx::type::k_document)
            return result;

        for (const auto &indexSize : elem.get_document().view()) {
            switch (indexSize.type()) {
            case bsoncxx::type::k_double:
                result.push_back(std::make_pair(
                    std::string(indexSize.key()), indexSize.get_double().value));
                break;
            case bsoncxx::type::k_int32:
                result.push_back(std::make_pair(
                    std::string(indexSize.key()), indexSize.get_int32().value));
                break;
            case bsoncxx::type::k_int64:
                result.push_back(std::make_pair(
                    std::string(indexSize.key()), static_cast<double>(indexSize.get_int64().value)));
                break;
            default:
                break;
            }
        }

        return result;
    }

    bsoncxx::document::value toModernBson(const mongo::BSONObj &obj)
    {
        return bsoncxx::from_json(obj.jsonString(mongo::Strict, false));
    }

    mongo::BSONObj toLegacyBson(const bsoncxx::document::view &doc)
    {
        return mongo::Robomongo::fromjson(bsoncxx::to_json(doc));
    }

    bool hasFields(const mongo::BSONObj &obj)
    {
        return !obj.isEmpty();
    }

    mongo::BSONObj queryObject(const mongo::Query &query)
    {
        return query.obj;
    }

    mongocxx::options::index makeIndexOptions(const Robomongo::IndexInfo &indexInfo)
    {
        mongocxx::options::index options;
        options.name(indexInfo._name);

        if (indexInfo._unique)
            options.unique(true);

        if (indexInfo._sparse)
            options.sparse(true);

        if (indexInfo._ttl > 0)
            options.expire_after(std::chrono::seconds(indexInfo._ttl));

        if (!indexInfo._defaultLanguage.empty())
            options.default_language(indexInfo._defaultLanguage);

        if (!indexInfo._languageOverride.empty())
            options.language_override(indexInfo._languageOverride);

        if (!mongo::Robomongo::fromjson(indexInfo._textWeights).isEmpty())
            options.weights(toModernBson(mongo::Robomongo::fromjson(indexInfo._textWeights)));

        return options;
    }

    void ensureCollectionDoesNotExist(mongocxx::database db, const std::string &collectionName)
    {
        for (const auto &name : db.list_collection_names()) {
            if (name == collectionName)
                throw std::runtime_error("Collection with same name already exists.");
        }
    }
}

namespace Robomongo
{
    ModernMongoDriver::ModernMongoDriver(const ConnectionSettings &settings) :
        _uri(modernDirectUri(settings))
    {
        modernDriverRuntime();
    }

    std::vector<std::string> ModernMongoDriver::listDatabaseNames() const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        return client.list_database_names();
    }

    std::vector<MongoCollectionInfo> ModernMongoDriver::listCollections(
        const std::string &databaseName) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        std::vector<MongoCollectionInfo> collections;

        for (const auto &name : client[databaseName].list_collection_names())
            collections.emplace_back(databaseName + "." + name);

        return collections;
    }

    MongoDatabaseStorageStats ModernMongoDriver::loadStorageStats(const std::string &databaseName) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        auto db = client[databaseName];
        MongoDatabaseStorageStats stats(databaseName);

        bsoncxx::builder::basic::document dbStatsCommand;
        dbStatsCommand.append(kvp("dbStats", 1), kvp("scale", 1));
        auto dbStats = db.run_command(dbStatsCommand.view());
        stats.storageSizeBytes = numberField(dbStats.view(), "storageSize");

        for (const auto &name : db.list_collection_names()) {
            try {
                bsoncxx::builder::basic::document collStatsCommand;
                collStatsCommand.append(kvp("collStats", name), kvp("scale", 1));
                auto collStats = db.run_command(collStatsCommand.view());
                MongoCollectionStorageStats collectionStats(
                    name, numberField(collStats.view(), "storageSize"));
                collectionStats.indexSizes = indexSizesFrom(collStats.view());
                stats.collections.push_back(collectionStats);
            } catch (...) {
                continue;
            }
        }

        return stats;
    }

    std::vector<MongoDocumentPtr> ModernMongoDriver::find(const MongoQueryInfo &queryInfo) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        mongocxx::options::find options;

        if (queryInfo._limit > 0)
            options.limit(queryInfo._limit);

        if (queryInfo._skip > 0)
            options.skip(queryInfo._skip);

        if (queryInfo._batchSize > 0)
            options.batch_size(queryInfo._batchSize);

        auto filter = toModernBson(queryInfo._query);

        if (hasFields(queryInfo._fields)) {
            auto projection = toModernBson(queryInfo._fields);
            options.projection(projection.view());
        }

        std::vector<MongoDocumentPtr> documents;
        auto collection = client[queryInfo._info._ns.databaseName()][queryInfo._info._ns.collectionName()];

        for (const auto &doc : collection.find(filter.view(), options))
            documents.push_back(MongoDocument::fromBsonObj(toLegacyBson(doc)));

        return documents;
    }

    std::vector<IndexInfo> ModernMongoDriver::listIndexes(const MongoCollectionInfo &collectionInfo) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        std::vector<IndexInfo> indexes;
        auto collection = client[collectionInfo.ns().databaseName()][collectionInfo.ns().collectionName()];

        for (const auto &index : collection.list_indexes())
            indexes.push_back(makeIndexInfoFromBsonObj(collectionInfo, toLegacyBson(index)));

        return indexes;
    }

    void ModernMongoDriver::createDatabase(const std::string &databaseName) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        client[databaseName]["__dino_robomongo_init"].insert_one(
            bsoncxx::from_json("{\"createdBy\":\"dino-robomongo\"}").view());
        client[databaseName]["__dino_robomongo_init"].drop();
    }

    void ModernMongoDriver::dropDatabase(const std::string &databaseName) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        client[databaseName].drop();
    }

    void ModernMongoDriver::createCollection(const MongoNamespace &ns) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        client[ns.databaseName()].create_collection(ns.collectionName());
    }

    void ModernMongoDriver::dropCollection(const MongoNamespace &ns) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        client[ns.databaseName()][ns.collectionName()].drop();
    }

    void ModernMongoDriver::renameCollection(const MongoNamespace &ns, const std::string &newCollectionName) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        client[ns.databaseName()][ns.collectionName()].rename(newCollectionName);
    }

    void ModernMongoDriver::duplicateCollection(const MongoNamespace &ns, const std::string &newCollectionName) const
    {
        copyCollectionTo(ns, MongoNamespace(ns.databaseName(), newCollectionName));
    }

    void ModernMongoDriver::copyCollectionTo(const MongoNamespace &from, const MongoNamespace &to) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        auto source = client[from.databaseName()][from.collectionName()];
        auto targetDb = client[to.databaseName()];
        ensureCollectionDoesNotExist(targetDb, to.collectionName());
        targetDb.create_collection(to.collectionName());
        auto target = targetDb[to.collectionName()];

        std::vector<bsoncxx::document::value> batch;
        batch.reserve(500);

        auto flush = [&]() {
            if (batch.empty())
                return;

            target.insert_many(batch);
            batch.clear();
        };

        for (const auto &doc : source.find({})) {
            batch.emplace_back(bsoncxx::document::value(doc));
            if (batch.size() >= 500)
                flush();
        }

        flush();
    }

    void ModernMongoDriver::insertDocument(const MongoNamespace &ns, const mongo::BSONObj &document) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        auto modernDoc = toModernBson(document);
        client[ns.databaseName()][ns.collectionName()].insert_one(modernDoc.view());
    }

    void ModernMongoDriver::saveDocument(const MongoNamespace &ns, const mongo::BSONObj &document) const
    {
        auto id = document.getField("_id");
        if (id.eoo())
            throw std::runtime_error("Cannot save document without _id.");

        mongo::BSONObjBuilder builder;
        builder.append(id);
        auto filter = toModernBson(builder.obj());
        auto replacement = toModernBson(document);
        mongocxx::options::replace options;
        options.upsert(true);

        mongocxx::client client { mongocxx::uri(_uri) };
        client[ns.databaseName()][ns.collectionName()].replace_one(
            filter.view(), replacement.view(), options);
    }

    void ModernMongoDriver::removeDocuments(const MongoNamespace &ns, const mongo::Query &query, bool justOne) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        auto filter = toModernBson(queryObject(query));
        auto collection = client[ns.databaseName()][ns.collectionName()];

        if (justOne)
            collection.delete_one(filter.view());
        else
            collection.delete_many(filter.view());
    }

    void ModernMongoDriver::addEditIndex(const IndexInfo &oldInfo, const IndexInfo &newInfo) const
    {
        bool const editIndex = !oldInfo._name.empty();
        mongocxx::client client { mongocxx::uri(_uri) };
        auto collection = client[newInfo._collection.ns().databaseName()]
                                [newInfo._collection.ns().collectionName()];

        if (editIndex)
            collection.indexes().drop_one(oldInfo._name);

        try {
            auto keys = toModernBson(mongo::Robomongo::fromjson(newInfo._keys));
            collection.create_index(keys.view(), makeIndexOptions(newInfo));
        } catch (...) {
            if (editIndex) {
                auto oldKeys = toModernBson(mongo::Robomongo::fromjson(oldInfo._keys));
                collection.create_index(oldKeys.view(), makeIndexOptions(oldInfo));
            }
            throw;
        }
    }

    void ModernMongoDriver::dropIndex(const MongoCollectionInfo &collectionInfo, const std::string &indexName) const
    {
        mongocxx::client client { mongocxx::uri(_uri) };
        client[collectionInfo.ns().databaseName()][collectionInfo.ns().collectionName()]
            .indexes().drop_one(indexName);
    }
}
