#include "FakeShellAccountingServiceAdapter.h"

#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

etfdt::shell_services::ShellAccountingServiceResult resultWithIssue(
    std::string code,
    std::string level,
    bool blocking,
    std::string dataQualityStatus)
{
    etfdt::shell_services::ShellAccountingServiceResult result;
    result.protocolSuccess = true;
    result.implemented = true;
    result.readOnly = true;
    result.writeEnabled = false;
    result.dataQualityStatus = std::move(dataQualityStatus);
    result.hasRows = true;
    result.issues.push_back({
        std::move(code),
        std::move(level),
        "issue is visible",
        blocking,
        "position.list",
    });
    return result;
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_service_adapter;

    struct Case final {
        std::string code;
        std::string level;
        bool blocking;
        std::string dataQualityStatus;
        etfdt::shell_services::ShellAccountingViewState expectedState;
    };

    const std::vector<Case> cases{
        {"MARKET_PRICE_MISSING", "WARNING", false, "WARNING", etfdt::shell_services::ShellAccountingViewState::Warning},
        {"MISSING_FEE", "WARNING", false, "WARNING", etfdt::shell_services::ShellAccountingViewState::Warning},
        {"NEGATIVE_CASH", "ERROR", true, "ERROR", etfdt::shell_services::ShellAccountingViewState::Error},
        {"SELL_EXCEEDS_POSITION", "ERROR", true, "ERROR", etfdt::shell_services::ShellAccountingViewState::Error},
        {"MULTI_CURRENCY_UNSUPPORTED", "ERROR", true, "ERROR", etfdt::shell_services::ShellAccountingViewState::Error},
        {"FX_RATE_MISSING", "ERROR", true, "ERROR", etfdt::shell_services::ShellAccountingViewState::Error},
        {"REPLAY_NOT_AVAILABLE", "ERROR", true, "ERROR", etfdt::shell_services::ShellAccountingViewState::Error},
    };

    auto fakeAdapter = std::make_shared<FakeShellAccountingServiceAdapter>();
    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    controller.setServiceAdapter(fakeAdapter);

    for (const auto& testCase : cases) {
        fakeAdapter->setNextResult(resultWithIssue(
            testCase.code,
            testCase.level,
            testCase.blocking,
            testCase.dataQualityStatus));
        controller.refreshPositionList(makeDemoServiceRequest("position.list"));
        expectTrue(controller.currentState() == testCase.expectedState, "issue maps to expected state");
        expectTrue(!controller.issues().empty(), "issue remains visible");
        expectTrue(controller.issues().front().code == testCase.code, "issue code is preserved");
        expectTrue(controller.issues().front().level == testCase.level, "issue level is preserved");
        expectTrue(!controller.issues().front().message.empty(), "issue message is preserved");
        expectTrue(controller.issues().front().blocking == testCase.blocking, "issue blocking is preserved");
    }

    return gFailures == 0 ? 0 : 1;
}
