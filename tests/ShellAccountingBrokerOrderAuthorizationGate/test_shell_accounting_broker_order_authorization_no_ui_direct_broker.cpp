#include "ShellAccountingBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto qml = productionQmlText(root);
    return containsAnyToken(qml, uiDirectBrokerTokens()) ? 1 : 0;
}
