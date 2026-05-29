#include "ShellAccountingProductionQmlBindingImplementation.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_production_qml_binding_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto page = readTextFile(authorizedPagePath(root));
    const auto contentHost = readTextFile(root / "apps" / "ETFDecisionShell" / "qml" / "layout" / "ContentHost.qml");
    const auto registry = readTextFile(root / "libs" / "ShellCore" / "src" / "ShellPageRegistry.cpp");
    if (page.find("objectName: \"shellAccountingReadOnlyPage\"") == std::string::npos ||
        contentHost.find("ShellAccountingReadOnlyPage") == std::string::npos ||
        registry.find("\"shell_accounting\"") == std::string::npos ||
        registry.find("\"ShellAccountingReadOnlyPage\"") == std::string::npos) {
        std::cerr << "authorized ShellAccounting page must be registered through existing navigation pattern\n";
        return 1;
    }
    return 0;
}
