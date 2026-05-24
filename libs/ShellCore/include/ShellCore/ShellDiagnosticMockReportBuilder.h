#pragma once

#include <string>

namespace etfdt::shell {

class ShellDiagnosticMockReportBuilder final {
public:
    [[nodiscard]] static std::string healthyReportJson();
    [[nodiscard]] static std::string warningReportJson();
    [[nodiscard]] static std::string errorReportJson();
    [[nodiscard]] static std::string mixedReportJson();
};

}  // namespace etfdt::shell
