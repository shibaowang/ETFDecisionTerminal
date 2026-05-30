#pragma once

#include "ShellAccountingAuditWriteImplementation.h"

#include <QJsonObject>

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_tradedraft {

using etfdt::tests::shell_accounting_audit_write::containsAnyToken;
using etfdt::tests::shell_accounting_audit_write::countRows;
using etfdt::tests::shell_accounting_audit_write::expect;
using etfdt::tests::shell_accounting_audit_write::filesUnder;
using etfdt::tests::shell_accounting_audit_write::joinFiles;
using etfdt::tests::shell_accounting_audit_write::migrationPath;
using etfdt::tests::shell_accounting_audit_write::openMigratedDatabase;
using etfdt::tests::shell_accounting_audit_write::readTextFile;
using etfdt::tests::shell_accounting_audit_write::seedReplayFixture;
using etfdt::tests::shell_accounting_audit_write::sourceRoot;

struct TradeDraftCall final {
    etfdt::protocol::ProtocolResponse response;
    QJsonObject payload;
};

[[nodiscard]] std::string authorizedTradeDraftPayload();
[[nodiscard]] TradeDraftCall invokeTradeDraftCreate(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson = authorizedTradeDraftPayload());
[[nodiscard]] bool expectSuccessfulTradeDraft(const TradeDraftCall& call);
[[nodiscard]] bool expectDuplicateTradeDraft(const TradeDraftCall& call);
[[nodiscard]] std::string latestTradeDraftUid(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] std::string latestTradeDraftAction(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] std::string latestTradeDraftAuditPayload(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] std::string docs100Text(const std::filesystem::path& root);
[[nodiscard]] std::string shellBoundaryText(const std::filesystem::path& root);
[[nodiscard]] std::string dataServiceActionsText(const std::filesystem::path& root);
[[nodiscard]] std::string tradeDraftRepositoryText(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::string> brokerTokens();
[[nodiscard]] std::vector<std::string> strategyTokens();
[[nodiscard]] std::vector<std::string> shellDraftTriggerTokens();
[[nodiscard]] std::vector<std::string> sensitivePayloadTokens();

}  // namespace etfdt::tests::shell_accounting_tradedraft
