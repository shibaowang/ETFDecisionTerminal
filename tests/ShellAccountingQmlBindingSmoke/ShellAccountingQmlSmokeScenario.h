#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_qml_binding_smoke {

struct SmokeIssue final {
    std::string code;
    std::string level;
    std::string message;
    bool blocking = false;
};

struct ShellAccountingQmlSmokeScenario final {
    std::string scenarioName;
    std::string objectName;
    std::string state;
    std::string payloadStatus;
    std::string dataQualityStatus;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
    bool hasRows = false;
    std::vector<SmokeIssue> issues;
    bool privacyMode = false;
    std::string expectedVisibleState;
    bool expectedIssueVisible = true;
    bool expectedBlockingVisible = false;
    std::string expectedDisplayTextPolicy;
    bool expectedAllowsTradeButton = false;
    bool expectedAllowsTradeDraft = false;
    bool expectedAllowsStrategyAction = false;
    bool expectedAllowsBrokerOrder = false;
    std::string notes;
};

[[nodiscard]] std::vector<ShellAccountingQmlSmokeScenario> stateMatrixScenarios();
[[nodiscard]] std::vector<ShellAccountingQmlSmokeScenario> guardPayloadScenarios();
[[nodiscard]] ShellAccountingQmlSmokeScenario privacyScenario(bool privacyMode);
[[nodiscard]] ShellAccountingQmlSmokeScenario warningIssueScenario();

}  // namespace etfdt::tests::shell_accounting_qml_binding_smoke
