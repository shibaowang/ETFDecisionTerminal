#include "ShellAccountingRealBrokerOrderImplementationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_implementation_gate;

int main(int argc, char** argv)
{
    const auto production = productionText(sourceRoot(argc, argv));
    bool ok = production.find("accounting.broker_order.dry_run") != std::string::npos;
    ok &= production.find("TASK-154_BROKER_ORDER_DRY_RUN") != std::string::npos;
    ok &= !containsAnyToken(production, realOrderTokens());
    return ok ? 0 : 1;
}
