#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return !containsAnyToken(productionText(root), automaticTradingTokens())
        && containsAllTokens(docs110Text(root) + docs111Text(root), {
               "no automatic trading",
               "no auto-trading trigger",
               "no background broker submission",
           }) ? 0 : 1;
}
