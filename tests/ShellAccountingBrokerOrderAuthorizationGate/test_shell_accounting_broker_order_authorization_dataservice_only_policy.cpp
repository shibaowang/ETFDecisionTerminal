#include "ShellAccountingBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = docs107Text(root) + docs108Text(root);
    return containsAllTokens(docs, {
        "Future broker order coordination can only happen inside the DataService",
        "QML must route through ShellAccounting presenter/controller/adapter",
        "must not call a broker adapter",
        "must not expose a generic broker",
    }) ? 0 : 1;
}
