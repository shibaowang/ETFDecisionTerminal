#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    const auto shell = shellBoundaryText(sourceRoot(argc, argv));
    return shell.find("ShellAccountingTradeDraftConfirmationRepository") == std::string::npos
        && shell.find("INSERT INTO trade_log") == std::string::npos
        && shell.find("INSERT INTO trade_execution_group") == std::string::npos
        ? 0
        : 1;
}
