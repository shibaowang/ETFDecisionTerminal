#include "ShellAccountingQmlSmokeExpectedBinding.h"

namespace etfdt::tests::shell_accounting_qml_binding_smoke {

std::vector<ExpectedBindingObject> allowedBindingObjects()
{
    return {
        {"ShellAccountingPresenter", true, false},
        {"ShellAccountingStatusObject", true, false},
        {"ShellAccountingIssueListModel", true, false},
        {"ShellPositionListModel", true, false},
        {"ShellCashSummaryObject", true, true},
        {"ShellPortfolioPnlObject", true, true},
        {"ShellBasePositionObject", true, true},
        {"ShellSniperPoolObject", true, true},
        {"ShellSniperTierListModel", true, true},
    };
}

std::vector<std::string> forbiddenBindingTargets()
{
    return {
        "DataServiceClient",
        "DataServiceApi",
        "DataService payload",
        "SQLite",
        "AccountingEngine",
        "DataAccess",
        "write action",
    };
}

std::vector<std::string> forbiddenTradeUiTokens()
{
    return {
        "buy button",
        "sell button",
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
        "data.audit.append",
        "writeEnabled=true",
        "sellableAboveBaseAmountText is not a sell suggestion",
        "remainingAmountText is not a buy suggestion",
        "completed is not calculated by QML",
        "trading buttons",
        "write menu",
    };
}

std::vector<std::string> requiredStaticGateTests()
{
    return {
        "shell_accounting_qml_static_gate_no_direct_service",
        "shell_accounting_qml_static_gate_no_accounting_binding_yet",
        "shell_accounting_qml_static_gate_no_write_tokens",
        "shell_accounting_qml_static_gate_accounting_scope_no_trade",
        "shell_accounting_qml_static_gate_docs_sync",
    };
}

}  // namespace etfdt::tests::shell_accounting_qml_binding_smoke
