#include "ShellAccountingBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = docs107Text(root) + docs108Text(root);
    return containsAllTokens(docs, {
        "requires an additional explicit user confirmation step",
        "independent authorization token",
        "Authorization missing must fail",
        "Silent success is forbidden",
    }) ? 0 : 1;
}
