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

void expectState(
    etfdt::shell_services::ShellAccountingViewState actual,
    etfdt::shell_services::ShellAccountingViewState expected,
    std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected "
                  << etfdt::shell_services::toString(expected) << " actual "
                  << etfdt::shell_services::toString(actual) << '\n';
    }
}

etfdt::shell_services::ShellAccountingStateSnapshot snapshot(
    std::string status,
    bool implemented,
    bool hasRows,
    std::string payloadStatus = {})
{
    etfdt::shell_services::ShellAccountingStateSnapshot value;
    value.actionName = "position.list";
    value.implemented = implemented;
    value.readOnly = true;
    value.writeEnabled = false;
    value.dataQualityStatus = std::move(status);
    value.payloadStatus = std::move(payloadStatus);
    value.hasRows = hasRows;
    return value;
}

}  // namespace

int main()
{
    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    expectTrue(controller.readOnly(), "controller defaults readOnly=true");
    expectTrue(!controller.writeEnabled(), "controller defaults writeEnabled=false");
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Idle, "controller initial state is Idle");

    controller.beginRefresh("position.list");
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Loading, "beginRefresh sets Loading");

    controller.applyStateSnapshot(snapshot("OK", false, true));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Unavailable, "implemented=false maps to Unavailable");

    controller.applyStateSnapshot(snapshot("OK", true, true, "POSITION_LIST_NOT_AVAILABLE"));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Unavailable, "POSITION_LIST_NOT_AVAILABLE maps to Unavailable");
    controller.applyStateSnapshot(snapshot("OK", true, true, "CASH_SUMMARY_NOT_AVAILABLE"));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Unavailable, "CASH_SUMMARY_NOT_AVAILABLE maps to Unavailable");
    controller.applyStateSnapshot(snapshot("OK", true, true, "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE"));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Unavailable, "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE maps to Unavailable");
    controller.applyStateSnapshot(snapshot("OK", true, true, "BASE_POSITION_SUMMARY_NOT_AVAILABLE"));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Unavailable, "BASE_POSITION_SUMMARY_NOT_AVAILABLE maps to Unavailable");
    controller.applyStateSnapshot(snapshot("OK", true, true, "SNIPER_POOL_SUMMARY_NOT_AVAILABLE"));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Unavailable, "SNIPER_POOL_SUMMARY_NOT_AVAILABLE maps to Unavailable");

    controller.applyStateSnapshot(snapshot("OK", true, true));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Loaded, "OK with rows maps to Loaded");
    controller.applyStateSnapshot(snapshot("OK", true, false));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Empty, "OK without rows maps to Empty");
    controller.applyStateSnapshot(snapshot("WARNING", true, true));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Warning, "WARNING maps to Warning");
    controller.applyStateSnapshot(snapshot("DEGRADED", true, true));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Warning, "DEGRADED maps to Warning");
    controller.applyStateSnapshot(snapshot("ERROR", true, false));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Error, "ERROR maps to Error");

    auto stale = snapshot("OK", true, true);
    stale.stale = true;
    controller.applyStateSnapshot(std::move(stale));
    expectState(controller.currentState(), etfdt::shell_services::ShellAccountingViewState::Stale, "stale snapshot maps to Stale");

    controller.applyStateSnapshot(snapshot("OK", true, false));
    expectTrue(controller.currentState() != etfdt::shell_services::ShellAccountingViewState::Unavailable, "Empty and Unavailable are distinct");

    return gFailures == 0 ? 0 : 1;
}
