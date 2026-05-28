#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::position_list_first_stage {

struct PositionListFirstStageScenario final {
    std::string scenarioName;
    std::string requestPayload;
    bool expectedFutureImplemented = true;
    bool currentGuardImplemented = false;
    bool expectedReadOnly = true;
    bool expectedWriteEnabled = false;
    std::string expectedDomainStatus;
    std::string expectedDataQualityStatus;
    std::vector<std::string> expectedIssues;
    std::string expectedPositionsPolicy;
    bool expectedNoWrite = true;
    bool requiresTradeFacts = false;
    bool requiresCashFacts = false;
    bool requiresMarketPriceFacts = false;
    bool requiresFxRateFacts = false;
    bool requiresAccountingEngine = false;
    bool requiresDataAccess = false;
    bool qmlAllowed = false;
    std::string notes;
};

struct FailureMatrixEntry final {
    std::string status;
    bool noWrite = true;
    bool readOnly = true;
    bool writeEnabled = false;
    bool blocking = false;
    bool partialPositionsAllowed = false;
    std::string protocolSuccessPolicy;
    std::string notes;
};

[[nodiscard]] std::vector<PositionListFirstStageScenario> firstStageScenarios();
[[nodiscard]] std::vector<FailureMatrixEntry> firstStageFailureMatrix();

[[nodiscard]] bool hasScenario(
    const std::vector<PositionListFirstStageScenario>& scenarios,
    const std::string& scenarioName);

[[nodiscard]] bool hasFailureStatus(
    const std::vector<FailureMatrixEntry>& matrix,
    const std::string& status);

}  // namespace etfdt::tests::position_list_first_stage
