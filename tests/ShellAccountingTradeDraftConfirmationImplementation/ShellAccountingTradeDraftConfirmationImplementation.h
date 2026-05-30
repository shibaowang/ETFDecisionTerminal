#pragma once

#include "ShellAccountingTradeDraftImplementation.h"

#include <QJsonObject>

#include <string>

namespace etfdt::tests::shell_accounting_tradedraft_confirmation {

using etfdt::tests::shell_accounting_tradedraft::authorizedTradeDraftPayload;
using etfdt::tests::shell_accounting_tradedraft::brokerTokens;
using etfdt::tests::shell_accounting_tradedraft::containsAnyToken;
using etfdt::tests::shell_accounting_tradedraft::countRows;
using etfdt::tests::shell_accounting_tradedraft::dataServiceActionsText;
using etfdt::tests::shell_accounting_tradedraft::expect;
using etfdt::tests::shell_accounting_tradedraft::invokeTradeDraftCreate;
using etfdt::tests::shell_accounting_tradedraft::latestTradeDraftAuditPayload;
using etfdt::tests::shell_accounting_tradedraft::migrationPath;
using etfdt::tests::shell_accounting_tradedraft::openMigratedDatabase;
using etfdt::tests::shell_accounting_tradedraft::readTextFile;
using etfdt::tests::shell_accounting_tradedraft::sensitivePayloadTokens;
using etfdt::tests::shell_accounting_tradedraft::seedReplayFixture;
using etfdt::tests::shell_accounting_tradedraft::shellBoundaryText;
using etfdt::tests::shell_accounting_tradedraft::sourceRoot;
using etfdt::tests::shell_accounting_tradedraft::strategyTokens;

struct ConfirmationCall final {
    etfdt::protocol::ProtocolResponse response;
    QJsonObject payload;
};

[[nodiscard]] std::string authorizedConfirmationPayload(std::int64_t draftId);
[[nodiscard]] ConfirmationCall invokeTradeDraftConfirm(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson);
[[nodiscard]] ConfirmationCall createDraftAndConfirm(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] bool expectSuccessfulConfirmation(const ConfirmationCall& call);
[[nodiscard]] bool expectDuplicateConfirmation(const ConfirmationCall& call);
[[nodiscard]] std::int64_t latestTradeDraftId(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] std::string latestTradeDraftStatus(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] std::int64_t latestTradeExecutionGroupId(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] std::int64_t latestTradeLogExecutionGroupId(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] std::string docs103Text(const std::filesystem::path& root);

}  // namespace etfdt::tests::shell_accounting_tradedraft_confirmation
