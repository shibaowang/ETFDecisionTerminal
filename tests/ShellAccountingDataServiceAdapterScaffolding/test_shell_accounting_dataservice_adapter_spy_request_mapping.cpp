#include "SpyAccountingDataServiceClient.h"

#include <iostream>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

bool contains(std::string_view text, std::string_view needle)
{
    return text.find(needle) != std::string_view::npos;
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_dataservice_adapter_scaffolding;

    etfdt::shell_services::ShellAccountingServiceRequest request;
    request.actionName = "position.list";
    request.accountId = "ACC-DEMO-001";
    request.portfolioId = "PF-DEMO-001";
    request.sourceFromTime = "2026-01-01T00:00:00Z";
    request.sourceToTime = "2026-01-31T23:59:59Z";
    request.includeIssues = true;
    request.includeDebugMetadata = true;
    request.calculationVersion = "accounting-replay-v0";
    request.requestedOutputs = {"positions", "issues"};
    request.timeoutMs = 5000;

    const auto payload = buildFuturePayload(request);
    expectTrue(payload.front() == '{' && payload.back() == '}', "future payload is JSON object text");
    expectTrue(contains(payload, "\"accountId\":\"ACC-DEMO-001\""), "accountId is retained");
    expectTrue(contains(payload, "\"portfolioId\":\"PF-DEMO-001\""), "portfolioId is retained");
    expectTrue(contains(payload, "\"sourceFromTime\":\"2026-01-01T00:00:00Z\""), "sourceFromTime is retained");
    expectTrue(contains(payload, "\"sourceToTime\":\"2026-01-31T23:59:59Z\""), "sourceToTime is retained");
    expectTrue(contains(payload, "\"includeIssues\":true"), "includeIssues is retained");
    expectTrue(contains(payload, "\"includeDebugMetadata\":true"), "includeDebugMetadata is retained");
    expectTrue(contains(payload, "\"calculationVersion\":\"accounting-replay-v0\""), "calculationVersion is retained");
    expectTrue(contains(payload, "\"positions\""), "requestedOutputs includes positions");
    expectTrue(contains(payload, "\"issues\""), "requestedOutputs includes issues");
    expectTrue(!contains(payload, "\"writeEnabled\":true"), "payload does not request writeEnabled=true");
    expectTrue(!contains(payload, "TradeDraft"), "payload does not include TradeDraft");
    expectTrue(!contains(payload, "tradeSuggestion"), "payload does not include trade suggestion");
    expectTrue(!contains(payload, "strategyExecution"), "payload does not include strategy execution");
    expectTrue(!contains(payload, "brokerOrder"), "payload does not include broker order");

    SpyAccountingDataServiceClient spy;
    (void)spy.positionList(payload, request.timeoutMs);
    expectTrue(spy.lastPayloadByAction().at("position.list") == payload, "spy stores payload by action");
    expectTrue(spy.lastTimeoutByAction().at("position.list") == 5000, "timeoutMs maps to wrapper timeout");
    expectTrue(!spy.writeActionCalled(), "request mapping does not write");

    SpyProtocolResponse invalidRequest;
    invalidRequest.success = false;
    invalidRequest.status = "INVALID_REQUEST";
    auto mapped = mapSpyProtocolResponse(invalidRequest);
    expectTrue(!mapped.protocolSuccess, "invalid request can map to protocol/domain error");
    expectTrue(!mapped.generatedTradeDraft, "invalid request does not generate TradeDraft");

    return gFailures == 0 ? 0 : 1;
}
