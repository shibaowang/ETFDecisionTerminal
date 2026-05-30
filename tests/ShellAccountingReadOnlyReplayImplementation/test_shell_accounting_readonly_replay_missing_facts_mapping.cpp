#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonArray>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    const auto payload = invokeReplayAction(*connection, "position.list");
    bool ok = expectReplayPayload(payload, "position.list", "empty replay");
    ok &= expect(payload.value("status").toString().toStdString() == "OK", "empty ledger replay is explicit OK");
    ok &= expect(payload.value("positions").toArray().isEmpty(), "empty ledger has no fake positions");
    return ok ? 0 : 1;
}
