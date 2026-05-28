#include "FakeShellAccountingServiceAdapter.h"

#include "ShellServices/ShellAccountingReadOnlyController.h"

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

bool hasIssueCode(
    const etfdt::shell_services::ShellAccountingReadOnlyController& controller,
    std::string_view code)
{
    for (const auto& issue : controller.issues()) {
        if (issue.code == code) {
            return true;
        }
    }
    return false;
}

void expectErrorState(
    const etfdt::shell_services::ShellAccountingReadOnlyController& controller,
    std::string_view message)
{
    expectTrue(
        controller.currentState() == etfdt::shell_services::ShellAccountingViewState::Error,
        message);
    expectTrue(controller.readOnly(), "error path remains readOnly=true");
    expectTrue(!controller.writeEnabled(), "error path remains writeEnabled=false");
}

}  // namespace

int main()
{
    using namespace etfdt::tests::shell_accounting_service_adapter;

    auto fakeAdapter = std::make_shared<FakeShellAccountingServiceAdapter>();
    etfdt::shell_services::ShellAccountingReadOnlyController controller;
    controller.setServiceAdapter(fakeAdapter);

    etfdt::shell_services::ShellAccountingServiceResult protocolFailure;
    protocolFailure.protocolSuccess = false;
    protocolFailure.protocolError = true;
    protocolFailure.payloadStatus = "MALFORMED_RESPONSE";
    fakeAdapter->setNextResult(protocolFailure);
    controller.refreshPositionList(makeDemoServiceRequest("position.list"));
    expectErrorState(controller, "protocolSuccess=false maps to Error");
    expectTrue(hasIssueCode(controller, "PROTOCOL_ERROR"), "protocol error issue is visible");

    etfdt::shell_services::ShellAccountingServiceResult timeoutFailure;
    timeoutFailure.protocolSuccess = true;
    timeoutFailure.timeout = true;
    fakeAdapter->setNextResult(timeoutFailure);
    controller.refreshPositionList(makeDemoServiceRequest("position.list"));
    expectErrorState(controller, "timeout maps to Error");
    expectTrue(hasIssueCode(controller, "TIMEOUT"), "timeout issue is visible");

    etfdt::shell_services::ShellAccountingServiceResult transportFailure;
    transportFailure.protocolSuccess = true;
    transportFailure.transportError = true;
    fakeAdapter->setNextResult(transportFailure);
    controller.refreshPositionList(makeDemoServiceRequest("position.list"));
    expectErrorState(controller, "transportError maps to Error");
    expectTrue(hasIssueCode(controller, "TRANSPORT_ERROR"), "transport error issue is visible");

    etfdt::shell_services::ShellAccountingServiceResult domainFailure;
    domainFailure.protocolSuccess = true;
    domainFailure.implemented = true;
    domainFailure.domainError = true;
    domainFailure.dataQualityStatus = "ERROR";
    domainFailure.issues.push_back({
        "POSITION_LIST_MAPPING_FAILED",
        "ERROR",
        "Mapping failed in fake adapter result.",
        true,
        "position.list",
    });
    fakeAdapter->setNextResult(domainFailure);
    controller.refreshPositionList(makeDemoServiceRequest("position.list"));
    expectErrorState(controller, "domainError with protocolSuccess=true maps via domain issue");
    expectTrue(
        hasIssueCode(controller, "POSITION_LIST_MAPPING_FAILED"),
        "domain issue is preserved");
    expectTrue(
        !hasIssueCode(controller, "PROTOCOL_ERROR"),
        "domain issue is not converted into protocol warning");

    return gFailures == 0 ? 0 : 1;
}
