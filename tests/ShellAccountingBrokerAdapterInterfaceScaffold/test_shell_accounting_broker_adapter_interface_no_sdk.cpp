#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    return containsAnyToken(brokerInterfaceText(sourceRoot(argc, argv)), {
        "BrokerSdk",
        "BrokerSDK",
        "#include <broker",
        "ctp_api",
        "xtp_api",
        "ibapi",
    }) ? 1 : 0;
}
