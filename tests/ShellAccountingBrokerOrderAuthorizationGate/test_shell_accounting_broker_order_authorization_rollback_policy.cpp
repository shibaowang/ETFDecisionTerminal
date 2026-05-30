#include "ShellAccountingBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = docs107Text(root) + docs108Text(root);
    return containsAllTokens(docs, {
        "disable broker order",
        "dry-run mode",
        "Fallback must not call a broker",
        "no fake broker success",
        "no strategy fallback",
        "no automatic trading fallback",
    }) ? 0 : 1;
}
