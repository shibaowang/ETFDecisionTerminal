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
             "write disabled",
             "authorization missing",
             "preview unavailable",
             "stale preview",
             "DB write failure",
             "transaction rollback",
             "schema mismatch",
             "duplicate snapshot",
             "partial write forbidden",
             "no silent success",
         }) {
        if (docs92.find(token) == std::string::npos && docs93.find(token) == std::string::npos) {
            std::cerr << "missing snapshot write authorization error mapping token `" << token << "`\n";
            ++failures;
        }
    }
    return failures == 0 ? 0 : 1;
}
