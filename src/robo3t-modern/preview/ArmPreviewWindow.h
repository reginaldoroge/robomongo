#pragma once

#include "legacy/LegacyMongoDriver.h"
#include "mongo/ModernMongoDriver.h"

#include <QMainWindow>

class QPushButton;
class QLineEdit;
class QTextEdit;

namespace Robo3T::Modern {

class ArmPreviewWindow final : public QMainWindow {
public:
    explicit ArmPreviewWindow(QWidget* parent = nullptr);

private:
    void buildUi();
    void pingLegacyServer();
    void pingModernServer();
    void setResult(const QString& message);

    ModernMongoDriver _driver;
    LegacyMongoDriver _legacyDriver;
    QLineEdit* _uriEdit = nullptr;
    QPushButton* _legacyPingButton = nullptr;
    QPushButton* _modernPingButton = nullptr;
    QTextEdit* _resultView = nullptr;
};

} // namespace Robo3T::Modern
