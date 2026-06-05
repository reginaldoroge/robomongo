#include "preview/ArmPreviewWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    Robo3T::Modern::ArmPreviewWindow window;
    window.show();

    return app.exec();
}
