#include "robomongo/gui/StartupSplash.h"

#include <QEventLoop>
#include <QPainter>
#include <QPixmap>
#include <QSplashScreen>
#include <QThread>

namespace
{
    const int SplashSize = 520;

    QPixmap frameWithProgress(const QPixmap &base, double progress)
    {
        QPixmap frame(base);
        QPainter painter(&frame);
        painter.setRenderHint(QPainter::Antialiasing);

        const int margin = frame.width() / 13;
        const int height = qMax(8, frame.width() / 64);
        const int y = frame.height() - margin;
        const int width = frame.width() - (margin * 2);
        const QRect track(margin, y, width, height);
        const int fillWidth = static_cast<int>(width * progress);

        painter.setPen(QPen(QColor(143, 223, 34), 2));
        painter.setBrush(QColor(8, 18, 9, 220));
        painter.drawRoundedRect(track, height / 2, height / 2);

        if (fillWidth > 0) {
            QRect fill(track.x(), track.y(), fillWidth, track.height());
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(126, 218, 31));
            painter.drawRoundedRect(fill, height / 2, height / 2);
        }

        return frame;
    }
}

namespace Robomongo
{
    void showStartupSplash(QApplication &app)
    {
        const QPixmap source(":/robomongo/icons/dino_splash.png");
        if (source.isNull())
            return;

        const QPixmap base = source.scaled(
            SplashSize, SplashSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QSplashScreen splash(frameWithProgress(base, 0.0));
        splash.setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        splash.show();
        app.processEvents();

        const int steps = 30;
        for (int step = 1; step <= steps; ++step) {
            splash.setPixmap(frameWithProgress(base, static_cast<double>(step) / steps));
            app.processEvents(QEventLoop::AllEvents, 20);
            QThread::msleep(100);
        }

        splash.hide();
        app.processEvents();
    }
}
