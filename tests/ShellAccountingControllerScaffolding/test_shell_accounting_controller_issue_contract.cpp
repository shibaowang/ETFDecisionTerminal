#include "ShellAccountingControllerScenario.h"
#include "ShellAccountingStateMappingTestUtils.h"

#include <iostream>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

}  // namespace

int main()
{
    const auto issues = etfdt::tests::shell_accounting::issueContractScenarios();
    expectTrue(etfdt::tests::shell_accounting::hasIssueCode(issues, "MARKET_PRICE_MISSING"), "MARKET_PRICE_MISSING issue exists");
    expectTrue(etfdt::tests::shell_accounting::hasIssueCode(issues, "MISSING_FEE"), "MISSING_FEE issue exists");
    expectTrue(etfdt::tests::shell_accounting::hasIssueCode(issues, "NEGATIVE_CASH"), "NEGATIVE_CASH issue exists");
    expectTrue(etfdt::tests::shell_accounting::hasIssueCode(issues, "SELL_EXCEEDS_POSITION"), "SELL_EXCEEDS_POSITION issue exists");
    expectTrue(etfdt::tests::shell_accounting::hasIssueCode(issues, "MULTI_CURRENCY_UNSUPPORTED"), "MULTI_CURRENCY_UNSUPPORTED issue exists");
    expectTrue(etfdt::tests::shell_accounting::hasIssueCode(issues, "FX_RATE_MISSING"), "FX_RATE_MISSING issue exists");
    expectTrue(etfdt::tests::shell_accounting::hasIssueCode(issues, "REPLAY_NOT_AVAILABLE"), "REPLAY_NOT_AVAILABLE issue exists");

    for (const auto& issue : issues) {
        expectTrue(!issue.code.empty(), "issue code is preserved");
        expectTrue(!issue.level.empty(), "issue level is preserved");
        expectTrue(!issue.message.empty(), "issue message is preserved");
        expectTrue(!issue.sourceId.empty(), "issue source id is preserved");

        if (issue.code == "MARKET_PRICE_MISSING" || issue.code == "MISSING_FEE") {
            expectTrue(etfdt::tests::shell_accounting::mapIssueToViewState(issue) == "Warning", "non-blocking data issues map to Warning");
            expectTrue(!issue.blocking, "warning issue remains non-blocking");
        }

        if (issue.code == "NEGATIVE_CASH" || issue.code == "SELL_EXCEEDS_POSITION" ||
            issue.code == "MULTI_CURRENCY_UNSUPPORTED" || issue.code == "FX_RATE_MISSING") {
            expectTrue(etfdt::tests::shell_accounting::mapIssueToViewState(issue) == "Error", "blocking issue maps to Error");
            expectTrue(issue.blocking, "blocking issue stays visible");
        }

        if (issue.code == "REPLAY_NOT_AVAILABLE") {
            expectTrue(etfdt::tests::shell_accounting::mapIssueToViewState(issue) == "Error", "blocking replay unavailable maps to Error");
        }
    }

    return gFailures == 0 ? 0 : 1;
}
