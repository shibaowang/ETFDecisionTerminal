#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    return containsAnyToken(shellBoundaryText(root), uiDirectEngineTokens()) ? 1 : 0;
}
