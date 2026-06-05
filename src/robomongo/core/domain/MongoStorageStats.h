#pragma once

#include <string>
#include <vector>

namespace Robomongo
{
    struct MongoCollectionStorageStats
    {
        MongoCollectionStorageStats() {}
        MongoCollectionStorageStats(const std::string &name, double storageSizeBytes);

        std::string name;
        double storageSizeBytes = 0;
        std::vector<std::pair<std::string, double>> indexSizes;
    };

    struct MongoDatabaseStorageStats
    {
        explicit MongoDatabaseStorageStats(const std::string &name = std::string());

        std::string name;
        double storageSizeBytes = 0;
        std::vector<MongoCollectionStorageStats> collections;
    };
}
