#include "ShellAccountingControllerScenario.h"
#include "ShellAccountingStateMappingTestUtils.h"

#include <algorithm>
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
    const auto boundaries = etfdt::tests::shell_accounting::noTradeActionBoundaries();
    expectTrue(boundaries.size() >= 5, "no trade action boundaries cover accounting surfaces");

    bool sawBasePositionSellable = false;
    bool sawSniperPoolRemaining = false;
    bool sawSniperPoolCompleted = false;
    bool sawControllerRefresh = false;

    for (const auto& boundary : boundaries) {
        expectTrue(etfdt::tests::shell_accounting::hidesTradeActions(boundary), "boundary hides trade actions");
        expectTrue(!boundary.generatesTradeDraft, "boundary does not generate TradeDraft");
        expectTrue(!boundary.generatesTradeSuggestion, "boundary does not generate trade suggestion");
        expectTrue(!boundary.executesStrategy, "boundary does not execute strategy");
        expectTrue(!boundary.sendsBrokerOrder, "boundary does not send broker order");
        expectTrue(!boundary.qmlCalculates, "boundary is not calculated by QML");

        if (boundary.fieldName == "sellableAboveBaseAmountText") {
            sawBasePositionSellable = true;
        }
        if (boundary.fieldName == "remainingAmountText") {
            sawSniperPoolRemaining = true;
        }
        if (boundary.fieldName == "completed") {
            sawSniperPoolCompleted = true;
        }
        if (boundary.fieldName == "refreshPositionList") {
            sawControllerRefresh = true;
        }
    }

    expectTrue(sawBasePositionSellable, "basePosition sellableAboveBaseAmountText boundary exists");
    expectTrue(sawSniperPoolRemaining, "sniperPool remainingAmountText boundary exists");
    expectTrue(sawSniperPoolCompleted, "sniperPool completed QML boundary exists");
    expectTrue(sawControllerRefresh, "controller refresh read-only boundary exists");

    const auto protectedTables = etfdt::tests::shell_accounting::protectedAccountingTables();
    expectTrue(!protectedTables.empty(), "protected accounting table list exists");
    expectTrue(etfdt::tests::shell_accounting::protectedAccountingTables().size() == 7, "protected accounting table list is fixed");
    expectTrue(std::find(protectedTables.begin(), protectedTables.end(), "trade_log") != protectedTables.end(), "protected tables include trade_log");
    expectTrue(std::find(protectedTables.begin(), protectedTables.end(), "trade_draft") != protectedTables.end(), "protected tables include trade_draft");
    expectTrue(std::find(protectedTables.begin(), protectedTables.end(), "cash_snapshot") != protectedTables.end(), "protected tables include cash_snapshot");
    expectTrue(std::find(protectedTables.begin(), protectedTables.end(), "position_snapshot") != protectedTables.end(), "protected tables include position_snapshot");
    expectTrue(std::find(protectedTables.begin(), protectedTables.end(), "portfolio_summary") != protectedTables.end(), "protected tables include portfolio_summary");
    expectTrue(std::find(protectedTables.begin(), protectedTables.end(), "audit_log") != protectedTables.end(), "protected tables include audit_log");

    return gFailures == 0 ? 0 : 1;
}
