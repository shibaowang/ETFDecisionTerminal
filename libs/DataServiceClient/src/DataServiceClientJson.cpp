#include "DataServiceClient/DataServiceClientJson.h"

#include "Protocol/Json.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <cstdint>
#include <sstream>
#include <optional>
#include <string_view>
#include <utility>
#include <vector>

namespace etfdt::data_service_client {
namespace {

std::string compactJson(const QJsonValue& value)
{
    if (value.isObject()) {
        return QJsonDocument(value.toObject()).toJson(QJsonDocument::Compact).toStdString();
    }
    if (value.isArray()) {
        return QJsonDocument(value.toArray()).toJson(QJsonDocument::Compact).toStdString();
    }
    return {};
}

std::optional<std::string> stringField(const QJsonObject& object, const QString& key)
{
    const auto value = object.value(key);
    if (!value.isString()) {
        return std::nullopt;
    }
    return value.toString().toStdString();
}

std::optional<bool> boolField(const QJsonObject& object, const QString& key)
{
    const auto value = object.value(key);
    if (!value.isBool()) {
        return std::nullopt;
    }
    return value.toBool();
}

std::optional<int> intField(const QJsonObject& object, const QString& key)
{
    const auto value = object.value(key);
    if (!value.isDouble()) {
        return std::nullopt;
    }
    return value.toInt();
}

std::string jsonString(std::string_view value)
{
    return "\"" + etfdt::protocol::escapeJsonString(value) + "\"";
}

bool readRequiredString(
    const QJsonObject& object,
    const QString& key,
    std::string& target,
    std::string& missingField)
{
    const auto value = stringField(object, key);
    if (!value.has_value()) {
        missingField = key.toStdString();
        return false;
    }
    target = *value;
    return true;
}

bool readRequiredBool(
    const QJsonObject& object,
    const QString& key,
    bool& target,
    std::string& missingField)
{
    const auto value = boolField(object, key);
    if (!value.has_value()) {
        missingField = key.toStdString();
        return false;
    }
    target = *value;
    return true;
}

bool readRequiredInt(
    const QJsonObject& object,
    const QString& key,
    int& target,
    std::string& missingField)
{
    const auto value = intField(object, key);
    if (!value.has_value()) {
        missingField = key.toStdString();
        return false;
    }
    target = *value;
    return true;
}

DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult> previewMappingFailure(
    const std::string& message)
{
    return DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult>::failure(
        etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
        message);
}

DataServiceClientResult<ExcelVbaImportPersistManualEntryResult> persistMappingFailure(
    const std::string& message)
{
    return DataServiceClientResult<ExcelVbaImportPersistManualEntryResult>::failure(
        etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
        message);
}

DataServiceClientResult<PortfolioReplayReadOnlySummaryResult> portfolioReplayMappingFailure(
    const std::string& message)
{
    return DataServiceClientResult<PortfolioReplayReadOnlySummaryResult>::failure(
        etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
        message);
}

DataServiceClientResult<StrategyRecommendationReadOnlySummaryResult>
strategyRecommendationMappingFailure(const std::string& message)
{
    return DataServiceClientResult<StrategyRecommendationReadOnlySummaryResult>::failure(
        etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
        message);
}

DataServiceClientResult<TradeDraftCreateFromRecommendationResult>
tradeDraftCreateMappingFailure(const std::string& message)
{
    return DataServiceClientResult<TradeDraftCreateFromRecommendationResult>::failure(
        etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
        message);
}

DataServiceClientResult<TradeDraftReadOnlySummaryResult>
tradeDraftSummaryMappingFailure(const std::string& message)
{
    return DataServiceClientResult<TradeDraftReadOnlySummaryResult>::failure(
        etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
        message);
}

}  // namespace

DataServiceClientResult<etfdt::protocol::ProtocolResponse> parseProtocolResponseJson(
    const std::string& json)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Invalid ProtocolResponse JSON");
    }

    const QJsonObject object = document.object();
    const auto protocolVersion = stringField(object, "protocolVersion");
    const auto msgId = stringField(object, "msgId");
    const auto traceId = stringField(object, "traceId");
    const auto errorCodeText = stringField(object, "errorCode");
    const auto errorMessage = stringField(object, "errorMessage");

    if (!protocolVersion || !msgId || !traceId || !object.value("success").isBool()
        || !errorCodeText || !errorMessage || !object.contains("payload")) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "ProtocolResponse is missing one or more required fields");
    }

    const std::string payloadJson = compactJson(object.value("payload"));
    if (payloadJson.empty()) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "ProtocolResponse payload must be a JSON object or array");
    }

    etfdt::protocol::ProtocolResponse response;
    response.protocolVersion = *protocolVersion;
    response.msgId = *msgId;
    response.traceId = *traceId;
    response.success = object.value("success").toBool();
    response.errorMessage = *errorMessage;
    response.payloadJson = payloadJson;

    if (!errorCodeText->empty()) {
        auto parsedErrorCode = etfdt::protocol::fromString<etfdt::protocol::ErrorCode>(
            *errorCodeText);
        if (!parsedErrorCode) {
            return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
                etfdt::protocol::ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR,
                "ProtocolResponse contains unknown errorCode");
        }
        response.errorCode = parsedErrorCode.value();
    }

    const auto validation = etfdt::protocol::validateProtocolResponse(response);
    if (!validation.valid) {
        return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::failure(
            validation.error.has_value()
                ? validation.error->code
                : etfdt::protocol::ErrorCode::E1000_UNKNOWN_PROTOCOL_ERROR,
            validation.error.has_value() ? validation.error->message
                                         : "ProtocolResponse validation failed");
    }

    return DataServiceClientResult<etfdt::protocol::ProtocolResponse>::success(response);
}

DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult>
parseExcelVbaImportReadOnlyPreviewPayloadJson(const std::string& json)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Invalid Excel/VBA import preview payload JSON");
    }

    const auto object = document.object();
    ExcelVbaImportReadOnlyPreviewResult result;
    std::string missingField;

    if (!readRequiredString(object, "action", result.action, missingField)
        || !readRequiredString(object, "task", result.task, missingField)
        || !readRequiredString(object, "mode", result.mode, missingField)
        || !readRequiredBool(
            object,
            "dataServiceReadOnlyActionRegistered",
            result.dataServiceReadOnlyActionRegistered,
            missingField)
        || !readRequiredBool(object, "parserBoundaryCalled", result.parserBoundaryCalled, missingField)
        || !readRequiredBool(object, "previewActionExecuted", result.previewActionExecuted, missingField)
        || !readRequiredBool(object, "diagnosticsBuilt", result.diagnosticsBuilt, missingField)
        || !readRequiredBool(
            object,
            "replayFactSummaryBuilt",
            result.replayFactSummaryBuilt,
            missingField)
        || !readRequiredBool(object, "accountingEngineCalled", result.accountingEngineCalled, missingField)
        || !readRequiredBool(
            object,
            "productionRuntimeIntegrationImplemented",
            result.productionRuntimeIntegrationImplemented,
            missingField)
        || !readRequiredBool(object, "accepted", result.accepted, missingField)
        || !readRequiredBool(object, "parserReadOnly", result.parserReadOnly, missingField)
        || !readRequiredBool(
            object,
            "parserNoSqliteAccess",
            result.parserNoSqliteAccess,
            missingField)
        || !readRequiredBool(object, "parserNoWrite", result.parserNoWrite, missingField)
        || !readRequiredBool(object, "productionWrite", result.productionWrite, missingField)
        || !readRequiredBool(
            object,
            "sqliteProductionWrite",
            result.sqliteProductionWrite,
            missingField)
        || !readRequiredBool(object, "auditWritten", result.auditWritten, missingField)
        || !readRequiredBool(object, "ledgerWritten", result.ledgerWritten, missingField)
        || !readRequiredBool(object, "snapshotWritten", result.snapshotWritten, missingField)
        || !readRequiredBool(object, "tradeLogWritten", result.tradeLogWritten, missingField)
        || !readRequiredBool(
            object,
            "readModelPersistentWrite",
            result.readModelPersistentWrite,
            missingField)
        || !readRequiredBool(object, "tradeDraftGenerated", result.tradeDraftGenerated, missingField)
        || !readRequiredBool(object, "strategyExecuted", result.strategyExecuted, missingField)
        || !readRequiredBool(
            object,
            "brokerOrderSubmitted",
            result.brokerOrderSubmitted,
            missingField)
        || !readRequiredBool(object, "networkAccess", result.networkAccess, missingField)
        || !readRequiredBool(object, "credentialAccess", result.credentialAccess, missingField)
        || !readRequiredBool(object, "endpointAccess", result.endpointAccess, missingField)
        || !readRequiredBool(object, "automaticTrading", result.automaticTrading, missingField)
        || !readRequiredBool(object, "rawUserDataExposed", result.rawUserDataExposed, missingField)) {
        return previewMappingFailure("Excel/VBA import preview payload missing field: " + missingField);
    }

    const auto diagnosticsValue = object.value("diagnostics");
    if (!diagnosticsValue.isArray()) {
        return previewMappingFailure("Excel/VBA import preview payload missing diagnostics");
    }
    for (const auto& value : diagnosticsValue.toArray()) {
        if (!value.isObject()) {
            return previewMappingFailure("Excel/VBA import preview diagnostic must be object");
        }
        const auto diagnosticObject = value.toObject();
        ExcelVbaImportReadOnlyPreviewDiagnostic diagnostic;
        if (!readRequiredString(diagnosticObject, "level", diagnostic.level, missingField)
            || !readRequiredString(diagnosticObject, "code", diagnostic.code, missingField)
            || !readRequiredString(diagnosticObject, "field", diagnostic.field, missingField)
            || !readRequiredString(diagnosticObject, "sheetName", diagnostic.sheetName, missingField)
            || !readRequiredString(diagnosticObject, "rowId", diagnostic.rowId, missingField)) {
            return previewMappingFailure(
                "Excel/VBA import preview diagnostic missing field: " + missingField);
        }
        result.diagnostics.push_back(std::move(diagnostic));
    }

    const auto diagnosticCodesValue = object.value("diagnosticCodes");
    if (!diagnosticCodesValue.isArray()) {
        return previewMappingFailure("Excel/VBA import preview payload missing diagnosticCodes");
    }
    for (const auto& value : diagnosticCodesValue.toArray()) {
        if (!value.isString()) {
            return previewMappingFailure("Excel/VBA import preview diagnostic code must be string");
        }
        result.diagnosticCodes.push_back(value.toString().toStdString());
    }

    const auto replayFactSummaryValue = object.value("replayFactSummary");
    if (!replayFactSummaryValue.isObject()) {
        return previewMappingFailure("Excel/VBA import preview payload missing replayFactSummary");
    }
    const auto replayFactSummary = replayFactSummaryValue.toObject();
    if (!readRequiredInt(
            replayFactSummary,
            "tradeFactCount",
            result.replayFactSummary.tradeFactCount,
            missingField)
        || !readRequiredInt(
            replayFactSummary,
            "cashFactCount",
            result.replayFactSummary.cashFactCount,
            missingField)
        || !readRequiredInt(
            replayFactSummary,
            "marketPriceFactCount",
            result.replayFactSummary.marketPriceFactCount,
            missingField)
        || !readRequiredInt(
            replayFactSummary,
            "fxRateFactCount",
            result.replayFactSummary.fxRateFactCount,
            missingField)) {
        return previewMappingFailure("Excel/VBA import preview replay fact summary missing field: " + missingField);
    }

    return DataServiceClientResult<ExcelVbaImportReadOnlyPreviewResult>::success(std::move(result));
}

