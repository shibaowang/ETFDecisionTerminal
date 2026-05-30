#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    return containsAnyToken(shellServicesText(sourceRoot(argc, argv)), {
        "ShellAccountingBrokerOrderPort",
        "submitOrderEnvelope",
        "accounting.broker_order.place",
        "accounting.broker_order.submit",
    }) ? 1 : 0;
}
