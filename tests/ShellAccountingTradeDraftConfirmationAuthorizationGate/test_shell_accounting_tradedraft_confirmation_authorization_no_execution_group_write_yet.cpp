#include "ShellAccountingTradeDraftConfirmationAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_tradedraft_confirmation_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto dataServiceText = dataServiceBoundaryText(root);
    const bool authorized = dataServiceText.find("ShellAccountingTradeDraftConfirmationRepository") != std::string::npos
        && dataServiceText.find("INSERT INTO trade_execution_group") != std::string::npos
        && dataServiceText.find("accounting.tradedraft.confirm") != std::string::npos;
    const bool shellWrites = containsAnyToken(shellBoundaryText(root), executionGroupWriteTokens());
    return authorized && !shellWrites ? 0 : 1;
}
