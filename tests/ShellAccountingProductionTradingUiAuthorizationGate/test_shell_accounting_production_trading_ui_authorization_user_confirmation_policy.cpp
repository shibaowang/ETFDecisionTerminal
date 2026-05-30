#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs104Text(sourceRoot(argc, argv)) + "\n" + docs105Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "Creation and confirmation are different actions",
        "confirmation needs explicit user step",
        "no default confirmation",
        "draft-only and confirmed-ledger states must be visible",
    }) ? 0 : 1;
}
