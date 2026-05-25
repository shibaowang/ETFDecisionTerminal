#include "ShellServices/ShellReadOnlyConnectionPresets.h"

#include <QString>
#include <QVariant>

namespace etfdt::shell_services {

namespace {

std::vector<ShellReadOnlyConnectionPreset> makeDefaultPresets()
{
    return {
        ShellReadOnlyConnectionPreset{
            "readonly_default",
            "DataService read-only service",
            "ETFDataServiceReadonly",
            "Development read-only DataService socket. It exposes only whitelisted read actions.",
            true,
            true,
            "ETFDataService --serve-readonly --db data/ETFDecision.db --socket-name ETFDataServiceReadonly"},
        ShellReadOnlyConnectionPreset{
            "audit_dev",
            "DataService audit development service",
            "ETFDataServiceAuditDev",
            "Development audit service socket. This preset is only a connection target and does not call write actions.",
            true,
            false,
            "ETFDataService --serve-dev-audit --db data/ETFDecision.db --socket-name ETFDataServiceAuditDev"},
        ShellReadOnlyConnectionPreset{
            "custom",
            "Custom socket",
            "",
            "Manual runtime socket name. It is not saved to disk.",
            true,
            false,
            "Enter a socketName manually."},
    };
}

}  // namespace

ShellReadOnlyConnectionPresetModel::ShellReadOnlyConnectionPresetModel(QObject* parent)
    : QAbstractListModel(parent)
    , presets_(makeDefaultPresets())
{
}

int ShellReadOnlyConnectionPresetModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : static_cast<int>(presets_.size());
}

QVariant ShellReadOnlyConnectionPresetModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0
        || index.row() >= static_cast<int>(presets_.size())) {
        return {};
    }
    const auto& preset = presets_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case KeyRole: return QString::fromStdString(preset.key);
    case TitleRole: return QString::fromStdString(preset.title);
    case SocketNameRole: return QString::fromStdString(preset.socketName);
    case DescriptionRole: return QString::fromStdString(preset.description);
    case DevelopmentOnlyRole: return preset.developmentOnly;
    case RecommendedRole: return preset.recommended;
    case CommandHintRole: return QString::fromStdString(preset.commandHint);
    default: return {};
    }
}

QHash<int, QByteArray> ShellReadOnlyConnectionPresetModel::roleNames() const
{
    return {
        {KeyRole, "key"},
        {TitleRole, "title"},
        {SocketNameRole, "socketName"},
        {DescriptionRole, "description"},
        {DevelopmentOnlyRole, "developmentOnly"},
        {RecommendedRole, "recommended"},
        {CommandHintRole, "commandHint"},
    };
}

const ShellReadOnlyConnectionPreset* ShellReadOnlyConnectionPresetModel::findByKey(
    const QString& key) const
{
    const auto target = key.toStdString();
    for (const auto& preset : presets_) {
        if (preset.key == target) {
            return &preset;
        }
    }
    return nullptr;
}

const std::vector<ShellReadOnlyConnectionPreset>& ShellReadOnlyConnectionPresetModel::presets()
    const noexcept
{
    return presets_;
}

}  // namespace etfdt::shell_services
