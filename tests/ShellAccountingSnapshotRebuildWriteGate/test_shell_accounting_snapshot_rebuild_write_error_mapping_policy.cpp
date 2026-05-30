#include "ShellAccountingSnapshotRebuildWriteGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_rebuild_write_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs87 = readTextFile(docs87Path(root));
    const auto docs88 = readTextFile(docs88Path(root));
    int failures = 0;

    for (const auto& token : {
             "rebuild disabled",
             "write authorization missing",
             "replay unavailable",
             "missing facts",
             "stale facts",
             "inconsistent facts",
             "DB write failure",
             "transaction rollback",
             "schema mismatch",
             "no silent success",
         }) {
        if (docs87.find(token) == std::string::npos && docs88.find(token) == std::string::npos) {
            std::cerr << "missing snapshot rebuild/write error mapping token `" << token << "`\n";
            ++failures;
        }
    }
    return failures == 0 ? 0 : 1;
}
