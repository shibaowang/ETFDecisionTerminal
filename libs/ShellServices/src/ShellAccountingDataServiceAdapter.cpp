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
    result.brokerOrderSubmitted = false;
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
