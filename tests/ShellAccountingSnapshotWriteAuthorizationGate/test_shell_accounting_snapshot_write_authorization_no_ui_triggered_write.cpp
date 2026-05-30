#include "ShellAccountingSnapshotWriteAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_snapshot_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellBoundary = shellUiAndServiceBoundaryText(root);
    return containsAnyToken(shellBoundary, uiTriggeredWriteForbiddenTokens()) ? 1 : 0;
}
