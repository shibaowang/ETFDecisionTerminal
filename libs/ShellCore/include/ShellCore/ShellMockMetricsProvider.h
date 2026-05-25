#pragma once

#include "ShellCore/ShellPageMetricTypes.h"

#include <QString>

namespace etfdt::shell {

class ShellMockMetricsProvider final {
public:
    [[nodiscard]] static ShellPageMetricList metricsForPage(const QString& pageKey);
    [[nodiscard]] static ShellPageActionHintList actionHintsForPage(const QString& pageKey);
};

}  // namespace etfdt::shell