DataServiceClientResult<ExcelVbaImportPersistManualEntryResult>
parseExcelVbaImportPersistManualEntryPayloadJson(
    const std::string& json,
    bool protocolSuccess)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return DataServiceClientResult<ExcelVbaImportPersistManualEntryResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Invalid Excel/VBA import persist payload JSON");
    }

    const auto object = document.object();
    ExcelVbaImportPersistManualEntryResult result;
    result.protocolSuccess = protocolSuccess;
    std::string missingField;

    if (!readRequiredString(object, "action", result.action, missingField)
        || !readRequiredString(object, "task", result.task, missingField)
        || !readRequiredBool(
            object,
            "dataServicePersistActionRegistered",
            result.dataServicePersistActionRegistered,
            missingField)
        || !readRequiredBool(object, "acceptedPreviewRequired", result.acceptedPreviewRequired, missingField)
        || !readRequiredBool(object, "parserBoundaryReused", result.parserBoundaryReused, missingField)
        || !readRequiredBool(
            object,
            "manualTransactionRepositoryUsed",
            result.manualTransactionRepositoryUsed,
            missingField)
        || !readRequiredBool(
            object,
            "directTradeLogSqlInDataService",
            result.directTradeLogSqlInDataService,
            missingField)
        || !readRequiredBool(object, "tempDbOnly", result.tempDbOnly, missingField)
        || !readRequiredBool(object, "productionDbTouched", result.productionDbTouched, missingField)
        || !readRequiredBool(object, "transactionCommitted", result.transactionCommitted, missingField)
        || !readRequiredBool(object, "tradeLogWritten", result.tradeLogWritten, missingField)
        || !readRequiredBool(object, "auditLogWritten", result.auditLogWritten, missingField)
        || !readRequiredBool(object, "idempotencyRequired", result.idempotencyRequired, missingField)
        || !readRequiredBool(
            object,
            "duplicateImportPrevented",
            result.duplicateImportPrevented,
            missingField)
        || !readRequiredBool(
            object,
            "idempotencyConflictRejected",
            result.idempotencyConflictRejected,
            missingField)
        || !readRequiredBool(
            object,
            "nonAcceptedPreviewRejected",
            result.nonAcceptedPreviewRejected,
            missingField)
        || !readRequiredBool(
            object,
            "supplementalDataAccessTransactionAuthorizationUsed",
            result.supplementalDataAccessTransactionAuthorizationUsed,
            missingField)
        || !readRequiredBool(
            object,
            "manualTransactionActiveTransactionMethodCreated",
            result.manualTransactionActiveTransactionMethodCreated,
            missingField)
        || !readRequiredBool(
            object,
            "existingManualTransactionApiPreserved",
            result.existingManualTransactionApiPreserved,
            missingField)
        || !readRequiredBool(
            object,
            "dataAccessCompositionRepositoryCreated",
            result.dataAccessCompositionRepositoryCreated,
            missingField)
        || !readRequiredBool(
            object,
            "singleTransactionBoundaryUsed",
            result.singleTransactionBoundaryUsed,
            missingField)
        || !readRequiredBool(
            object,
            "manualFactsAndAuditCommittedTogether",
            result.manualFactsAndAuditCommittedTogether,
            missingField)
        || !readRequiredBool(
            object,
            "auditFailureRollsBackManualFacts",
            result.auditFailureRollsBackManualFacts,
            missingField)
        || !readRequiredBool(
            object,
            "nestedTransactionAttempted",
            result.nestedTransactionAttempted,
            missingField)
        || !readRequiredBool(object, "qmlWiringChanged", result.qmlWiringChanged, missingField)
        || !readRequiredBool(object, "importButtonAdded", result.importButtonAdded, missingField)
        || !readRequiredBool(object, "accountingEngineCalled", result.accountingEngineCalled, missingField)
        || !readRequiredBool(object, "brokerOrderSubmitted", result.brokerOrderSubmitted, missingField)
        || !readRequiredBool(object, "networkAccess", result.networkAccess, missingField)
        || !readRequiredBool(object, "automaticTrading", result.automaticTrading, missingField)
        || !readRequiredString(object, "status", result.status, missingField)) {
        return persistMappingFailure("Excel/VBA import persist payload missing field: " + missingField);
    }

    if (const auto value = stringField(object, "previewDigest")) {
        result.previewDigest = *value;
    }
    if (const auto value = stringField(object, "idempotencyKey")) {
        result.idempotencyKey = *value;
    }
    if (const auto value = boolField(object, "credentialAccess")) {
        result.credentialAccess = *value;
    }
    if (const auto value = boolField(object, "endpointAccess")) {
        result.endpointAccess = *value;
    }
    if (const auto value = intField(object, "tradeLogRowsWritten")) {
        result.tradeLogRowsWritten = *value;
    }
    if (const auto value = object.value(QStringLiteral("auditLogId")); value.isDouble()) {
        result.auditLogId = static_cast<std::int64_t>(value.toDouble());
    }

    const auto issuesValue = object.value("issues");
    if (!issuesValue.isArray()) {
        return persistMappingFailure("Excel/VBA import persist payload missing issues");
    }
    for (const auto& value : issuesValue.toArray()) {
        if (!value.isString()) {
            return persistMappingFailure("Excel/VBA import persist issue must be string");
        }
        result.issues.push_back(value.toString().toStdString());
    }

    return DataServiceClientResult<ExcelVbaImportPersistManualEntryResult>::success(std::move(result));
}

