#include "robomongo/core/domain/QueryResultFormatter.h"

#include <QStringList>
#include <mongo/bson/bsonobj.h>

#include "robomongo/core/domain/MongoDocument.h"
#include "robomongo/core/utils/BsonUtils.h"
#include "robomongo/core/utils/QtUtils.h"

namespace
{
    QString bsonToJson(const mongo::BSONObj &obj, int pretty,
                       Robomongo::UUIDEncoding uuidEncoding,
                       Robomongo::SupportedTimes timeZone)
    {
        QString json = Robomongo::QtUtils::toQString(
            Robomongo::BsonUtils::jsonString(obj, mongo::TenGen, pretty, uuidEncoding, timeZone));

        if (!pretty)
            json.replace('\n', ' ');

        return json;
    }

    QString bsonElementToText(const mongo::BSONElement &element,
                              Robomongo::UUIDEncoding uuidEncoding,
                              Robomongo::SupportedTimes timeZone)
    {
        if (element.eoo() || element.isNull())
            return QString();

        if (element.type() == mongo::String || element.type() == mongo::Symbol)
            return Robomongo::QtUtils::toQString(element.String());

        return Robomongo::QtUtils::toQString(
            Robomongo::BsonUtils::jsonString(element, mongo::TenGen, false, 0,
                                             uuidEncoding, timeZone));
    }

    QString csvEscape(const QString &value)
    {
        QString escaped = value;
        escaped.replace("\"", "\"\"");

        if (escaped.contains(',') || escaped.contains('"') ||
            escaped.contains('\n') || escaped.contains('\r'))
            return "\"" + escaped + "\"";

        return escaped;
    }

    QStringList csvHeaders(const std::vector<Robomongo::MongoDocumentPtr> &documents)
    {
        QStringList headers;
        for (auto const &document : documents) {
            mongo::BSONObjIterator iterator(document->bsonObj());
            while (iterator.more()) {
                const QString fieldName = QString::fromUtf8(iterator.next().fieldName());
                if (!headers.contains(fieldName))
                    headers << fieldName;
            }
        }
        return headers;
    }

    QString formatJson(const std::vector<Robomongo::MongoDocumentPtr> &documents,
                       Robomongo::UUIDEncoding uuidEncoding,
                       Robomongo::SupportedTimes timeZone)
    {
        QString output("[\n");
        for (std::size_t i = 0; i < documents.size(); ++i) {
            output += bsonToJson(documents[i]->bsonObj(), 1, uuidEncoding, timeZone);
            output += (i + 1 == documents.size()) ? "\n" : ",\n";
        }
        output += "]\n";
        return output;
    }

    QString formatJsonLines(const std::vector<Robomongo::MongoDocumentPtr> &documents,
                            Robomongo::UUIDEncoding uuidEncoding,
                            Robomongo::SupportedTimes timeZone)
    {
        QString output;
        for (auto const &document : documents) {
            output += bsonToJson(document->bsonObj(), 0, uuidEncoding, timeZone);
            output += "\n";
        }
        return output;
    }

    QString formatCsv(const std::vector<Robomongo::MongoDocumentPtr> &documents,
                      Robomongo::UUIDEncoding uuidEncoding,
                      Robomongo::SupportedTimes timeZone)
    {
        const QStringList headers = csvHeaders(documents);
        QString output = headers.join(',') + "\n";

        for (auto const &document : documents) {
            QStringList row;
            for (const QString &header : headers) {
                const mongo::BSONElement element =
                    document->bsonObj().getField(Robomongo::QtUtils::toStdString(header));
                row << csvEscape(bsonElementToText(element, uuidEncoding, timeZone));
            }
            output += row.join(',') + "\n";
        }

        return output;
    }
}

namespace Robomongo
{
    QString QueryResultFormatter::format(const std::vector<MongoDocumentPtr> &documents,
                                         QueryResultExportFormat format,
                                         UUIDEncoding uuidEncoding,
                                         SupportedTimes timeZone)
    {
        switch (format) {
        case QueryResultExportFormat::Json:
            return formatJson(documents, uuidEncoding, timeZone);
        case QueryResultExportFormat::JsonLines:
            return formatJsonLines(documents, uuidEncoding, timeZone);
        case QueryResultExportFormat::Csv:
            return formatCsv(documents, uuidEncoding, timeZone);
        }

        return QString();
    }
}
