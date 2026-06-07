#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"

#include "DataServiceClient/DataServiceClient.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include <optional>
#include <string>
#include <utility>

namespace etfdt::shell_services {

namespace {

constexpr const char* kClientNotConfiguredStatus = "DATASERVICE_CLIENT_NOT_CONFIGURED";
constexpr const char* kClientCallFailedStatus = "DATASERVICE_CLIENT_CALL_FAILED";
constexpr const char* kMalformedPayloadStatus = "DATASERVICE_CLIENT_MALFORMED_PAYLOAD";
constexpr const char* kUnavailableQuality = "UNAVAILABLE";
constexpr const char* kErrorQuality = "ERROR";
constexpr const char* kSource = "ShellAccountingDataServiceClientPortAdapter";

ShellAccountingIssue makeIssue(
    std::string code,
    std::string level,
    std::string message,
    bool blocking)
{
    return {
        std::move(code),
        std::move(level),
        std::move(message),
        blocking,
        kSource,
    };
}

ShellAccountingDataServiceClientResponse makeUnavailableResponse(
    const ShellAccountingDataServiceClientRequest& request,
    std::string status,
    std::string message,
    bool transportError,
    bool timeout)
{
    ShellAccountingDataServiceClientResponse response;
    response.actionName = request.actionName;
    response.protocolSuccess = false;
    response.implemented = false;
    response.readOnly = true;
    response.writeEnabled = false;
    response.payloadStatus = status;
    response.dataQualityStatus = kUnavailableQuality;
    response.timeout = timeout;
    response.transportError = transportError;
    response.protocolError = !transportError && !timeout;
    response.domainError = false;
    response.errorMessage = message;
    response.issues.push_back(makeIssue(status, "ERROR", std::move(message), true));
    return response;
}

std::optional<bool> boolField(const QJsonObject& object, const QString& key)
{
    const auto value = object.value(key);
    if (!value.isBool()) {
        return std::nullopt;
    }
    return value.toBool();
}

std::string stringField(const QJsonObject& object, const QString& key, std::string fallback = {})
{
    const auto value = object.value(key);
    return value.isString() ? value.toString().toStdString() : std::move(fallback);
}

ShellAccountingIssue issueFromObject(const QJsonObject& object, const char* fallbackLevel)
{
    const auto code = stringField(object, "code", "UNKNOWN_ISSUE");
    const auto level = stringField(object, "level", fallbackLevel);
    const auto message = stringField(object, "message", code);
    const auto blocking = boolField(object, "blocking").value_or(level == "ERROR");
    const auto sourceId = stringField(object, "sourceId", kSource);
    return {code, level, message, blocking, sourceId};
}

void appendIssueArray(
    std::vector<ShellAccountingIssue>& target,
    const QJsonValue& value,
    const char* fallbackLevel,
    bool blocking)
{
    if (!value.isArray()) {
        return;
    }

    const auto array = value.toArray();
    for (const auto& item : array) {
        if (item.isObject()) {
            target.push_back(issueFromObject(item.toObject(), fallbackLevel));
            continue;
        }
        if (item.isString()) {
            const auto text = item.toString().toStdString();
            target.push_back(makeIssue(text, fallbackLevel, text, blocking));
        }
    }
}

ShellAccountingDataServiceClientResponse mapProtocolResponse(
    const ShellAccountingDataServiceClientRequest& request,
    const etfdt::protocol::ProtocolResponse& protocolResponse)
{
    ShellAccountingDataServiceClientResponse response;
    response.actionName = request.actionName;
    response.protocolSuccess = protocolResponse.success;
    response.readOnly = true;
    response.writeEnabled = false;
    response.rawPayload = protocolResponse.payloadJson;
    response.errorMessage = protocolResponse.errorMessage;

    if (!protocolResponse.success) {
        response.protocolError = true;
        response.payloadStatus = kClientCallFailedStatus;
        response.dataQualityStatus = kErrorQuality;
        response.issues.push_back(makeIssue(
            kClientCallFailedStatus,
            "ERROR",
            protocolResponse.errorMessage.empty()
                ? "DataServiceClient returned an unsuccessful ProtocolResponse."
                : protocolResponse.errorMessage,
            true));
        return response;
    }

    QJsonParseError parseError;
    const auto document =
        QJsonDocument::fromJson(QByteArray::fromStdString(protocolResponse.payloadJson), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        response.protocolSuccess = false;
        response.protocolError = true;
        response.payloadStatus = kMalformedPayloadStatus;
        response.dataQualityStatus = kErrorQuality;
        response.issues.push_back(makeIssue(
            kMalformedPayloadStatus,
            "ERROR",
            "DataServiceClient returned a malformed accounting payload.",
            true));
        return response;
    }

    const auto object = document.object();
    response.actionName = stringField(object, "action", request.actionName);
    response.implemented = boolField(object, "implemented").value_or(false);
    response.readOnly = boolField(object, "readOnly").value_or(true);
    response.writeEnabled = boolField(object, "writeEnabled").value_or(false);
    response.payloadStatus = stringField(object, "status", response.implemented ? "OK" : kUnavailableQuality);
    response.dataQualityStatus = stringField(
        object,
        "dataQualityStatus",
        response.implemented ? "OK" : kUnavailableQuality);
    response.hasRows = boolField(object, "hasRows").value_or(false);
    response.domainError = !response.implemented || response.dataQualityStatus == kErrorQuality
        || response.dataQualityStatus == kUnavailableQuality;

    appendIssueArray(response.issues, object.value("issues"), "ERROR", true);
    appendIssueArray(response.warnings, object.value("warnings"), "WARNING", false);
    appendIssueArray(response.errors, object.value("errors"), "ERROR", true);
    return response;
}

ShellAccountingDataServiceClientResponse mapClientResult(
    const ShellAccountingDataServiceClientRequest& request,
    const etfdt::data_service_client::DataServiceClientResult<etfdt::protocol::ProtocolResponse>& result)
{
    if (result) {
        return mapProtocolResponse(request, result.value());
    }

    const auto message = result.error().message.empty()
        ? "DataServiceClient accounting wrapper call failed."
        : result.error().message;
    const bool timeout = result.error().errorCode == etfdt::protocol::ErrorCode::E9002_HEARTBEAT_TIMEOUT;
    const bool transport = result.error().errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE;
    return makeUnavailableResponse(request, kClientCallFailedStatus, message, transport, timeout);
}

ShellAccountingImportPreviewDiagnostic mapPreviewDiagnostic(
    const etfdt::data_service_client::ExcelVbaImportReadOnlyPreviewDiagnostic& diagnostic)
{
    return {
        diagnostic.level,
        diagnostic.code,
        diagnostic.field,
        diagnostic.sheetName,
        diagnostic.rowId,
    };
}

ShellAccountingDataServiceClientResponse mapPreviewClientResult(
    const ShellAccountingDataServiceClientRequest& request,
    const etfdt::data_service_client::DataServiceClientResult<
        etfdt::data_service_client::ExcelVbaImportReadOnlyPreviewResult>& result)
{
    if (!result) {
        const auto message = result.error().message.empty()
            ? "DataServiceClient Excel/VBA import preview call failed."
            : result.error().message;
        const bool timeout =
            result.error().errorCode == etfdt::protocol::ErrorCode::E9002_HEARTBEAT_TIMEOUT;
        const bool transport =
            result.error().errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE;
        auto response = makeUnavailableResponse(
            request,
            kClientCallFailedStatus,
            message,
            transport,
            timeout);
        response.importPreviewRejected = true;
        return response;
    }

    const auto& preview = result.value();
    ShellAccountingDataServiceClientResponse response;
    response.actionName = preview.action.empty() ? request.actionName : preview.action;
    response.protocolSuccess = true;
    response.implemented = true;
    response.readOnly = true;
    response.writeEnabled = false;
    response.payloadStatus = preview.accepted ? "ACCEPTED" : "REJECTED";
    response.dataQualityStatus = preview.accepted ? "OK" : kErrorQuality;
    response.hasRows =
        preview.replayFactSummary.tradeFactCount > 0 ||
        preview.replayFactSummary.cashFactCount > 0 ||
        preview.replayFactSummary.marketPriceFactCount > 0 ||
        preview.replayFactSummary.fxRateFactCount > 0;
    response.domainError = !preview.accepted;
    response.importPreviewAccepted = preview.accepted;
    response.importPreviewRejected = !preview.accepted;
    response.importPreviewDiagnosticCodes = preview.diagnosticCodes;
    response.importPreviewFactSummary = {
        preview.replayFactSummary.tradeFactCount,
        preview.replayFactSummary.cashFactCount,
        preview.replayFactSummary.marketPriceFactCount,
        preview.replayFactSummary.fxRateFactCount,
    };
    response.accountingEngineCalled = preview.accountingEngineCalled;
    response.productionWrite = preview.productionWrite;
    response.sqliteProductionWrite = preview.sqliteProductionWrite;
    response.auditWritten = preview.auditWritten;
    response.ledgerWritten = preview.ledgerWritten;
    response.snapshotWritten = preview.snapshotWritten;
    response.tradeLogWritten = preview.tradeLogWritten;
    response.readModelPersistentWrite = preview.readModelPersistentWrite;
    response.networkAccess = preview.networkAccess;
    response.credentialAccess = preview.credentialAccess;
    response.endpointAccess = preview.endpointAccess;
    response.automaticTrading = preview.automaticTrading;
    response.rawUserDataExposed = preview.rawUserDataExposed;

    for (const auto& diagnostic : preview.diagnostics) {
        const auto mapped = mapPreviewDiagnostic(diagnostic);
        response.importPreviewDiagnostics.push_back(mapped);
        const auto code = mapped.code.empty() ? "IMPORT_PREVIEW_DIAGNOSTIC" : mapped.code;
        const auto level = mapped.level.empty() ? "ERROR" : mapped.level;
        const bool blocking = level == "ERROR";
        response.issues.push_back(makeIssue(
            code,
            level,
            code,
            blocking));
    }
    if (!preview.accepted && response.issues.empty()) {
        response.issues.push_back(makeIssue(
            "IMPORT_PREVIEW_REJECTED",
            "ERROR",
            "Excel/VBA import preview was rejected.",
            true));
    }
    return response;
}

std::optional<etfdt::data_service_client::ExcelVbaImportPersistManualEntryRequest>
parsePersistManualEntryRequestPayload(const std::string& payloadJson)
{
    QJsonParseError parseError;
    const auto document =
        QJsonDocument::fromJson(QByteArray::fromStdString(payloadJson), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return std::nullopt;
    }

    const auto object = document.object();
    const auto factSummaryValue = object.value(QStringLiteral("previewFactSummary"));
    const auto importPayloadValue = object.value(QStringLiteral("importPayload"));
    if (!factSummaryValue.isObject() || !importPayloadValue.isObject()) {
        return std::nullopt;
    }

    const auto factSummary = factSummaryValue.toObject();
    etfdt::data_service_client::ExcelVbaImportPersistManualEntryRequest request;
    request.previewStatus = stringField(object, "previewStatus");
    request.previewDigest = stringField(object, "previewDigest");
    request.idempotencyKey = stringField(object, "idempotencyKey");
    request.requestId = stringField(object, "requestId");
    request.schemaVersion = stringField(object, "schemaVersion");
    request.source = stringField(object, "source");
    request.acceptedAt = stringField(object, "acceptedAt");
    request.importBatchLabel = stringField(object, "importBatchLabel");
    request.factSummary.tradeFactCount =
        factSummary.value(QStringLiteral("tradeFactCount")).toInt();
    request.factSummary.cashFactCount =
        factSummary.value(QStringLiteral("cashFactCount")).toInt();
    request.factSummary.marketPriceFactCount =
        factSummary.value(QStringLiteral("marketPriceFactCount")).toInt();
    request.factSummary.fxRateFactCount =
        factSummary.value(QStringLiteral("fxRateFactCount")).toInt();
    request.sanitizedImportPayloadJson =
        QJsonDocument(importPayloadValue.toObject()).toJson(QJsonDocument::Compact).toStdString();
    return request;
}

QJsonObject persistResultObject(
    const etfdt::data_service_client::ExcelVbaImportPersistManualEntryResult& persist)
{
    QJsonArray issues;
    for (const auto& issue : persist.issues) {
        issues.push_back(QString::fromStdString(issue));
    }
    return QJsonObject{
        {"action", QString::fromStdString(persist.action)},
        {"task", QString::fromStdString(persist.task)},
        {"status", QString::fromStdString(persist.status)},
        {"transactionCommitted", persist.transactionCommitted},
        {"tradeLogWritten", persist.tradeLogWritten},
        {"auditLogWritten", persist.auditLogWritten},
        {"duplicateImportPrevented", persist.duplicateImportPrevented},
        {"idempotencyConflictRejected", persist.idempotencyConflictRejected},
        {"idempotencyRequired", persist.idempotencyRequired},
        {"tradeLogRowsWritten", persist.tradeLogRowsWritten},
        {"auditLogId", static_cast<double>(persist.auditLogId)},
        {"issues", issues},
    };
}

ShellAccountingDataServiceClientResponse mapPersistManualEntryClientResult(
    const ShellAccountingDataServiceClientRequest& request,
    const etfdt::data_service_client::DataServiceClientResult<
        etfdt::data_service_client::ExcelVbaImportPersistManualEntryResult>& result)
{
    if (!result) {
        const auto message = result.error().message.empty()
            ? "DataServiceClient Excel/VBA import persist call failed."
            : result.error().message;
        const bool timeout =
            result.error().errorCode == etfdt::protocol::ErrorCode::E9002_HEARTBEAT_TIMEOUT;
        const bool transport =
            result.error().errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE;
        return makeUnavailableResponse(
            request,
            kClientCallFailedStatus,
            message,
            transport,
            timeout);
    }

    const auto& persist = result.value();
    ShellAccountingDataServiceClientResponse response;
    response.actionName = persist.action.empty() ? request.actionName : persist.action;
    response.protocolSuccess = persist.protocolSuccess;
    response.implemented = true;
    response.readOnly = false;
    response.writeEnabled = true;
    response.payloadStatus = persist.status.empty() ? kClientCallFailedStatus : persist.status;
    response.dataQualityStatus = persist.protocolSuccess ? "OK" : kErrorQuality;
    response.hasRows = persist.tradeLogRowsWritten > 0;
    response.domainError = !persist.protocolSuccess;
    response.transactionCommitted = persist.transactionCommitted;
    response.duplicateImportPrevented = persist.duplicateImportPrevented;
    response.idempotencyConflictRejected = persist.idempotencyConflictRejected;
    response.idempotencyRequired = persist.idempotencyRequired;
    response.auditWritten = persist.auditLogWritten;
    response.tradeLogWritten = persist.tradeLogWritten;
    response.accountingEngineCalled = persist.accountingEngineCalled;
    response.productionWrite = !persist.tempDbOnly || persist.productionDbTouched;
    response.sqliteProductionWrite = persist.productionDbTouched;
    response.networkAccess = persist.networkAccess;
    response.credentialAccess = persist.credentialAccess;
    response.endpointAccess = persist.endpointAccess;
    response.automaticTrading = persist.automaticTrading;
    response.rawPayload =
        QJsonDocument(persistResultObject(persist)).toJson(QJsonDocument::Compact).toStdString();

    for (const auto& issue : persist.issues) {
        response.issues.push_back(makeIssue(issue, "ERROR", issue, true));
    }
    if (!persist.protocolSuccess && response.issues.empty()) {
        response.issues.push_back(makeIssue(
            response.payloadStatus,
            "ERROR",
            response.payloadStatus,
            true));
    }
    return response;
}

}  // namespace

ShellAccountingDataServiceClientPortAdapter::ShellAccountingDataServiceClientPortAdapter(
    std::shared_ptr<etfdt::data_service_client::DataServiceClient> client)
    : client_(std::move(client))
{
}

bool ShellAccountingDataServiceClientPortAdapter::hasClient() const noexcept
{
    return static_cast<bool>(client_);
}

bool ShellAccountingDataServiceClientPortAdapter::readOnly() const noexcept
{
    return true;
}

bool ShellAccountingDataServiceClientPortAdapter::writeEnabled() const noexcept
{
    return false;
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callPositionList(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting port.",
            true,
            false);
    }
    return mapClientResult(request, client_->positionList(request.payloadJson, request.timeoutMs));
}

