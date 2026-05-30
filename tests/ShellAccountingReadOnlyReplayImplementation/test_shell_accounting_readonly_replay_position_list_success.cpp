#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonArray>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "position.list");
    bool ok = expectReplayPayload(payload, "position.list", "position replay");
    ok &= expect(payload.value("positions").toArray().size() == 1, "one replay position");
    return ok ? 0 : 1;
}
