#pragma once

#include "FakeAccountingClientPayload.h"

#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_fake {

struct FakeAccountingClientScenario final {
    std::string scenarioName;
    FakeAccountingClientPayload payload;
    etfdt::shell_services::ShellAccountingViewState expectedState =
        etfdt::shell_services::ShellAccountingViewState::Unavailable;
    bool expectedIssuesVisible = false;
    bool expectedBlockingVisible = false;
    bool expectedReadOnly = true;
    bool expectedWriteEnabled = false;
    bool expectedTradeDraftGenerated = false;
    bool expectedTradeSuggestionGenerated = false;
    bool expectedStrategyExecuted = false;
    bool expectedBrokerOrder = false;
    std::string expectedPrivacyDisplayText;
};

[[nodiscard]] std::vector<FakeAccountingClientScenario> guardPayloadScenarios();
[[nodiscard]] std::vector<FakeAccountingClientScenario> stateTransitionScenarios();
[[nodiscard]] std::vector<FakeAccountingClientScenario> issueMappingScenarios();
[[nodiscard]] FakeAccountingClientScenario privacyScenario();
[[nodiscard]] FakeAccountingClientScenario noTradeScenario();

[[nodiscard]] bool hasActionScenario(
    const std::vector<FakeAccountingClientScenario>& scenarios,
    const std::string& actionName);
[[nodiscard]] bool hasScenario(
    const std::vector<FakeAccountingClientScenario>& scenarios,
    const std::string& scenarioName);

}  // namespace etfdt::tests::shell_accounting_fake
