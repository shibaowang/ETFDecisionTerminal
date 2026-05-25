#include "ShellCore/ShellPageActionHintModel.h"

#include <QString>

#include <utility>

namespace etfdt::shell {

ShellPageActionHintModel::ShellPageActionHintModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellPageActionHintModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(hints_.size());
}

QVariant ShellPageActionHintModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const auto& hint = hints_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case KeyRole:
        return QString::fromStdString(hint.key);
    case TitleRole:
        return QString::fromStdString(hint.title);
    case DescriptionRole:
        return QString::fromStdString(hint.description);
    case SeverityRole:
        return QString::fromStdString(hint.severity);
    case EnabledRole:
        return hint.enabled;
    case MockRole:
        return hint.mock;
    case ActionTextRole:
        return QString::fromStdString(hint.actionText);
    default:
        return {};
    }
}

QHash<int, QByteArray> ShellPageActionHintModel::roleNames() const
{
    return {
        {KeyRole, "key"},
        {TitleRole, "title"},
        {DescriptionRole, "description"},
        {SeverityRole, "severity"},
        {EnabledRole, "enabled"},
        {MockRole, "mock"},
        {ActionTextRole, "actionText"},
    };
}

void ShellPageActionHintModel::setHints(ShellPageActionHintList hints)
{
    beginResetModel();
    hints_ = std::move(hints);
    endResetModel();
}

const ShellPageActionHintList& ShellPageActionHintModel::hints() const
{
    return hints_;
}

}  // namespace etfdt::shell
