#include "ShellAccountingAuditWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_audit_write;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto shell = shellBoundaryText(root);
    const auto actions = dataServiceActionsText(root);
    const auto repository = auditWriteRepositoryText(root);

    bool ok = expect(shell.find("accounting.audit.write") == std::string::npos,
                     "Shell/QML does not call accounting.audit.write");
    ok &= expect(shell.find("ShellAccountingAuditWriteRepository") == std::string::npos,
                 "Shell/QML does not depend on audit write repository");
    ok &= expect(actions.find("handleAccountingAuditWrite") != std::string::npos,
                 "DataService owns audit write action");
    ok &= expect(repository.find("AuditLogRepository") != std::string::npos
                     && repository.find("insertAuditLog") != std::string::npos,
                 "DataAccess audit repository writes audit_log through the audit repository");
    return ok ? 0 : 1;
}
