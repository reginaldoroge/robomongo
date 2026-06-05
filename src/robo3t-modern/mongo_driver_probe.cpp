#include "mongo/ModernMongoDriver.h"

#include <QCoreApplication>
#include <QDebug>

#include <exception>
#include <string>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    try {
        Robo3T::Modern::ModernMongoDriver driver;
        std::string const uri = argc > 1 ? argv[1] : "";
        bool const shouldPing = argc > 2 && std::string(argv[2]) == "--ping";

        auto result = shouldPing ? driver.ping(uri) : driver.makeProbeDocument(uri);
        qInfo().noquote() << QString::fromStdString(Robo3T::Modern::toJson(result.view()));
        return 0;
    } catch (const std::exception& ex) {
        qCritical().noquote() << QString::fromStdString(ex.what());
        return 1;
    }
}
