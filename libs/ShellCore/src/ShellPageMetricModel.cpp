#include "ShellCore/ShellPageMetricModel.h"

#include <QString>

#include <utility>

namespace etfdt::shell {

ShellPageMetricModel::ShellPageMetricModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int ShellPageMetricModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(metrics_.size());
}

QVariant ShellPageMetricModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount()) {
        return {};
    }

    const auto& metric = metrics_[static_cast<std::size_t>(index.row())];
    switch (role) {
    case KeyRole:
        return QString::fromStdString(metric.key);
    case LabelRole:
        return QString::fromStdString(metric.label);
    case ValueTextRole:
        return QString::fromStdString(metric.valueText);
    case UnitTextRole:
        return QString::fromStdString(metric.unitText);
    case TrendTextRole:
        return QString::fromStdString(metric.trendText);
    case SeverityRole:
        return QString::fromStdString(metric.severity);
    case MockRole:
        return metric.mock;
    case DescriptionRole:
        return QString::fromStdString(metric.description);
    default:
        return {};
    }
}

QHash<int, QByteArray> ShellPageMetricModel::roleNames() const
{
    return {
        {KeyRole, "key"},
        {LabelRole, "label"},
        {ValueTextRole, "valueText"},
        {UnitTextRole, "unitText"},
        {TrendTextRole, "trendText"},
        {SeverityRole, "severity"},
        {MockRole, "mock"},
        {DescriptionRole, "description"},
    };
}

void ShellPageMetricModel::setMetrics(ShellPageMetricList metrics)
{
    beginResetModel();
    metrics_ = std::move(metrics);
    endResetModel();
}

const ShellPageMetricList& ShellPageMetricModel::metrics() const
{
    return metrics_;
}

}  // namespace etfdt::shell
