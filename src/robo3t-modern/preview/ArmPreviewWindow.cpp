#include "preview/ArmPreviewWindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <exception>

namespace Robo3T::Modern {

ArmPreviewWindow::ArmPreviewWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUi();
}

void ArmPreviewWindow::buildUi()
{
    auto* root = new QWidget(this);
    auto* layout = new QVBoxLayout(root);

    auto* title = new QLabel("Robo 3T ARM Preview", root);
    title->setStyleSheet("font-size: 18px; font-weight: 600;");

    auto* row = new QHBoxLayout;
    _uriEdit = new QLineEdit("mongodb://aspinator.local:27017", root);
    _uriEdit->setMinimumWidth(420);

    _legacyPingButton = new QPushButton("Ping Legacy", root);
    connect(_legacyPingButton, &QPushButton::clicked, this, &ArmPreviewWindow::pingLegacyServer);

    _modernPingButton = new QPushButton("Ping Modern", root);
    connect(_modernPingButton, &QPushButton::clicked, this, &ArmPreviewWindow::pingModernServer);

    row->addWidget(_uriEdit, 1);
    row->addWidget(_legacyPingButton);
    row->addWidget(_modernPingButton);

    _resultView = new QTextEdit(root);
    _resultView->setReadOnly(true);
    _resultView->setMinimumSize(640, 280);

    layout->addWidget(title);
    layout->addLayout(row);
    layout->addWidget(_resultView, 1);

    setCentralWidget(root);
    setWindowTitle("Robo 3T ARM Preview");
    resize(820, 420);
    setResult("Native ARM Qt app ready. Use Ping Legacy for MongoDB wire version 5 servers.");
}

void ArmPreviewWindow::pingLegacyServer()
{
    _legacyPingButton->setEnabled(false);
    setResult("Connecting with mongo-c-driver 1.19.2...");

    try {
        auto result = _legacyDriver.ping(_uriEdit->text().toStdString());
        setResult(QString::fromStdString(result));
    } catch (const std::exception& ex) {
        setResult(QString::fromStdString(ex.what()));
    }

    _legacyPingButton->setEnabled(true);
}

void ArmPreviewWindow::pingModernServer()
{
    _modernPingButton->setEnabled(false);
    setResult("Connecting with mongo-cxx-driver...");

    try {
        auto result = _driver.ping(_uriEdit->text().toStdString());
        setResult(QString::fromStdString(toJson(result.view())));
    } catch (const std::exception& ex) {
        setResult(QString::fromStdString(ex.what()));
    }

    _modernPingButton->setEnabled(true);
}

void ArmPreviewWindow::setResult(const QString& message)
{
    _resultView->setPlainText(message);
}

} // namespace Robo3T::Modern
