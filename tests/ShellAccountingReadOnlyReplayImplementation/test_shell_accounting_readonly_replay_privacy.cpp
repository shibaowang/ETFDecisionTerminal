#include "ShellAccountingReadOnlyReplayImplementation.h"

using namespace etfdt::tests::shell_accounting_readonly_replay;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto payload = invokeReplayAction(*connection, "position.list");
    bool ok = expectReplayPayload(payload, "position.list", "privacy replay");
    const auto source = readOnlyRegionText(sourceRoot(argc, argv));
    ok &= expect(source.find("rawReplayPayloadExposed") != std::string::npos, "raw replay payload is hidden");
    return ok ? 0 : 1;
}
