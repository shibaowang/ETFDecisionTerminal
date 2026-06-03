#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceActionRegistrar.h"
#include "DataServiceApi/DataServiceActions.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace fs = std::filesystem;

int g_checks = 0;

struct Harness {
    fs::path root;
};

struct DbFixture {
    fs::path directory;
    std::unique_ptr<etfdt::data_access::SQLiteConnection> connection =
        std::make_unique<etfdt::data_access::SQLiteConnection>();
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
    std::int64_t instrumentId = 0;
};

struct RowCounts {
    int tradeExecutionGroup = 0;
    int tradeLog = 0;
    int cashAdjustment = 0;
    int auditLog = 0;
};

std::string readFile(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Unable to read " + path.string());
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void require(bool condition, const std::string& message)
{
    ++g_checks;
    if (!condition) {
        throw std::runtime_error(message);
    }
}

bool contains(const std::string& text, const std::string& token)
{
    return text.find(token) != std::string::npos;
}

void requireContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(contains(text, token), context + " missing `" + token + "`");
}

void requireNotContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(!contains(text, token), context + " unexpectedly contains `" + token + "`");
}

void requireAll(const std::string& text, const std::vector<std::string>& tokens, const std::string& context)
{
    for (const auto& token : tokens) {
        requireContains(text, token, context);
    }
}

std::int64_t singleInt64(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    require(rows.hasValue(), "query failed: " + sql);
    require(!rows.value().empty() && !rows.value().front().empty(), "query returned no row: " + sql);
    return rows.value().front().front().int64Value;
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    return static_cast<int>(singleInt64(connection, "SELECT COUNT(*) FROM " + table + ";"));
}

RowCounts rowCounts(etfdt::data_access::SQLiteConnection& connection)
{
    return RowCounts{
        countRows(connection, "trade_execution_group"),
        countRows(connection, "trade_log"),
        countRows(connection, "cash_adjustment"),
        countRows(connection, "audit_log"),
    };
}

void requireSameCounts(const RowCounts& before, const RowCounts& after, const std::string& context)
{
    require(before.tradeExecutionGroup == after.tradeExecutionGroup, context + " changed trade_execution_group");
    require(before.tradeLog == after.tradeLog, context + " changed trade_log");
    require(before.cashAdjustment == after.cashAdjustment, context + " changed cash_adjustment");
    require(before.auditLog == after.auditLog, context + " changed audit_log");
}

DbFixture makeDb(const Harness& h, const std::string& label)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("task211_manual_entry_sell_withdrawal_" + label + "_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "task211.sqlite";
    config.enableWal = false;
    require(fixture.connection->open(config).hasValue(), "open temporary sqlite");
    require(fixture.connection->applyMigrationFile(h.root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply migration 001");
    require(fixture.connection->applyMigrationFile(
                h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
                .hasValue(),
        "apply migration 002");

    auto account = fixture.connection->executeStatement(
        "INSERT INTO account(uid,name,account_type,base_currency,is_active,initial_cash_cents,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-211-account-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-211 Account"),
         etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic TASK-211 fixture account"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(account.hasValue(), "insert synthetic account fixture");
    fixture.accountId = fixture.connection->lastInsertRowId();

    auto portfolio = fixture.connection->executeStatement(
        "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
        "VALUES(?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-211-portfolio-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-211 Portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic SELL Withdrawal acceptance fixture"),
         etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(portfolio.hasValue(), "insert synthetic portfolio fixture");
    fixture.portfolioId = fixture.connection->lastInsertRowId();

    auto instrument = fixture.connection->executeStatement(
        "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-211-instrument-" + label),
         etfdt::data_access::SqlStatementParameter::textValue("159509"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-211 ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::textValue("synthetic TASK-211 fixture instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-03T12:00:00Z")});
    require(instrument.hasValue(), "insert synthetic instrument fixture");
    fixture.instrumentId = fixture.connection->lastInsertRowId();

    return fixture;
}

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-211-manual-entry-sell-withdrawal";
    request.traceId = "task-211-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-06-03T12:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

etfdt::protocol::ProtocolResponse dispatch(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& action,
    const std::string& payloadJson)
{
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    return dispatcher.dispatch(envelope(action, payloadJson));
}

std::string transactionPayload(
    const DbFixture& fixture,
    const std::string& side,
    std::int64_t quantity,
    std::int64_t gross,
    std::int64_t fee,
    std::int64_t tax,
    const std::string& idempotencyKey)
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\","
            << "\"instrumentId\":\"" << fixture.instrumentId << "\","
            << "\"securityCode\":\"159509\","
            << "\"tradeSide\":\"" << side << "\","
            << "\"quantityUnits\":" << quantity << ','
            << "\"priceAmountMinor\":20100,"
            << "\"grossAmountMinor\":" << gross << ','
            << "\"feeAmountMinor\":" << fee << ','
            << "\"taxAmountMinor\":" << tax << ','
            << "\"occurredAt\":\"2026-06-03T12:34:56Z\","
            << "\"sourceMemo\":\"task 211 synthetic manual transaction\","
            << "\"requestId\":\"task-211-transaction-request-" << idempotencyKey << "\","
            << "\"idempotencyKey\":\"" << idempotencyKey << "\""
            << "}";
    return payload.str();
}

std::string cashPayload(
    const DbFixture& fixture,
    const std::string& movementType,
    std::int64_t amount,
    const std::string& idempotencyKey)
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\","
            << "\"movementType\":\"" << movementType << "\","
            << "\"amountMinor\":" << amount << ','
            << "\"currency\":\"CNY\","
            << "\"occurredAt\":\"2026-06-03T12:35:56Z\","
            << "\"sourceMemo\":\"task 211 synthetic cash movement\","
            << "\"sourceReference\":\"bank-ref-task-211\","
            << "\"requestId\":\"task-211-cash-request-" << idempotencyKey << "\","
            << "\"idempotencyKey\":\"" << idempotencyKey << "\""
            << "}";
    return payload.str();
}

std::string readPayload(const DbFixture& fixture)
{
    std::ostringstream payload;
    payload << "{"
            << "\"accountId\":\"" << fixture.accountId << "\","
            << "\"portfolioId\":\"" << fixture.portfolioId << "\""
            << "}";
    return payload.str();
}

void requireSanitizedPayload(const std::string& payload, const std::string& context)
{
    for (const auto& token : {
             "raw SQL",
             "SELECT ",
             "INSERT INTO",
             "UPDATE ",
             "DELETE FROM",
             "REPLACE INTO",
             "credential=",
             "credentials=",
             "password",
             "api_key",
             "endpoint=",
             "brokerOrderId",
             "realOrderId",
             "stack trace",
             "broker payload",
             "full trade payload",
             "full snapshot payload",
         }) {
        requireNotContains(payload, token, context);
    }
    requireAll(payload, {
        "\"rawSqlExposed\":false",
        "\"rawPayloadExposed\":false",
        "\"credentialsExposed\":false",
        "\"endpointExposed\":false",
    }, context);
}

void requireSafeReadbackPayload(const std::string& payload, const std::string& context)
{
    requireSanitizedPayload(payload, context);
    requireAll(payload, {
        "\"writeEnabled\":false",
        "\"replayExecuted\":false",
        "\"snapshotRebuilt\":false",
        "\"accountingEngineCalled\":false",
        "\"tradeDraftGenerated\":false",
        "\"tradeSuggestionGenerated\":false",
        "\"strategyExecuted\":false",
        "\"brokerOrderSubmitted\":false",
    }, context);
}

void requireTransactionWriteResponse(const etfdt::protocol::ProtocolResponse& response, const std::string& context)
{
    require(response.success, context + " succeeds");
    requireAll(response.payloadJson, {
        "\"validationAccepted\":true",
        "\"databaseWritten\":true",
        "\"tradeExecutionGroupWritten\":true",
        "\"tradeLogWritten\":true",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
    }, context + " response");
    requireSanitizedPayload(response.payloadJson, context + " response");
}

void requireCashWriteResponse(const etfdt::protocol::ProtocolResponse& response, const std::string& context)
{
    require(response.success, context + " succeeds");
    requireAll(response.payloadJson, {
        "\"validationAccepted\":true",
        "\"databaseWritten\":true",
        "\"tradeLogWritten\":true",
        "\"cashAdjustmentWritten\":true",
        "\"auditWritten\":false",
        "\"ledgerWritten\":false",
    }, context + " response");
    requireSanitizedPayload(response.payloadJson, context + " response");
}

void writeBuy(DbFixture& fixture, const std::string& key = "task-211-buy")
{
    const auto buy = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture, "Buy", 1000000, 20100, 100, 20, key));
    requireTransactionWriteResponse(buy, "manual BUY setup");
}

void testSellReductionDailyUseAcceptance(const Harness& h)
{
    auto fixture = makeDb(h, "sell-reduction");
    writeBuy(fixture, "task-211-buy-before-sell");

    const auto sell = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture, "Sell", 250000, 5025, 50, 10, "task-211-sell"));
    requireTransactionWriteResponse(sell, "manual SELL");
    require(countRows(*fixture.connection, "trade_execution_group") == 2, "BUY + SELL write two execution groups");
    require(countRows(*fixture.connection, "trade_log") == 2, "BUY + SELL write two trade_log rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "SELL path writes no cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 0, "SELL path writes no audit_log");

    const auto beforeReadback = rowCounts(*fixture.connection);

    const auto position = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionPositionList,
        readPayload(fixture));
    require(position.success, "position.list SELL reduction readback succeeds");
    requireAll(position.payloadJson, {
        "\"action\":\"position.list\"",
        "\"status\":\"PARTIAL\"",
        "\"dataQualityStatus\":\"PARTIAL\"",
        "\"manualEntryReadback\":true",
        "\"realizedPnlComputed\":false",
        "\"unrealizedPnlComputed\":false",
        "\"accountId\":\"" + std::to_string(fixture.accountId) + "\"",
        "\"portfolioId\":\"" + std::to_string(fixture.portfolioId) + "\"",
        "\"instrumentCode\":\"159509\"",
        "\"quantityText\":\"0.75\"",
        "\"costAmountText\":\"151.25\"",
        "\"marketValueText\":\"UNAVAILABLE\"",
        "\"unrealizedPnlText\":\"UNAVAILABLE\"",
        "MANUAL_ENTRY_READBACK_MAPPING",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "position.list SELL reduction payload");
    requireSafeReadbackPayload(position.payloadJson, "position.list SELL reduction payload");
    requireNotContains(position.payloadJson, "\"marketValueText\":\"0.00\"", "position.list SELL reduction payload");
    requireNotContains(position.payloadJson, "\"unrealizedPnlText\":\"0.00\"", "position.list SELL reduction payload");
    requireNotContains(position.payloadJson, "\"totalPnlText\":\"0.00\"", "position.list SELL reduction payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "position.list SELL reduction readback");

    const auto cash = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionCashSummary,
        readPayload(fixture));
    require(cash.success, "cash.summary SELL cash inflow readback succeeds");
    requireAll(cash.payloadJson, {
        "\"action\":\"cash.summary\"",
        "\"status\":\"OK\"",
        "\"dataQualityStatus\":\"OK\"",
        "\"manualEntryReadback\":true",
        "\"cashBalanceText\":\"-152.55\"",
        "\"principalBaseText\":\"0.00\"",
        "MANUAL_ENTRY_READBACK_MAPPING",
    }, "cash.summary SELL cash inflow payload");
    requireSafeReadbackPayload(cash.payloadJson, "cash.summary SELL cash inflow payload");
    requireNotContains(cash.payloadJson, "\"cashBalanceText\":\"0.00\"", "cash.summary SELL cash inflow payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "cash.summary SELL readback");

    const auto portfolio = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionPortfolioPnlSummary,
        readPayload(fixture));
    require(portfolio.success, "portfolio.pnl.summary SELL readback succeeds");
    requireAll(portfolio.payloadJson, {
        "\"action\":\"portfolio.pnl.summary\"",
        "\"status\":\"PARTIAL\"",
        "\"dataQualityStatus\":\"PARTIAL\"",
        "\"manualEntryReadback\":true",
        "\"realizedPnlComputed\":false",
        "\"unrealizedPnlComputed\":false",
        "\"cashBalanceText\":\"-152.55\"",
        "\"principalBaseText\":\"0.00\"",
        "\"holdingCostText\":\"151.25\"",
        "\"totalAssetsText\":\"UNAVAILABLE\"",
        "\"totalMarketValueText\":\"UNAVAILABLE\"",
        "\"totalPnlText\":\"UNAVAILABLE\"",
        "\"totalReturnText\":\"UNAVAILABLE\"",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "portfolio.pnl.summary SELL payload");
    requireSafeReadbackPayload(portfolio.payloadJson, "portfolio.pnl.summary SELL payload");
    requireNotContains(portfolio.payloadJson, "\"totalPnlText\":\"0.00\"", "portfolio.pnl.summary SELL payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "portfolio.pnl.summary SELL readback");
}

void testSellExceedsPositionSafeBehavior(const Harness& h)
{
    auto fixture = makeDb(h, "sell-exceeds");
    writeBuy(fixture, "task-211-buy-before-oversell");
    const auto beforeAttempt = rowCounts(*fixture.connection);

    const auto sell = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualTransactionCreate,
        transactionPayload(fixture, "Sell", 2000000, 40200, 50, 10, "task-211-oversell"));
    requireSanitizedPayload(sell.payloadJson, "manual oversell response");
    if (!sell.success) {
        requireContains(sell.payloadJson, "\"databaseWritten\":false", "manual oversell rejected response");
        requireSameCounts(beforeAttempt, rowCounts(*fixture.connection), "manual oversell rejected");
        return;
    }

    requireTransactionWriteResponse(sell, "manual oversell write");
    require(countRows(*fixture.connection, "trade_execution_group") == beforeAttempt.tradeExecutionGroup + 1,
        "oversell write adds one execution group when accepted");
    require(countRows(*fixture.connection, "trade_log") == beforeAttempt.tradeLog + 1,
        "oversell write adds one trade_log row when accepted");
    require(countRows(*fixture.connection, "cash_adjustment") == beforeAttempt.cashAdjustment,
        "oversell write adds no cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == beforeAttempt.auditLog, "oversell write adds no audit");

    const auto beforeReadback = rowCounts(*fixture.connection);
    const auto position = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionPositionList,
        readPayload(fixture));
    require(position.success, "position.list oversell readback succeeds safely");
    requireAll(position.payloadJson, {
        "\"action\":\"position.list\"",
        "\"status\":\"PARTIAL\"",
        "\"manualEntryReadback\":true",
        "\"quantityText\":\"-1\"",
        "\"marketValueText\":\"UNAVAILABLE\"",
        "\"unrealizedPnlText\":\"UNAVAILABLE\"",
        "MANUAL_ENTRY_READBACK_MAPPING",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "position.list oversell safe payload");
    requireSafeReadbackPayload(position.payloadJson, "position.list oversell safe payload");
    requireNotContains(position.payloadJson, "\"unrealizedPnlText\":\"0.00\"", "position.list oversell safe payload");
    requireNotContains(position.payloadJson, "\"marketValueText\":\"0.00\"", "position.list oversell safe payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "position.list oversell readback");
}

void testWithdrawalDailyUseAcceptance(const Harness& h)
{
    auto fixture = makeDb(h, "withdrawal");

    const auto deposit = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture, "Deposit", 50000, "task-211-deposit-before-withdrawal"));
    requireCashWriteResponse(deposit, "manual Deposit setup");

    const auto withdrawal = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture, "Withdrawal", 12500, "task-211-withdrawal"));
    requireCashWriteResponse(withdrawal, "manual Withdrawal");
    require(countRows(*fixture.connection, "trade_execution_group") == 0, "Withdrawal path writes no execution group");
    require(countRows(*fixture.connection, "trade_log") == 2, "Deposit + Withdrawal write two trade_log rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 2, "Deposit + Withdrawal write two cash adjustments");
    require(countRows(*fixture.connection, "audit_log") == 0, "Withdrawal path writes no audit_log");

    const auto beforeReadback = rowCounts(*fixture.connection);

    const auto cash = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionCashSummary,
        readPayload(fixture));
    require(cash.success, "cash.summary Withdrawal readback succeeds");
    requireAll(cash.payloadJson, {
        "\"action\":\"cash.summary\"",
        "\"status\":\"OK\"",
        "\"dataQualityStatus\":\"OK\"",
        "\"manualEntryReadback\":true",
        "\"cashBalanceText\":\"375.00\"",
        "\"principalBaseText\":\"375.00\"",
        "MANUAL_ENTRY_READBACK_MAPPING",
    }, "cash.summary Withdrawal payload");
    requireSafeReadbackPayload(cash.payloadJson, "cash.summary Withdrawal payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "cash.summary Withdrawal readback");

    const auto portfolio = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionPortfolioPnlSummary,
        readPayload(fixture));
    require(portfolio.success, "portfolio.pnl.summary Withdrawal readback succeeds");
    requireAll(portfolio.payloadJson, {
        "\"action\":\"portfolio.pnl.summary\"",
        "\"manualEntryReadback\":true",
        "\"realizedPnlComputed\":false",
        "\"unrealizedPnlComputed\":false",
        "\"cashBalanceText\":\"375.00\"",
        "\"principalBaseText\":\"375.00\"",
        "\"totalPnlText\":\"UNAVAILABLE\"",
        "\"totalReturnText\":\"UNAVAILABLE\"",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "portfolio.pnl.summary Withdrawal payload");
    requireSafeReadbackPayload(portfolio.payloadJson, "portfolio.pnl.summary Withdrawal payload");
    requireNotContains(portfolio.payloadJson, "\"totalPnlText\":\"0.00\"", "portfolio.pnl.summary Withdrawal payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "portfolio.pnl.summary Withdrawal readback");
}

void testInsufficientWithdrawalSafeBehavior(const Harness& h)
{
    auto fixture = makeDb(h, "insufficient-withdrawal");
    const auto beforeAttempt = rowCounts(*fixture.connection);

    const auto withdrawal = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionAccountingManualCashMovementCreate,
        cashPayload(fixture, "Withdrawal", 25000, "task-211-insufficient-withdrawal"));
    requireSanitizedPayload(withdrawal.payloadJson, "manual insufficient Withdrawal response");
    if (!withdrawal.success) {
        requireContains(withdrawal.payloadJson, "\"databaseWritten\":false", "manual insufficient Withdrawal rejected");
        requireSameCounts(beforeAttempt, rowCounts(*fixture.connection), "manual insufficient Withdrawal rejected");
        return;
    }

    requireCashWriteResponse(withdrawal, "manual insufficient Withdrawal write");
    require(countRows(*fixture.connection, "trade_log") == beforeAttempt.tradeLog + 1,
        "insufficient Withdrawal write adds one trade_log row when accepted");
    require(countRows(*fixture.connection, "cash_adjustment") == beforeAttempt.cashAdjustment + 1,
        "insufficient Withdrawal write adds one cash_adjustment row when accepted");
    require(countRows(*fixture.connection, "audit_log") == beforeAttempt.auditLog,
        "insufficient Withdrawal write adds no audit");

    const auto beforeReadback = rowCounts(*fixture.connection);
    const auto cash = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionCashSummary,
        readPayload(fixture));
    require(cash.success, "cash.summary negative cash readback succeeds safely");
    requireAll(cash.payloadJson, {
        "\"action\":\"cash.summary\"",
        "\"manualEntryReadback\":true",
        "\"cashBalanceText\":\"-250.00\"",
        "\"principalBaseText\":\"-250.00\"",
        "MANUAL_ENTRY_READBACK_MAPPING",
    }, "cash.summary negative cash safe payload");
    requireSafeReadbackPayload(cash.payloadJson, "cash.summary negative cash safe payload");
    requireNotContains(cash.payloadJson, "\"cashBalanceText\":\"0.00\"", "cash.summary negative cash safe payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "cash.summary negative cash readback");

    const auto portfolio = dispatch(
        *fixture.connection,
        etfdt::data_service_api::kActionPortfolioPnlSummary,
        readPayload(fixture));
    require(portfolio.success, "portfolio.pnl.summary negative cash readback succeeds safely");
    requireAll(portfolio.payloadJson, {
        "\"manualEntryReadback\":true",
        "\"cashBalanceText\":\"-250.00\"",
        "\"principalBaseText\":\"-250.00\"",
        "\"totalPnlText\":\"UNAVAILABLE\"",
        "PNL_UNAVAILABLE_WITHOUT_REPLAY",
    }, "portfolio.pnl.summary negative cash safe payload");
    requireSafeReadbackPayload(portfolio.payloadJson, "portfolio.pnl.summary negative cash safe payload");
    requireSameCounts(beforeReadback, rowCounts(*fixture.connection), "portfolio.pnl.summary negative cash readback");
}

void testDocsAndIndexes(const Harness& h)
{
    require(fs::exists(h.root / "docs" / "218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md"),
        "docs/218 exists");
    require(fs::exists(
                h.root / "docs" / "219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md"),
        "docs/219 exists");
    requireContains(readFile(h.root / "README.md"),
        "docs/218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md",
        "README");
    requireContains(readFile(h.root / "README.md"),
        "docs/219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md",
        "README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "README.md"),
        "219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md",
        "docs/README");
    requireContains(readFile(h.root / "docs" / "12_codex_prompt_template.md"), "TASK-211", "docs/12");
    requireContains(readFile(h.root / "tests" / "CMakeLists.txt"),
        "ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance",
        "tests/CMakeLists");
}

void testStaticBoundaries(const Harness& h)
{
    const auto qml = readFile(h.root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml");
    const auto presenter = readFile(h.root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp");
    const auto controller =
        readFile(h.root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp");
    const auto actions = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp");

    for (const auto& token : {
             "SQLite",
             "DataAccess",
             "DataServiceClient",
             "INSERT INTO",
             "UPDATE ",
             "DELETE FROM",
             "REPLACE INTO",
             "calculateSell",
             "calculateWithdrawal",
             "calculatePnl",
             "brokerOrder",
             "placeOrder",
             "realOrderId",
             "automaticTrading",
         }) {
        requireNotContains(qml, token, "production QML");
    }
    for (const auto& token : {
             "DataAccess/ShellAccountingManual",
             "SQLite",
             "INSERT INTO",
             "UPDATE ",
             "DELETE FROM",
             "REPLACE INTO",
             "BrokerOrderPort",
             "StrategyEngine",
         }) {
        requireNotContains(presenter + controller, token, "Presenter / Controller");
    }
    requireAll(actions, {
        "loadManualEntryReadbackPositions",
        "loadManualEntryReadbackCashSummaries",
        "manualEntryPortfolioSummariesFromReadback",
        "manualEntryReadbackIssuesJson",
        "kActionAccountingManualTransactionCreate",
        "kActionAccountingManualCashMovementCreate",
    }, "DataServiceActions retained manual entry read/write boundaries");
    requireNotContains(actions, "runShellAccountingReadOnlyReplay(context.request.payloadJson)", "DataServiceActions");
}

void testRetainedRegressionGates(const Harness& h)
{
    const auto testsCmake = readFile(h.root / "tests" / "CMakeLists.txt");
    for (const auto& token : {
             "ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate",
             "ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance",
             "ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate",
             "ShellAccountingManualEntryReadbackMappingImplementation",
             "ShellAccountingManualEntryReadbackMappingAuthorizationGate",
             "ShellAccountingManualEntryMvpRuntimeE2eAcceptance",
             "ShellAccountingManualEntryPostWriteReadbackRefreshImplementation",
             "ShellAccountingManualEntryQmlPresenterImplementation",
             "ShellAccountingManualEntryDataServiceWriteWiringImplementation",
             "ShellAccountingManualCashMovementRepositoryDualWriteImplementation",
             "ShellAccountingManualTransactionRepositoryWriteImplementation",
             "ShellAccountingBrokerAdapterDisabledWiring",
             "ShellAccountingBrokerOrderImplementation",
             "ShellAccountingRealBrokerOrderAuthorizationGate",
             "ShellAccountingRealBrokerOrderImplementationGate",
         }) {
        requireContains(testsCmake, token, "retained regression gate");
    }

    for (const auto& path : {
             h.root / "migrations" / "001_initial_schema.sql",
             h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql",
             h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualTransactionRepository.cpp",
             h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualCashMovementRepository.cpp",
             h.root / "apps" / "ETFDecisionShell" / "src" / "main.cpp",
         }) {
        require(fs::exists(path), "required retained file exists: " + path.string());
    }
}

void runAll(const Harness& h)
{
    testDocsAndIndexes(h);
    testSellReductionDailyUseAcceptance(h);
    testSellExceedsPositionSafeBehavior(h);
    testWithdrawalDailyUseAcceptance(h);
    testInsufficientWithdrawalSafeBehavior(h);
    testStaticBoundaries(h);
    testRetainedRegressionGates(h);
    require(g_checks >= 145, "TASK-211 SELL / Withdrawal daily-use runtime acceptance must execute at least 145 checks");
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        Harness h;
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--source-root" && i + 1 < argc) {
                h.root = argv[++i];
            }
        }
        if (h.root.empty()) {
            throw std::runtime_error("Usage: --source-root <path>");
        }
        runAll(h);
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
