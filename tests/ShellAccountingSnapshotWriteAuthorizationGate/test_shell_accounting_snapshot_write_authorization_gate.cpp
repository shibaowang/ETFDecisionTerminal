#include "ShellAccountingSnapshotWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs92 = readTextFile(docs92Path(root));
    const auto docs93 = readTextFile(docs93Path(root));
    int failures = 0;

    if (!std::filesystem::exists(docs92Path(root)) || !std::filesystem::exists(docs93Path(root))) {
        std::cerr << "docs/92 and docs/93 must exist\n";
        ++failures;
    }
    for (const auto& task : taskPrerequisiteTokens()) {
        if (docs92.find(task) == std::string::npos) {
            std::cerr << "docs/92 missing prerequisite " << task << "\n";
            ++failures;
        }
    }
    for (const auto& token : {
             "TASK-143",
             "explicit user authorization",
             "snapshot write implementation must be a separate TASK",
             "snapshot write input may only come from TASK-142 snapshot rebuild preview",
             "snapshot write must go through the DataService boundary",
             "schema migration must be a separate TASK",
         }) {
        if (docs92.find(token) == std::string::npos) {
            std::cerr << "docs/92 missing `" << token << "`\n";
            ++failures;
        }
    }
    if (docs93.find("Test Matrix") == std::string::npos) {
        std::cerr << "docs/93 missing Test Matrix\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
