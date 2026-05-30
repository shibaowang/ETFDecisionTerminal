#include "ShellAccountingRealBrokerOrderAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_real_broker_order_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs110Text(sourceRoot(argc, argv)) + docs111Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "Future real broker order must be DataService-only",
        "no QML direct broker",
        "no Presenter direct broker",
        "no DataAccess direct broker",
        "no generic broker/write escape hatch",
    }) ? 0 : 1;
}
