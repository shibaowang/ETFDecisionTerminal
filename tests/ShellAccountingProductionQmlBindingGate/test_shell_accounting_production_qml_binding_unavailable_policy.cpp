#include "ShellAccountingProductionQmlBindingGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs = readTextFile(root / "docs" / "72_shell_accounting_production_qml_binding_gate.md") +
                      readTextFile(root / "docs" / "73_shell_accounting_production_qml_binding_test_plan.md");
    for (const auto& token : {
             "Unavailable",
             "must not fake data",
             "must not call DataServiceClient",
             "must not write database",
             "must not generate TradeDraft",
             "must not show trade UI",
         }) {
        if (docs.find(token) == std::string::npos) {
            std::cerr << "production QML binding unavailable policy missing token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
