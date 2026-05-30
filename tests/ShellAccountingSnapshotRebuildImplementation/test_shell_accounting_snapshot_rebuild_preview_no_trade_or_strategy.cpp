#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    return containsAnyToken(readOnlyRegionText(sourceRoot(argc, argv)), {
        "TradeDraft",
        "createTradeDraft",
        "brokerOrder(",
        "placeOrder",
        "\"strategyExecute\"",
        "executeStrategy(",
        "buyButton",
        "sellButton",
        "orderConfirmation",
    }) ? 1 : 0;
}