DataServiceClientResult<PortfolioReplayReadOnlySummaryResult>
parsePortfolioReplayReadOnlySummaryPayloadJson(
    const std::string& json,
    bool protocolSuccess)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return DataServiceClientResult<PortfolioReplayReadOnlySummaryResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Invalid portfolio replay read-only summary payload JSON");
    }

    const auto object = document.object();
    PortfolioReplayReadOnlySummaryResult result;
    result.protocolSuccess = protocolSuccess;
    result.rawPayloadJson = json;
    std::string missingField;

    if (!readRequiredString(object, "action", result.action, missingField)
        || !readRequiredString(object, "task", result.task, missingField)
        || !readRequiredString(object, "mode", result.mode, missingField)
        || !readRequiredString(object, "status", result.status, missingField)
        || !readRequiredString(object, "dataQualityStatus", result.dataQualityStatus, missingField)
        || !readRequiredBool(object, "accepted", result.accepted, missingField)
        || !readRequiredBool(object, "replayExecuted", result.replayExecuted, missingField)
        || !readRequiredBool(
            object,
            "portfolioReplayEngineCreated",
            result.portfolioReplayEngineCreated,
            missingField)
        || !readRequiredBool(
            object,
            "dataServiceReadOnlyActionCreated",
            result.dataServiceReadOnlyActionCreated,
            missingField)
        || !readRequiredBool(object, "accountingEngineCalled", result.accountingEngineCalled, missingField)
        || !readRequiredInt(object, "positionCount", result.positionCount, missingField)
        || !readRequiredInt(object, "cashSummaryCount", result.cashSummaryCount, missingField)
        || !readRequiredBool(
            object,
            "readOnlyReplayNoWrite",
            result.readOnlyReplayNoWrite,
            missingField)
        || !readRequiredBool(
            object,
            "tradeLogRowsWrittenByReplay",
            result.tradeLogRowsWrittenByReplay,
            missingField)
        || !readRequiredBool(
            object,
            "cashAdjustmentRowsWrittenByReplay",
            result.cashAdjustmentRowsWrittenByReplay,
            missingField)
        || !readRequiredBool(
            object,
            "auditLogRowsWrittenByReplay",
            result.auditLogRowsWrittenByReplay,
            missingField)
        || !readRequiredBool(object, "productionWrite", result.productionWrite, missingField)
        || !readRequiredBool(
            object,
            "sqliteProductionWrite",
            result.sqliteProductionWrite,
            missingField)
        || !readRequiredBool(object, "auditWritten", result.auditWritten, missingField)
        || !readRequiredBool(object, "ledgerWritten", result.ledgerWritten, missingField)
        || !readRequiredBool(object, "snapshotWritten", result.snapshotWritten, missingField)
        || !readRequiredBool(object, "tradeLogWritten", result.tradeLogWritten, missingField)
        || !readRequiredBool(
            object,
            "readModelPersistentWrite",
            result.readModelPersistentWrite,
            missingField)
        || !readRequiredBool(object, "productionDbTouched", result.productionDbTouched, missingField)
        || !readRequiredBool(object, "networkAccess", result.networkAccess, missingField)
        || !readRequiredBool(object, "credentialAccess", result.credentialAccess, missingField)
        || !readRequiredBool(object, "endpointAccess", result.endpointAccess, missingField)
        || !readRequiredBool(
            object,
            "brokerOrderSubmitted",
            result.brokerOrderSubmitted,
            missingField)
        || !readRequiredBool(object, "automaticTrading", result.automaticTrading, missingField)) {
        return portfolioReplayMappingFailure(
            "Portfolio replay read-only summary payload missing field: " + missingField);
    }

    const auto pnlValue = object.value("pnlSummary");
    if (!pnlValue.isObject()) {
        return portfolioReplayMappingFailure("Portfolio replay payload missing pnlSummary");
    }
    const auto pnl = pnlValue.toObject();
    if (!readRequiredString(pnl, "currency", result.pnlSummary.currency, missingField)
        || !readRequiredString(pnl, "realizedPnlText", result.pnlSummary.realizedPnlText, missingField)
        || !readRequiredString(pnl, "unrealizedPnlText", result.pnlSummary.unrealizedPnlText, missingField)
        || !readRequiredString(pnl, "totalFeeText", result.pnlSummary.totalFeeText, missingField)
        || !readRequiredString(
            pnl,
            "totalMarketValueText",
            result.pnlSummary.totalMarketValueText,
            missingField)
        || !readRequiredString(
            pnl,
            "dataQualityStatus",
            result.pnlSummary.dataQualityStatus,
            missingField)) {
        return portfolioReplayMappingFailure(
            "Portfolio replay pnlSummary missing field: " + missingField);
    }

    const auto issuesValue = object.value("issues");
    if (!issuesValue.isArray()) {
        return portfolioReplayMappingFailure("Portfolio replay payload missing issues");
    }
    for (const auto& issueValue : issuesValue.toArray()) {
        if (!issueValue.isObject()) {
            return portfolioReplayMappingFailure("Portfolio replay issue must be an object");
        }
        const auto issue = issueValue.toObject();
        if (const auto code = stringField(issue, "code")) {
            result.issueCodes.push_back(*code);
        }
    }

    return DataServiceClientResult<PortfolioReplayReadOnlySummaryResult>::success(std::move(result));
}

