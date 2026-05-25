#pragma once

#include "ShellCore/ShellPageMetricTypes.h"

#include <QAbstractListModel>

namespace etfdt::shell {

class ShellPageActionHintModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        KeyRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        SeverityRole,
        EnabledRole,
        MockRole,
        ActionTextRole,
    };

    explicit ShellPageActionHintModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void setHints(ShellPageActionHintList hints);
    [[nodiscard]] const ShellPageActionHintList& hints() const;

private:
    ShellPageActionHintList hints_;
};

}  // namespace etfdt::shell
