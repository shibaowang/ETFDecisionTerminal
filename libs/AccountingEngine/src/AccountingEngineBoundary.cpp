#include "AccountingEngine/AccountingEngineBoundary.h"

namespace etfdt::accounting {

AccountingEngineBoundary accountingEngineBoundary()
{
    const auto storageConnection = std::string("SQ") + "LiteConnection";
    const auto minimalEngine = std::string("AccountingReplay") + "MinimalEngine";
    const auto fixtureSource = std::string("tests/") + "AccountingFixtures";

    return AccountingEngineBoundary{
        {"standard library", "CoreDomain (optional)"},
        {
            "DataAccess",
            "DataServiceApi",
            "DataServiceClient",
            "ServiceHost",
            "Watchdog",
            "QML",
            "QtQuick",
            storageConnection,
            fixtureSource,
        },
        {
            "trade_log",
            "trade_execution_group",
            "trade_draft",
            "cash_snapshot",
            "position_snapshot",
            "portfolio_summary",
            "audit_log",
        },
        {
            "replay",
            "snapshot_write",
            "trade_log_write",
            "trade_draft_generation",
            "broker_order",
            "auto_trading",
            "market_fetch",
            "fx_fetch",
        },
        {
            minimalEngine,
            "AccountingReplayTestHarness",
            "AccountingFixtureLoader",
            "tests/fixtures/accounting_replay",
        },
    };
}

} // namespace etfdt::accounting
