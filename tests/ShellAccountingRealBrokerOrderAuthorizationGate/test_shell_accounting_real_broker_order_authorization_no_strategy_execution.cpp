#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return !containsAnyToken(productionText(root), strategyExecutionTokens())
        && containsAllTokens(docs110Text(root) + docs111Text(root), {
               "no strategy execution",
               "no StrategyEngine execution",
               "no strategy-generated broker order",
           }) ? 0 : 1;
}
