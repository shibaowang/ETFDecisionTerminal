#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    int failures = 0;
    failures += containsAnyToken(shellBoundaryText(root), uiDirectEngineTokens()) ? 1 : 0;
    failures += expect(readOnlyRegionText(root).find("AccountingReplayEngine") != std::string::npos,
                       "AccountingReplayEngine appears only in DataService read-only region") ? 0 : 1;
    return failures == 0 ? 0 : 1;
}
