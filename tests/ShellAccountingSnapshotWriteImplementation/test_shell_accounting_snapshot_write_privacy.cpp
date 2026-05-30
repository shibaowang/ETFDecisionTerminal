#include "ShellAccountingSnapshotWriteImplementation.h"

using namespace etfdt::tests::shell_accounting_snapshot_write;

int main(int argc, char** argv)
{
    auto connection = openMigratedDatabase(migrationPath(argc, argv));
    seedReplayFixture(*connection);
    const auto call = invokeSnapshotWrite(*connection);
    const auto privacy = call.payload.value("privacy").toObject();

    bool ok = expectSuccessfulSnapshotWrite(call);
    ok &= expect(!privacy.value("rawSqlExposed").toBool(true), "raw SQL not exposed");
    ok &= expect(!privacy.value("rawTradeLogPayloadExposed").toBool(true), "raw trade_log payload not exposed");
    ok &= expect(!privacy.value("internalStackExposed").toBool(true), "internal stack not exposed");
    ok &= expect(call.response.payloadJson.find("INSERT INTO") == std::string::npos, "response contains no raw SQL");
    return ok ? 0 : 1;
}
