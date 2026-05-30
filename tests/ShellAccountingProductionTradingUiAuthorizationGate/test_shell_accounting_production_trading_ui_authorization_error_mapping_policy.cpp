#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs104Text(sourceRoot(argc, argv)) + "\n" + docs105Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "authorization missing",
        "trading UI disabled",
        "draft creation failed",
        "confirmation failed",
        "stale draft",
        "duplicate confirmation",
        "audit unavailable",
        "ledger write failed",
        "transport unavailable",
        "no silent success",
    }) ? 0 : 1;
}
