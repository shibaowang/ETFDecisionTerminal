#include "ShellAccountingAuditWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs95 = readTextFile(docs95Path(sourceRoot(argc, argv)));
    const auto docs96 = readTextFile(docs96Path(sourceRoot(argc, argv)));
    const auto combined = docs95 + docs96;
    for (const std::string token : {
             "no full `trade_log` payload",
             "no full snapshot payload",
             "no raw SQL",
             "no internal exception stack",
             "no sensitive raw payload dump",
             "no generic JSON mirror of source write payload",
         }) {
        if (combined.find(token) == std::string::npos) {
            std::cerr << "missing forbidden payload token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
