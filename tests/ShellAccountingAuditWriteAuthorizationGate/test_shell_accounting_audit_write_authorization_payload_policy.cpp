#include "ShellAccountingAuditWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs95 = readTextFile(docs95Path(sourceRoot(argc, argv)));
    for (const std::string token : {
             "action name",
             "authorization token",
             "snapshot uid / batch id",
             "result status",
             "sanitized issue code",
             "timestamp",
             "no raw SQL",
             "no raw trade_log payload",
             "no full snapshot payload",
             "no internal stack trace",
         }) {
        if (docs95.find(token) == std::string::npos) {
            std::cerr << "missing audit payload token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
