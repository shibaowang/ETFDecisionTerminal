#include "ShellAccountingAuditWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs95 = readTextFile(docs95Path(sourceRoot(argc, argv)));
    const auto docs96 = readTextFile(docs96Path(sourceRoot(argc, argv)));
    const auto combined = docs95 + docs96;
    for (const std::string token : {
             "authorized DataService write action result",
             "TASK-144 snapshot write result",
             "source write unavailable",
             "source write failed",
             "must not consume raw SQL",
             "must not consume raw SQL, raw facts, or full snapshot payloads",
         }) {
        if (combined.find(token) == std::string::npos) {
            std::cerr << "missing snapshot write input token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
