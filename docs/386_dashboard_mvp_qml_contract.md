# Dashboard MVP QML Contract

The Dashboard MVP is implemented on `ShellAccountingReadOnlyPage.qml` and uses
Presenter-exposed properties and methods only.

## Required Object Names

General:

- `shellAccountingDashboardRoot`
- `shellAccountingDashboardStatusBanner`
- `shellAccountingDashboardIssueList`
- `shellAccountingDashboardResetButton`

Import:

- `shellAccountingExcelVbaImportPanel`
- `shellAccountingExcelVbaLoadSampleButton`
- `shellAccountingExcelVbaPreviewButton`
- `shellAccountingExcelVbaPreviewStatusText`
- `shellAccountingExcelVbaPersistConfirmationCheckBox`
- `shellAccountingExcelVbaPersistButton`
- `shellAccountingExcelVbaPersistRowCountText`
- `shellAccountingExcelVbaPostWriteRefreshStatusText`

Replay:

- `shellAccountingPortfolioReplayPanel`
- `shellAccountingPortfolioReplayButton`
- `shellAccountingPortfolioReplayStatusText`
- `shellAccountingPortfolioReplayCashText`
- `shellAccountingPortfolioReplayPositionText`
- `shellAccountingPortfolioReplayPnlText`

Market data:

- `shellAccountingMarketDataPanel`
- `shellAccountingMarketDataRefreshButton`
- `shellAccountingMarketDataProviderStatusText`
- `shellAccountingMarketDataCurrentPriceText`
- `shellAccountingMarketDataHistoricalHighText`
- `shellAccountingMarketDataDrawdownText`
- `shellAccountingMarketDataPremiumDiscountText`
- `shellAccountingMarketDataIssueText`

Strategy:

- `shellAccountingStrategyRecommendationPanel`
- `shellAccountingStrategyRecommendationButton`
- `shellAccountingStrategyRecommendationStatusText`
- `shellAccountingStrategyRecommendationActionText`
- `shellAccountingStrategyRecommendationReasonText`
- `shellAccountingStrategyRecommendationQuantityText`
- `shellAccountingStrategyRecommendationAmountText`

TradeDraft:

- `shellAccountingTradeDraftPanel`
- `shellAccountingTradeDraftPreviewButton`
- `shellAccountingTradeDraftConfirmationCheckBox`
- `shellAccountingTradeDraftCreateButton`
- `shellAccountingTradeDraftStatusText`
- `shellAccountingTradeDraftSummaryText`

OTCMap:

- `shellAccountingOtcMapPanel`
- `shellAccountingOtcMapPreviewButton`
- `shellAccountingOtcMapConfirmationCheckBox`
- `shellAccountingOtcMapCreateDraftButton`
- `shellAccountingOtcMapLegCountText`
- `shellAccountingOtcMapSummaryText`
- `shellAccountingOtcMapIssueText`

## Forbidden QML Tokens

Dashboard QML must not contain direct access tokens for network, database,
service-client, engine, external execution, secret, or order-placement paths:

- `XMLHttpRequest`
- `fetch(`
- `WebSocket`
- `DataServiceClient`
- `SQLite`
- `AccountingEngine`
- `StrategyEngine`
- `MarketEngine`
- `broker`
- `credential`
- `endpoint`
- `access_token`
- `cookie`
- `placeOrder`
- `submitOrder`
- `autoTrade`
- `automaticTrading`
