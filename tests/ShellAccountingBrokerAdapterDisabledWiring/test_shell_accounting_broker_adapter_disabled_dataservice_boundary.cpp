#include "ShellAccountingBrokerAdapterDisabledWiring.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAllTokens(dataServiceApiText(root), {
        "defaultShellAccountingBrokerOrderPort",
        "brokerOrderPortRequestFromDryRun",
    }) && !containsAnyToken(qmlText(root) + presenterText(root), {
        "defaultShellAccountingBrokerOrderPort",
        "ShellAccountingBrokerOrderPort",
    }) ? 0 : 1;
}
