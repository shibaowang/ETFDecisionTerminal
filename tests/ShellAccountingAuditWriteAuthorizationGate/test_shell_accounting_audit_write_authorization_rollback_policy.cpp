#include "ShellAccountingAuditWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs95 = readTextFile(docs95Path(root));
    const auto docs96 = readTextFile(docs96Path(root));
    const auto docs94 = readTextFile(docs94Path(root));
    const auto combined = docs95 + "\n" + docs96;

    for (const std::string token : {
             "rollback",
             "disable",
             "audit write must be reversible",
             "disable audit write",
             "snapshot write must remain valid",
             "must not fake audit success",
             "must not write audit_log",
             "must not write trade_log",
             "must not write trade_draft",
             "must not generate TradeDraft",
             "must not execute strategy",
         }) {
        if (combined.find(token) == std::string::npos) {
            std::cerr << "missing audit rollback token `" << token << "`\n";
            return 1;
        }
    }

    if (docs94.find("audit_log") == std::string::npos || docs94.find("forbidden") == std::string::npos) {
        std::cerr << "docs/94 must continue to record audit_log as forbidden for TASK-144\n";
        return 1;
    }

    return 0;
}
