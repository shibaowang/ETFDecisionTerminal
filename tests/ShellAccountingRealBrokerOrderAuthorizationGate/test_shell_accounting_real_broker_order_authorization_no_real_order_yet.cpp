#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto production = productionText(root);
    bool ok = production.find("accounting.broker_order.dry_run") != std::string::npos;
    ok &= production.find("TASK-154_BROKER_ORDER_DRY_RUN") != std::string::npos;
    ok &= !containsAnyToken(production, realBrokerImplementationTokens());
    ok &= !containsAnyToken(production, brokerAdapterTokens());
    ok &= !containsAnyToken(production, sandboxPaperBrokerTokens());
    ok &= !containsAnyToken(production, networkBrokerCallTokens());
    return ok ? 0 : 1;
}
