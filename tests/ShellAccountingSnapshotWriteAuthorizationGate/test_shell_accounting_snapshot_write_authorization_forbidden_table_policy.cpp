#include "ShellAccountingSnapshotWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs92 = readTextFile(docs92Path(root));
    const auto docs93 = readTextFile(docs93Path(root));
    int failures = 0;

    for (const auto& token : {
             "trade_log",
             "trade_execution_group",
             "trade_draft",
             "audit_log",
             "unless a future task separately authorizes audit writes",
         }) {
        if (docs92.find(token) == std::string::npos && docs93.find(token) == std::string::npos) {
            std::cerr << "missing forbidden table policy token `" << token << "`\n";
            ++failures;
        }
    }
    return failures == 0 ? 0 : 1;
}
