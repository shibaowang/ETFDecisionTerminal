#include "ShellAccountingSnapshotWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs92 = readTextFile(docs92Path(root));
    const auto docs93 = readTextFile(docs93Path(root));
    const auto docs91 = readTextFile(docs91Path(root));
    int failures = 0;

    for (const auto& token : {
             "disable switch",
             "disable snapshot write",
             "TASK-142 snapshot rebuild preview",
             "no fake data",
             "no write fallback",
             "rollback policy",
         }) {
        if (docs92.find(token) == std::string::npos && docs93.find(token) == std::string::npos) {
            std::cerr << "missing snapshot write authorization rollback token `" << token << "`\n";
            ++failures;
        }
    }
    if (docs91.find("Snapshot write remains unimplemented") == std::string::npos) {
        std::cerr << "docs/91 must preserve snapshot write unimplemented wording\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
