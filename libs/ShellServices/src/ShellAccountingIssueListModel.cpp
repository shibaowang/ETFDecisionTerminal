#include "ShellServices/ShellAccountingIssueListModel.h"

#include <QString>
#include <QVariant>

#include <algorithm>
#include <utility>

namespace etfdt::shell_services {

namespace {

bool invalidIndex(const QModelIndex& index, const std::vector<ShellAccountingIssue>& issues)
{
    return !index.isValid() || index.row() < 0
        || index.row() >= static_cast<int>(issues.size());
}

}  // namespace

ShellAccountingIssueListModel::ShellAccountingIssueListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellAccountingIssueListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(issues_.size());
}

QVariant ShellAccountingIssueListModel::data(const QModelIndex& index, int role) const
{
    if (invalidIndex(index, issues_)) {
        return {};
    }

    const auto& issue = issues_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case CodeRole: return QString::fromStdString(issue.code);
    case LevelRole: return QString::fromStdString(issue.level);
    case MessageRole: return QString::fromStdString(issue.message);
    case BlockingRole: return issue.blocking;
    case SourceIdRole: return QString::fromStdString(issue.sourceId);
    default: return {};
    }
}

QHash<int, QByteArray> ShellAccountingIssueListModel::roleNames() const
{
    return {
        {CodeRole, "code"},
        {LevelRole, "level"},
        {MessageRole, "message"},
        {BlockingRole, "blocking"},
        {SourceIdRole, "sourceId"},
    };
}

void ShellAccountingIssueListModel::setIssues(std::vector<ShellAccountingIssue> issues)
{
    beginResetModel();
    issues_ = std::move(issues);
    endResetModel();
}

void ShellAccountingIssueListModel::clearIssues()
{
    setIssues(std::vector<ShellAccountingIssue>{});
}

std::size_t ShellAccountingIssueListModel::blockingIssueCount() const noexcept
{
    return static_cast<std::size_t>(std::count_if(
        issues_.begin(),
        issues_.end(),
        [](const ShellAccountingIssue& issue) { return issue.blocking; }));
}

const std::vector<ShellAccountingIssue>& ShellAccountingIssueListModel::issues() const noexcept
{
    return issues_;
}

}  // namespace etfdt::shell_services
