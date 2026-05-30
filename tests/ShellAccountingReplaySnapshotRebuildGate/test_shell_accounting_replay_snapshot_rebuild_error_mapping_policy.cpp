#include "ShellAccountingReplaySnapshotRebuildGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_replay_snapshot_rebuild_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs84 = readTextFile(docs84Path(root));
    const auto docs85 = readTextFile(docs85Path(root));
    int failures = 0;

    for (const auto& token : {
             "missing facts",
             "inconsistent facts",
             "missing market price",
             "missing FX rate",
             "replay disabled",
             "rebuild disabled",
             "write authorization missing",
             "no silent success",
         }) {
        if (docs84.find(token) == std::string::npos && docs85.find(token) == std::string::npos) {
            std::cerr << "missing replay/rebuild error mapping token `" << token << "`\n";
            ++failures;
        }
    }
    return failures == 0 ? 0 : 1;
}
