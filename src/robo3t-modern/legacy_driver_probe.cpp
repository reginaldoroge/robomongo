#include "legacy/LegacyMongoDriver.h"

#include <QCoreApplication>
#include <QDebug>

#include <exception>
#include <string>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    try {
        Robo3T::Modern::LegacyMongoDriver driver;
        std::string const uri = argc > 1 ? argv[1] : "";
        qInfo().noquote() << QString::fromStdString(driver.ping(uri));
        return 0;
    } catch (const std::exception& ex) {
        qCritical().noquote() << QString::fromStdString(ex.what());
        return 1;
    }
}