ShellAccountingDataServiceClientResponse ShellAccountingDataServiceClientPortAdapter::callCashSummary(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting port.",
            true,
            false);
    }
    return mapClientResult(request, client_->cashSummary(request.payloadJson, request.timeoutMs));
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callPortfolioPnlSummary(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting port.",
            true,
            false);
    }
    return mapClientResult(
        request,
        client_->portfolioPnlSummary(request.payloadJson, request.timeoutMs));
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callBasePositionSummary(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting port.",
            true,
            false);
    }
    return mapClientResult(
        request,
        client_->basePositionSummary(request.payloadJson, request.timeoutMs));
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callSniperPoolSummary(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting port.",
            true,
            false);
    }
    return mapClientResult(request, client_->sniperPoolSummary(request.payloadJson, request.timeoutMs));
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callTradeDraftCreate(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting port.",
            true,
            false);
    }
    return mapClientResult(
        request,
        client_->sendRaw(client_->makeRequest(request.actionName, request.payloadJson), request.timeoutMs));
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callTradeDraftConfirm(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting port.",
            true,
            false);
    }
    return mapClientResult(
        request,
        client_->sendRaw(client_->makeRequest(request.actionName, request.payloadJson), request.timeoutMs));
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callManualTransactionCreate(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting port.",
            true,
            false);
    }
    return mapClientResult(
        request,
        client_->sendRaw(client_->makeRequest(request.actionName, request.payloadJson), request.timeoutMs));
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callManualCashMovementCreate(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting port.",
            true,
            false);
    }
    return mapClientResult(
        request,
        client_->sendRaw(client_->makeRequest(request.actionName, request.payloadJson), request.timeoutMs));
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callExcelVbaImportReadOnlyPreview(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        auto response = makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting preview port.",
            true,
            false);
        response.importPreviewRejected = true;
        return response;
    }
    return mapPreviewClientResult(
        request,
        client_->accountingExcelVbaImportReadOnlyPreview(request.payloadJson, request.timeoutMs));
}

ShellAccountingDataServiceClientResponse
ShellAccountingDataServiceClientPortAdapter::callExcelVbaImportPersistManualEntry(
    const ShellAccountingDataServiceClientRequest& request)
{
    if (!client_) {
        return makeUnavailableResponse(
            request,
            kClientNotConfiguredStatus,
            "DataServiceClient is not configured for Shell accounting persist port.",
            true,
            false);
    }
    const auto parsed = parsePersistManualEntryRequestPayload(request.payloadJson);
    if (!parsed) {
        return makeUnavailableResponse(
            request,
            kMalformedPayloadStatus,
            "Shell accounting persist request payload is malformed.",
            false,
            false);
    }
    return mapPersistManualEntryClientResult(
        request,
        client_->accountingExcelVbaImportPersistManualEntry(*parsed, request.timeoutMs));
}

}  // namespace etfdt::shell_services
