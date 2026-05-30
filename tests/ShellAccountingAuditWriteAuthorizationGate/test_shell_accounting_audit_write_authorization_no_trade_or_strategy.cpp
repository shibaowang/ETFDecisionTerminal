#include "ShellAccountingAuditWriteAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shellText = shellUiAndServiceBoundaryText(root);
    const auto snapshotAction = snapshotWriteActionRegion(root);
    return containsAnyToken(shellText + snapshotAction, tradeOrStrategyForbiddenTokens()) ? 1 : 0;
}
