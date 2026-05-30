#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    TestOnlyFakeShellAccountingBrokerOrderPort fake;
    fake.nextResponse.status = etfdt::dataservice::ShellAccountingBrokerOrderStatus::AcceptedPlaceholder;
    fake.nextResponse.success = true;
    fake.nextResponse.disabled = false;
    fake.nextResponse.dryRunOnly = true;
    fake.nextResponse.errorCode.clear();
    fake.nextResponse.sanitizedMessage = "Accepted placeholder from test-only fake.";

    const auto response = fake.submitOrderEnvelope(validRequest());
    bool ok = expect(fake.callCount == 1, "fake adapter records one test-only call");
    ok &= expect(response.status == etfdt::dataservice::ShellAccountingBrokerOrderStatus::AcceptedPlaceholder,
                 "fake adapter can return accepted placeholder");
    ok &= expect(brokerInterfaceText(sourceRoot(argc, argv)).find("TestOnlyFakeShellAccountingBrokerOrderPort") ==
                     std::string::npos,
                 "fake adapter is not in production interface source");
    return ok ? 0 : 1;
}
