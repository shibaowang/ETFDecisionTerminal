#include "ShellCore/ShellDiagnosticIssueListModel.h"

#include <QString>

namespace etfdt::shell {

ShellDiagnosticIssueListModel::ShellDiagnosticIssueListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellDiagnosticIssueListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(issues_.size());
}

QVariant ShellDiagnosticIssueListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const auto& issue = issues_.at(static_cast<std::size_t>(index.row()));
    switch (role) {
    case Qt::DisplayRole:
    case DisplayTextRole:
        return QString::fromStdString(issue.displayText);
    case LevelRole:
        return QString::fromStdString(issue.level);
    case CodeRole:
        return QString::fromStdString(issue.code);
    case MessageRole:
        return QString::fromStdString(issue.message);
    case SeverityRankRole:
        return issue.severityRank;
    default:
        return {};
    }
}

QHash<int, QByteArray> ShellDiagnosticIssueListModel::roleNames() const
{
    return {
        {LevelRole, "level"},
        {CodeRole, "code"},
        {MessageRole, "message"},
        {DisplayTextRole, "displayText"},
        {SeverityRankRole, "severityRank"},
    };
}

void ShellDiagnosticIssueListModel::setIssues(std::vector<ShellDiagnosticIssueRow> issues)
{
    beginResetModel();
    issues_ = std::move(issues);
    endResetModel();
}

void ShellDiagnosticIssueListModel::clearIssues()
{
    setIssues({});
}

const std::vector<ShellDiagnosticIssueRow>& ShellDiagnosticIssueListModel::issues() const
{
    return issues_;
}

}  // namespace etfdt::shell
