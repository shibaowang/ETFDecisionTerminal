#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto call = invokeSnapshotWrite(*connection);

    bool ok = expectSuccessfulSnapshotWrite(call);
    ok &= expect(call.payload.value("source").toString().toStdString() == "snapshotRebuildPreview",
                 "snapshot write source is preview");
    ok &= expect(call.payload.value("input").toString().toStdString() == "TASK-142 snapshotRebuildPreview",
                 "snapshot write records TASK-142 input");
    ok &= expect(docs94Text(sourceRoot(argc, argv)).find("TASK-144") != std::string::npos,
                 "docs/94 records TASK-144");
    return ok ? 0 : 1;
}
