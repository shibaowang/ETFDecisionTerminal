#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "independent authorization token",
        "TASK-154 dry-run token is insufficient for real order",
        "authorization missing fails",
        "disabled state fails",
    }) ? 0 : 1;
}
