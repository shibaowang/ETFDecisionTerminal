#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellText = shellBoundaryText(root);
    const auto dataServiceText = dataServiceActionsText(root);
    const auto repositoryText = tradeDraftRepositoryText(root);

    bool ok = expect(shellText.find("accounting.tradedraft.create") == std::string::npos,
                     "Shell/QML does not call accounting.tradedraft.create");
    ok &= expect(shellText.find("TASK-148_TRADEDRAFT_WRITE") == std::string::npos,
                 "Shell/QML does not hold TradeDraft write authorization");
    ok &= expect(dataServiceText.find("handleAccountingTradeDraftCreate") != std::string::npos,
                 "DataService owns TradeDraft action");
    ok &= expect(repositoryText.find("INSERT INTO trade_draft") != std::string::npos,
                 "DataAccess repository owns trade_draft write");
    return ok ? 0 : 1;
}
