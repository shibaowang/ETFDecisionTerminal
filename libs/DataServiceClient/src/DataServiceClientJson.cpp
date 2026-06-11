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

}  // namespace etfdt::data_service_client
