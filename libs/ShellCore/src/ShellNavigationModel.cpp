#include "ShellCore/ShellNavigationModel.h"

#include "ShellCore/ShellPageRegistry.h"

#include <QString>

namespace etfdt::shell {

ShellNavigationModel::ShellNavigationModel(QObject* parent)
    : QAbstractListModel(parent),
      items_(ShellPageRegistry::listNavigationItems())
{
}

int ShellNavigationModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(items_.size());
}

QVariant ShellNavigationModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const auto& item = items_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case KeyRole:
        return QString::fromStdString(item.key);
    case TitleRole:
        return QString::fromStdString(item.title);
    case IconNameRole:
        return QString::fromStdString(item.iconName);
    case CategoryRole:
        return QString::fromStdString(item.category);
    case EnabledRole:
        return item.enabled;
    case PlaceholderRole:
        return item.placeholder;
    case SortOrderRole:
        return item.sortOrder;
    default:
        return {};
    }
}

QHash<int, QByteArray> ShellNavigationModel::roleNames() const
{
    return {
        {KeyRole, "key"},
        {TitleRole, "title"},
        {IconNameRole, "iconName"},
        {CategoryRole, "category"},
        {EnabledRole, "enabled"},
        {PlaceholderRole, "placeholder"},
        {SortOrderRole, "sortOrder"},
    };
}

const std::vector<ShellNavigationItem>& ShellNavigationModel::items() const
{
    return items_;
}

const ShellNavigationItem* ShellNavigationModel::itemAt(int row) const
{
    if (row < 0 || row >= rowCount()) {
        return nullptr;
    }
    return &items_[static_cast<std::size_t>(row)];
}

}  // namespace etfdt::shell
