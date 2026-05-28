#include "SpyAccountingDataServiceClient.h"

#include <sstream>
#include <utility>

namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding {

namespace {

std::string jsonString(const std::string& value)
{
    std::ostringstream output;
    output << '"';
    for (const char ch : value) {
        if (ch == '"' || ch == '\\') {
            output << '\\';
        }
        output << ch;
    }
    output << '"';
    return output.str();
}

std::vector<etfdt::shell_services::ShellAccountingIssue> mapIssues(
    const std::vector<SpyProtocolIssue>& issues)
{
    std::vector<etfdt::shell_services::ShellAccountingIssue> mapped;
    mapped.reserve(issues.size());
    for (const auto& issue : issues) {
        mapped.push_back({
            issue.code,
            issue.level,
            issue.message,
            issue.blocking,
            "ShellAccountingDataServiceAdapterSpy",
        });
    }
    return mapped;
}

}  // namespace

void SpyAccountingDataServiceClient::setNextResponse(SpyProtocolResponse response)
{
    nextResponse_ = std::move(response);
}

SpyProtocolResponse SpyAccountingDataServiceClient::positionList(
    std::string payloadJson,
    int timeoutMs)
{
    return recordReadOnlyCall("position.list", "positionList", std::move(payloadJson), timeoutMs);
}

SpyProtocolResponse SpyAccountingDataServiceClient::cashSummary(
    std::string payloadJson,
    int timeoutMs)
{
    return recordReadOnlyCall("cash.summary", "cashSummary", std::move(payloadJson), timeoutMs);
}

SpyProtocolResponse SpyAccountingDataServiceClient::portfolioPnlSummary(
    std::string payloadJson,
    int timeoutMs)
{
    return recordReadOnlyCall(
        "portfolio.pnl.summary",
        "portfolioPnlSummary",
        std::move(payloadJson),
        timeoutMs);
}

SpyProtocolResponse SpyAccountingDataServiceClient::basePositionSummary(
    std::string payloadJson,
    int timeoutMs)
{
    return recordReadOnlyCall(
        "base_position.summary",
        "basePositionSummary",
        std::move(payloadJson),
        timeoutMs);
}

SpyProtocolResponse SpyAccountingDataServiceClient::sniperPoolSummary(
    std::string payloadJson,
    int timeoutMs)
{
    return recordReadOnlyCall(
        "sniper_pool.summary",
        "sniperPoolSummary",
        std::move(payloadJson),
        timeoutMs);
}

void SpyAccountingDataServiceClient::dataAuditAppend()
{
    forbiddenActionCalled_ = true;
    writeActionCalled_ = true;
}

void SpyAccountingDataServiceClient::tradeDraftCreate()
{
    forbiddenActionCalled_ = true;
    writeActionCalled_ = true;
    generatedTradeDraft_ = true;
}

void SpyAccountingDataServiceClient::snapshotWrite()
{
    forbiddenActionCalled_ = true;
    writeActionCalled_ = true;
}

void SpyAccountingDataServiceClient::strategyExecute()
{
    forbiddenActionCalled_ = true;
    strategyExecuted_ = true;
    generatedTradeSuggestion_ = true;
}

void SpyAccountingDataServiceClient::brokerOrderSubmit()
{
    forbiddenActionCalled_ = true;
    brokerOrderSubmitted_ = true;
}

const std::vector<std::string>& SpyAccountingDataServiceClient::calledActions() const noexcept
{
    return calledActions_;
}

const std::vector<std::string>& SpyAccountingDataServiceClient::calledWrappers() const noexcept
{
    return calledWrappers_;
}

const std::map<std::string, std::string>&
SpyAccountingDataServiceClient::lastPayloadByAction() const noexcept
{
    return lastPayloadByAction_;
}

const std::map<std::string, int>&
SpyAccountingDataServiceClient::lastTimeoutByAction() const noexcept
{
    return lastTimeoutByAction_;
}

bool SpyAccountingDataServiceClient::forbiddenActionCalled() const noexcept
{
    return forbiddenActionCalled_;
}

bool SpyAccountingDataServiceClient::writeActionCalled() const noexcept
{
    return writeActionCalled_;
}

bool SpyAccountingDataServiceClient::generatedTradeDraft() const noexcept
{
    return generatedTradeDraft_;
}

bool SpyAccountingDataServiceClient::generatedTradeSuggestion() const noexcept
{
    return generatedTradeSuggestion_;
}

bool SpyAccountingDataServiceClient::strategyExecuted() const noexcept
{
    return strategyExecuted_;
}

bool SpyAccountingDataServiceClient::brokerOrderSubmitted() const noexcept
{
    return brokerOrderSubmitted_;
}

SpyProtocolResponse SpyAccountingDataServiceClient::recordReadOnlyCall(
    std::string actionName,
    std::string wrapperName,
    std::string payloadJson,
    int timeoutMs)
{
    calledActions_.push_back(actionName);
    calledWrappers_.push_back(std::move(wrapperName));
    lastPayloadByAction_[actionName] = std::move(payloadJson);
    lastTimeoutByAction_[actionName] = timeoutMs;

    auto response = nextResponse_;
    response.action = std::move(actionName);
    return response;
}

std::string buildFuturePayload(
    const etfdt::shell_services::ShellAccountingServiceRequest& request)
{
    std::ostringstream output;
    output << '{';
    output << "\"actionName\":" << jsonString(request.actionName) << ',';
    output << "\"accountId\":" << jsonString(request.accountId) << ',';
    output << "\"portfolioId\":" << jsonString(request.portfolioId) << ',';
    output << "\"sourceFromTime\":" << jsonString(request.sourceFromTime) << ',';
    output << "\"sourceToTime\":" << jsonString(request.sourceToTime) << ',';
    output << "\"includeIssues\":" << (request.includeIssues ? "true" : "false") << ',';
    output << "\"includeDebugMetadata\":"
           << (request.includeDebugMetadata ? "true" : "false") << ',';
    output << "\"calculationVersion\":" << jsonString(request.calculationVersion) << ',';
    output << "\"requestedOutputs\":[";
    for (std::size_t i = 0; i < request.requestedOutputs.size(); ++i) {
        if (i > 0) {
            output << ',';
        }
        output << jsonString(request.requestedOutputs[i]);
    }
    output << ']';
    output << '}';
    return output.str();
}

etfdt::shell_services::ShellAccountingServiceResult mapSpyProtocolResponse(
    const SpyProtocolResponse& response)
{
    etfdt::shell_services::ShellAccountingServiceResult result;
    result.actionName = response.action;
    result.protocolSuccess = response.success && !response.malformed &&
                             !response.timeout && !response.transportError &&
                             !response.serverUnavailable;
    result.implemented = response.implemented;
    result.readOnly = response.readOnly;
    result.writeEnabled = response.writeEnabled;
    result.payloadStatus = response.status;
    result.dataQualityStatus = response.dataQualityStatus;
    result.issues = mapIssues(response.issues);
    result.warnings = mapIssues(response.warnings);
    result.errors = mapIssues(response.errors);
    result.rawPayload = response.rawPayload;
    result.timeout = response.timeout;
    result.transportError = response.transportError || response.serverUnavailable;
    result.protocolError = !response.success || response.malformed;
    result.domainError = !result.issues.empty() || !result.warnings.empty() ||
                         !result.errors.empty() || !response.status.empty();
    return result;
}

}  // namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding
