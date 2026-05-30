#pragma once

#include "ShellAccountingTradeDraftConfirmationImplementation.h"

#include <QJsonObject>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_broker_order {

using etfdt::tests::shell_accounting_tradedraft_confirmation::authorizedConfirmationPayload;
using etfdt::tests::shell_accounting_tradedraft_confirmation::containsAnyToken;
using etfdt::tests::shell_accounting_tradedraft_confirmation::countRows;
using etfdt::tests::shell_accounting_tradedraft_confirmation::createDraftAndConfirm;
using etfdt::tests::shell_accounting_tradedraft_confirmation::dataServiceActionsText;
using etfdt::tests::shell_accounting_tradedraft_confirmation::expect;
using etfdt::tests::shell_accounting_tradedraft_confirmation::latestTradeDraftId;
using etfdt::tests::shell_accounting_tradedraft_confirmation::migrationPath;
using etfdt::tests::shell_accounting_tradedraft_confirmation::openMigratedDatabase;
using etfdt::tests::shell_accounting_tradedraft_confirmation::readTextFile;
using etfdt::tests::shell_accounting_tradedraft_confirmation::seedReplayFixture;
using etfdt::tests::shell_accounting_tradedraft_confirmation::shellBoundaryText;
using etfdt::tests::shell_accounting_tradedraft_confirmation::sourceRoot;

struct BrokerOrderCall final {
    etfdt::protocol::ProtocolResponse response;
    QJsonObject payload;
};

[[nodiscard]] std::string authorizedBrokerOrderDryRunPayload(std::int64_t draftId);
[[nodiscard]] BrokerOrderCall invokeBrokerOrderDryRun(
    etfdt::data_access::SQLiteConnection& connection,
    std::string payloadJson);
[[nodiscard]] BrokerOrderCall createConfirmedTradeAndDryRun(
    etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] bool expectSuccessfulBrokerOrderDryRun(const BrokerOrderCall& call);
[[nodiscard]] bool expectDuplicateBrokerOrderDryRun(const BrokerOrderCall& call);
[[nodiscard]] std::string latestBrokerOrderAuditPayload(etfdt::data_access::SQLiteConnection& connection);
[[nodiscard]] std::string docs109Text(const std::filesystem::path& root);
[[nodiscard]] std::string brokerOrderRepositoryText(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::string> brokerSdkTokens();
[[nodiscard]] std::vector<std::string> realOrderPlacementTokens();
[[nodiscard]] std::vector<std::string> strategyExecutionTokens();
[[nodiscard]] std::vector<std::string> automaticTradingTokens();
[[nodiscard]] std::vector<std::string> sensitivePayloadTokens();

inline void seedReplayFixture(std::unique_ptr<etfdt::data_access::SQLiteConnection>& connection)
{
    etfdt::tests::shell_accounting_tradedraft_confirmation::seedReplayFixture(*connection);
}

inline int countRows(
    std::unique_ptr<etfdt::data_access::SQLiteConnection>& connection,
    const std::string& tableName)
{
    return etfdt::tests::shell_accounting_tradedraft_confirmation::countRows(*connection, tableName);
}

inline etfdt::tests::shell_accounting_tradedraft_confirmation::ConfirmationCall createDraftAndConfirm(
    std::unique_ptr<etfdt::data_access::SQLiteConnection>& connection)
{
    return etfdt::tests::shell_accounting_tradedraft_confirmation::createDraftAndConfirm(*connection);
}

inline std::int64_t latestTradeDraftId(std::unique_ptr<etfdt::data_access::SQLiteConnection>& connection)
{
    return etfdt::tests::shell_accounting_tradedraft_confirmation::latestTradeDraftId(*connection);
}

inline BrokerOrderCall invokeBrokerOrderDryRun(
    std::unique_ptr<etfdt::data_access::SQLiteConnection>& connection,
    std::string payloadJson)
{
    return invokeBrokerOrderDryRun(*connection, std::move(payloadJson));
}

inline BrokerOrderCall createConfirmedTradeAndDryRun(
    std::unique_ptr<etfdt::data_access::SQLiteConnection>& connection)
{
    return createConfirmedTradeAndDryRun(*connection);
}

inline std::string latestBrokerOrderAuditPayload(
    std::unique_ptr<etfdt::data_access::SQLiteConnection>& connection)
{
    return latestBrokerOrderAuditPayload(*connection);
}

}  // namespace etfdt::tests::shell_accounting_broker_order
