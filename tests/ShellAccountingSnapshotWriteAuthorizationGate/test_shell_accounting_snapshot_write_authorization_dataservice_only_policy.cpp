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
             "future snapshot write may only be coordinated by DataService",
             "QML, ShellServices, ShellCore, Presenter, and Shell must not directly trigger snapshot write",
             "must not expose a generic write-enabled escape hatch",
             "Every future snapshot write action must be allowlisted",
         }) {
        if (docs92.find(token) == std::string::npos) {
            std::cerr << "docs/92 missing DataService-only policy `" << token << "`\n";
            ++failures;
        }
    }
    if (docs93.find("DataService-only write") == std::string::npos) {
        std::cerr << "docs/93 missing DataService-only write matrix row\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
