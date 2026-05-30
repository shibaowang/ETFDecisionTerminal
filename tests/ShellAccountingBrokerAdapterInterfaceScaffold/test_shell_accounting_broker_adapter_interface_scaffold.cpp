#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    const auto text = brokerInterfaceText(sourceRoot(argc, argv));
    bool ok = containsAllTokens(text, {
        "ShellAccountingBrokerOrderPort",
        "ShellAccountingBrokerOrderRequest",
        "ShellAccountingBrokerOrderResponse",
        "ShellAccountingBrokerOrderStatus",
        "DisabledShellAccountingBrokerOrderPort",
    });
    etfdt::dataservice::DisabledShellAccountingBrokerOrderPort port;
    const auto response = port.submitOrderEnvelope(validRequest());
    ok &= expect(response.status == etfdt::dataservice::ShellAccountingBrokerOrderStatus::Disabled,
                 "disabled port returns disabled status");
    return ok ? 0 : 1;
}
