#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "SpyShellAccountingDataServiceClientPort.h"

#include <iostream>
#include <memory>
#include <string>
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

void expectContains(const std::string& text, std::string_view expected)
{
    expectTrue(text.find(expected) != std::string::npos, "payload contains expected field");
}

void expectNotContains(const std::string& text, std::string_view forbidden)
{
    expectTrue(text.find(forbidden) == std::string::npos, "payload omits forbidden field");
}

}  // namespace

int main()
{
    using etfdt::shell_services::ShellAccountingDataServiceAdapter;
    using etfdt::shell_services::tests::SpyShellAccountingDataServiceClientPort;
    using etfdt::shell_services::tests::makeLiveCallSkeletonRequest;
    using etfdt::shell_services::tests::makeOkClientResponse;

    auto spy = std::make_shared<SpyShellAccountingDataServiceClientPort>();
    spy->setNextResponse(makeOkClientResponse("position.list"));
    ShellAccountingDataServiceAdapter adapter(spy);

    static_cast<void>(adapter.fetchPositionList(makeLiveCallSkeletonRequest("position.list")));

    const auto& portRequest = spy->lastRequest();
    const auto& payload = portRequest.payloadJson;
    expectTrue(portRequest.actionName == "position.list", "actionName maps to port request");
    expectTrue(portRequest.timeoutMs == 1500, "timeoutMs maps to port request");
    expectTrue(!payload.empty() && payload.front() == '{' && payload.back() == '}', "payload is object-like JSON");

    expectContains(payload, "\"accountId\":\"ACC-001\"");
    expectContains(payload, "\"portfolioId\":\"PF-001\"");
    expectContains(payload, "\"sourceFromTime\":\"2026-01-01T00:00:00Z\"");
    expectContains(payload, "\"sourceToTime\":\"2026-01-31T23:59:59Z\"");
    expectContains(payload, "\"includeIssues\":true");
    expectContains(payload, "\"includeDebugMetadata\":true");
    expectContains(payload, "\"calculationVersion\":\"calc-v1\"");
    expectContains(payload, "\"requestedOutputs\":[\"positions\",\"issues\"]");
    expectContains(payload, "\"timeoutMs\":1500");

    expectNotContains(payload, "\"writeEnabled\":true");
    expectNotContains(payload, "TradeDraft");
    expectNotContains(payload, "tradeSuggestion");
    expectNotContains(payload, "strategy");
    expectNotContains(payload, "broker");

    return gFailures == 0 ? 0 : 1;
}
