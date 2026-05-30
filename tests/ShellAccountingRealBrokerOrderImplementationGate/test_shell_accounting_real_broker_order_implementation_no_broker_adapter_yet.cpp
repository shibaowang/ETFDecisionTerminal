#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    return containsAnyToken(productionText(sourceRoot(argc, argv)), brokerAdapterTokens()) ? 1 : 0;
}
