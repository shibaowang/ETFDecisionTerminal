#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellText = shellBoundaryText(root);
    const auto dataServiceText = dataServiceActionsText(root);

    bool ok = expect(!containsAnyToken(shellText, {
                         "confirmTradeDraft",
                         "brokerOrder(",
                         "placeOrder",
                         "strategyExecute(",
                         "confirmTrade",
                         "buy button",
                         "sell button",
                     }),
                     "Shell/QML has no trade or strategy trigger");
    ok &= expect(dataServiceText.find("tradeDraftGenerated") != std::string::npos
                     && dataServiceText.find("false") != std::string::npos,
                 "DataService response states no TradeDraft generation");
    ok &= expect(dataServiceText.find("strategyExecuted") != std::string::npos
                     && dataServiceText.find("false") != std::string::npos,
                 "DataService response states no strategy execution");
    return ok ? 0 : 1;
}
