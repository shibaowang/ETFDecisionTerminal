#include "AccountingEngine/AccountingEngine.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

bool contains(const std::vector<std::string>& values, const std::string& expected)
{
    return std::find(values.begin(), values.end(), expected) != values.end();
}

int expect(bool condition, const std::string& message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "FAILED: " << message << '\n';
    return 1;
}

} // namespace

int main()
{
    using namespace etfdt::accounting;

    int failures = 0;

    const auto info = accountingEngineInfo();
    failures += expect(info.moduleName == "AccountingEngine", "moduleName should be AccountingEngine");
    failures += expect(!info.moduleVersion.empty(), "moduleVersion should be non-empty");
    failures += expect(!info.contractVersion.empty(), "contractVersion should be non-empty");
    failures += expect(!info.productionReady, "productionReady should be false");
    failures += expect(!info.replayImplemented, "replayImplemented should be false");
    failures += expect(!info.snapshotImplemented, "snapshotImplemented should be false");
    failures += expect(!info.writeEnabled, "writeEnabled should be false");

    const auto capabilities = accountingEngineCapabilities();
    failures += expect(!capabilities.replayImplemented, "capabilities.replayImplemented should be false");
    failures += expect(capabilities.emptyLedgerReplaySupported, "emptyLedgerReplaySupported should be true");
    failures += expect(capabilities.singleBuyReplaySupported, "singleBuyReplaySupported should be true");
    failures += expect(capabilities.buySellPartialReplaySupported, "buySellPartialReplaySupported should be true");
    failures += expect(capabilities.sellExceedsPositionDetectionSupported,
                       "sellExceedsPositionDetectionSupported should be true");
    failures += expect(capabilities.missingFeeDetectionSupported, "missingFeeDetectionSupported should be true");
    failures += expect(capabilities.negativeCashDetectionSupported, "negativeCashDetectionSupported should be true");
    failures += expect(capabilities.multiInstrumentBuyReplaySupported, "multiInstrumentBuyReplaySupported should be true");
    failures += expect(capabilities.multiAccountBuyReplaySupported, "multiAccountBuyReplaySupported should be true");
    failures += expect(!capabilities.readOnlyReplaySupported, "readOnlyReplaySupported should be false");
    failures += expect(!capabilities.writeEnabled, "writeEnabled should be false");
    failures += expect(!capabilities.snapshotWriteEnabled, "snapshotWriteEnabled should be false");
    failures += expect(!capabilities.tradeLogWriteEnabled, "tradeLogWriteEnabled should be false");
    failures += expect(!capabilities.dataServiceActionImplemented, "dataServiceActionImplemented should be false");
    failures += expect(!capabilities.fixtureReplayAvailableInProduction, "fixtureReplayAvailableInProduction should be false");
    failures += expect(!capabilities.supportsPositionList, "supportsPositionList should be false");
    failures += expect(!capabilities.supportsCashSummary, "supportsCashSummary should be false");
    failures += expect(!capabilities.supportsPortfolioPnl, "supportsPortfolioPnl should be false");
    failures += expect(!capabilities.supportsBasePosition, "supportsBasePosition should be false");
    failures += expect(!capabilities.supportsSniperPool, "supportsSniperPool should be false");
    failures += expect(!capabilities.supportsMarketPrice, "supportsMarketPrice should be false");
    failures += expect(!capabilities.supportsFxRate, "supportsFxRate should be false");
    failures += expect(!capabilities.supportsMultiCurrency, "supportsMultiCurrency should be false");
    failures += expect(!capabilities.productionReady, "capabilities.productionReady should be false");

    const auto boundary = accountingEngineBoundary();
    failures += expect(contains(boundary.forbiddenDependencies, "DataAccess"), "forbiddenDependencies should include DataAccess");
    failures += expect(contains(boundary.forbiddenDependencies, "DataServiceApi"), "forbiddenDependencies should include DataServiceApi");
    failures += expect(contains(boundary.forbiddenDependencies, "ServiceHost"), "forbiddenDependencies should include ServiceHost");
    failures += expect(contains(boundary.forbiddenDependencies, "Watchdog"), "forbiddenDependencies should include Watchdog");
    failures += expect(contains(boundary.forbiddenDependencies, "tests/AccountingFixtures"), "forbiddenDependencies should include tests/AccountingFixtures");
    failures += expect(contains(boundary.forbiddenDependencies, "SQLiteConnection"), "forbiddenDependencies should include SQLiteConnection");
    failures += expect(contains(boundary.forbiddenWrites, "trade_log"), "forbiddenWrites should include trade_log");
    failures += expect(contains(boundary.forbiddenWrites, "position_snapshot"), "forbiddenWrites should include position_snapshot");
    failures += expect(contains(boundary.forbiddenWrites, "portfolio_summary"), "forbiddenWrites should include portfolio_summary");
    failures += expect(contains(boundary.forbiddenRuntimeActions, "replay"), "forbiddenRuntimeActions should include replay");
    failures += expect(contains(boundary.forbiddenRuntimeActions, "broker_order"), "forbiddenRuntimeActions should include broker_order");
    failures += expect(contains(boundary.testOnlySourcesNotAllowed, "AccountingReplayMinimalEngine"),
                       "testOnlySourcesNotAllowed should include AccountingReplayMinimalEngine");

    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
