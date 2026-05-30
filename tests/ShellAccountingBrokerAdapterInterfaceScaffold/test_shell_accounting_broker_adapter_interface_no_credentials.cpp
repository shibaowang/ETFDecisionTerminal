#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    return containsAnyToken(brokerInterfaceText(sourceRoot(argc, argv)), {
        "BROKER_API_KEY",
        "BROKER_SECRET",
        "BROKER_PASSWORD",
        "apiSecret",
        "secretKey",
        "privateKey",
        "password",
    }) ? 1 : 0;
}
