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
             "Snapshot rebuild output may only be an in-memory derived snapshot preview",
             "preview may describe calculated cash",
             "must not write `cash_snapshot`",
             "must not become TradeDraft",
             "QML trading action",
         }) {
        if (docs89.find(token) == std::string::npos) {
            std::cerr << "docs/89 missing preview-only policy `" << token << "`\n";
            ++failures;
        }
    }
    if (docs90.find("Future rebuild output is an in-memory derived snapshot preview only") == std::string::npos) {
        std::cerr << "docs/90 missing preview-only matrix row\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
