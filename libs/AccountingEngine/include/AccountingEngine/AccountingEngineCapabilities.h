#pragma once

namespace etfdt::accounting {

struct AccountingEngineCapabilities {
    bool replayImplemented = false;
    bool readOnlyReplaySupported = false;
    bool snapshotWriteEnabled = false;
    bool tradeLogWriteEnabled = false;
    bool dataServiceActionImplemented = false;
    bool fixtureReplayAvailableInProduction = false;
    bool supportsPositionList = false;
    bool supportsCashSummary = false;
    bool supportsPortfolioPnl = false;
    bool supportsBasePosition = false;
    bool supportsSniperPool = false;
    bool supportsMarketPrice = false;
    bool supportsFxRate = false;
    bool supportsMultiCurrency = false;
    bool productionReady = false;
};

[[nodiscard]] AccountingEngineCapabilities accountingEngineCapabilities();

} // namespace etfdt::accounting
