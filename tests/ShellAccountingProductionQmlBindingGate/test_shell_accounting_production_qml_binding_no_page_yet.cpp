#include "ShellAccountingProductionQmlBindingGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_gate;

int main(int argc, char** argv)
{
    const auto qmlFiles = productionQmlFiles(sourceRoot(argc, argv));
    for (const auto& token : {
             "ShellAccountingPage",
             "ShellAccountingReadOnlyPage",
             "ShellAccountingDashboard",
             "ShellAccountingPanel",
             "AccountingDashboard",
             "shell_accounting",
             "buyButton",
             "sellButton",
             "createTradeDraft",
             "brokerOrder",
             "strategyExecute",
         }) {
        if (containsToken(qmlFiles, token)) {
            std::cerr << "production QML must not add ShellAccounting page/component/route yet\n";
            return 1;
        }
    }
    return 0;
}
