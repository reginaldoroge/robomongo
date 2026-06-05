#include "robomongo/gui/dialogs/ResultExportDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVariant>
#include <QVBoxLayout>

#include <limits>

namespace
{
    QString extensionFor(Robomongo::QueryResultExportFormat format)
    {
        switch (format) {
        case Robomongo::QueryResultExportFormat::Json:
            return "json";
        case Robomongo::QueryResultExportFormat::JsonLines:
            return "jsonl";
        case Robomongo::QueryResultExportFormat::Csv:
            return "csv";
        }

        return "json";
    }

    QString filterFor(Robomongo::QueryResultExportFormat format)
    {
        switch (format) {
        case Robomongo::QueryResultExportFormat::Json:
            return "JSON (*.json)";
        case Robomongo::QueryResultExportFormat::JsonLines:
            return "JSON Lines (*.jsonl)";
        case Robomongo::QueryResultExportFormat::Csv:
            return "CSV (*.csv)";
        }

        return "JSON (*.json)";
    }
}

namespace Robomongo
{
    ResultExportDialog::ResultExportDialog(bool canExportAll, int visibleCount, int defaultLimit,
                                           QWidget *parent) :
        QDialog(parent),
        _formatCombo(new QComboBox(this)),
        _visibleScope(new QRadioButton(this)),
        _limitScope(new QRadioButton(this)),
        _limitSpinBox(new QSpinBox(this)),
        _allScope(new QRadioButton(this)),
        _pathEdit(new QLineEdit(this)),
        _buttonBox(new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Save, this))
    {
        setWindowTitle("Export Results");
        setMinimumWidth(460);

        _formatCombo->addItem("JSON", QVariant::fromValue(static_cast<int>(QueryResultExportFormat::Json)));
        _formatCombo->addItem("JSON Lines", QVariant::fromValue(static_cast<int>(QueryResultExportFormat::JsonLines)));
        _formatCombo->addItem("CSV", QVariant::fromValue(static_cast<int>(QueryResultExportFormat::Csv)));

        _visibleScope->setText(QString("Visible results (%1)").arg(visibleCount));
        _visibleScope->setChecked(true);

        if (defaultLimit <= 0)
            defaultLimit = visibleCount > 0 ? visibleCount : 50;

        _limitScope->setText("Limit query to");
        _limitScope->setEnabled(canExportAll);
        _limitScope->setToolTip(canExportAll
            ? "Run the same find query again using this export limit."
            : "Available for find query results.");

        _limitSpinBox->setRange(1, std::numeric_limits<int>::max());
        _limitSpinBox->setValue(defaultLimit);

        _allScope->setText("All results from this query");
        _allScope->setEnabled(canExportAll);
        _allScope->setToolTip(canExportAll
            ? "Run the same find query again without the page limit."
            : "Available for find query results.");

        auto browseButton = new QPushButton("Browse...", this);
        auto pathLayout = new QHBoxLayout();
        pathLayout->setContentsMargins(0, 0, 0, 0);
        pathLayout->addWidget(_pathEdit);
        pathLayout->addWidget(browseButton);

        auto form = new QFormLayout();
        form->addRow("Format:", _formatCombo);
        form->addRow("File:", pathLayout);

        auto scopeLayout = new QVBoxLayout();
        scopeLayout->setContentsMargins(0, 0, 0, 0);
        scopeLayout->addWidget(_visibleScope);

        auto limitLayout = new QHBoxLayout();
        limitLayout->setContentsMargins(0, 0, 0, 0);
        limitLayout->addWidget(_limitScope);
        limitLayout->addWidget(_limitSpinBox);
        limitLayout->addWidget(new QLabel("documents", this));
        limitLayout->addStretch();
        scopeLayout->addLayout(limitLayout);

        scopeLayout->addWidget(_allScope);

        auto mainLayout = new QVBoxLayout();
        mainLayout->addLayout(form);
        mainLayout->addSpacing(6);
        mainLayout->addWidget(new QLabel("Scope:", this));
        mainLayout->addLayout(scopeLayout);
        mainLayout->addWidget(_buttonBox);
        setLayout(mainLayout);

        _buttonBox->button(QDialogButtonBox::Save)->setText("Export");

        connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
        connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
        connect(browseButton, SIGNAL(clicked()), this, SLOT(browse()));
        connect(_formatCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(updateExtensionHint(int)));
        connect(_limitScope, SIGNAL(toggled(bool)), this, SLOT(updateLimitControls()));

        updateExtensionHint(_formatCombo->currentIndex());
        updateLimitControls();
    }

    ResultExportSelection ResultExportDialog::selection() const
    {
        ResultExportSelection result;
        result.format = selectedFormat();
        if (_allScope->isChecked())
            result.scope = ResultExportScope::AllQueryResults;
        else if (_limitScope->isChecked()) {
            result.scope = ResultExportScope::LimitedQueryResults;
            result.limit = _limitSpinBox->value();
        }
        else
            result.scope = ResultExportScope::VisibleResults;

        result.filePath = _pathEdit->text();
        return result;
    }

    void ResultExportDialog::accept()
    {
        QString path = _pathEdit->text().trimmed();
        if (path.isEmpty()) {
            QMessageBox::warning(this, "Export Results", "Choose a file before exporting.");
            return;
        }

        QFileInfo fileInfo(path);
        if (fileInfo.suffix().isEmpty())
            path += "." + defaultExtension();

        _pathEdit->setText(path);
        QDialog::accept();
    }

    void ResultExportDialog::browse()
    {
        QString selected = selectedFilter();
        QString path = QFileDialog::getSaveFileName(
            this, "Export Results", _pathEdit->text(), 
            "JSON (*.json);;JSON Lines (*.jsonl);;CSV (*.csv);;All Files (*.*)", &selected);

        if (!path.isEmpty()) {
            if (QFileInfo(path).suffix().isEmpty())
                path += "." + defaultExtension();
            _pathEdit->setText(path);
        }
    }

    void ResultExportDialog::updateExtensionHint(int)
    {
        _pathEdit->setPlaceholderText(QString("Choose a .%1 file").arg(defaultExtension()));
    }

    void ResultExportDialog::updateLimitControls()
    {
        _limitSpinBox->setEnabled(_limitScope->isEnabled() && _limitScope->isChecked());
    }

    QString ResultExportDialog::defaultExtension() const
    {
        return extensionFor(selectedFormat());
    }

    QString ResultExportDialog::selectedFilter() const
    {
        return filterFor(selectedFormat());
    }

    QueryResultExportFormat ResultExportDialog::selectedFormat() const
    {
        return static_cast<QueryResultExportFormat>(
            _formatCombo->currentData().toInt());
    }
}
