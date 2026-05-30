#include "ShellAccountingSnapshotWriteAuthorizationGate.h"

#include <iostream>

using namespace etfdt::tests::shell_accounting_snapshot_write_authorization_gate;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs92 = readTextFile(docs92Path(root));
    const auto docs93 = readTextFile(docs93Path(root));
    const auto dataServiceRegion = dataServiceReadOnlyAccountingRegion(root);
    int failures = 0;

    for (const auto& token : {
             "TASK-142 snapshot rebuild preview",
             "snapshotRebuildPreview",
             "preview input only",
             "snapshot write must not read directly from replay or facts query when preview is unavailable",
         }) {
        if (docs92.find(token) == std::string::npos && docs93.find(token) == std::string::npos) {
            std::cerr << "missing preview input policy token `" << token << "`\n";
            ++failures;
        }
    }
    if (dataServiceRegion.find("snapshotRebuildPreview") == std::string::npos) {
        std::cerr << "TASK-142 preview payload must remain present\n";
        ++failures;
    }
    return failures == 0 ? 0 : 1;
}
