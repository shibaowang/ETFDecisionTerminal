#include "ShellAccountingProductionQmlBindingGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs72 = readTextFile(root / "docs" / "72_shell_accounting_production_qml_binding_gate.md");
    const auto docs73 = readTextFile(root / "docs" / "73_shell_accounting_production_qml_binding_test_plan.md");

    for (const auto& token : {
             "TASK-119",
             "TASK-121",
             "TASK-122",
             "TASK-125",
             "TASK-126",
             "TASK-127",
             "TASK-128",
             "TASK-129",
         }) {
        if (docs72.find(token) == std::string::npos || docs73.find(token) == std::string::npos) {
            std::cerr << "production QML binding docs missing pre-gate token `" << token << "`\n";
            return 1;
        }
    }
    if (docs72.find("explicit user authorization") == std::string::npos ||
        docs73.find("User explicitly authorized production QML binding") == std::string::npos ||
        docs72.find("not real accounting action") == std::string::npos ||
        docs72.find("not trading UI") == std::string::npos) {
        std::cerr << "production QML binding gate must define authorization and non-trading boundaries\n";
        return 1;
    }
    return 0;
}
