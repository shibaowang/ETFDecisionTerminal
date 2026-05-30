#pragma once

#include "DataAccess/ShellAccountingSnapshotWriteRepository.h"
#include "Protocol/MessageEnvelope.h"
#include "ShellAccountingReadOnlyReplayImplementation.h"

#include <QJsonObject>

#include <filesystem>
#include <string>

namespace etfdt::tests::shell_accounting_snapshot_write {

using etfdt::tests::shell_accounting_readonly_replay::containsAnyToken;
using etfdt::tests::shell_accounting_readonly_replay::filesUnder;
using etfdt::tests::shell_accounting_readonly_replay::joinFiles;
using etfdt::tests::shell_accounting_readonly_replay::migrationPath;
using etfdt::tests::shell_accounting_readonly_replay::openMigratedDatabase;
using etfdt::tests::shell_accounting_readonly_replay::readTextFile;
using etfdt::tests::shell_accounting_readonly_replay::seedReplayFixture;
using etfdt::tests::shell_accounting_readonly_replay::sourceRoot;

struct SnapshotWriteCall final {
    etfdt::protocol::ProtocolResponse response;
    QJsonObject payload;
};

[[nodiscard]] std::string authorizedSnapshotWritePayload();
[[nodiscard]] SnapshotWriteCall invokeSnapshotWrite(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson = authorizedSnapshotWritePayload());
[[nodiscard]] int countRows(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& tableName);
[[nodiscard]] std::int64_t querySingleInt64(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& sql);
[[nodiscard]] bool expect(bool condition, const std::string& message);
[[nodiscard]] bool expectSuccessfulSnapshotWrite(const SnapshotWriteCall& call);
[[nodiscard]] std::string docs94Text(const std::filesystem::path& root);
[[nodiscard]] std::string shellBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string dataServiceActionsText(const std::filesystem::path& root);
[[nodiscard]] std::string dataAccessSnapshotWriteText(const std::filesystem::path& root);

}  // namespace etfdt::tests::shell_accounting_snapshot_write