DataServiceClientResult<StrategyRecommendationReadOnlySummaryResult>
parseStrategyRecommendationReadOnlySummaryPayloadJson(
    const std::string& json,
    bool protocolSuccess)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return DataServiceClientResult<StrategyRecommendationReadOnlySummaryResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Invalid strategy recommendation read-only summary payload JSON");
    }

    const auto object = document.object();
    StrategyRecommendationReadOnlySummaryResult result;
    result.protocolSuccess = protocolSuccess;
    result.rawPayloadJson = json;
    std::string missingField;

    if (!readRequiredString(object, "action", result.action, missingField)
        || !readRequiredString(object, "task", result.task, missingField)
        || !readRequiredString(object, "mode", result.mode, missingField)
        || !readRequiredString(object, "status", result.status, missingField)
        || !readRequiredString(object, "dataQualityStatus", result.dataQualityStatus, missingField)
        || !readRequiredBool(object, "accepted", result.accepted, missingField)
        || !readRequiredBool(
            object,
            "recommendationComputed",
            result.recommendationComputed,
            missingField)
        || !readRequiredBool(
            object,
            "strategyRecommendationEngineCreated",
            result.strategyRecommendationEngineCreated,
            missingField)
        || !readRequiredBool(
            object,
            "dataServiceReadOnlyActionCreated",
            result.dataServiceReadOnlyActionCreated,
            missingField)
        || !readRequiredString(object, "actionCode", result.actionCode, missingField)
        || !readRequiredString(object, "actionLabel", result.actionLabel, missingField)
        || !readRequiredString(object, "sourceCode", result.sourceCode, missingField)
        || !readRequiredString(object, "sourceLabel", result.sourceLabel, missingField)
        || !readRequiredString(object, "reasonCode", result.reasonCode, missingField)
        || !readRequiredString(object, "reasonLabel", result.reasonLabel, missingField)
        || !readRequiredString(object, "tierLabel", result.tierLabel, missingField)
        || !readRequiredString(object, "targetAmountText", result.targetAmountText, missingField)
        || !readRequiredString(
            object,
            "suggestedQuantityText",
            result.suggestedQuantityText,
            missingField)
        || !readRequiredString(
            object,
            "suggestedAmountText",
            result.suggestedAmountText,
            missingField)
        || !readRequiredString(
            object,
            "netCashImpactText",
            result.netCashImpactText,
            missingField)
        || !readRequiredString(object, "feeText", result.feeText, missingField)
        || !readRequiredBool(
            object,
            "baseProtectionPassed",
            result.baseProtectionPassed,
            missingField)
        || !readRequiredBool(object, "cashLimitApplied", result.cashLimitApplied, missingField)
        || !readRequiredBool(
            object,
            "readOnlyRecommendationNoWrite",
            result.readOnlyRecommendationNoWrite,
            missingField)
        || !readRequiredBool(object, "tradeDraftCreated", result.tradeDraftCreated, missingField)
        || !readRequiredBool(
            object,
            "tradeLogRowsWrittenByRecommendation",
            result.tradeLogRowsWrittenByRecommendation,
            missingField)
        || !readRequiredBool(
            object,
            "cashAdjustmentRowsWrittenByRecommendation",
            result.cashAdjustmentRowsWrittenByRecommendation,
            missingField)
        || !readRequiredBool(
            object,
            "auditLogRowsWrittenByRecommendation",
            result.auditLogRowsWrittenByRecommendation,
            missingField)
        || !readRequiredBool(object, "productionWrite", result.productionWrite, missingField)
        || !readRequiredBool(
            object,
            "sqliteProductionWrite",
            result.sqliteProductionWrite,
            missingField)
        || !readRequiredBool(object, "auditWritten", result.auditWritten, missingField)
        || !readRequiredBool(object, "ledgerWritten", result.ledgerWritten, missingField)
        || !readRequiredBool(object, "snapshotWritten", result.snapshotWritten, missingField)
        || !readRequiredBool(object, "tradeLogWritten", result.tradeLogWritten, missingField)
        || !readRequiredBool(
            object,
            "readModelPersistentWrite",
            result.readModelPersistentWrite,
            missingField)
        || !readRequiredBool(object, "productionDbTouched", result.productionDbTouched, missingField)
        || !readRequiredBool(object, "networkAccess", result.networkAccess, missingField)
        || !readRequiredBool(object, "credentialAccess", result.credentialAccess, missingField)
        || !readRequiredBool(object, "endpointAccess", result.endpointAccess, missingField)
        || !readRequiredBool(
            object,
            "brokerOrderSubmitted",
            result.brokerOrderSubmitted,
            missingField)
        || !readRequiredBool(object, "automaticTrading", result.automaticTrading, missingField)) {
        return strategyRecommendationMappingFailure(
            "Strategy recommendation payload missing field: " + missingField);
    }

    const auto issuesValue = object.value("issues");
    if (!issuesValue.isArray()) {
        return strategyRecommendationMappingFailure("Strategy recommendation payload missing issues");
    }
    for (const auto& issueValue : issuesValue.toArray()) {
        if (!issueValue.isObject()) {
            return strategyRecommendationMappingFailure("Strategy recommendation issue must be an object");
        }
        const auto issueObject = issueValue.toObject();
        StrategyRecommendationIssue issue;
        if (!readRequiredString(issueObject, "level", issue.level, missingField)
            || !readRequiredString(issueObject, "code", issue.code, missingField)
            || !readRequiredString(issueObject, "message", issue.message, missingField)
            || !readRequiredString(issueObject, "field", issue.field, missingField)
            || !readRequiredBool(issueObject, "blocking", issue.blocking, missingField)) {
            return strategyRecommendationMappingFailure(
                "Strategy recommendation issue missing field: " + missingField);
        }
        result.issueCodes.push_back(issue.code);
        result.issues.push_back(std::move(issue));
    }

    return DataServiceClientResult<StrategyRecommendationReadOnlySummaryResult>::success(
        std::move(result));
}

