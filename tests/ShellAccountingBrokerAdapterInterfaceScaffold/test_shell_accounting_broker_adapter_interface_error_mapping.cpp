#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    return containsAllTokens(brokerInterfaceText(sourceRoot(argc, argv)), {
        "Disabled",
        "DryRunOnly",
        "Rejected",
        "AcceptedPlaceholder",
        "PendingPlaceholder",
        "UnknownStatePlaceholder",
        "Error",
        "errorCode",
        "sanitizedMessage",
    }) ? 0 : 1;
}
