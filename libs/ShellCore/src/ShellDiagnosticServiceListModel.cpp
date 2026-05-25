#include "ShellCore/ShellDiagnosticServiceListModel.h"

#include <QString>

namespace etfdt::shell {

ShellDiagnosticServiceListModel::ShellDiagnosticServiceListModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellDiagnosticServiceListModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(rows_.size());
}

QVariant ShellDiagnosticServiceListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const auto& row = rows_.at(static_cast<std::size_t>(index.row()));
    switch (role) {
    case Qt::DisplayRole:
    case ServiceNameRole:
        return QString::fromStdString(row.serviceName);
    case EnabledRole:
        return row.enabled;
    case SupportedRole:
        return row.supported;
    case CanStartRole:
        return row.canStart;
    case HealthCheckSupportedRole:
        return row.healthCheckSupported;
    case ExecutableExistsRole:
        return row.executableExists;
    case WorkingDirectoryExistsRole:
        return row.workingDirectoryExists;
    case SocketNamePresentRole:
        return row.socketNamePresent;
    case StatusTextRole:
        return QString::fromStdString(row.statusText);
    case StatusSeverityRole:
        return QString::fromStdString(row.statusSeverity);
    case StatusSeverityRankRole:
        return row.statusSeverityRank;
    case IssueCountRole:
        return row.issueCount;
    default:
        return {};
    }
}

QHash<int, QByteArray> ShellDiagnosticServiceListModel::roleNames() const
{
    return {
        {ServiceNameRole, "serviceName"},
        {EnabledRole, "enabled"},
        {SupportedRole, "supported"},
        {CanStartRole, "canStart"},
        {HealthCheckSupportedRole, "healthCheckSupported"},
        {ExecutableExistsRole, "executableExists"},
        {WorkingDirectoryExistsRole, "workingDirectoryExists"},
        {SocketNamePresentRole, "socketNamePresent"},
        {StatusTextRole, "statusText"},
        {StatusSeverityRole, "statusSeverity"},
        {StatusSeverityRankRole, "statusSeverityRank"},
        {IssueCountRole, "issueCount"},
    };
}

void ShellDiagnosticServiceListModel::setViewModel(const ShellDiagnosticViewModel& viewModel)
{
    setRows(viewModel.serviceRows);
}

void ShellDiagnosticServiceListModel::setRows(std::vector<ShellDiagnosticServiceRow> rows)
{
    beginResetModel();
    rows_ = std::move(rows);
    endResetModel();
}

const std::vector<ShellDiagnosticServiceRow>& ShellDiagnosticServiceListModel::rows() const
{
    return rows_;
}

const ShellDiagnosticServiceRow* ShellDiagnosticServiceListModel::rowAt(int row) const
{
    if (row < 0 || row >= rowCount()) {
        return nullptr;
    }
    return &rows_.at(static_cast<std::size_t>(row));
}

}  // namespace etfdt::shell
