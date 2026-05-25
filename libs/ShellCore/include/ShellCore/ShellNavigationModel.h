#pragma once

#include "ShellCore/ShellNavigationTypes.h"

#include <QAbstractListModel>

#include <vector>

namespace etfdt::shell {

class ShellNavigationModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        KeyRole = Qt::UserRole + 1,
        TitleRole,
        IconNameRole,
        CategoryRole,
        EnabledRole,
        PlaceholderRole,
        SortOrderRole,
    };

    explicit ShellNavigationModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] const std::vector<ShellNavigationItem>& items() const;
    [[nodiscard]] const ShellNavigationItem* itemAt(int row) const;

private:
    std::vector<ShellNavigationItem> items_;
};

}  // namespace etfdt::shell
