#include "ShellAccountingAuditWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs95 = readTextFile(docs95Path(root));
    const auto docs96 = readTextFile(docs96Path(root));
    const auto combined = docs95 + "\n" + docs96;

    for (const std::string token : {
             "audit disabled",
             "authorization missing",
             "source write unavailable",
             "source write failed",
             "audit payload invalid",
             "DB write failure",
             "transaction rollback",
             "schema mismatch",
             "duplicate audit event",
             "no silent success",
         }) {
        if (combined.find(token) == std::string::npos) {
            std::cerr << "missing audit error mapping token `" << token << "`\n";
            return 1;
        }
    }

    return 0;
}
