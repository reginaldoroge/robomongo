#include <gtest/gtest.h>

#include "robomongo/core/domain/QueryResultExportPlan.h"

using namespace Robomongo;

TEST(QueryResultExportPlanTests, appliesRequestedLimitWhenQueryHasNoLimit)
{
    EXPECT_EQ(50, QueryResultExportPlan::effectiveLimit(0, 50));
}

TEST(QueryResultExportPlanTests, doesNotExceedOriginalQueryLimit)
{
    EXPECT_EQ(20, QueryResultExportPlan::effectiveLimit(20, 50));
}

TEST(QueryResultExportPlanTests, keepsOriginalLimitWhenNoExportLimitIsRequested)
{
    EXPECT_EQ(0, QueryResultExportPlan::effectiveLimit(0, 0));
    EXPECT_EQ(20, QueryResultExportPlan::effectiveLimit(20, 0));
}
