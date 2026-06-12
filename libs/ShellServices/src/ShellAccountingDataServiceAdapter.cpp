#include "ShellServices/ShellAccountingDataServiceAdapter.h"

#include <sstream>
#include <utility>

namespace etfdt::shell_services {

namespace {

constexpr const char* kAdapterNotConnectedStatus =
    "SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED";
constexpr const char* kDataQualityUnavailable = "UNAVAILABLE";
constexpr const char* kIssueLevelError = "ERROR";
constexpr const char* kIssueSource = "ShellAccountingDataServiceAdapter";

ShellAccountingIssue makeAdapterNotConnectedIssue()
{
    return {
        kAdapterNotConnectedStatus,
        kIssueLevelError,
        "Shell accounting DataService adapter is a skeleton and has no live client connected.",
        true,
        kIssueSource,
    };
}

ShellAccountingServiceResult makeNotConnectedResult(
    const ShellAccountingServiceRequest& request,
    const char* fallbackActionName)
{
    ShellAccountingServiceResult result;
    result.actionName = request.actionName.empty() ? fallbackActionName : request.actionName;
    result.protocolSuccess = true;
    result.implemented = false;
    result.readOnly = true;
    result.writeEnabled = false;
    result.payloadStatus = kAdapterNotConnectedStatus;
    result.dataQualityStatus = kDataQualityUnavailable;
    result.domainError = true;
    result.issues.push_back(makeAdapterNotConnectedIssue());
    result.generatedTradeDraft = false;
    result.generatedTradeSuggestion = false;
    result.strategyExecuted = false;
    result.brokerOrderSubmitted = false;
    return result;
}

std::string escapeJsonString(const std::string& value)
{
    std::string escaped;
    escaped.reserve(value.size());
    for (const auto ch : value) {
        if (ch == '\\' || ch == '"') {
            escaped.push_back('\\');
        }
        escaped.push_back(ch);
    }
    return escaped;
}

void appendJsonStringField(
    std::ostringstream& stream,
    const char* name,
    const std::string& value,
    bool& needsComma)
{
    if (needsComma) {
        stream << ',';
    }
    stream << '"' << name << "\":\"" << escapeJsonString(value) << '"';
    needsComma = true;
}

void appendJsonBoolField(
    std::ostringstream& stream,
    const char* name,
    bool value,
    bool& needsComma)
{
    if (needsComma) {
        stream << ',';
    }
    stream << '"' << name << "\":" << (value ? "true" : "false");
    needsComma = true;
}

void appendJsonIntField(
    std::ostringstream& stream,
    const char* name,
    int value,
    bool& needsComma)
{
    if (needsComma) {
        stream << ',';
    }
    stream << '"' << name << "\":" << value;
    needsComma = true;
}

void appendJsonRawField(
    std::ostringstream& stream,
    const char* name,
    const std::string& rawJson,
    bool& needsComma)
{
    if (needsComma) {
        stream << ',';
    }
    stream << '"' << name << "\":" << (rawJson.empty() ? "{}" : rawJson);
    needsComma = true;
}

void appendJsonInt64Field(
    std::ostringstream& stream,
    const char* name,
    std::int64_t value,
    bool& needsComma)
{
    if (needsComma) {
        stream << ',';
    }
    stream << '"' << name << "\":" << value;
    needsComma = true;
}

void appendRiskFlags(
    std::ostringstream& stream,
    bool checked,
    bool blocked,
    bool& needsComma)
{
    if (needsComma) {
        stream << ',';
    }
    stream << "\"riskFlags\":{\"checked\":" << (checked ? "true" : "false")
           << ",\"blocked\":" << (blocked ? "true" : "false") << '}';
    needsComma = true;
}

void appendRequestedOutputs(
    std::ostringstream& stream,
    const std::vector<std::string>& requestedOutputs,
    bool& needsComma)
{
    if (needsComma) {
        stream << ',';
    }
    stream << "\"requestedOutputs\":[";
    for (std::size_t index = 0; index < requestedOutputs.size(); ++index) {
        if (index > 0) {
            stream << ',';
        }
        stream << '"' << escapeJsonString(requestedOutputs[index]) << '"';
    }
    stream << ']';
    needsComma = true;
}

std::string makePayloadJson(
    const ShellAccountingServiceRequest& request,
    const char* fallbackActionName)
{
    std::ostringstream stream;
    bool needsComma = false;
    stream << '{';
    appendJsonStringField(
        stream,
        "actionName",
        request.actionName.empty() ? fallbackActionName : request.actionName,
        needsComma);
    appendJsonStringField(stream, "accountId", request.accountId, needsComma);
    appendJsonStringField(stream, "portfolioId", request.portfolioId, needsComma);
    appendJsonStringField(stream, "sourceFromTime", request.sourceFromTime, needsComma);
    appendJsonStringField(stream, "sourceToTime", request.sourceToTime, needsComma);
    appendJsonBoolField(stream, "includeIssues", request.includeIssues, needsComma);
    appendJsonBoolField(stream, "includeDebugMetadata", request.includeDebugMetadata, needsComma);
    appendJsonStringField(stream, "calculationVersion", request.calculationVersion, needsComma);
    appendRequestedOutputs(stream, request.requestedOutputs, needsComma);
    appendJsonIntField(stream, "timeoutMs", request.timeoutMs, needsComma);
    stream << '}';
    return stream.str();
}

std::string makeCreateDraftPayloadJson(const ShellAccountingServiceRequest& request)
{
    std::ostringstream stream;
    bool needsComma = false;
    stream << '{';
    appendJsonStringField(stream, "accountId", request.accountId, needsComma);
    appendJsonStringField(stream, "portfolioId", request.portfolioId, needsComma);
    appendJsonStringField(stream, "strategyId", request.strategyId, needsComma);
    appendJsonStringField(stream, "strategyCode", request.strategyCode, needsComma);
    appendJsonStringField(stream, "instrumentId", request.instrumentId, needsComma);
    appendJsonStringField(stream, "instrumentCode", request.instrumentCode, needsComma);
    appendJsonStringField(stream, "side", request.side, needsComma);
    appendJsonInt64Field(stream, "quantity1e6", request.quantity1e6, needsComma);
    appendJsonStringField(stream, "reason", request.reason, needsComma);
    appendJsonStringField(stream, "source", request.source, needsComma);
    appendJsonStringField(stream, "sourceSnapshotId", request.sourceSnapshotId, needsComma);
    appendJsonStringField(stream, "sourceReplayId", request.sourceReplayId, needsComma);
    appendRiskFlags(stream, request.riskChecked, request.riskBlocked, needsComma);
    appendJsonStringField(stream, "authorization", request.authorization, needsComma);
    appendJsonBoolField(stream, "tradeDraftDisabled", request.tradeDraftDisabled, needsComma);
    stream << '}';
    return stream.str();
}

std::string makeTradeDraftRecommendationPayloadJson(const ShellAccountingServiceRequest& request)
{
    std::ostringstream stream;
    bool needsComma = false;
    stream << '{';
    appendJsonStringField(stream, "recommendationPayloadJson", request.strategyRecommendationPayloadJson, needsComma);
    appendJsonStringField(stream, "idempotencyKey", request.idempotencyKey, needsComma);
    appendJsonStringField(stream, "recommendationDigest", request.recommendationDigest, needsComma);
    appendJsonStringField(stream, "accountId", request.accountId, needsComma);
    appendJsonStringField(stream, "portfolioId", request.portfolioId, needsComma);
    appendJsonStringField(stream, "strategyId", request.strategyId, needsComma);
    appendJsonStringField(stream, "instrumentId", request.instrumentId, needsComma);
    appendJsonStringField(stream, "strategyCode", request.strategyCode, needsComma);
    appendJsonStringField(stream, "instrumentCode", request.instrumentCode, needsComma);
    appendJsonStringField(stream, "instrumentType", request.instrumentType, needsComma);
    appendJsonStringField(stream, "tradeSource", request.tradeSource, needsComma);
    appendJsonStringField(stream, "expiresAtUtc", request.expiresAtUtc, needsComma);
    appendJsonBoolField(stream, "userConfirmed", request.userConfirmed, needsComma);
    stream << '}';
    return stream.str();
}

std::string makeTradeDraftSummaryPayloadJson(const ShellAccountingServiceRequest& request)
{
    std::ostringstream stream;
    bool needsComma = false;
    stream << '{';
    appendJsonStringField(stream, "idempotencyKey", request.idempotencyKey, needsComma);
    appendJsonInt64Field(stream, "draftId", request.draftId, needsComma);
    stream << '}';
    return stream.str();
}

std::string makeConfirmDraftPayloadJson(const ShellAccountingServiceRequest& request)
{
    std::ostringstream stream;
    bool needsComma = false;
    stream << '{';
    appendJsonInt64Field(stream, "draftId", request.draftId, needsComma);
    appendJsonStringField(stream, "accountId", request.accountId, needsComma);
    appendJsonStringField(stream, "portfolioId", request.portfolioId, needsComma);
    appendJsonStringField(stream, "strategyId", request.strategyId, needsComma);
    appendJsonStringField(stream, "strategyCode", request.strategyCode, needsComma);
    appendJsonStringField(stream, "instrumentId", request.instrumentId, needsComma);
    appendJsonStringField(stream, "instrumentCode", request.instrumentCode, needsComma);
    appendJsonStringField(stream, "side", request.side, needsComma);
    appendJsonInt64Field(stream, "quantity1e6", request.quantity1e6, needsComma);
    appendJsonStringField(stream, "source", request.source, needsComma);
    appendJsonStringField(stream, "sourceSnapshotId", request.sourceSnapshotId, needsComma);
    appendJsonStringField(stream, "sourceReplayId", request.sourceReplayId, needsComma);
    appendJsonStringField(stream, "authorization", request.authorization, needsComma);
    appendJsonBoolField(stream, "confirmationDisabled", request.confirmationDisabled, needsComma);
    stream << '}';
    return stream.str();
}

std::string makeManualTransactionPayloadJson(const ShellAccountingServiceRequest& request)
{
    std::ostringstream stream;
    bool needsComma = false;
    stream << '{';
    appendJsonStringField(stream, "accountId", request.accountId, needsComma);
    appendJsonStringField(stream, "portfolioId", request.portfolioId, needsComma);
    appendJsonStringField(stream, "instrumentId", request.instrumentId, needsComma);
    appendJsonStringField(stream, "securityCode", request.securityCode, needsComma);
    appendJsonStringField(stream, "tradeSide", request.tradeSide, needsComma);
    appendJsonStringField(stream, "quantityUnits", request.quantityUnits, needsComma);
    appendJsonStringField(stream, "priceAmountMinor", request.priceAmountMinor, needsComma);
    appendJsonStringField(stream, "grossAmountMinor", request.grossAmountMinor, needsComma);
    appendJsonStringField(stream, "feeAmountMinor", request.feeAmountMinor, needsComma);
    appendJsonStringField(stream, "taxAmountMinor", request.taxAmountMinor, needsComma);
    appendJsonStringField(stream, "occurredAt", request.occurredAt, needsComma);
    appendJsonStringField(stream, "sourceMemo", request.sourceMemo, needsComma);
    appendJsonStringField(stream, "requestId", request.requestId, needsComma);
    appendJsonStringField(stream, "idempotencyKey", request.idempotencyKey, needsComma);
    stream << '}';
    return stream.str();
}

std::string makeManualCashMovementPayloadJson(const ShellAccountingServiceRequest& request)
{
    std::ostringstream stream;
    bool needsComma = false;
    stream << '{';
    appendJsonStringField(stream, "accountId", request.accountId, needsComma);
    appendJsonStringField(stream, "portfolioId", request.portfolioId, needsComma);
    appendJsonStringField(stream, "movementType", request.movementType, needsComma);
    appendJsonStringField(stream, "amountMinor", request.amountMinor, needsComma);
    appendJsonStringField(stream, "currency", request.currency, needsComma);
    appendJsonStringField(stream, "occurredAt", request.occurredAt, needsComma);
    appendJsonStringField(stream, "sourceMemo", request.sourceMemo, needsComma);
    appendJsonStringField(stream, "sourceReference", request.sourceReference, needsComma);
    appendJsonStringField(stream, "requestId", request.requestId, needsComma);
    appendJsonStringField(stream, "idempotencyKey", request.idempotencyKey, needsComma);
    stream << '}';
    return stream.str();
}

std::string makeExcelVbaImportPersistManualEntryPayloadJson(
    const ShellAccountingServiceRequest& request)
{
    std::ostringstream stream;
    bool needsComma = false;
    stream << '{';
    appendJsonStringField(stream, "previewStatus", request.previewStatus, needsComma);
    appendJsonStringField(stream, "previewDigest", request.previewDigest, needsComma);
    appendJsonStringField(stream, "idempotencyKey", request.idempotencyKey, needsComma);
    appendJsonStringField(stream, "requestId", request.requestId, needsComma);
    appendJsonStringField(stream, "schemaVersion", request.schemaVersion, needsComma);
    appendJsonStringField(stream, "source", request.source, needsComma);
    appendJsonStringField(stream, "acceptedAt", request.acceptedAt, needsComma);
    appendJsonStringField(stream, "importBatchLabel", request.importBatchLabel, needsComma);
    if (needsComma) {
        stream << ',';
    }
    stream << "\"previewFactSummary\":{";
    bool summaryNeedsComma = false;
    appendJsonIntField(stream, "tradeFactCount", request.importTradeFactCount, summaryNeedsComma);
    appendJsonIntField(stream, "cashFactCount", request.importCashFactCount, summaryNeedsComma);
    appendJsonIntField(
        stream,
        "marketPriceFactCount",
        request.importMarketPriceFactCount,
        summaryNeedsComma);
    appendJsonIntField(stream, "fxRateFactCount", request.importFxRateFactCount, summaryNeedsComma);
    stream << '}';
    needsComma = true;
    appendJsonRawField(stream, "importPayload", request.importPayloadJson, needsComma);
    stream << '}';
    return stream.str();
}

ShellAccountingDataServiceClientRequest makeClientRequest(
    const ShellAccountingServiceRequest& request,
    const char* fallbackActionName)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = request.actionName.empty() ? fallbackActionName : request.actionName;
    clientRequest.payloadJson = makePayloadJson(request, fallbackActionName);
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeCreateDraftClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.tradedraft.create";
    clientRequest.payloadJson = makeCreateDraftPayloadJson(request);
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeConfirmDraftClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.tradedraft.confirm";
    clientRequest.payloadJson = makeConfirmDraftPayloadJson(request);
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeTradeDraftRecommendationClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.tradedraft.create_from_recommendation";
    clientRequest.payloadJson = makeTradeDraftRecommendationPayloadJson(request);
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeTradeDraftSummaryClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.tradedraft.readonly_summary";
    clientRequest.payloadJson = makeTradeDraftSummaryPayloadJson(request);
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeManualTransactionClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.manual_transaction.create";
    clientRequest.payloadJson = makeManualTransactionPayloadJson(request);
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeManualCashMovementClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.manual_cash_movement.create";
    clientRequest.payloadJson = makeManualCashMovementPayloadJson(request);
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeExcelVbaImportPreviewClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.excel_vba_import.readonly_preview";
    clientRequest.payloadJson = request.importPayloadJson.empty() ? "{}" : request.importPayloadJson;
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeExcelVbaImportPersistManualEntryClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.excel_vba_import.persist_manual_entry";
    clientRequest.payloadJson = makeExcelVbaImportPersistManualEntryPayloadJson(request);
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makePortfolioReplayReadOnlySummaryClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.portfolio_replay.readonly_summary";
    clientRequest.payloadJson =
        request.portfolioReplayPayloadJson.empty() ? "{}" : request.portfolioReplayPayloadJson;
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeStrategyRecommendationReadOnlySummaryClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "strategy.recommendation.readonly_summary";
    clientRequest.payloadJson =
        request.strategyRecommendationPayloadJson.empty() ? "{}" : request.strategyRecommendationPayloadJson;
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeOtcMapPreviewClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.otcmap_multichannel.readonly_preview";
    clientRequest.payloadJson = request.otcMapPayloadJson.empty() ? "{}" : request.otcMapPayloadJson;
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingDataServiceClientRequest makeOtcMapDraftCreateClientRequest(
    const ShellAccountingServiceRequest& request)
{
    ShellAccountingDataServiceClientRequest clientRequest;
    clientRequest.actionName = "accounting.tradedraft.create_otcmap_multichannel";
    clientRequest.payloadJson = request.otcMapPayloadJson.empty() ? "{}" : request.otcMapPayloadJson;
    clientRequest.timeoutMs = request.timeoutMs;
    return clientRequest;
}

ShellAccountingServiceResult mapClientResponse(
    ShellAccountingDataServiceClientResponse response,
    const ShellAccountingServiceRequest& request,
    const char* fallbackActionName)
{
    ShellAccountingServiceResult result;
    result.actionName = response.actionName.empty()
        ? (request.actionName.empty() ? fallbackActionName : request.actionName)
        : std::move(response.actionName);
    result.protocolSuccess = response.protocolSuccess;
    result.implemented = response.implemented;
    result.readOnly = response.readOnly;
    result.writeEnabled = response.writeEnabled;
    result.payloadStatus = std::move(response.payloadStatus);
    result.dataQualityStatus = std::move(response.dataQualityStatus);
    result.hasRows = response.hasRows;
    result.issues = std::move(response.issues);
    result.warnings = std::move(response.warnings);
    result.errors = std::move(response.errors);
    result.rawPayload = std::move(response.rawPayload);
    result.timeout = response.timeout;
    result.transportError = response.transportError;
    result.protocolError = response.protocolError;
    result.domainError = response.domainError;
    result.generatedTradeDraft = result.actionName == "accounting.tradedraft.create"
        && response.protocolSuccess && response.implemented
        && (response.payloadStatus == "OK" || response.payloadStatus == "DUPLICATE");
    result.generatedTradeSuggestion = false;
    result.strategyExecuted = false;
    result.transactionCommitted = response.transactionCommitted;
    result.duplicateImportPrevented = response.duplicateImportPrevented;
    result.idempotencyConflictRejected = response.idempotencyConflictRejected;
    result.idempotencyRequired = response.idempotencyRequired;
    result.importPreviewAccepted = response.importPreviewAccepted;
    result.importPreviewRejected = response.importPreviewRejected;
    result.importPreviewDiagnostics = std::move(response.importPreviewDiagnostics);
    result.importPreviewDiagnosticCodes = std::move(response.importPreviewDiagnosticCodes);
    result.importPreviewFactSummary = response.importPreviewFactSummary;
    result.portfolioReplayAccepted = response.portfolioReplayAccepted;
    result.portfolioReplayExecuted = response.portfolioReplayExecuted;
    result.portfolioReplayPositionCount = response.portfolioReplayPositionCount;
    result.portfolioReplayCashSummaryCount = response.portfolioReplayCashSummaryCount;
    result.portfolioReplayRealizedPnlText = std::move(response.portfolioReplayRealizedPnlText);
    result.portfolioReplayUnrealizedPnlText = std::move(response.portfolioReplayUnrealizedPnlText);
    result.portfolioReplayTotalFeeText = std::move(response.portfolioReplayTotalFeeText);
    result.portfolioReplayTotalMarketValueText = std::move(response.portfolioReplayTotalMarketValueText);
    result.portfolioReplayIssueCodes = std::move(response.portfolioReplayIssueCodes);
    result.strategyRecommendationAccepted = response.strategyRecommendationAccepted;
    result.strategyRecommendationComputed = response.strategyRecommendationComputed;
    result.strategyRecommendationActionCode = std::move(response.strategyRecommendationActionCode);
    result.strategyRecommendationActionLabel = std::move(response.strategyRecommendationActionLabel);
    result.strategyRecommendationSourceCode = std::move(response.strategyRecommendationSourceCode);
    result.strategyRecommendationSourceLabel = std::move(response.strategyRecommendationSourceLabel);
    result.strategyRecommendationReasonCode = std::move(response.strategyRecommendationReasonCode);
    result.strategyRecommendationReasonLabel = std::move(response.strategyRecommendationReasonLabel);
    result.strategyRecommendationTierLabel = std::move(response.strategyRecommendationTierLabel);
    result.strategyRecommendationTargetAmountText = std::move(response.strategyRecommendationTargetAmountText);
    result.strategyRecommendationSuggestedQuantityText = std::move(response.strategyRecommendationSuggestedQuantityText);
    result.strategyRecommendationSuggestedAmountText = std::move(response.strategyRecommendationSuggestedAmountText);
    result.strategyRecommendationNetCashImpactText = std::move(response.strategyRecommendationNetCashImpactText);
    result.strategyRecommendationFeeText = std::move(response.strategyRecommendationFeeText);
    result.strategyRecommendationBaseProtectionPassed = response.strategyRecommendationBaseProtectionPassed;
    result.strategyRecommendationCashLimitApplied = response.strategyRecommendationCashLimitApplied;
    result.strategyRecommendationIssueCodes = std::move(response.strategyRecommendationIssueCodes);
    result.tradeDraftManualRecommendationFlowCreated = response.tradeDraftManualRecommendationFlowCreated;
    result.tradeDraftUserConfirmationRequired = response.tradeDraftUserConfirmationRequired;
    result.tradeDraftEligible = response.tradeDraftEligible;
    result.tradeDraftDuplicate = response.tradeDraftDuplicate;
    result.tradeDraftIdempotencyConflict = response.tradeDraftIdempotencyConflict;
    result.tradeDraftIsNotOrder = response.tradeDraftIsNotOrder;
    result.tradeDraftSummaryFound = response.tradeDraftSummaryFound;
    result.tradeDraftId = response.tradeDraftId;
    result.tradeDraftStatus = std::move(response.tradeDraftStatus);
    result.tradeDraftSide = std::move(response.tradeDraftSide);
    result.tradeDraftInstrumentCode = std::move(response.tradeDraftInstrumentCode);
    result.tradeDraftQuantityText = std::move(response.tradeDraftQuantityText);
    result.tradeDraftAmountText = std::move(response.tradeDraftAmountText);
    result.tradeDraftNetCashImpactText = std::move(response.tradeDraftNetCashImpactText);
    result.tradeDraftSummary = std::move(response.tradeDraftSummary);
    result.tradeDraftIssueCodes = std::move(response.tradeDraftIssueCodes);
    result.otcMapPreviewAccepted = response.otcMapPreviewAccepted;
    result.otcMapPreviewExecuted = response.otcMapPreviewExecuted;
    result.otcMapPreviewLegCount = response.otcMapPreviewLegCount;
    result.otcMapPreviewStatus = std::move(response.otcMapPreviewStatus);
    result.otcMapPreviewTotalAmountText = std::move(response.otcMapPreviewTotalAmountText);
    result.otcMapPreviewSummary = std::move(response.otcMapPreviewSummary);
    result.otcMapPreviewIssueCodes = std::move(response.otcMapPreviewIssueCodes);
    result.otcMapDraftDuplicate = response.otcMapDraftDuplicate;
    result.otcMapDraftIdempotencyConflict = response.otcMapDraftIdempotencyConflict;
    result.otcMapDraftId = response.otcMapDraftId;
    result.otcMapDraftLegCount = response.otcMapDraftLegCount;
    result.otcMapDraftStatus = std::move(response.otcMapDraftStatus);
    result.otcMapDraftSummary = std::move(response.otcMapDraftSummary);
    result.otcMapDraftIssueCodes = std::move(response.otcMapDraftIssueCodes);
    result.accountingEngineCalled = response.accountingEngineCalled;
    result.productionFileLoading = response.productionFileLoading;
    result.productionWrite = response.productionWrite;
    result.sqliteProductionWrite = response.sqliteProductionWrite;
    result.auditWritten = response.auditWritten;
    result.ledgerWritten = response.ledgerWritten;
    result.snapshotWritten = response.snapshotWritten;
    result.tradeLogWritten = response.tradeLogWritten;
    result.readModelPersistentWrite = response.readModelPersistentWrite;
    result.networkAccess = response.networkAccess;
    result.credentialAccess = response.credentialAccess;
    result.endpointAccess = response.endpointAccess;
    result.automaticTrading = response.automaticTrading;
    result.rawUserDataExposed = response.rawUserDataExposed;
    return result;
}

}  // namespace

ShellAccountingDataServiceAdapter::ShellAccountingDataServiceAdapter(
    std::shared_ptr<ShellAccountingDataServiceClientPort> clientPort)
    : clientPort_(std::move(clientPort))
{
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchPositionList(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callPositionList(makeClientRequest(request, "position.list")),
            request,
            "position.list");
    }
    return makeNotConnectedResult(request, "position.list");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchCashSummary(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callCashSummary(makeClientRequest(request, "cash.summary")),
            request,
            "cash.summary");
    }
    return makeNotConnectedResult(request, "cash.summary");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchPortfolioPnlSummary(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callPortfolioPnlSummary(
                makeClientRequest(request, "portfolio.pnl.summary")),
            request,
            "portfolio.pnl.summary");
    }
    return makeNotConnectedResult(request, "portfolio.pnl.summary");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchBasePositionSummary(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callBasePositionSummary(
                makeClientRequest(request, "base_position.summary")),
            request,
            "base_position.summary");
    }
    return makeNotConnectedResult(request, "base_position.summary");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchSniperPoolSummary(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callSniperPoolSummary(makeClientRequest(request, "sniper_pool.summary")),
            request,
            "sniper_pool.summary");
    }
    return makeNotConnectedResult(request, "sniper_pool.summary");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::createDraft(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callTradeDraftCreate(makeCreateDraftClientRequest(request)),
            request,
            "accounting.tradedraft.create");
    }
    return makeNotConnectedResult(request, "accounting.tradedraft.create");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::createDraftFromRecommendation(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callTradeDraftCreateFromRecommendation(
                makeTradeDraftRecommendationClientRequest(request)),
            request,
            "accounting.tradedraft.create_from_recommendation");
    }
    return makeNotConnectedResult(request, "accounting.tradedraft.create_from_recommendation");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchTradeDraftReadOnlySummary(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callTradeDraftReadOnlySummary(makeTradeDraftSummaryClientRequest(request)),
            request,
            "accounting.tradedraft.readonly_summary");
    }
    return makeNotConnectedResult(request, "accounting.tradedraft.readonly_summary");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::confirmDraft(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callTradeDraftConfirm(makeConfirmDraftClientRequest(request)),
            request,
            "accounting.tradedraft.confirm");
    }
    return makeNotConnectedResult(request, "accounting.tradedraft.confirm");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::submitManualTransaction(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callManualTransactionCreate(makeManualTransactionClientRequest(request)),
            request,
            "accounting.manual_transaction.create");
    }
    return makeNotConnectedResult(request, "accounting.manual_transaction.create");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::submitManualCashMovement(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callManualCashMovementCreate(makeManualCashMovementClientRequest(request)),
            request,
            "accounting.manual_cash_movement.create");
    }
    return makeNotConnectedResult(request, "accounting.manual_cash_movement.create");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::previewExcelVbaImportReadOnly(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callExcelVbaImportReadOnlyPreview(
                makeExcelVbaImportPreviewClientRequest(request)),
            request,
            "accounting.excel_vba_import.readonly_preview");
    }
    auto result = makeNotConnectedResult(request, "accounting.excel_vba_import.readonly_preview");
    result.importPreviewRejected = true;
    return result;
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::persistExcelVbaImportManualEntry(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callExcelVbaImportPersistManualEntry(
                makeExcelVbaImportPersistManualEntryClientRequest(request)),
            request,
            "accounting.excel_vba_import.persist_manual_entry");
    }
    return makeNotConnectedResult(request, "accounting.excel_vba_import.persist_manual_entry");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::fetchPortfolioReplayReadOnlySummary(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callPortfolioReplayReadOnlySummary(
                makePortfolioReplayReadOnlySummaryClientRequest(request)),
            request,
            "accounting.portfolio_replay.readonly_summary");
    }
    return makeNotConnectedResult(request, "accounting.portfolio_replay.readonly_summary");
}

