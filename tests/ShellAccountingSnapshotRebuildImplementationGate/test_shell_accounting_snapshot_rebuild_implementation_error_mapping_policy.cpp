#include "ShellAccountingSnapshotRebuildImplementationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_implementation_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs89 = readTextFile(docs89Path(root));
    const auto docs90 = readTextFile(docs90Path(root));
    int failures = 0;

    for (const auto& token : {
             "replay unavailable",
             "missing facts",
             "stale facts",
             "inconsistent facts",
             "unsupported currency",
             "missing market price",
             "missing FX",
             "rebuild disabled",
             "no silent success",
         }) {
        if (docs89.find(token) == std::string::npos && docs90.find(token) == std::string::npos) {
            std::cerr << "missing snapshot rebuild implementation error mapping token `" << token << "`\n";
            ++failures;
        }
    }
    return failures == 0 ? 0 : 1;
}
