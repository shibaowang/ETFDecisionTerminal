#include "ShellAccountingBrokerAdapterDisabledWiring.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_disabled_wiring;

int main(int argc, char** argv)
{
    return containsAnyToken(dataServiceApiText(sourceRoot(argc, argv)), brokerNetworkTokens()) ? 1 : 0;
}