ShellAccountingServiceResult
ShellAccountingDataServiceAdapter::fetchStrategyRecommendationReadOnlySummary(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callStrategyRecommendationReadOnlySummary(
                makeStrategyRecommendationReadOnlySummaryClientRequest(request)),
            request,
            "strategy.recommendation.readonly_summary");
    }
    return makeNotConnectedResult(request, "strategy.recommendation.readonly_summary");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::previewOtcMapMultiChannelDraft(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callOtcMapMultiChannelReadOnlyPreview(
                makeOtcMapPreviewClientRequest(request)),
            request,
            "accounting.otcmap_multichannel.readonly_preview");
    }
    return makeNotConnectedResult(request, "accounting.otcmap_multichannel.readonly_preview");
}

ShellAccountingServiceResult ShellAccountingDataServiceAdapter::createOtcMapMultiChannelTradeDraft(
    const ShellAccountingServiceRequest& request)
{
    if (clientPort_) {
        return mapClientResponse(
            clientPort_->callTradeDraftCreateOtcMapMultiChannel(
                makeOtcMapDraftCreateClientRequest(request)),
            request,
            "accounting.tradedraft.create_otcmap_multichannel");
    }
    return makeNotConnectedResult(request, "accounting.tradedraft.create_otcmap_multichannel");
}

bool ShellAccountingDataServiceAdapter::hasLiveClient() const noexcept
{
    return false;
}

bool ShellAccountingDataServiceAdapter::hasClientPort() const noexcept
{
    return static_cast<bool>(clientPort_);
}

void ShellAccountingDataServiceAdapter::setClientPort(
    std::shared_ptr<ShellAccountingDataServiceClientPort> clientPort) noexcept
{
    clientPort_ = std::move(clientPort);
}

void ShellAccountingDataServiceAdapter::clearClientPort() noexcept
{
    clientPort_.reset();
}

bool ShellAccountingDataServiceAdapter::readOnly() const noexcept
{
    return true;
}

bool ShellAccountingDataServiceAdapter::writeEnabled() const noexcept
{
    return false;
}

}  // namespace etfdt::shell_services
