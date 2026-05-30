#include "ShellAccountingTradeDraftConfirmationImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation;

int main(int argc, char** argv)
{
    const auto shell = shellBoundaryText(sourceRoot(argc, argv));
    return shell.find("accounting.tradedraft.confirm") == std::string::npos ? 0 : 1;
}
