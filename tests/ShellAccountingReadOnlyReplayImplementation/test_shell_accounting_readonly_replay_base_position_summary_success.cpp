#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonArray>

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "base_position.summary");
    bool ok = expectReplayPayload(payload, "base_position.summary", "base replay");
    ok &= expect(payload.value("basePositions").toArray().size() == 1, "base position facts remain available");
    return ok ? 0 : 1;
}
