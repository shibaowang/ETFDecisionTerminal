#include "ShellAccountingProductionTradingUiAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_production_trading_ui_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs = docs104Text(sourceRoot(argc, argv)) + "\n" + docs105Text(sourceRoot(argc, argv));
    return containsAllTokens(docs, {
        "UI only through presenter/controller/adapter/DataService boundary",
        "QML must not directly access `DataServiceClient`",
        "QML must not directly access SQLite or DataAccess",
        "no generic write escape hatch",
    }) ? 0 : 1;
}
