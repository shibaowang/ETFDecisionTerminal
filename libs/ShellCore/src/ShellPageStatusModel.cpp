#include "ShellCore/ShellPageStatusModel.h"

#include "ShellCore/ShellPageRegistry.h"

#include <QString>

namespace etfdt::shell {

namespace {

ShellPageStatus makeStatus(const ShellPageMetadata& metadata)
{
    ShellPageStatus status;
    status.pageKey = metadata.key;
    status.title = metadata.title;
    status.placeholder = metadata.placeholder;

    if (metadata.key == "diagnostics") {
        status.moduleStatus = "MOCK";
        status.dataMode = "MOCK_DATA";
        status.connectionStatus = "MOCK";
        status.warningCount = 0;
        status.errorCount = 0;
        status.statusText = "Diagnostics mock data";
        status.detailText = "Diagnostics Center is using ShellCore mock data and is not connected to real services.";
        return status;
    }

    if (metadata.key == "readonly_data") {
        status.moduleStatus = "READY";
        status.dataMode = "REAL_DATA_PENDING";
        status.connectionStatus = "NOT_CONNECTED";
        status.warningCount = 0;
        status.errorCount = 0;
        status.statusText = "Read-only prototype";
        status.detailText = "This page can connect through ShellServices to DataService read-only actions only.";
        return status;
    }

    status.moduleStatus = "PLACEHOLDER";
    status.dataMode = "NO_DATA";
    status.connectionStatus = "NOT_CONNECTED";
    status.warningCount = 1;
    status.errorCount = 0;
    status.statusText = "Placeholder module";
    status.detailText = "This module is a placeholder mock. Real business data is not connected.";
    return status;
}

}  // namespace

ShellPageStatusModel::ShellPageStatusModel(QObject* parent)
    : QAbstractListModel(parent)
{
    const auto pages = ShellPageRegistry::pages();
    statuses_.reserve(pages.size());
    for (const auto& page : pages) {
        statuses_.push_back(makeStatus(page));
    }
}

int ShellPageStatusModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(statuses_.size());
}

QVariant ShellPageStatusModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const auto& status = statuses_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case PageKeyRole:
        return QString::fromStdString(status.pageKey);
    case TitleRole:
        return QString::fromStdString(status.title);
    case ModuleStatusRole:
        return QString::fromStdString(status.moduleStatus);
    case DataModeRole:
        return QString::fromStdString(status.dataMode);
    case ConnectionStatusRole:
        return QString::fromStdString(status.connectionStatus);
    case PlaceholderRole:
        return status.placeholder;
    case WarningCountRole:
        return status.warningCount;
    case ErrorCountRole:
        return status.errorCount;
    case StatusTextRole:
        return QString::fromStdString(status.statusText);
    case DetailTextRole:
        return QString::fromStdString(status.detailText);
    default:
        return {};
    }
}

QHash<int, QByteArray> ShellPageStatusModel::roleNames() const
{
    return {
        {PageKeyRole, "pageKey"},
        {TitleRole, "title"},
        {ModuleStatusRole, "moduleStatus"},
        {DataModeRole, "dataMode"},
        {ConnectionStatusRole, "connectionStatus"},
        {PlaceholderRole, "placeholder"},
        {WarningCountRole, "warningCount"},
        {ErrorCountRole, "errorCount"},
        {StatusTextRole, "statusText"},
        {DetailTextRole, "detailText"},
    };
}

const ShellPageStatus* ShellPageStatusModel::statusForKey(const QString& pageKey) const
{
    const auto key = pageKey.toStdString();
    for (const auto& status : statuses_) {
        if (status.pageKey == key) {
            return &status;
        }
    }
    return nullptr;
}

const std::vector<ShellPageStatus>& ShellPageStatusModel::statuses() const
{
    return statuses_;
}

}  // namespace etfdt::shell
