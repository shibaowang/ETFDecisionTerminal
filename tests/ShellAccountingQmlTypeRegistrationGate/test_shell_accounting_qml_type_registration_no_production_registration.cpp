#include "ShellAccountingQmlTypeRegistrationGate.h"

#include <filesystem>
#include <iostream>
#include <vector>

using namespace etfdt::tests::shell_accounting_qml_type_registration_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const std::vector<std::string> qmlTokens{
        "ShellAccountingPresenter",
        "accountingPresenter",
        "ETFDecisionTerminal.ShellAccounting",
    };
    if (containsAnyToken(productionQmlFiles(root), qmlTokens)) {
        return 1;
    }

    const std::vector<std::string> appRegistrationTokens{
        "qmlRegisterType<ShellAccountingPresenter>",
        "qmlRegisterType<ShellAccountingStatusObject>",
        "qmlRegisterType<ShellAccountingIssueListModel>",
        "qmlRegisterType<ShellPositionListModel>",
        "qmlRegisterUncreatableType<ShellAccountingPresenter>",
        "qmlRegisterUncreatableType<ShellAccountingStatusObject>",
        "qmlRegisterUncreatableType<ShellAccountingIssueListModel>",
        "qmlRegisterUncreatableType<ShellPositionListModel>",
        "qmlRegisterSingletonType<ShellAccountingPresenter>",
        "qmlRegisterSingletonType<ShellAccountingStatusObject>",
        "qmlRegisterSingletonType<ShellAccountingIssueListModel>",
        "qmlRegisterSingletonType<ShellPositionListModel>",
        "ETFDecisionTerminal.ShellAccounting",
    };
    if (containsAnyToken(productionRegistrationFiles(root), appRegistrationTokens)) {
        return 1;
    }

    const auto helperPath =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingQmlRegistration.cpp";
    const auto helperHeaderPath =
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingQmlRegistration.h";
    if (!std::filesystem::exists(helperPath)) {
        std::cerr << "centralized ShellAccounting QML registration helper is missing\n";
        return 1;
    }

    const auto helperText = readTextFile(helperHeaderPath) + readTextFile(helperPath);
    const std::vector<std::string> requiredHelperTokens{
        "qmlRegisterUncreatableType<T>",
        "ShellAccountingPresenter",
        "ShellAccountingStatusObject",
        "ShellAccountingIssueListModel",
        "ShellPositionListModel",
        "ETFDecisionTerminal.ShellAccounting",
    };
    for (const auto& token : requiredHelperTokens) {
        if (helperText.find(token) == std::string::npos) {
            std::cerr << "centralized helper missing token `" << token << "`\n";
            return 1;
        }
    }

    const std::vector<std::string> forbiddenHelperTokens{
        "DataServiceClient",
        "DataServiceApi",
        "ShellAccountingDataServiceAdapter",
        "ShellAccountingDataServiceClientPortAdapter",
        "DataAccess",
        "SQLite",
        "AccountingEngine",
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
    };
    for (const auto& token : forbiddenHelperTokens) {
        if (helperText.find(token) != std::string::npos) {
            std::cerr << "centralized helper contains forbidden token `" << token << "`\n";
            return 1;
        }
    }

    const auto shellServiceFiles =
        filesUnder(root / "libs" / "ShellServices", {".cpp", ".h", ".hpp"});
    for (const auto& file : shellServiceFiles) {
        if (file == helperPath) {
            continue;
        }
        const auto text = readTextFile(file);
        if (text.find("qmlRegisterUncreatableType<ShellAccounting") != std::string::npos ||
            text.find("qmlRegisterType<ShellAccounting") != std::string::npos ||
            text.find("qmlRegisterSingletonType<ShellAccounting") != std::string::npos) {
            std::cerr << file.generic_string()
                      << ": ShellAccounting QML registration must stay centralized\n";
            return 1;
        }
    }

    if (helperText.find("ShellCashSummaryObject") != std::string::npos ||
        helperText.find("ShellPortfolioPnlObject") != std::string::npos ||
        helperText.find("ShellBasePositionObject") != std::string::npos ||
        helperText.find("ShellSniperPoolObject") != std::string::npos ||
        helperText.find("ShellSniperTierListModel") != std::string::npos) {
        std::cerr << "centralized helper registers a non-first-stage ShellAccounting type\n";
        return 1;
    }
    return 0;
}
