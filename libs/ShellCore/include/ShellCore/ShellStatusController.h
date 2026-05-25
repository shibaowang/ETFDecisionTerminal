#pragma once

#include "ShellCore/ShellMockLogModel.h"
#include "ShellCore/ShellPageActionHintModel.h"
#include "ShellCore/ShellPageInfoObject.h"
#include "ShellCore/ShellPageMetricModel.h"
#include "ShellCore/ShellPageStatusModel.h"

#include <QObject>
#include <QString>

namespace etfdt::shell {

class ShellStatusController final : public QObject {
    Q_OBJECT
    Q_PROPERTY(ShellPageStatusModel* statusModel READ statusModel CONSTANT)
    Q_PROPERTY(ShellPageInfoObject* pageInfo READ pageInfo NOTIFY currentPageStatusChanged)
    Q_PROPERTY(ShellMockLogModel* logModel READ logModel CONSTANT)
    Q_PROPERTY(ShellPageMetricModel* metricsModel READ metricsModel CONSTANT)
    Q_PROPERTY(ShellPageActionHintModel* actionHintModel READ actionHintModel CONSTANT)

public:
    explicit ShellStatusController(QObject* parent = nullptr);

    [[nodiscard]] ShellPageStatusModel* statusModel();
    [[nodiscard]] ShellPageInfoObject* pageInfo();
    [[nodiscard]] ShellMockLogModel* logModel();
    [[nodiscard]] ShellPageMetricModel* metricsModel();
    [[nodiscard]] ShellPageActionHintModel* actionHintModel();

    Q_INVOKABLE [[nodiscard]] bool updateCurrentPageStatus(const QString& pageKey);

signals:
    void currentPageStatusChanged();

private:
    ShellPageStatusModel statusModel_;
    ShellPageInfoObject pageInfo_;
    ShellMockLogModel logModel_;
    ShellPageMetricModel metricsModel_;
    ShellPageActionHintModel actionHintModel_;
};

}  // namespace etfdt::shell