DataServiceClientResult<TradeDraftCreateFromRecommendationResult>
parseTradeDraftCreateFromRecommendationPayloadJson(
    const std::string& json,
    bool protocolSuccess)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return DataServiceClientResult<TradeDraftCreateFromRecommendationResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Invalid TradeDraft create-from-recommendation payload JSON");
    }

    const auto object = document.object();
    TradeDraftCreateFromRecommendationResult result;
    result.protocolSuccess = protocolSuccess;
    std::string missingField;
    int draftId = 0;
    int auditLogId = 0;

    if (!readRequiredString(object, "action", result.action, missingField)
        || !readRequiredString(object, "task", result.task, missingField)
        || !readRequiredString(object, "mode", result.mode, missingField)
        || !readRequiredBool(
            object,
            "dataServiceWriteActionCreated",
            result.dataServiceWriteActionCreated,
            missingField)
        || !readRequiredBool(
            object,
            "tradeDraftManualRecommendationFlowCreated",
            result.tradeDraftManualRecommendationFlowCreated,
            missingField)
        || !readRequiredBool(
            object,
            "tradeDraftRequiresExplicitUserConfirmation",
            result.tradeDraftRequiresExplicitUserConfirmation,
            missingField)
        || !readRequiredBool(
            object,
            "explicitUserConfirmationRequired",
            result.explicitUserConfirmationRequired,
            missingField)
        || !readRequiredBool(
            object,
            "recommendationEngineCalled",
            result.recommendationEngineCalled,
            missingField)
        || !readRequiredBool(object, "recommendationAccepted", result.recommendationAccepted, missingField)
        || !readRequiredBool(object, "tradeDraftEligible", result.tradeDraftEligible, missingField)
        || !readRequiredBool(object, "conversionNoWrite", result.conversionNoWrite, missingField)
        || !readRequiredBool(object, "draftWritten", result.draftWritten, missingField)
        || !readRequiredBool(object, "legWritten", result.legWritten, missingField)
        || !readRequiredBool(object, "auditWritten", result.auditWritten, missingField)
        || !readRequiredBool(object, "transactionCommitted", result.transactionCommitted, missingField)
        || !readRequiredBool(object, "duplicateDraft", result.duplicateDraft, missingField)
        || !readRequiredBool(object, "idempotencyConflict", result.idempotencyConflict, missingField)
        || !readRequiredString(object, "status", result.status, missingField)
        || !readRequiredInt(object, "draftId", draftId, missingField)
        || !readRequiredInt(object, "auditLogId", auditLogId, missingField)
        || !readRequiredString(object, "side", result.side, missingField)
        || !readRequiredString(object, "instrumentCode", result.instrumentCode, missingField)
        || !readRequiredString(object, "quantityText", result.quantityText, missingField)
        || !readRequiredString(object, "amountText", result.amountText, missingField)
        || !readRequiredString(object, "netCashImpactText", result.netCashImpactText, missingField)
        || !readRequiredBool(object, "tradeDraftIsNotOrder", result.tradeDraftIsNotOrder, missingField)
        || !readRequiredBool(
            object,
            "multiChannelOtcLegsCreated",
            result.multiChannelOtcLegsCreated,
            missingField)
        || !readRequiredBool(
            object,
            "tradeLogRowsWrittenByTradeDraft",
            result.tradeLogRowsWrittenByTradeDraft,
            missingField)
        || !readRequiredBool(
            object,
            "cashAdjustmentRowsWrittenByTradeDraft",
            result.cashAdjustmentRowsWrittenByTradeDraft,
            missingField)
        || !readRequiredBool(
            object,
            "positionChangedByTradeDraft",
            result.positionChangedByTradeDraft,
            missingField)
        || !readRequiredBool(
            object,
            "cashChangedByTradeDraft",
            result.cashChangedByTradeDraft,
            missingField)
        || !readRequiredBool(object, "productionWrite", result.productionWrite, missingField)
        || !readRequiredBool(
            object,
            "sqliteProductionWrite",
            result.sqliteProductionWrite,
            missingField)
        || !readRequiredBool(
            object,
            "brokerOrderSubmitted",
            result.brokerOrderSubmitted,
            missingField)
        || !readRequiredBool(object, "networkAccess", result.networkAccess, missingField)
        || !readRequiredBool(object, "credentialAccess", result.credentialAccess, missingField)
        || !readRequiredBool(object, "endpointAccess", result.endpointAccess, missingField)
        || !readRequiredBool(object, "automaticTrading", result.automaticTrading, missingField)) {
        return tradeDraftCreateMappingFailure(
            "TradeDraft create-from-recommendation payload missing field: " + missingField);
    }

    result.draftId = draftId;
    result.auditLogId = auditLogId;
    if (const auto value = stringField(object, "draftUid")) {
        result.draftUid = *value;
    }
    if (const auto value = stringField(object, "draftSignature")) {
        result.draftSignature = *value;
    }

    const auto issueCodesValue = object.value("issueCodes");
    if (!issueCodesValue.isArray()) {
        return tradeDraftCreateMappingFailure("TradeDraft create payload missing issueCodes");
    }
    for (const auto& value : issueCodesValue.toArray()) {
        if (!value.isString()) {
            return tradeDraftCreateMappingFailure("TradeDraft create issue code must be string");
        }
        result.issueCodes.push_back(value.toString().toStdString());
    }

    return DataServiceClientResult<TradeDraftCreateFromRecommendationResult>::success(std::move(result));
}

