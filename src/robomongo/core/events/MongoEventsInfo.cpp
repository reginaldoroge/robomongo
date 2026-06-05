#include "robomongo/core/events/MongoEventsInfo.h"

#include "robomongo/core/utils/BsonUtils.h"

namespace Robomongo
{
    IndexInfo makeIndexInfoFromBsonObj(const MongoCollectionInfo &collection, const mongo::BSONObj &obj)
    {
        using namespace Robomongo::BsonUtils;
        IndexInfo info(collection);
        info._name = obj.getStringField("name");
        mongo::BSONObj keyObj = obj.getObjectField("key");
        if (keyObj.isValid())
            info._keys = jsonString(keyObj, mongo::TenGen, 1, Robomongo::DefaultEncoding, Robomongo::Utc);

        info._unique = obj.getBoolField("unique");
        info._backGround = obj.getBoolField("background");
        info._sparse = obj.getBoolField("sparse");
        info._ttl = obj.getIntField("expireAfterSeconds");
        info._defaultLanguage = obj.getStringField("default_language");
        info._languageOverride = obj.getStringField("language_override");
        mongo::BSONObj weightsObj = obj.getObjectField("weights");
        if (weightsObj.isValid())
            info._textWeights = jsonString(weightsObj, mongo::TenGen, 1, Robomongo::DefaultEncoding,
                                           Robomongo::Utc);

        return info;
    }

    IndexInfo::IndexInfo(
        const MongoCollectionInfo &collection, const std::string &name, const std::string &keys,
        bool isUnique, bool isBackGround, bool isSparce, int expireAfter,
        const std::string &defaultLanguage, const std::string &languageOverride, const std::string &textWeights) :
        _name(name),
        _collection(collection),
        _keys(keys),
        _unique(isUnique),
        _backGround(isBackGround),
        _sparse(isSparce),
        _ttl(expireAfter),
        _defaultLanguage(defaultLanguage),
        _languageOverride(languageOverride),
        _textWeights(textWeights) {}

        ConnectionInfo::ConnectionInfo(std::string const& uuid) :
            _address(),
            _databases(),
            _version(0.0f),
            _dbVersionStr(),
            _uuid(uuid),
            _driverName("Legacy driver"),
            _driverVersion("4.2"),
            _maxWireVersion(0)
        {}

        ConnectionInfo::ConnectionInfo(const std::string &address, const std::vector<std::string> &databases, 
            float version, const std::string& dbVersionStr, const std::string& storageEngine,
            std::string const& uuid, const std::string &driverName, const std::string &driverVersion,
            int maxWireVersion) :
           _address(address),
           _databases(databases),
           _version(version),
            _dbVersionStr(dbVersionStr),
           _storageEngineType(storageEngine),
           _uuid(uuid),
           _driverName(driverName),
           _driverVersion(driverVersion),
           _maxWireVersion(maxWireVersion)
        {}
}
