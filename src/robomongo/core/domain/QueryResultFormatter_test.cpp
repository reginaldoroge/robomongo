#include "gtest/gtest.h"

#include "robomongo/core/domain/MongoDocument.h"
#include "robomongo/core/domain/QueryResultFormatter.h"

#include <mongo/bson/bsonobjbuilder.h>

namespace
{
    std::vector<Robomongo::MongoDocumentPtr> makeDocuments()
    {
        std::vector<Robomongo::MongoDocumentPtr> docs;
        docs.push_back(Robomongo::MongoDocument::fromBsonObj(BSON("name" << "Ana" << "age" << 30)));
        docs.push_back(Robomongo::MongoDocument::fromBsonObj(BSON("name" << "Bob, Jr." << "active" << true)));
        return docs;
    }
}

TEST(QueryResultFormatterTests, formatsDocumentsAsJsonLines)
{
    const QString output = Robomongo::QueryResultFormatter::format(
        makeDocuments(), Robomongo::QueryResultExportFormat::JsonLines,
        Robomongo::DefaultEncoding, Robomongo::Utc);

    EXPECT_EQ(QString("{ \"name\" : \"Ana\", \"age\" : 30 }\n"
                      "{ \"name\" : \"Bob, Jr.\", \"active\" : true }\n"), output);
}

TEST(QueryResultFormatterTests, formatsDocumentsAsCsvWithEscapedCells)
{
    const QString output = Robomongo::QueryResultFormatter::format(
        makeDocuments(), Robomongo::QueryResultExportFormat::Csv,
        Robomongo::DefaultEncoding, Robomongo::Utc);

    EXPECT_EQ(QString("name,age,active\n"
                      "Ana,30,\n"
                      "\"Bob, Jr.\",,true\n"), output);
}
