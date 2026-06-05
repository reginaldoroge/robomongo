#include "robomongo/core/domain/QueryResultExportPlan.h"

#include <algorithm>

namespace Robomongo
{
    int QueryResultExportPlan::effectiveLimit(int originalLimit, int requestedLimit)
    {
        if (requestedLimit <= 0)
            return originalLimit;

        if (originalLimit > 0)
            return std::min(originalLimit, requestedLimit);

        return requestedLimit;
    }
}
