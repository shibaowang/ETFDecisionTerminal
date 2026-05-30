#include "ShellAccountingBrokerAdapterInterfaceScaffold.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_interface;

int main(int argc, char** argv)
{
    return containsAnyToken(brokerInterfaceText(sourceRoot(argc, argv)), {
        "INSERT ",
        "UPDATE ",
        "DELETE ",
        "REPLACE ",
        "CREATE ",
        "DROP ",
        "ALTER ",
        "DatabaseConnection",
        "sqlite3",
    }) ? 1 : 0;
}
