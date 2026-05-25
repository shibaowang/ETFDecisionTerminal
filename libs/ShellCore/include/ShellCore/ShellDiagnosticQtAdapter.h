#pragma once

#include "ShellCore/ShellDiagnosticIssueListModel.h"
#include "ShellCore/ShellDiagnosticPresenter.h"
#include "ShellCore/ShellDiagnosticServiceListModel.h"
#include "ShellCore/ShellDiagnosticSummaryObject.h"

#include <QObject>
#include <QString>

namespace etfdt::shell {

class ShellDiagnosticQtAdapter : public QObject {
    Q_OBJECT
    Q_PROPERTY(ShellDiagnosticServiceListModel* serviceModel READ serviceModel NOTIFY stateChanged)
    Q_PROPERTY(ShellDiagnosticIssueListModel* issueModel READ issueModel NOTIFY stateChanged)
    Q_PROPERTY(ShellDiagnosticSummaryObject* summaryObject READ summaryObject NOTIFY stateChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY stateChanged)

public:
    explicit ShellDiagnosticQtAdapter(QObject* parent = nullptr);

    Q_INVOKABLE [[nodiscard]] bool loadMockHealthy();
    Q_INVOKABLE [[nodiscard]] bool loadMockWithWarnings();
    Q_INVOKABLE [[nodiscard]] bool loadMockWithErrors();
    Q_INVOKABLE [[nodiscard]] bool loadMockMixed();
    [[nodiscard]] bool loadFromFile(const QString& path);

    void setFilter(const ShellDiagnosticFilter& filter);
    void setSort(const ShellDiagnosticSort& sort);
    Q_INVOKABLE void clearFilter();
    Q_INVOKABLE void showAll();
    Q_INVOKABLE void showOnlyBlocked();
    Q_INVOKABLE void showOnlyIssues();
    Q_INVOKABLE [[nodiscard]] bool selectService(int index);
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