DataServiceClientResult<TradeDraftReadOnlySummaryResult>
parseTradeDraftReadOnlySummaryPayloadJson(
    const std::string& json,
    bool protocolSuccess)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return DataServiceClientResult<TradeDraftReadOnlySummaryResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Invalid TradeDraft read-only summary payload JSON");
    }

    const auto object = document.object();
    TradeDraftReadOnlySummaryResult result;
    result.protocolSuccess = protocolSuccess;
    std::string missingField;
    int draftId = 0;

    if (!readRequiredString(object, "action", result.action, missingField)
        || !readRequiredString(object, "task", result.task, missingField)
        || !readRequiredString(object, "mode", result.mode, missingField)
        || !readRequiredBool(object, "readOnly", result.readOnly, missingField)
        || !readRequiredBool(object, "found", result.found, missingField)
        || !readRequiredInt(object, "draftId", draftId, missingField)
        || !readRequiredString(object, "draftUid", result.draftUid, missingField)
        || !readRequiredString(object, "draftSignature", result.draftSignature, missingField)
        || !readRequiredString(object, "status", result.status, missingField)
        || !readRequiredString(object, "side", result.side, missingField)
        || !readRequiredString(object, "accountId", result.accountId, missingField)
        || !readRequiredString(object, "portfolioId", result.portfolioId, missingField)
        || !readRequiredString(object, "instrumentCode", result.instrumentCode, missingField)
        || !readRequiredString(object, "strategyCode", result.strategyCode, missingField)
        || !readRequiredString(object, "quantityText", result.quantityText, missingField)
        || !readRequiredString(object, "amountText", result.amountText, missingField)
        || !readRequiredString(object, "netCashImpactText", result.netCashImpactText, missingField)
        || !readRequiredString(object, "idempotencyKey", result.idempotencyKey, missingField)
        || !readRequiredBool(object, "productionWrite", result.productionWrite, missingField)
        || !readRequiredBool(
            object,
            "tradeLogRowsWrittenByTradeDraft",
            result.tradeLogRowsWrittenByTradeDraft,
            missingField)
        || !readRequiredBool(
            object,
            "cashAdjustmentRowsWrittenByTradeDraft",
            result.cashAdjustmentRowsWrittenByTradeDraft,
            missingField)
        || !readRequiredBool(
            object,
            "brokerOrderSubmitted",
            result.brokerOrderSubmitted,
            missingField)
        || !readRequiredBool(object, "networkAccess", result.networkAccess, missingField)
        || !readRequiredBool(object, "credentialAccess", result.credentialAccess, missingField)
        || !readRequiredBool(object, "endpointAccess", result.endpointAccess, missingField)
        || !readRequiredBool(object, "automaticTrading", result.automaticTrading, missingField)) {
        return tradeDraftSummaryMappingFailure(
            "TradeDraft read-only summary payload missing field: " + missingField);
    }

    result.draftId = draftId;
    return DataServiceClientResult<TradeDraftReadOnlySummaryResult>::success(std::move(result));
}

DataServiceClientResult<OtcMapMultiChannelDraftResult>
parseOtcMapMultiChannelDraftPayloadJson(
    const std::string& json,
    bool protocolSuccess)
{
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return DataServiceClientResult<OtcMapMultiChannelDraftResult>::failure(
            etfdt::protocol::ErrorCode::E1001_INVALID_JSON,
            "Invalid OTCMap multi-channel payload JSON");
    }

    const auto object = document.object();
    OtcMapMultiChannelDraftResult result;
    result.protocolSuccess = protocolSuccess;

    result.action = stringField(object, "action").value_or({});
    result.task = stringField(object, "task").value_or({});
    result.mode = stringField(object, "mode").value_or({});
    result.status = stringField(object, "status").value_or({});
    result.accepted = boolField(object, "accepted").value_or(protocolSuccess);
    result.eligibleForTradeDraft =
        boolField(object, "eligibleForTradeDraft").value_or(result.accepted);
    result.dataServiceReadOnlyActionCreated =
        boolField(object, "dataServiceReadOnlyActionCreated").value_or(false);
    result.dataServiceWriteActionCreated =
        boolField(object, "dataServiceWriteActionCreated").value_or(false);
    result.previewActionExecuted =
        boolField(object, "previewActionExecuted").value_or(result.dataServiceReadOnlyActionCreated);
    result.otcMapAcMultiChannelEngineCreated =
        boolField(object, "otcMapAcMultiChannelEngineCreated").value_or(false);
    result.multiLegTradeDraftCreated =
        boolField(object, "multiLegTradeDraftCreated").value_or(false);
    result.draftWritten = boolField(object, "draftWritten").value_or(false);
    result.legWritten = boolField(object, "legWritten").value_or(false);
    result.auditWritten = boolField(object, "auditWritten").value_or(false);
    result.transactionCommitted = boolField(object, "transactionCommitted").value_or(false);
    result.duplicateDraft = boolField(object, "duplicateDraft").value_or(false);
    result.idempotencyConflict = boolField(object, "idempotencyConflict").value_or(false);
    result.draftId = static_cast<std::int64_t>(object.value(QStringLiteral("draftId")).toDouble(0.0));
    result.auditLogId =
        static_cast<std::int64_t>(object.value(QStringLiteral("auditLogId")).toDouble(0.0));
    result.legCount = intField(object, "legCount").value_or(
        intField(object, "totalLegCount").value_or(0));
    result.draftUid = stringField(object, "draftUid").value_or({});
    result.draftSignature = stringField(object, "draftSignature").value_or({});
    result.side = stringField(object, "side").value_or({});
    result.instrumentCode = stringField(object, "instrumentCode").value_or({});
    result.totalQuantityText = stringField(object, "totalQuantityText").value_or({});
    result.totalAmountText = stringField(object, "totalAmountText").value_or({});
    result.totalNetCashImpactText = stringField(object, "totalNetCashImpactText").value_or({});
    result.quantityText = stringField(object, "quantityText").value_or(result.totalQuantityText);
    result.amountText = stringField(object, "amountText").value_or(result.totalAmountText);
    result.netCashImpactText =
        stringField(object, "netCashImpactText").value_or(result.totalNetCashImpactText);
    result.baseProtectionPassed = boolField(object, "baseProtectionPassed").value_or(false);
    result.cashLimitApplied = boolField(object, "cashLimitApplied").value_or(false);
    result.tradeLogRowsWrittenByOtcMapDraft =
        boolField(object, "tradeLogRowsWrittenByOtcMapDraft").value_or(false);
    result.cashAdjustmentRowsWrittenByOtcMapDraft =
        boolField(object, "cashAdjustmentRowsWrittenByOtcMapDraft").value_or(false);
    result.positionChangedByOtcMapDraft =
        boolField(object, "positionChangedByOtcMapDraft").value_or(false);
    result.cashChangedByOtcMapDraft =
        boolField(object, "cashChangedByOtcMapDraft").value_or(false);
    result.brokerOrderSubmitted = boolField(object, "brokerOrderSubmitted").value_or(false);
    result.networkAccess = boolField(object, "networkAccess").value_or(false);
    result.credentialAccess = boolField(object, "credentialAccess").value_or(false);
    result.endpointAccess = boolField(object, "endpointAccess").value_or(false);
    result.realOrderPlacement = boolField(object, "realOrderPlacement").value_or(false);
    result.automaticTrading = boolField(object, "automaticTrading").value_or(false);
    result.productionDbTouched = boolField(object, "productionDbTouched").value_or(false);

    const auto issueCodesValue = object.value("issueCodes");
    if (issueCodesValue.isArray()) {
        for (const auto& value : issueCodesValue.toArray()) {
            if (value.isString()) {
                result.issueCodes.push_back(value.toString().toStdString());
            }
        }
    }
    const auto issuesValue = object.value("issues");
    if (issuesValue.isArray()) {
        for (const auto& value : issuesValue.toArray()) {
            if (value.isString()) {
                result.issueCodes.push_back(value.toString().toStdString());
            } else if (value.isObject()) {
                if (const auto code = stringField(value.toObject(), "code")) {
                    result.issueCodes.push_back(*code);
                }
            }
        }
    }

    if (result.action.empty() || result.task.empty() || result.status.empty()) {
        return DataServiceClientResult<OtcMapMultiChannelDraftResult>::failure(
            etfdt::protocol::ErrorCode::E1002_MISSING_REQUIRED_FIELD,
            "OTCMap multi-channel payload is missing action, task, or status");
    }

    return DataServiceClientResult<OtcMapMultiChannelDraftResult>::success(std::move(result));
}

