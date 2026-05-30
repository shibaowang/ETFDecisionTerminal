#include "ShellAccountingAuditWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_audit_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto docs95 = readTextFile(docs95Path(sourceRoot(argc, argv)));
    for (const std::string token : {
             "Future ShellAccounting audit write may only be coordinated by DataService",
             "QML, ShellServices, ShellCore, Presenter, and Shell must not directly trigger audit write",
             "must not expose a generic write-enabled escape hatch",
             "allowlisted, statically scannable, and test-covered",
         }) {
        if (docs95.find(token) == std::string::npos) {
            std::cerr << "missing DataService-only audit token `" << token << "`\n";
            return 1;
        }
    }
    return 0;
}
