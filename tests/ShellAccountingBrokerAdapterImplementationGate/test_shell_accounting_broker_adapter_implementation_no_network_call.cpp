#include "ShellAccountingBrokerAdapterImplementationGate.h"

using namespace etfdt::tests::shell_accounting_broker_adapter_implementation_gate;

int main(int argc, char** argv)
{
    return containsAnyToken(productionText(sourceRoot(argc, argv)), brokerNetworkTokens()) ? 1 : 0;
}
