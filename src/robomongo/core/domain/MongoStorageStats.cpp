#include "robomongo/core/domain/MongoStorageStats.h"

namespace Robomongo
{
    MongoCollectionStorageStats::MongoCollectionStorageStats(
        const std::string &name, double storageSizeBytes) :
        name(name),
        storageSizeBytes(storageSizeBytes)
    {
    }

    MongoDatabaseStorageStats::MongoDatabaseStorageStats(const std::string &name) :
        name(name)
    {
    }
}
