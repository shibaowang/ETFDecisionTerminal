#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    const auto docs = docs94Text(sourceRoot(argc, argv));
    bool ok = expect(docs.find("rollback") != std::string::npos, "docs/94 includes rollback");
    ok &= expect(docs.find("disable") != std::string::npos, "docs/94 includes disable strategy");
    ok &= expect(docs.find("partial write forbidden") != std::string::npos,
                 "docs/94 documents partial write forbidden");
    ok &= expect(docs.find("TASK-142 snapshotRebuildPreview") != std::string::npos,
                 "docs/94 documents preview input");
    ok &= expect(docs.find("audit write") != std::string::npos,
                 "docs/94 keeps audit write separate");
    return ok ? 0 : 1;
}
