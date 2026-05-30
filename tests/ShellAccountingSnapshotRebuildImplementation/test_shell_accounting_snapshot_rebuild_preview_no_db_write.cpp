#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    return containsAnyToken(readOnlyRegionText(sourceRoot(argc, argv)), {
        "INSERT INTO cash_snapshot",
        "INSERT INTO position_snapshot",
        "INSERT INTO portfolio_summary",
        "UPDATE cash_snapshot",
        "UPDATE position_snapshot",
        "UPDATE portfolio_summary",
        "DELETE FROM cash_snapshot",
        "DELETE FROM position_snapshot",
        "DELETE FROM portfolio_summary",
        "REPLACE INTO cash_snapshot",
        "REPLACE INTO position_snapshot",
        "REPLACE INTO portfolio_summary",
        "data.audit.append",
    }) ? 1 : 0;
}
