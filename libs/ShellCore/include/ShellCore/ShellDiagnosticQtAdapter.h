#pragma once

#include "ShellCore/ShellDiagnosticIssueListModel.h"
#include "ShellCore/ShellDiagnosticPresenter.h"
#include "ShellCore/ShellDiagnosticServiceListModel.h"
#include "ShellCore/ShellDiagnosticSummaryObject.h"

#include <QObject>
#include <QString>

namespace etfdt::shell {

class ShellDiagnosticQtAdapter final : public QObject {
    Q_OBJECT

public:
    explicit ShellDiagnosticQtAdapter(QObject* parent = nullptr);

    [[nodiscard]] bool loadMockHealthy();
    [[nodiscard]] bool loadMockWithWarnings();
    [[nodiscard]] bool loadMockWithErrors();
    [[nodiscard]] bool loadMockMixed();
    [[nodiscard]] bool loadFromFile(const QString& path);

    void setFilter(const ShellDiagnosticFilter& filter);
    void setSort(const ShellDiagnosticSort& sort);
    void clearFilter();
    [[nodiscard]] bool selectService(int index);
    [[nodiscard]] ShellDiagnosticRefreshResult refreshIfChanged();

    [[nodiscard]] ShellDiagnosticServiceListModel* serviceModel();
    [[nodiscard]] ShellDiagnosticIssueListModel* issueModel();
    [[nodiscard]] ShellDiagnosticSummaryObject* summaryObject();
    [[nodiscard]] QString lastError() const;

signals:
    void stateChanged();

private:
    void syncModels();

    ShellDiagnosticPresenter presenter_;
    ShellDiagnosticServiceListModel serviceModel_;
    ShellDiagnosticIssueListModel issueModel_;
    ShellDiagnosticSummaryObject summaryObject_;
    QString lastError_;
    int selectedServiceIndex_ = -1;
};

}  // namespace etfdt::shell
