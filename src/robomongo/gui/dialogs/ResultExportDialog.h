#pragma once

#include <QDialog>
#include <QString>

#include "robomongo/core/domain/QueryResultFormatter.h"

class QComboBox;
class QDialogButtonBox;
class QLineEdit;
class QRadioButton;
class QSpinBox;

namespace Robomongo
{
    enum class ResultExportScope
    {
        VisibleResults,
        LimitedQueryResults,
        AllQueryResults
    };

    struct ResultExportSelection
    {
        QueryResultExportFormat format = QueryResultExportFormat::Json;
        ResultExportScope scope = ResultExportScope::VisibleResults;
        int limit = 0;
        QString filePath;
    };

    class ResultExportDialog : public QDialog
    {
        Q_OBJECT

    public:
        ResultExportDialog(bool canExportAll, int visibleCount, int defaultLimit,
                           QWidget *parent = nullptr);

        ResultExportSelection selection() const;

    public Q_SLOTS:
        void accept() override;

    private Q_SLOTS:
        void browse();
        void updateExtensionHint(int index);
        void updateLimitControls();

    private:
        QString defaultExtension() const;
        QString selectedFilter() const;
        QueryResultExportFormat selectedFormat() const;

        QComboBox *_formatCombo;
        QRadioButton *_visibleScope;
        QRadioButton *_limitScope;
        QSpinBox *_limitSpinBox;
        QRadioButton *_allScope;
        QLineEdit *_pathEdit;
        QDialogButtonBox *_buttonBox;
    };
}
