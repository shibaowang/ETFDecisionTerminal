#include "ShellAccountingQmlTypeRegistrationScenario.h"

#include <utility>

namespace etfdt::tests::shell_accounting_qml_type_registration {
namespace {

ShellAccountingQmlTypeRegistrationScenario allowedType(std::string typeName)
{
    return {
        std::move(typeName),
        registrationModuleName(),
        "first-stage",
        true,
        true,
        false,
        false,
        false,
        false,
        requiredRegistrationGates(),
        "read-only ShellAccounting QML registration candidate",
    };
}

ShellAccountingQmlTypeRegistrationScenario deniedType(std::string typeName, std::string notes)
{
    return {
        std::move(typeName),
        registrationModuleName(),
        "never",
        false,
        false,
        true,
        true,
        true,
        true,
        {},
        std::move(notes),
    };
}

}  // namespace

std::string registrationModuleName()
{
    return "ETFDecisionTerminal.ShellAccounting";
}

std::vector<std::string> requiredRegistrationGates()
{
    return {
        "TASK-119",
        "TASK-121",
        "TASK-122",
    };
}

std::vector<ShellAccountingQmlTypeRegistrationScenario> firstStageAllowedTypes()
{
    return {
        allowedType("ShellAccountingPresenter"),
        allowedType("ShellAccountingStatusObject"),
        allowedType("ShellAccountingIssueListModel"),
        allowedType("ShellPositionListModel"),
    };
}

std::vector<ShellAccountingQmlTypeRegistrationScenario> forbiddenTypeScenarios()
{
    return {
        deniedType("DataServiceClient", "direct service client must never be exposed to QML"),
        deniedType("DataServiceApi", "DataService protocol surface is not a QML binding object"),
        deniedType("ShellAccountingDataServiceAdapter", "adapter remains below controller boundary"),
        deniedType("ShellAccountingDataServiceClientPortAdapter", "concrete port remains ShellServices-internal"),
        deniedType("DataAccess", "QML must not access storage"),
        deniedType("SQLite", "QML must not access SQLite"),
        deniedType("SQLiteConnection", "QML must not access SQLite connection objects"),
        deniedType("AccountingEngine", "QML must not call accounting engine"),
        deniedType("AccountingReplayEngine", "QML must not call replay engine"),
        deniedType("AccountingTradeFactReader", "QML must not read accounting facts"),
        deniedType("StrategyEngine", "QML must not execute strategies"),
        deniedType("MarketEngine", "QML must not call market engine"),
        deniedType("BrokerOrderApi", "QML must not submit broker orders"),
        deniedType("WriteService", "QML must not receive any write service"),
    };
}

ShellAccountingQmlTypeRegistrationScenario moduleBoundaryScenario()
{
    return {
        "ShellAccountingReadOnlyModule",
        registrationModuleName(),
        "future-registration",
        true,
        true,
        false,
        false,
        false,
        false,
        requiredRegistrationGates(),
        "module import is read-only and does not imply production QML binding",
    };
}

}  // namespace etfdt::tests::shell_accounting_qml_type_registration
