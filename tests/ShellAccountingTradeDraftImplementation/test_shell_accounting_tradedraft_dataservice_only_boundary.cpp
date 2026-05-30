#include "ShellAccountingTradeDraftImplementation.h"

using namespace etfdt::tests::shell_accounting_tradedraft;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellText = shellBoundaryText(root);
    const auto dataServiceText = dataServiceActionsText(root);
    const auto repositoryText = tradeDraftRepositoryText(root);

    bool ok = expect(shellText.find("ShellAccountingTradeDraftRepository") == std::string::npos,
                     "Shell/QML does not depend on TradeDraft repository");
    ok &= expect(shellText.find("INSERT INTO trade_draft") == std::string::npos,
                 "Shell/QML does not write trade_draft directly");
    ok &= expect(shellText.find("DataServiceClient") == std::string::npos ||
                 shellText.find("ShellAccountingDataServiceClientPortAdapter") != std::string::npos,
                 "Shell/QML reaches TradeDraft creation only through the ShellAccounting adapter boundary");
    ok &= expect(dataServiceText.find("handleAccountingTradeDraftCreate") != std::string::npos,
                 "DataService owns TradeDraft action");
    ok &= expect(repositoryText.find("INSERT INTO trade_draft") != std::string::npos,
                 "DataAccess repository owns trade_draft write");
    return ok ? 0 : 1;
}
