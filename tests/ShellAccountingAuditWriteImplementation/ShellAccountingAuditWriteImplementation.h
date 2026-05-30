#pragma once

#include "ShellAccountingSnapshotWriteImplementation.h"

#include <QJsonObject>

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_audit_write {

using etfdt::tests::shell_accounting_snapshot_write::countRows;
using etfdt::tests::shell_accounting_snapshot_write::expect;
using etfdt::tests::shell_accounting_snapshot_write::expectSuccessfulSnapshotWrite;
using etfdt::tests::shell_accounting_snapshot_write::filesUnder;
using etfdt::tests::shell_accounting_snapshot_write::invokeSnapshotWrite;
using etfdt::tests::shell_accounting_snapshot_write::joinFiles;
using etfdt::tests::shell_accounting_snapshot_write::migrationPath;
using etfdt::tests::shell_accounting_snapshot_write::openMigratedDatabase;
using etfdt::tests::shell_accounting_snapshot_write::readTextFile;
using etfdt::tests::shell_accounting_snapshot_write::seedReplayFixture;
using etfdt::tests::shell_accounting_snapshot_write::sourceRoot;

struct AuditWriteCall final {
    etfdt::protocol::ProtocolResponse response;
    QJsonObject payload;
};

[[nodiscard]] std::string authorizedAuditWritePayload();
[[nodiscard]] std::string auditWritePayloadFromSnapshot(
    const etfdt::tests::shell_accounting_snapshot_write::SnapshotWriteCall& snapshotWrite);
[[nodiscard]] AuditWriteCall invokeAuditWrite(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson = authorizedAuditWritePayload());
[[nodiscard]] bool expectSuccessfulAuditWrite(const AuditWriteCall& call);
[[nodiscard]] bool expectDuplicateAuditWrite(const AuditWriteCall& call);
[[nodiscard]] std::string latestAuditNewValueJson(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] std::string shellBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string dataServiceActionsText(const std::filesystem::path& root);
[[nodiscard]] std::string auditWriteRepositoryText(const std::filesystem::path& root);
[[nodiscard]] std::string docs97Text(const std::filesystem::path& root);
[[nodiscard]] bool containsAnyToken(const std::string& text, const std::vector<std::string>& tokens);

}  // namespace etfdt::tests::shell_accounting_audit_write
