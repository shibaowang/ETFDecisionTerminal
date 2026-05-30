#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    return containsAnyToken(brokerInterfaceText(sourceRoot(argc, argv)), {
        "brokerEndpoint",
        "brokerUrl",
        "orderEndpoint",
        "submitOrderHttp",
        "sendBrokerRequest",
        "http://",
        "https://",
        "socket",
    }) ? 1 : 0;
}
