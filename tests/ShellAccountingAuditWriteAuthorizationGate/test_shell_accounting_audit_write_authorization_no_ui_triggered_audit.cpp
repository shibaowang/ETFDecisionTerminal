#include "ShellAccountingAuditWriteAuthorizationGate.h"

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto shellText = shellUiAndServiceBoundaryText(sourceRoot(argc, argv));
    return containsAnyToken(shellText, uiTriggeredAuditForbiddenTokens()) ? 1 : 0;
}
