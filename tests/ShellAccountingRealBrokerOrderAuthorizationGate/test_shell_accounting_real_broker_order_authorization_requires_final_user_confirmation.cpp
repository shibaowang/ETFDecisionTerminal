#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "final user confirmation",
        "final confirmation cannot be defaulted",
        "final confirmation missing must fail",
        "UI must not auto-submit real order",
    }) ? 0 : 1;
}
