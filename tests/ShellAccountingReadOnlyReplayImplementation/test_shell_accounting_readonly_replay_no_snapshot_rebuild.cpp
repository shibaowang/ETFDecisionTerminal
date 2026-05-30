#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto source = readOnlyRegionText(root);
    bool ok = !containsAnyToken(source, {"rebuildFromFacts", "snapshotRebuilt\":true", "snapshot rebuild"});
    ok &= expect(docs86Text(root).find("snapshot rebuild still not implemented") != std::string::npos,
                 "docs/86 keeps rebuild disabled");
    return ok ? 0 : 1;
}
