#pragma once

#include "Protocol/Protocol.h"

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace etfdt::data_service_client {

struct DataServiceClientError final {
    std::optional<etfdt::protocol::ErrorCode> errorCode;
    std::string message;
};

template <typename T>
class DataServiceClientResult final {
public:
    static DataServiceClientResult success(T value)
    {
        return DataServiceClientResult(std::move(value));
    }

    static DataServiceClientResult failure(
        std::optional<etfdt::protocol::ErrorCode> errorCode,
        std::string message)
    {
        return DataServiceClientResult(DataServiceClientError{errorCode, std::move(message)});
    }

    [[nodiscard]] bool hasValue() const noexcept
    {
        return hasValue_;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return hasValue();
    }

    [[nodiscard]] const T& value() const
    {
        return value_;
    }

    [[nodiscard]] T& value()
    {
        return value_;
    }

    [[nodiscard]] const DataServiceClientError& error() const noexcept
    {
        return error_;
    }

private:
    explicit DataServiceClientResult(T value)
        : hasValue_(true)
        , value_(std::move(value))
    {
    }

    explicit DataServiceClientResult(DataServiceClientError error)
        : hasValue_(false)
        , error_(std::move(error))
    {
    }

    bool hasValue_ = false;
    T value_{};
    DataServiceClientError error_;
};

struct AuditAppendRequest final {
    std::string entityType;
    std::string entityId;
    std::string action;
    std::string reason;
    std::string operatorName = "local-user";
    std::string oldValueJson = "{}";
    std::string newValueJson = "{}";
};

struct ExcelVbaImportReadOnlyPreviewDiagnostic final {
    std::string level;
    std::string code;
    std::string field;
    std::string sheetName;
    std::string rowId;
};

struct ExcelVbaImportReadOnlyPreviewFactSummary final {
    int tradeFactCount = 0;
    int cashFactCount = 0;
    int marketPriceFactCount = 0;
    int fxRateFactCount = 0;
};

struct ExcelVbaImportReadOnlyPreviewResult final {
    std::string action;
    std::string task;
    std::string mode;
    bool dataServiceReadOnlyActionRegistered = false;
    bool parserBoundaryCalled = false;
    bool previewActionExecuted = false;
    bool diagnosticsBuilt = false;
    bool replayFactSummaryBuilt = false;
    bool accountingEngineCalled = false;
    bool productionRuntimeIntegrationImplemented = false;
    bool accepted = false;
    std::vector<ExcelVbaImportReadOnlyPreviewDiagnostic> diagnostics;
    std::vector<std::string> diagnosticCodes;
    ExcelVbaImportReadOnlyPreviewFactSummary replayFactSummary;
    bool parserReadOnly = false;
    bool parserNoSqliteAccess = false;
    bool parserNoWrite = false;
    bool productionWrite = false;
    bool sqliteProductionWrite = false;
    bool auditWritten = false;
    bool ledgerWritten = false;
    bool snapshotWritten = false;
    bool tradeLogWritten = false;
    bool readModelPersistentWrite = false;
    bool tradeDraftGenerated = false;
    bool strategyExecuted = false;
    bool brokerOrderSubmitted = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool automaticTrading = false;
    bool rawUserDataExposed = false;
};

struct ExcelVbaImportPersistManualEntryFactSummary final {
    int tradeFactCount = 0;
    int cashFactCount = 0;
    int marketPriceFactCount = 0;
    int fxRateFactCount = 0;
};

struct ExcelVbaImportPersistManualEntryRequest final {
    std::string previewStatus;
    std::string previewDigest;
    std::string idempotencyKey;
    std::string schemaVersion;
    std::string source;
    std::string acceptedAt;
    std::string importBatchLabel;
    std::string requestId;
    ExcelVbaImportPersistManualEntryFactSummary factSummary;
    std::string sanitizedImportPayloadJson = "{}";
};

struct ExcelVbaImportPersistManualEntryResult final {
    std::string action;
    std::string task;
    std::string status;
    std::string previewDigest;
    std::string idempotencyKey;
    bool protocolSuccess = false;
    bool dataServicePersistActionRegistered = false;
    bool acceptedPreviewRequired = false;
    bool parserBoundaryReused = false;
    bool manualTransactionRepositoryUsed = false;
    bool directTradeLogSqlInDataService = false;
    bool tempDbOnly = false;
    bool productionDbTouched = false;
    bool transactionCommitted = false;
    bool tradeLogWritten = false;
    bool auditLogWritten = false;
    bool idempotencyRequired = false;
    bool duplicateImportPrevented = false;
    bool idempotencyConflictRejected = false;
    bool nonAcceptedPreviewRejected = false;
    bool supplementalDataAccessTransactionAuthorizationUsed = false;
    bool manualTransactionActiveTransactionMethodCreated = false;
    bool existingManualTransactionApiPreserved = false;
    bool dataAccessCompositionRepositoryCreated = false;
    bool singleTransactionBoundaryUsed = false;
    bool manualFactsAndAuditCommittedTogether = false;
    bool auditFailureRollsBackManualFacts = false;
    bool nestedTransactionAttempted = false;
    bool qmlWiringChanged = false;
    bool importButtonAdded = false;
    bool accountingEngineCalled = false;
    bool brokerOrderSubmitted = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool automaticTrading = false;
    int tradeLogRowsWritten = 0;
    std::int64_t auditLogId = 0;
    std::vector<std::string> issues;
};

struct PortfolioReplayReadOnlySummaryRequest final {
    std::string replayPayloadJson = "{}";
};

struct StrategyRecommendationReadOnlySummaryRequest final {
    std::string recommendationPayloadJson = "{}";
};

struct TradeDraftCreateFromRecommendationRequest final {
    std::string recommendationPayloadJson = "{}";
    std::string idempotencyKey;
    std::string recommendationDigest;
    std::string accountId;
    std::string portfolioId;
    std::string strategyId;
    std::string instrumentId;
    std::string strategyCode;
    std::string instrumentCode;
    std::string instrumentType;
    std::string tradeSource;
    std::string expiresAtUtc;
    std::string userNote;
    bool userConfirmed = false;
};

struct TradeDraftCreateFromRecommendationResult final {
    std::string action;
    std::string task;
    std::string mode;
    std::string status;
    bool protocolSuccess = false;
    bool dataServiceWriteActionCreated = false;
    bool tradeDraftManualRecommendationFlowCreated = false;
    bool tradeDraftRequiresExplicitUserConfirmation = false;
    bool explicitUserConfirmationRequired = false;
    bool recommendationEngineCalled = false;
    bool recommendationAccepted = false;
    bool tradeDraftEligible = false;
    bool conversionNoWrite = false;
    bool draftWritten = false;
    bool legWritten = false;
    bool auditWritten = false;
    bool transactionCommitted = false;
    bool duplicateDraft = false;
    bool idempotencyConflict = false;
    std::int64_t draftId = 0;
    std::int64_t auditLogId = 0;
    std::string draftUid;
    std::string draftSignature;
    std::string side;
    std::string instrumentCode;
    std::string quantityText;
    std::string amountText;
    std::string netCashImpactText;
    std::vector<std::string> issueCodes;
    bool tradeDraftIsNotOrder = false;
    bool multiChannelOtcLegsCreated = false;
    bool tradeLogRowsWrittenByTradeDraft = false;
    bool cashAdjustmentRowsWrittenByTradeDraft = false;
    bool positionChangedByTradeDraft = false;
    bool cashChangedByTradeDraft = false;
    bool productionWrite = false;
    bool sqliteProductionWrite = false;
    bool brokerOrderSubmitted = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool automaticTrading = false;
};

struct TradeDraftReadOnlySummaryRequest final {
    std::string idempotencyKey;
    std::int64_t draftId = 0;
};

struct TradeDraftReadOnlySummaryResult final {
    std::string action;
    std::string task;
    std::string mode;
    bool protocolSuccess = false;
    bool readOnly = false;
    bool found = false;
    std::int64_t draftId = 0;
    std::string draftUid;
    std::string draftSignature;
    std::string status;
    std::string side;
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string strategyCode;
    std::string quantityText;
    std::string amountText;
    std::string netCashImpactText;
    std::string idempotencyKey;
    bool productionWrite = false;
    bool tradeLogRowsWrittenByTradeDraft = false;
    bool cashAdjustmentRowsWrittenByTradeDraft = false;
    bool brokerOrderSubmitted = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool automaticTrading = false;
};

struct StrategyRecommendationIssue final {
    std::string level;
    std::string code;
    std::string message;
    std::string field;
    bool blocking = true;
};

struct StrategyRecommendationReadOnlySummaryResult final {
    std::string action;
    std::string task;
    std::string mode;
    std::string status;
    std::string dataQualityStatus;
    bool protocolSuccess = false;
    bool accepted = false;
    bool recommendationComputed = false;
    bool strategyRecommendationEngineCreated = false;
    bool dataServiceReadOnlyActionCreated = false;
    std::string actionCode;
    std::string actionLabel;
    std::string sourceCode;
    std::string sourceLabel;
    std::string reasonCode;
    std::string reasonLabel;
    std::string tierLabel;
    std::string targetAmountText;
    std::string suggestedQuantityText;
    std::string suggestedAmountText;
    std::string netCashImpactText;
    std::string feeText;
    bool baseProtectionPassed = false;
    bool cashLimitApplied = false;
    std::vector<StrategyRecommendationIssue> issues;
    std::vector<std::string> issueCodes;
    std::string rawPayloadJson;
    bool readOnlyRecommendationNoWrite = false;
    bool tradeDraftCreated = false;
    bool tradeLogRowsWrittenByRecommendation = false;
    bool cashAdjustmentRowsWrittenByRecommendation = false;
    bool auditLogRowsWrittenByRecommendation = false;
    bool productionWrite = false;
    bool sqliteProductionWrite = false;
    bool auditWritten = false;
    bool ledgerWritten = false;
    bool snapshotWritten = false;
    bool tradeLogWritten = false;
    bool readModelPersistentWrite = false;
    bool productionDbTouched = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool brokerOrderSubmitted = false;
    bool automaticTrading = false;
};

struct PortfolioReplayReadOnlyPnlSummary final {
    std::string currency;
    std::string realizedPnlText;
    std::string unrealizedPnlText;
    std::string totalFeeText;
    std::string totalMarketValueText;
    std::string dataQualityStatus;
};

struct PortfolioReplayReadOnlySummaryResult final {
    std::string action;
    std::string task;
    std::string mode;
    std::string status;
    std::string dataQualityStatus;
    bool protocolSuccess = false;
    bool accepted = false;
    bool replayExecuted = false;
    bool portfolioReplayEngineCreated = false;
    bool dataServiceReadOnlyActionCreated = false;
    bool accountingEngineCalled = false;
    int positionCount = 0;
    int cashSummaryCount = 0;
    PortfolioReplayReadOnlyPnlSummary pnlSummary;
    std::vector<std::string> issueCodes;
    std::string rawPayloadJson;
    bool readOnlyReplayNoWrite = false;
    bool tradeLogRowsWrittenByReplay = false;
    bool cashAdjustmentRowsWrittenByReplay = false;
    bool auditLogRowsWrittenByReplay = false;
    bool productionWrite = false;
    bool sqliteProductionWrite = false;
    bool auditWritten = false;
    bool ledgerWritten = false;
    bool snapshotWritten = false;
    bool tradeLogWritten = false;
    bool readModelPersistentWrite = false;
    bool productionDbTouched = false;
    bool networkAccess = false;
    bool credentialAccess = false;
    bool endpointAccess = false;
    bool brokerOrderSubmitted = false;
    bool automaticTrading = false;
};

}  // namespace etfdt::data_service_client
