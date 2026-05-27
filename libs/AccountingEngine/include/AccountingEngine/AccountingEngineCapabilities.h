#pragma once

namespace etfdt::accounting {

struct AccountingEngineCapabilities {
    bool replayImplemented = false;
    bool emptyLedgerReplaySupported = true;
    bool singleBuyReplaySupported = true;
    bool buySellPartialReplaySupported = true;
    bool sellExceedsPositionDetectionSupported = true;
    bool missingFeeDetectionSupported = true;
    bool negativeCashDetectionSupported = true;
    bool multiInstrumentBuyReplaySupported = true;
    bool multiAccountBuyReplaySupported = true;
    bool multiCurrencyUnsupportedDetectionSupported = true;
    bool missingMarketPriceDetectionSupported = true;
    bool readOnlyReplaySupported = false;
    bool writeEnabled = false;
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
