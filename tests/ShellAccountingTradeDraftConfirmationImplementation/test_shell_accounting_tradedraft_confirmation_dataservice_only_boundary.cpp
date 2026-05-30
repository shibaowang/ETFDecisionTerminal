#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    const auto shell = shellBoundaryText(sourceRoot(argc, argv));
    bool ok = expect(shell.find("INSERT INTO trade_log") == std::string::npos,
                     "Shell/QML do not write trade_log directly");
    ok &= expect(shell.find("INSERT INTO trade_execution_group") == std::string::npos,
                 "Shell/QML do not write trade_execution_group directly");
    ok &= expect(shell.find("ShellAccountingTradeDraftConfirmationRepository") == std::string::npos,
                 "Shell/QML do not depend on confirmation repository");
    return ok ? 0 : 1;
}
