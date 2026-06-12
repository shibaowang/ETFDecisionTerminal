#pragma once

#include "ShellServices/ShellAccountingServiceTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace etfdt::shell_services {

struct ShellAccountingDataServiceClientRequest final {
    std::string actionName;
    std::string payloadJson;
    int timeoutMs = 0;
};

struct ShellAccountingDataServiceClientResponse final {
    std::string actionName;
    bool protocolSuccess = true;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
    std::string payloadStatus;
    std::string dataQualityStatus = "UNAVAILABLE";
    bool hasRows = false;
    std::vector<ShellAccountingIssue> issues;
    std::vector<ShellAccountingIssue> warnings;
    std::vector<ShellAccountingIssue> errors;
    std::string rawPayload;
    bool timeout = false;
    bool transportError = false;
    bool protocolError = false;
    bool domainError = false;
    bool transactionCommitted = false;
    bool duplicateImportPrevented = false;
    bool idempotencyConflictRejected = false;
    bool idempotencyRequired = false;
    std::string errorMessage;
    bool importPreviewAccepted = false;
    bool importPreviewRejected = false;
    std::vector<ShellAccountingImportPreviewDiagnostic> importPreviewDiagnostics;
    std::vector<std::string> importPreviewDiagnosticCodes;
    ShellAccountingImportPreviewFactSummary importPreviewFactSummary;
    bool portfolioReplayAccepted = false;
    bool portfolioReplayExecuted = false;
    int portfolioReplayPositionCount = 0;
    int portfolioReplayCashSummaryCount = 0;
    std::string portfolioReplayRealizedPnlText;
    std::string portfolioReplayUnrealizedPnlText;
    std::string portfolioReplayTotalFeeText;
    std::string portfolioReplayTotalMarketValueText;
    std::vector<std::string> portfolioReplayIssueCodes;
    bool strategyRecommendationAccepted = false;
    bool strategyRecommendationComputed = false;
    std::string strategyRecommendationActionCode;
    std::string strategyRecommendationActionLabel;
    std::string strategyRecommendationSourceCode;
    std::string strategyRecommendationSourceLabel;
    std::string strategyRecommendationReasonCode;
    std::string strategyRecommendationReasonLabel;
    std::string strategyRecommendationTierLabel;
    std::string strategyRecommendationTargetAmountText;
    std::string strategyRecommendationSuggestedQuantityText;
    std::string strategyRecommendationSuggestedAmountText;
    std::string strategyRecommendationNetCashImpactText;
    std::string strategyRecommendationFeeText;
    bool strategyRecommendationBaseProtectionPassed = false;
    bool strategyRecommendationCashLimitApplied = false;
    std::vector<std::string> strategyRecommendationIssueCodes;
    bool marketDataAccepted = false;
    bool marketDataRefreshEngineCreated = false;
    bool marketDataProviderContractCreated = false;
    bool marketDataProviderDisabled = false;
    bool marketDataStale = false;
    bool marketDataPartial = false;
    bool marketDataLiveProviderDisabledByDefault = true;
    bool marketDataLiveProviderDeferredForSafety = true;
    std::string marketDataInstrumentCode;
    std::string marketDataInstrumentType;
    std::string marketDataCurrentPriceText;
    std::string marketDataPreviousCloseText;
    std::string marketDataHistoricalHighText;
    std::string marketDataDisplayedHighText;
    std::string marketDataHistoricalHighDate;
    std::string marketDataDrawdownFromHighText;
    std::string marketDataPremiumDiscountText;
    std::string marketDataProviderSource;
    std::vector<std::string> marketDataIssueCodes;
    bool tradeDraftManualRecommendationFlowCreated = false;
    bool tradeDraftUserConfirmationRequired = false;
    bool tradeDraftEligible = false;
    bool tradeDraftDuplicate = false;
    bool tradeDraftIdempotencyConflict = false;
    bool tradeDraftIsNotOrder = true;
    bool tradeDraftSummaryFound = false;
    std::int64_t tradeDraftId = 0;
    std::string tradeDraftStatus;
    std::string tradeDraftSide;
    std::string tradeDraftInstrumentCode;
    std::string tradeDraftQuantityText;
    std::string tradeDraftAmountText;
    std::string tradeDraftNetCashImpactText;
    std::string tradeDraftSummary;
    std::vector<std::string> tradeDraftIssueCodes;
    bool otcMapPreviewAccepted = false;
    bool otcMapPreviewExecuted = false;
    int otcMapPreviewLegCount = 0;
    std::string otcMapPreviewStatus;
    std::string otcMapPreviewTotalAmountText;
    std::string otcMapPreviewSummary;
    std::vector<std::string> otcMapPreviewIssueCodes;
    bool otcMapDraftDuplicate = false;
    bool otcMapDraftIdempotencyConflict = false;
    std::int64_t otcMapDraftId = 0;
    int otcMapDraftLegCount = 0;
    std::string otcMapDraftStatus;
    std::string otcMapDraftSummary;
    std::vector<std::string> otcMapDraftIssueCodes;
    bool accountingEngineCalled = false;
    bool brokerOrderSubmitted = false;
    bool productionFileLoading = false;
    bool productionWrite = false;
    bool sqliteProductionWrite = false;
    bool auditWritten = false;
    bool ledgerWritten = false;
    bool snapshotWritten = false;
    bool tradeLogWritten = false;
    bool readModelPersistentWrite = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool automaticTrading = false;
    bool rawUserDataExposed = false;
};

class ShellAccountingDataServiceClientPort {
public:
    virtual ~ShellAccountingDataServiceClientPort() = default;

    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callPositionList(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callCashSummary(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callPortfolioPnlSummary(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callBasePositionSummary(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callSniperPoolSummary(
        const ShellAccountingDataServiceClientRequest& request) = 0;
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callTradeDraftCreate(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callTradeDraftCreateFromRecommendation(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callTradeDraftReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callTradeDraftConfirm(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callManualTransactionCreate(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callManualCashMovementCreate(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callExcelVbaImportReadOnlyPreview(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callExcelVbaImportPersistManualEntry(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callPortfolioReplayReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callStrategyRecommendationReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callMarketDataRefreshReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callMarketDataHistoricalHighReadOnlySummary(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callOtcMapMultiChannelReadOnlyPreview(
        const ShellAccountingDataServiceClientRequest& request);
    [[nodiscard]] virtual ShellAccountingDataServiceClientResponse callTradeDraftCreateOtcMapMultiChannel(
        const ShellAccountingDataServiceClientRequest& request);
};

}  // namespace etfdt::shell_services
