#pragma once

#include <string>
#include <vector>

namespace etfdt::shell {

struct ShellPageMetric final {
    std::string key;
    std::string label;
    std::string valueText;
    std::string unitText;
    std::string trendText;
    std::string severity;
    bool mock = true;
    std::string description;
};

struct ShellPageActionHint final {
    std::string key;
    std::string title;
    std::string description;
    std::string severity;
    bool enabled = false;
    bool mock = true;
    std::string actionText;
};

using ShellPageMetricList = std::vector<ShellPageMetric>;
using ShellPageActionHintList = std::vector<ShellPageActionHint>;

}  // namespace etfdt::shell
