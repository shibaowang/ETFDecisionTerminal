#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "ShellAccountingDataServiceClientPortAdapterTestUtils.h"

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
    etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter adapter;
    expectTrue(!adapter.hasClient(), "null construction hasClient=false");
    expectTrue(adapter.readOnly(), "adapter readOnly=true");
    expectTrue(!adapter.writeEnabled(), "adapter writeEnabled=false");

    const auto response = adapter.callPositionList(
        etfdt::shell_services::tests::makePortAdapterRequest("position.list"));
    expectTrue(!response.protocolSuccess, "null client response is protocol failure");
    expectTrue(response.transportError, "null client response is transport/config unavailable");
    expectTrue(response.payloadStatus == "DATASERVICE_CLIENT_NOT_CONFIGURED", "null client status");
    expectTrue(response.readOnly, "null client readOnly=true");
    expectTrue(!response.writeEnabled, "null client writeEnabled=false");
    expectTrue(etfdt::shell_services::tests::containsIssueCode(response, "DATASERVICE_CLIENT_NOT_CONFIGURED"), "null client issue visible");

    return gFailures == 0 ? 0 : 1;
}
