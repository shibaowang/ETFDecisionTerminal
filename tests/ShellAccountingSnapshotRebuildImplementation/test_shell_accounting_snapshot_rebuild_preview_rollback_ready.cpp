#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs91 = readTextFile(root / "docs" / "91_shell_accounting_snapshot_rebuild_implementation.md");
    bool ok = expect(docs91.find("rollback") != std::string::npos, "docs/91 documents rollback");
    ok &= expect(docs91.find("fallback to TASK-139 read-only replay") != std::string::npos,
                 "docs/91 documents TASK-139 fallback");
    ok &= expect(docs91.find("snapshot write must be a separate TASK") != std::string::npos,
                 "docs/91 keeps write separate");
    return ok ? 0 : 1;
}
