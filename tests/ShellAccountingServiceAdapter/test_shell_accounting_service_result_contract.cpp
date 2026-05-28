#include "ShellServices/ShellAccountingServiceTypes.h"

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

}  // namespace

int main()
{
    etfdt::shell_services::ShellAccountingServiceResult defaultResult;
    expectTrue(defaultResult.readOnly, "default readOnly=true");
    expectTrue(!defaultResult.writeEnabled, "default writeEnabled=false");
    expectTrue(!defaultResult.generatedTradeDraft, "default generatedTradeDraft=false");
    expectTrue(!defaultResult.generatedTradeSuggestion, "default generatedTradeSuggestion=false");
    expectTrue(!defaultResult.strategyExecuted, "default strategyExecuted=false");
    expectTrue(!defaultResult.brokerOrderSubmitted, "default brokerOrderSubmitted=false");
    expectTrue(
        etfdt::shell_services::isShellAccountingServiceResultReadOnly(defaultResult),
        "default result satisfies read-only contract");

    etfdt::shell_services::ShellAccountingServiceResult protocolFailure;
    protocolFailure.protocolSuccess = false;
    protocolFailure.protocolError = true;
    protocolFailure.payloadStatus = "MALFORMED_RESPONSE";
    expectTrue(!protocolFailure.protocolSuccess, "protocolSuccess=false expresses protocol error");
    expectTrue(protocolFailure.protocolError, "protocolError flag is preserved");
    expectTrue(!protocolFailure.domainError, "protocol error remains distinct from domain issue");

    etfdt::shell_services::ShellAccountingServiceResult guardUnavailable;
    guardUnavailable.protocolSuccess = true;
    guardUnavailable.implemented = false;
    guardUnavailable.payloadStatus = "POSITION_LIST_NOT_AVAILABLE";
    guardUnavailable.domainError = true;
    guardUnavailable.issues.push_back({
        "POSITION_LIST_NOT_AVAILABLE",
        "ERROR",
        "position.list is not implemented.",
        true,
        "position.list",
    });
    expectTrue(guardUnavailable.protocolSuccess, "guard domain issue can have protocolSuccess=true");
    expectTrue(!guardUnavailable.implemented, "implemented=false can express guard unavailable");
    expectTrue(guardUnavailable.payloadStatus == "POSITION_LIST_NOT_AVAILABLE", "payloadStatus preserves guard status");
    expectTrue(!guardUnavailable.issues.empty(), "issues are retained");
    expectTrue(guardUnavailable.issues.front().code == "POSITION_LIST_NOT_AVAILABLE", "issue code is retained");
    expectTrue(guardUnavailable.issues.front().level == "ERROR", "issue level is retained");
    expectTrue(!guardUnavailable.issues.front().message.empty(), "issue message is retained");
    expectTrue(guardUnavailable.issues.front().blocking, "issue blocking flag is retained");

    etfdt::shell_services::ShellAccountingServiceResult timeoutResult;
    timeoutResult.protocolSuccess = false;
    timeoutResult.timeout = true;
    timeoutResult.transportError = true;
    expectTrue(timeoutResult.timeout, "timeout can be represented");
    expectTrue(timeoutResult.transportError, "transportError can be represented");

    return gFailures == 0 ? 0 : 1;
}
