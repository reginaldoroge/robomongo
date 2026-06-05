#pragma once

#include <QString>
#include <vector>

#include "robomongo/core/Core.h"
#include "robomongo/core/Enums.h"

namespace Robomongo
{
    enum class QueryResultExportFormat
    {
        Json,
        JsonLines,
        Csv
    };

    class QueryResultFormatter
    {
    public:
        static QString format(const std::vector<MongoDocumentPtr> &documents,
                              QueryResultExportFormat format,
                              UUIDEncoding uuidEncoding,
                              SupportedTimes timeZone);
    };
}
