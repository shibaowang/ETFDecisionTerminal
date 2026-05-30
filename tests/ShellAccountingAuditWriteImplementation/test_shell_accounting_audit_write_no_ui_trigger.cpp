#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    const auto shell = shellBoundaryText(sourceRoot(argc, argv));
    return containsAnyToken(shell, {
        "accounting.audit.write",
        "ShellAccountingAuditWriteRepository",
        "TASK-146_AUDIT_WRITE",
        "data.audit.append",
        "audit_log",
    }) ? 1 : 0;
}
