#include "ShellServices/ShellAccountingStatusObject.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void expectTrue(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

etfdt::shell_services::ShellAccountingStateSnapshot snapshot(
    std::string payloadStatus,
    std::string dataQualityStatus,
    bool implemented,
    bool hasRows,
    bool stale = false)
{
    etfdt::shell_services::ShellAccountingStateSnapshot value;
    value.actionName = "position.list";
    value.implemented = implemented;
    value.readOnly = true;
    value.writeEnabled = false;
    value.payloadStatus = std::move(payloadStatus);
    value.dataQualityStatus = std::move(dataQualityStatus);
    value.hasRows = hasRows;
    value.stale = stale;
    value.issues = {
        {"VISIBLE_WARNING", "WARNING", "warning remains visible", false, "status-test"},
        {"VISIBLE_BLOCKING", "ERROR", "blocking remains visible", true, "status-test"},
    };
    return value;
}

void expectState(
    etfdt::shell_services::ShellAccountingStatusObject& status,
    etfdt::shell_services::ShellAccountingStateSnapshot value,
    etfdt::shell_services::ShellAccountingViewState expected,
    const char* message)
{
    status.applyStateSnapshot(std::move(value));
    expectTrue(status.state() == expected, message);
}

}  // namespace

int main()
{
    using etfdt::shell_services::ShellAccountingStatusObject;
    using etfdt::shell_services::ShellAccountingViewState;

    ShellAccountingStatusObject status;
    expectTrue(status.state() == ShellAccountingViewState::Idle, "default state is Idle");
    expectTrue(status.readOnly(), "status object is read-only by default");
    expectTrue(!status.writeEnabled(), "status object writeEnabled is false by default");

    expectState(status, snapshot("", "OK", false, true), ShellAccountingViewState::Unavailable, "implemented=false maps to Unavailable");
    expectTrue(status.unavailable(), "implemented=false exposes unavailable flag");
    expectTrue(status.issueCount() == 2, "issues remain visible");
    expectTrue(status.blockingIssueCount() == 1, "blocking issue count remains visible");
    expectTrue(status.readOnly(), "readOnly remains true");
    expectTrue(!status.writeEnabled(), "writeEnabled remains false");

    expectState(status, snapshot("POSITION_LIST_NOT_AVAILABLE", "UNAVAILABLE", false, false), ShellAccountingViewState::Unavailable, "position.list guard maps to Unavailable");
    expectState(status, snapshot("CASH_SUMMARY_NOT_AVAILABLE", "UNAVAILABLE", false, false), ShellAccountingViewState::Unavailable, "cash.summary guard maps to Unavailable");
    expectState(status, snapshot("PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE", "UNAVAILABLE", false, false), ShellAccountingViewState::Unavailable, "portfolio.pnl.summary guard maps to Unavailable");
    expectState(status, snapshot("BASE_POSITION_SUMMARY_NOT_AVAILABLE", "UNAVAILABLE", false, false), ShellAccountingViewState::Unavailable, "base_position.summary guard maps to Unavailable");
    expectState(status, snapshot("SNIPER_POOL_SUMMARY_NOT_AVAILABLE", "UNAVAILABLE", false, false), ShellAccountingViewState::Unavailable, "sniper_pool.summary guard maps to Unavailable");

    expectState(status, snapshot("", "OK", true, true), ShellAccountingViewState::Loaded, "OK with rows maps to Loaded");
    expectState(status, snapshot("", "OK", true, false), ShellAccountingViewState::Empty, "OK without rows maps to Empty");
    expectTrue(status.empty(), "empty flag is visible");
    expectTrue(!status.unavailable(), "Empty is not Unavailable");

    expectState(status, snapshot("", "WARNING", true, true), ShellAccountingViewState::Warning, "Warning state is visible");
    expectTrue(status.warning(), "warning flag is visible");
    expectState(status, snapshot("", "ERROR", true, true), ShellAccountingViewState::Error, "Error state is visible");
    expectTrue(status.error(), "error flag is visible");
    expectState(status, snapshot("", "OK", true, true, true), ShellAccountingViewState::Stale, "Stale state is visible");
    expectTrue(status.stale(), "stale flag is visible");
    expectState(status, snapshot("", "LOADING", true, false), ShellAccountingViewState::Loading, "Loading state is visible");
    expectTrue(status.loading(), "loading flag is visible");

    return 0;
}
