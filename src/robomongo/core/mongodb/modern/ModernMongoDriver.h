#pragma once

#include <string>
#include <vector>

#include <mongo/client/dbclient_base.h>

#include "robomongo/core/domain/MongoCollectionInfo.h"
#include "robomongo/core/domain/MongoDocument.h"
#include "robomongo/core/domain/MongoQueryInfo.h"
#include "robomongo/core/domain/MongoStorageStats.h"
#include "robomongo/core/events/MongoEventsInfo.h"

namespace Robomongo
{
    class ConnectionSettings;

    class ModernMongoDriver
    {
    public:
        explicit ModernMongoDriver(const ConnectionSettings &settings);

        std::vector<std::string> listDatabaseNames() const;
        std::vector<MongoCollectionInfo> listCollections(const std::string &databaseName) const;
        MongoDatabaseStorageStats loadStorageStats(const std::string &databaseName) const;
        std::vector<IndexInfo> listIndexes(const MongoCollectionInfo &collection) const;
        std::vector<MongoDocumentPtr> find(const MongoQueryInfo &queryInfo) const;
        void createDatabase(const std::string &databaseName) const;
        void dropDatabase(const std::string &databaseName) const;
        void createCollection(const MongoNamespace &ns) const;
        void dropCollection(const MongoNamespace &ns) const;
        void renameCollection(const MongoNamespace &ns, const std::string &newCollectionName) const;
        void duplicateCollection(const MongoNamespace &ns, const std::string &newCollectionName) const;
        void copyCollectionTo(const MongoNamespace &from, const MongoNamespace &to) const;
        void insertDocument(const MongoNamespace &ns, const mongo::BSONObj &document) const;
        void saveDocument(const MongoNamespace &ns, const mongo::BSONObj &document) const;
        void removeDocuments(const MongoNamespace &ns, const mongo::Query &query, bool justOne) const;
        void addEditIndex(const IndexInfo &oldInfo, const IndexInfo &newInfo) const;
        void dropIndex(const MongoCollectionInfo &collection, const std::string &indexName) const;

    private:
        std::string _uri;
    };
}
