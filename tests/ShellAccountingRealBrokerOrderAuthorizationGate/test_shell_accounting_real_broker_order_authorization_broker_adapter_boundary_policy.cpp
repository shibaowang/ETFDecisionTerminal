#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "broker adapter must be behind DataService action",
        "no hidden plugin registration",
        "no background order placement",
        "no QML direct broker access",
    }) ? 0 : 1;
}
