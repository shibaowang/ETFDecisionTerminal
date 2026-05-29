#include "ShellAccountingQmlStartupRegistrationGate.h"

#include <iostream>
#include <string>

using namespace etfdt::tests::shell_accounting_qml_startup_registration_gate;

namespace {

int countToken(const std::vector<std::filesystem::path>& files, const std::string& token)
{
    int count = 0;
    for (const auto& file : files) {
        const auto text = readTextFile(file);
        std::size_t offset = 0;
        while ((offset = text.find(token, offset)) != std::string::npos) {
            ++count;
            offset += token.size();
        }
    }
    return count;
}

}  // namespace

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto startupFiles = productionStartupFiles(root);
    const auto qmlFiles = productionQmlFiles(root);

    if (countToken(startupFiles, "registerShellAccountingQmlTypes") != 1) {
        std::cerr << "production startup must call registerShellAccountingQmlTypes exactly once after TASK-129\n";
        return 1;
    }
    if (containsToken(startupFiles, "qmlRegisterType<ShellAccounting") ||
        containsToken(startupFiles, "qmlRegisterUncreatableType<ShellAccounting") ||
        containsToken(startupFiles, "qmlRegisterSingletonType<ShellAccounting")) {
        std::cerr << "production startup must not directly register ShellAccounting types after TASK-129\n";
        return 1;
    }
    if (containsToken(qmlFiles, "import ETFDecisionTerminal.ShellAccounting") ||
        containsToken(qmlFiles, "accountingPresenter") ||
        containsToken(qmlFiles, "ShellAccountingPresenter")) {
        std::cerr << "production QML must remain unbound after TASK-129 startup wiring\n";
        return 1;
    }
    return 0;
}
