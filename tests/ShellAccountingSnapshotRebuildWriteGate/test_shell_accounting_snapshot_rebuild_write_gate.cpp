#include "ShellAccountingSnapshotRebuildWriteGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs87 = readTextFile(docs87Path(root));
    const auto docs88 = readTextFile(docs88Path(root));
    int failures = 0;

    if (!std::filesystem::exists(docs87Path(root)) || !std::filesystem::exists(docs88Path(root))) {
        std::cerr << "docs/87 and docs/88 must exist\n";
        ++failures;
    }
    for (const auto& task : taskPrerequisiteTokens()) {
        if (docs87.find(task) == std::string::npos) {
            std::cerr << "docs/87 missing prerequisite " << task << "\n";
            ++failures;
        }
    }
    for (const auto& token : {
             "explicit user authorization",
             "Snapshot rebuild is not read-only replay",
             "Snapshot write is not trade execution",
             "Snapshot write, if ever needed, must be separately authorized",
             "schema migration task explicitly authorizes",
         }) {
        if (docs87.find(token) == std::string::npos) {
            std::cerr << "docs/87 missing `" << token << "`\n";
            ++failures;
        }
    }
    if (docs88.find("Test Matrix") == std::string::npos) {
        std::cerr << "docs/88 missing Test Matrix\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
