#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    return containsAnyToken(readOnlyRegionText(sourceRoot(argc, argv)), snapshotWriteTokens()) ? 1 : 0;
}
