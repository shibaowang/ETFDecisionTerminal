#pragma once

#include "ShellCore/ShellPageStatusTypes.h"

#include <QAbstractListModel>

#include <vector>

namespace etfdt::shell {

class ShellPageStatusModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        PageKeyRole = Qt::UserRole + 1,
        TitleRole,
        ModuleStatusRole,
        DataModeRole,
        ConnectionStatusRole,
        PlaceholderRole,
        WarningCountRole,
        ErrorCountRole,
        StatusTextRole,
        DetailTextRole,
    };

    explicit ShellPageStatusModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] const ShellPageStatus* statusForKey(const QString& pageKey) const;
    [[nodiscard]] const std::vector<ShellPageStatus>& statuses() const;

private:
    std::vector<ShellPageStatus> statuses_;
};

}  // namespace etfdt::shell
