#pragma once

namespace Robomongo
{
    class QueryResultExportPlan
    {
    public:
        static int effectiveLimit(int originalLimit, int requestedLimit);
    };
}
