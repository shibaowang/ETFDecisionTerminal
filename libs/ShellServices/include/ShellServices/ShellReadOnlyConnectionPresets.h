#pragma once

#include <QAbstractListModel>

#include <string>
#include <vector>

namespace etfdt::shell_services {

struct ShellReadOnlyConnectionPreset final {
    std::string key;
    std::string title;
    std::string socketName;
    std::string description;
    bool developmentOnly = true;
    bool recommended = false;
    std::string commandHint;
};

class ShellReadOnlyConnectionPresetModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        KeyRole = Qt::UserRole + 1,
        TitleRole,
        SocketNameRole,
        DescriptionRole,
        DevelopmentOnlyRole,
        RecommendedRole,
        CommandHintRole,
    };

    explicit ShellReadOnlyConnectionPresetModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] const ShellReadOnlyConnectionPreset* findByKey(const QString& key) const;
    [[nodiscard]] const std::vector<ShellReadOnlyConnectionPreset>& presets() const noexcept;

private:
    std::vector<ShellReadOnlyConnectionPreset> presets_;
};

}  // namespace etfdt::shell_services
