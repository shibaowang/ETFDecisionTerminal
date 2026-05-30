#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    return containsAnyToken(qmlText(sourceRoot(argc, argv)), {
        "ShellAccountingBrokerOrderPort",
        "submitOrderEnvelope",
        "accounting.broker_order.place",
        "accounting.broker_order.submit",
        "TASK-157",
    }) ? 1 : 0;
}
