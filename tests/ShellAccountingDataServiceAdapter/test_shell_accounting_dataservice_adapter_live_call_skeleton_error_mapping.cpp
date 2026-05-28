#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "SpyShellAccountingDataServiceClientPort.h"

#include <iostream>
#include <memory>
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

etfdt::shell_services::ShellAccountingServiceResult mapResponse(
    etfdt::shell_services::ShellAccountingDataServiceClientResponse response)
{
    auto spy =
        std::make_shared<etfdt::shell_services::tests::SpyShellAccountingDataServiceClientPort>();
    spy->setNextResponse(std::move(response));
    etfdt::shell_services::ShellAccountingDataServiceAdapter adapter(spy);
    return adapter.fetchPositionList(
        etfdt::shell_services::tests::makeLiveCallSkeletonRequest("position.list"));
}

}  // namespace

int main()
{
    using etfdt::shell_services::ShellAccountingDataServiceClientResponse;
    using etfdt::shell_services::tests::makePortIssue;

    ShellAccountingDataServiceClientResponse protocol;
    protocol.actionName = "position.list";
    protocol.protocolSuccess = false;
    protocol.protocolError = true;
    protocol.rawPayload = "{malformed";
    expectTrue(!mapResponse(protocol).protocolSuccess, "protocolSuccess=false maps");
    expectTrue(mapResponse(protocol).protocolError, "protocolError maps");

    ShellAccountingDataServiceClientResponse timeout;
    timeout.actionName = "position.list";
    timeout.timeout = true;
    expectTrue(mapResponse(timeout).timeout, "timeout maps");

    ShellAccountingDataServiceClientResponse transport;
    transport.actionName = "position.list";
    transport.transportError = true;
    expectTrue(mapResponse(transport).transportError, "transportError maps");

    ShellAccountingDataServiceClientResponse issues;
    issues.actionName = "position.list";
    issues.protocolSuccess = true;
    issues.implemented = true;
    issues.readOnly = true;
    issues.writeEnabled = false;
    issues.payloadStatus = "OK";
    issues.dataQualityStatus = "WARNING";
    issues.issues.push_back(makePortIssue("MARKET_PRICE_MISSING", "WARNING", false));
    issues.issues.push_back(makePortIssue("MULTI_CURRENCY_UNSUPPORTED", "ERROR", true));
    issues.issues.push_back(makePortIssue("FX_RATE_MISSING", "ERROR", true));
    issues.issues.push_back(makePortIssue("REPLAY_NOT_AVAILABLE", "ERROR", true));
    const auto issueResult = mapResponse(issues);
    expectTrue(issueResult.issues.size() == 4, "all domain issues are retained");
    expectTrue(issueResult.issues.front().code == "MARKET_PRICE_MISSING", "warning issue retained");
    expectTrue(issueResult.issues.back().code == "REPLAY_NOT_AVAILABLE", "blocking replay issue retained");
    expectTrue(issueResult.issues.back().blocking, "blocking flag retained");

    return gFailures == 0 ? 0 : 1;
}
