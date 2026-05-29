#include "ShellAccountingQmlTypeRegistrationExpectedType.h"

#include "ShellAccountingQmlTypeRegistrationScenario.h"

namespace etfdt::tests::shell_accounting_qml_type_registration {

std::vector<std::string> forbiddenMethodTokens()
{
    return {
        "buy",
        "sell",
        "createTradeDraft",
        "brokerOrder",
        "submitOrder",
        "strategyExecute",
        "executeStrategy",
        "data.audit.append",
        "writeAction",
        "snapshotWrite",
        "tradeLogWrite",
        "positionSnapshotWrite",
        "cashSnapshotWrite",
        "portfolioSummaryWrite",
        "setWriteEnabled",
        "enableTrading",
        "submitTrade",
        "placeOrder",
        "generateSuggestion",
    };
}

std::vector<std::string> forbiddenPropertyTokens()
{
    return {
        "writeEnabled: true",
    };
}

std::vector<ShellAccountingQmlTypeRegistrationExpectedType> firstStageExpectedTypes()
{
    const auto methods = forbiddenMethodTokens();
    const auto properties = forbiddenPropertyTokens();
    const auto gates = requiredRegistrationGates();
    return {
        {"ShellAccountingPresenter", true, "read-only presenter boundary", methods, properties, gates},
        {"ShellAccountingStatusObject", true, "read-only status object", methods, properties, gates},
        {"ShellAccountingIssueListModel", true, "read-only issue model", methods, properties, gates},
        {"ShellPositionListModel", true, "read-only position model", methods, properties, gates},
    };
}

std::vector<std::string> forbiddenTypeNames()
{
    return {
        "DataServiceClient",
        "DataServiceApi",
        "ShellAccountingDataServiceAdapter",
        "ShellAccountingDataServiceClientPortAdapter",
        "DataAccess",
        "SQLite",
        "SQLiteConnection",
        "AccountingEngine",
        "AccountingReplayEngine",
        "AccountingTradeFactReader",
        "StrategyEngine",
        "MarketEngine",
        "BrokerOrderApi",
        "WriteService",
    };
}

}  // namespace etfdt::tests::shell_accounting_qml_type_registration
