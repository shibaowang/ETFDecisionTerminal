#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <filesystem>
#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs74 = root / "docs" / "74_shell_accounting_production_qml_binding_implementation.md";
    if (!std::filesystem::exists(docs74) || !std::filesystem::exists(authorizedPagePath(root))) {
        std::cerr << "TASK-131 docs or authorized QML shell page is missing\n";
        return 1;
    }
    const auto docs = readTextFile(docs74);
    if (docs.find("TASK-131") == std::string::npos ||
        docs.find("read-only") == std::string::npos ||
        docs.find("unavailable-safe") == std::string::npos ||
        docs.find("rollback") == std::string::npos) {
        std::cerr << "docs/74 must record TASK-131 scope and rollback\n";
        return 1;
    }
    return 0;
}
