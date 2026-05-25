#include "ShellCore/ShellDiagnosticQtAdapter.h"

namespace etfdt::shell {

ShellDiagnosticQtAdapter::ShellDiagnosticQtAdapter(QObject* parent)
    : QObject(parent),
      serviceModel_(this),
      issueModel_(this),
      summaryObject_(this)
{
}

bool ShellDiagnosticQtAdapter::loadMockHealthy()
{
    const bool ok = presenter_.loadMockHealthy();
    syncModels();
    return ok;
}

bool ShellDiagnosticQtAdapter::loadMockWithWarnings()
{
    const bool ok = presenter_.loadMockWithWarnings();
    syncModels();
    return ok;
}

bool ShellDiagnosticQtAdapter::loadMockWithErrors()
{
    const bool ok = presenter_.loadMockWithErrors();
    syncModels();
    return ok;
}

bool ShellDiagnosticQtAdapter::loadMockMixed()
{
    const bool ok = presenter_.loadMockMixed();
    syncModels();
    return ok;
}

bool ShellDiagnosticQtAdapter::loadFromFile(const QString& path)
{
    const bool ok = presenter_.loadFromFile(path.toStdString());
    syncModels();
    return ok;
}

void ShellDiagnosticQtAdapter::setFilter(const ShellDiagnosticFilter& filter)
{
    presenter_.setFilter(filter);
    selectedServiceIndex_ = -1;
    syncModels();
}

void ShellDiagnosticQtAdapter::setSort(const ShellDiagnosticSort& sort)
{
    presenter_.setSort(sort);
    selectedServiceIndex_ = -1;
    syncModels();
}

void ShellDiagnosticQtAdapter::clearFilter()
{
    presenter_.clearFilter();
    selectedServiceIndex_ = -1;
    syncModels();
}

void ShellDiagnosticQtAdapter::showAll()
{
    clearFilter();
}

void ShellDiagnosticQtAdapter::showOnlyBlocked()
{
    ShellDiagnosticFilter filter;
    filter.onlyBlocked = true;
    setFilter(filter);
}

void ShellDiagnosticQtAdapter::showOnlyIssues()
{
    ShellDiagnosticFilter filter;
    filter.onlyWithIssues = true;
    setFilter(filter);
}

bool ShellDiagnosticQtAdapter::selectService(int index)
{
    const auto* row = serviceModel_.rowAt(index);
    if (row == nullptr) {
        selectedServiceIndex_ = -1;
        issueModel_.clearIssues();
        emit stateChanged();
        return false;
    }

    selectedServiceIndex_ = index;
    issueModel_.setIssues(row->issues);
    emit stateChanged();
    return true;
}

ShellDiagnosticRefreshResult ShellDiagnosticQtAdapter::refreshIfChanged()
{
    auto result = presenter_.refreshIfChanged();
    syncModels();
    return result;
}

ShellDiagnosticServiceListModel* ShellDiagnosticQtAdapter::serviceModel()
{
    return &serviceModel_;
}

ShellDiagnosticIssueListModel* ShellDiagnosticQtAdapter::issueModel()
{
    return &issueModel_;
}

ShellDiagnosticSummaryObject* ShellDiagnosticQtAdapter::summaryObject()
{
    return &summaryObject_;
}

QString ShellDiagnosticQtAdapter::lastError() const
{
    return lastError_;
}

void ShellDiagnosticQtAdapter::syncModels()
{
    const auto state = presenter_.currentState();
    lastError_ = QString::fromStdString(state.lastError);
    serviceModel_.setViewModel(state.viewModel);
    summaryObject_.setViewModel(state.viewModel);

    if (selectedServiceIndex_ >= 0) {
        const auto* row = serviceModel_.rowAt(selectedServiceIndex_);
        if (row != nullptr) {
            issueModel_.setIssues(row->issues);
        } else {
            selectedServiceIndex_ = -1;
            issueModel_.clearIssues();
        }
    } else {
        issueModel_.clearIssues();
    }
    emit stateChanged();
}

}  // namespace etfdt::shell
