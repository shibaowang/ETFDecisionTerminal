#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    bool ok = !containsAnyToken(brokerInterfaceText(sourceRoot(argc, argv)), {
        "brokerOrderId",
        "realOrderId",
        "exchangeOrderId",
    });
    TestOnlyFakeShellAccountingBrokerOrderPort fake;
    fake.nextResponse.status = etfdt::dataservice::ShellAccountingBrokerOrderStatus::PendingPlaceholder;
    fake.nextResponse.success = true;
    fake.nextResponse.sanitizedCorrelationId = "SHOULD_BE_CLEARED";
    ok &= expect(fake.submitOrderEnvelope(validRequest()).sanitizedCorrelationId.empty(),
                 "test fake clears real order id-like correlation");
    return ok ? 0 : 1;
}