std::string auditAppendPayloadJson(const AuditAppendRequest& request)
{
    std::ostringstream stream;
    stream << "{"
           << "\"entityType\":" << jsonString(request.entityType) << ','
           << "\"entityId\":" << jsonString(request.entityId) << ','
           << "\"action\":" << jsonString(request.action) << ','
           << "\"reason\":" << jsonString(request.reason) << ','
           << "\"operatorName\":" << jsonString(request.operatorName) << ','
           << "\"oldValue\":" << request.oldValueJson << ','
           << "\"newValue\":" << request.newValueJson
           << "}";
    return stream.str();
}

std::string excelVbaImportPersistManualEntryPayloadJson(
    const ExcelVbaImportPersistManualEntryRequest& request)
{
    std::ostringstream stream;
    stream << "{"
           << "\"previewStatus\":" << jsonString(request.previewStatus) << ','
           << "\"previewDigest\":" << jsonString(request.previewDigest) << ','
           << "\"idempotencyKey\":" << jsonString(request.idempotencyKey) << ','
           << "\"requestId\":" << jsonString(request.requestId) << ','
           << "\"schemaVersion\":" << jsonString(request.schemaVersion) << ','
           << "\"source\":" << jsonString(request.source) << ','
           << "\"acceptedAt\":" << jsonString(request.acceptedAt) << ','
           << "\"importBatchLabel\":" << jsonString(request.importBatchLabel) << ','
           << "\"previewFactSummary\":{"
           << "\"tradeFactCount\":" << request.factSummary.tradeFactCount << ','
           << "\"cashFactCount\":" << request.factSummary.cashFactCount << ','
           << "\"marketPriceFactCount\":" << request.factSummary.marketPriceFactCount << ','
           << "\"fxRateFactCount\":" << request.factSummary.fxRateFactCount
           << "},"
           << "\"importPayload\":" << request.sanitizedImportPayloadJson
           << "}";
    return stream.str();
}

std::string portfolioReplayReadOnlySummaryPayloadJson(
    const PortfolioReplayReadOnlySummaryRequest& request)
{
    return request.replayPayloadJson.empty() ? "{}" : request.replayPayloadJson;
}

std::string strategyRecommendationReadOnlySummaryPayloadJson(
    const StrategyRecommendationReadOnlySummaryRequest& request)
{
    return request.recommendationPayloadJson.empty() ? "{}" : request.recommendationPayloadJson;
}

std::string tradeDraftCreateFromRecommendationPayloadJson(
    const TradeDraftCreateFromRecommendationRequest& request)
{
    std::ostringstream stream;
    stream << "{"
           << "\"recommendationPayloadJson\":" << jsonString(request.recommendationPayloadJson) << ','
           << "\"idempotencyKey\":" << jsonString(request.idempotencyKey) << ','
           << "\"recommendationDigest\":" << jsonString(request.recommendationDigest) << ','
           << "\"accountId\":" << jsonString(request.accountId) << ','
           << "\"portfolioId\":" << jsonString(request.portfolioId) << ','
           << "\"strategyId\":" << jsonString(request.strategyId) << ','
           << "\"instrumentId\":" << jsonString(request.instrumentId) << ','
           << "\"strategyCode\":" << jsonString(request.strategyCode) << ','
           << "\"instrumentCode\":" << jsonString(request.instrumentCode) << ','
           << "\"instrumentType\":" << jsonString(request.instrumentType) << ','
           << "\"tradeSource\":" << jsonString(request.tradeSource) << ','
           << "\"expiresAtUtc\":" << jsonString(request.expiresAtUtc) << ','
           << "\"userNote\":" << jsonString(request.userNote) << ','
           << "\"userConfirmed\":" << (request.userConfirmed ? "true" : "false")
           << "}";
    return stream.str();
}

std::string tradeDraftReadOnlySummaryPayloadJson(
    const TradeDraftReadOnlySummaryRequest& request)
{
    std::ostringstream stream;
    stream << "{"
           << "\"idempotencyKey\":" << jsonString(request.idempotencyKey) << ','
           << "\"draftId\":" << request.draftId
           << "}";
    return stream.str();
}

std::string otcMapMultiChannelPreviewPayloadJson(
    const OtcMapMultiChannelReadOnlyPreviewRequest& request)
{
    return request.payloadJson.empty() ? "{}" : request.payloadJson;
}

std::string otcMapTradeDraftCreatePayloadJson(
    const OtcMapTradeDraftCreateRequest& request)
{
    return request.payloadJson.empty() ? "{}" : request.payloadJson;
}

}  // namespace etfdt::data_service_client
