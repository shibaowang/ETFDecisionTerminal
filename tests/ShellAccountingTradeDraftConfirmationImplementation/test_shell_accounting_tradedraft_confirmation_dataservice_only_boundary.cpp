#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    const auto shell = shellBoundaryText(sourceRoot(argc, argv));
    bool ok = expect(shell.find("accounting.tradedraft.confirm") == std::string::npos,
                     "Shell/QML do not call confirmation action");
    ok &= expect(shell.find("ShellAccountingTradeDraftConfirmationRepository") == std::string::npos,
                 "Shell/QML do not depend on confirmation repository");
    return ok ? 0 : 1;
}
