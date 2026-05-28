#pragma once

#include <string>
#include <vector>

namespace etfdt::tests::position_list_no_write {

struct PositionListRealActionNoWriteScenario final {
    std::string scenarioName;
    std::string requestPayload;
    std::string expectedDomainStatus;
    bool expectedImplemented = false;
    bool expectedReadOnly = true;
    bool expectedWriteEnabled = false;
    bool expectedNoWrite = true;
    std::string notes;
};

struct FakePositionListActionResult final {
    std::string scenarioName;
    std::string domainStatus;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
};

[[nodiscard]] std::vector<PositionListRealActionNoWriteScenario>
positionListRealActionNoWriteScenarios();

[[nodiscard]] std::vector<PositionListRealActionNoWriteScenario>
positionListRealActionFailureNoWriteScenarios();

[[nodiscard]] bool hasScenario(
    const std::vector<PositionListRealActionNoWriteScenario>& scenarios,
    const std::string& scenarioName);

[[nodiscard]] FakePositionListActionResult runFakePositionListRealActionCallbackForTest(
    const PositionListRealActionNoWriteScenario& scenario);

}  // namespace etfdt::tests::position_list_no_write
