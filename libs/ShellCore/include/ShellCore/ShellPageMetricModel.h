#pragma once

#include "ShellCore/ShellPageMetricTypes.h"

#include <QAbstractListModel>

namespace etfdt::shell {

class ShellPageMetricModel final : public QAbstractListModel {
    Q_OBJECT

public:
    enum Role {
        KeyRole = Qt::UserRole + 1,
        LabelRole,
        ValueTextRole,
        UnitTextRole,
        TrendTextRole,
        SeverityRole,
        MockRole,
        DescriptionRole,
    };

    explicit ShellPageMetricModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    void setMetrics(ShellPageMetricList metrics);
    [[nodiscard]] const ShellPageMetricList& metrics() const;

private:
    ShellPageMetricList metrics_;
};

}  // namespace etfdt::shell
