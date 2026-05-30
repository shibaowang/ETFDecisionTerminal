#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs104Text(sourceRoot(argc, argv)) + "\n" + docs105Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "disable production trading UI",
        "fallback keeps read-only accounting page",
        "no fake draft",
        "no fake confirmation",
        "no broker fallback",
        "no strategy fallback",
    }) ? 0 : 1;
}
