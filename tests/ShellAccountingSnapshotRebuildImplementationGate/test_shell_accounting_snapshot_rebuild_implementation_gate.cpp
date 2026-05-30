#include "ShellAccountingSnapshotRebuildImplementationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs89 = readTextFile(docs89Path(root));
    const auto docs90 = readTextFile(docs90Path(root));
    int failures = 0;

    if (!std::filesystem::exists(docs89Path(root)) || !std::filesystem::exists(docs90Path(root))) {
        std::cerr << "docs/89 and docs/90 must exist\n";
        ++failures;
    }
    for (const auto& task : taskPrerequisiteTokens()) {
        if (docs89.find(task) == std::string::npos) {
            std::cerr << "docs/89 missing prerequisite " << task << "\n";
            ++failures;
        }
    }
    for (const auto& token : {
             "explicit user authorization",
             "Snapshot rebuild is not snapshot write",
             "Snapshot rebuild is not TradeDraft",
             "Snapshot rebuild is not trade suggestion",
             "in-memory derived snapshot preview",
             "Snapshot write remains separately gated",
         }) {
        if (docs89.find(token) == std::string::npos) {
            std::cerr << "docs/89 missing `" << token << "`\n";
            ++failures;
        }
    }
    if (docs90.find("Test Matrix") == std::string::npos) {
        std::cerr << "docs/90 missing Test Matrix\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
