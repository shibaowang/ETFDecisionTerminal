#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting {

struct ShellAccountingIssueDescriptor final {
    std::string code;
    std::string level;
    std::string message;
    bool blocking = false;
    std::string sourceId;
};

struct ShellAccountingControllerScenario final {
    std::string scenarioName;
    std::string actionName;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
    std::string dataQualityStatus;
    std::string payloadStatus;
    std::vector<ShellAccountingIssueDescriptor> issues;
    bool hasRows = false;
    std::string expectedViewState;
    bool expectedIssueVisible = false;
    bool expectedBlockingVisible = false;
    bool expectedAmountMasked = false;
    bool expectedAllowsTradeAction = false;
    bool expectedGeneratesTradeDraft = false;
    std::string notes;
};

struct ShellAccountingTradeActionBoundary final {
    std::string boundaryName;
    std::string fieldName;
    bool allowsTradeAction = false;
    bool generatesTradeDraft = false;
    bool generatesTradeSuggestion = false;
    bool executesStrategy = false;
    bool sendsBrokerOrder = false;
    bool qmlCalculates = false;
    std::string notes;
};

[[nodiscard]] std::vector<ShellAccountingControllerScenario> guardPayloadScenarios();
[[nodiscard]] std::vector<ShellAccountingControllerScenario> stateContractScenarios();
[[nodiscard]] std::vector<ShellAccountingIssueDescriptor> issueContractScenarios();
[[nodiscard]] std::vector<ShellAccountingTradeActionBoundary> noTradeActionBoundaries();
[[nodiscard]] std::vector<std::string> protectedAccountingTables();

[[nodiscard]] bool hasActionScenario(
    const std::vector<ShellAccountingControllerScenario>& scenarios,
    const std::string& actionName);

[[nodiscard]] bool hasViewStateScenario(
    const std::vector<ShellAccountingControllerScenario>& scenarios,
    const std::string& viewState);

[[nodiscard]] bool hasIssueCode(
    const std::vector<ShellAccountingIssueDescriptor>& issues,
    const std::string& code);

}  // namespace etfdt::tests::shell_accounting
