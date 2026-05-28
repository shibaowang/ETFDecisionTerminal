#include "ShellServices/ShellAccountingReadOnlyController.h"

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

void expectIssueState(
    const etfdt::shell_services::ShellAccountingIssue& issue,
    etfdt::shell_services::ShellAccountingViewState expected,
    std::string_view message)
{
    const auto actual = etfdt::shell_services::shellAccountingViewStateFromIssue(issue);
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected "
                  << etfdt::shell_services::toString(expected) << " actual "
                  << etfdt::shell_services::toString(actual) << '\n';
    }
}

}  // namespace

int main()
{
    const etfdt::shell_services::ShellAccountingIssue warning{
        "MARKET_PRICE_MISSING",
        "WARNING",
        "Market price is missing.",
        false,
        "market_price",
    };
    expectTrue(warning.code == "MARKET_PRICE_MISSING", "issue code is preserved");
    expectTrue(warning.level == "WARNING", "issue level is preserved");
    expectTrue(warning.message == "Market price is missing.", "issue message is preserved");
    expectTrue(!warning.blocking, "issue blocking flag is preserved");
    expectIssueState(warning, etfdt::shell_services::ShellAccountingViewState::Warning, "MARKET_PRICE_MISSING maps to Warning");

    expectIssueState(
        {"MISSING_FEE", "WARNING", "Trade fee is missing.", false, "trade_fact"},
        etfdt::shell_services::ShellAccountingViewState::Warning,
        "MISSING_FEE maps to Warning");
    expectIssueState(
        {"NEGATIVE_CASH", "ERROR", "Cash would become negative.", true, "replay"},
        etfdt::shell_services::ShellAccountingViewState::Error,
        "NEGATIVE_CASH maps to Error");
    expectIssueState(
        {"SELL_EXCEEDS_POSITION", "ERROR", "Sell exceeds position.", true, "replay"},
        etfdt::shell_services::ShellAccountingViewState::Error,
        "SELL_EXCEEDS_POSITION maps to Error");
    expectIssueState(
        {"MULTI_CURRENCY_UNSUPPORTED", "ERROR", "Multi-currency unsupported.", true, "fx"},
        etfdt::shell_services::ShellAccountingViewState::Error,
        "MULTI_CURRENCY_UNSUPPORTED maps to Error");
    expectIssueState(
        {"FX_RATE_MISSING", "ERROR", "FX rate is missing.", true, "fx"},
        etfdt::shell_services::ShellAccountingViewState::Error,
        "FX_RATE_MISSING maps to Error");
    expectIssueState(
        {"REPLAY_NOT_AVAILABLE", "ERROR", "Replay not available.", true, "replay"},
        etfdt::shell_services::ShellAccountingViewState::Error,
        "blocking REPLAY_NOT_AVAILABLE maps to Error");

    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    controller.markUnavailable(
        "position.list",
        {"REPLAY_NOT_AVAILABLE", "ERROR", "Replay not available.", true, "replay"});
    expectTrue(controller.issues().size() == 1, "controller exposes issue list");
    expectTrue(controller.issues().front().blocking, "controller keeps blocking issue visible");

    return gFailures == 0 ? 0 : 1;
}
