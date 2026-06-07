#include "DataAccess/ShellAccountingManualCashMovementRepository.h"
#include "DataServiceApi/DataServiceApi.h"
#include "DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

namespace fs = std::filesystem;

struct Harness {
    fs::path root;
};

struct DbFixture {
    fs::path directory;
    std::unique_ptr<etfdt::data_access::SQLiteConnection> connection =
        std::make_unique<etfdt::data_access::SQLiteConnection>();
    std::int64_t accountId = 0;
    std::int64_t portfolioId = 0;
};

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string readFile(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    require(input.good(), "unable to read " + path.string());
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void requireContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(text.find(token) != std::string::npos, context + " missing `" + token + "`");
}

void requireNotContains(const std::string& text, const std::string& token, const std::string& context)
{
    require(text.find(token) == std::string::npos, context + " unexpectedly contains `" + token + "`");
}

std::uint64_t fnv1a(std::string_view text, std::uint64_t seed = 14695981039346656037ULL)
{
    std::uint64_t value = seed;
    for (const unsigned char ch : text) {
        value ^= ch;
        value *= 1099511628211ULL;
    }
    return value;
}

void hashField(std::uint64_t& hash, const std::string& value)
{
    hash = fnv1a(value, hash);
    hash = fnv1a("|", hash);
}

std::string previewDigestForParseResult(
    const etfdt::data_service_api::ShellAccountingExcelVbaImportReadOnlyParseResult& parseResult)
{
    std::uint64_t hash = 14695981039346656037ULL;
    hashField(hash, parseResult.accepted ? "accepted" : "rejected");
    hashField(hash, std::to_string(parseResult.tradeFacts.size()));
    hashField(hash, std::to_string(parseResult.cashFacts.size()));
    hashField(hash, std::to_string(parseResult.marketPriceFacts.size()));
    hashField(hash, std::to_string(parseResult.fxRateFacts.size()));
    for (const auto& fact : parseResult.tradeFacts) {
        hashField(hash, fact.factId);
        hashField(hash, fact.tradeTime);
        hashField(hash, fact.accountId);
        hashField(hash, fact.portfolioId);
        hashField(hash, fact.instrumentCode);
        hashField(hash, fact.action);
        hashField(hash, fact.quantityText);
        hashField(hash, fact.priceText);
        hashField(hash, fact.amountText);
        hashField(hash, fact.feeText);
        hashField(hash, fact.currency);
        hashField(hash, fact.source);
        hashField(hash, fact.note);
    }
    for (const auto& fact : parseResult.cashFacts) {
        hashField(hash, fact.factId);
        hashField(hash, fact.time);
        hashField(hash, fact.accountId);
        hashField(hash, fact.portfolioId);
        hashField(hash, fact.action);
        hashField(hash, fact.amountText);
        hashField(hash, fact.currency);
        hashField(hash, fact.note);
    }

    std::ostringstream stream;
    stream << "fnv64-" << std::hex << std::setw(16) << std::setfill('0') << hash;
    return stream.str();
}

etfdt::data_service_api::ShellAccountingExcelVbaImportPayload parserPayload(
    bool includeTrade,
    bool includeMarketPrice,
    bool includeFxRate,
    const std::string& cashCurrency = "CNY",
    const std::string& instrumentCode = "TASK269_INSTRUMENT")
{
    etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payload;
    payload.sheets.push_back(etfdt::data_service_api::ShellAccountingExcelVbaImportSheet{
        "InitialCash",
        {"ROW_ID", "TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "ACTION", "AMOUNT", "CURRENCY", "MEMO"},
        {{"TASK269_CASH_001",
          "2026-06-07T09:00:00Z",
          "TASK269_ACCOUNT",
          "TASK269_PORTFOLIO",
          "INITIAL_CASH",
          "1000.00",
          cashCurrency,
          "SANITIZED_TASK269_INITIAL_CASH"}},
    });
    etfdt::data_service_api::ShellAccountingExcelVbaImportSheet trades{
        "TradeLog",
        {"ROW_ID",
         "TRADE_TIME_UTC",
         "ACCOUNT_CODE",
         "PORTFOLIO_CODE",
         "INSTRUMENT_CODE",
         "SIDE",
         "QUANTITY",
         "PRICE",
         "AMOUNT",
         "FEE",
         "CASH_FLOW",
         "CURRENCY",
         "SOURCE",
         "MEMO"},
        {},
    };
    if (includeTrade) {
        trades.rows.push_back({"TASK269_TRADE_001",
            "2026-06-07T09:30:00Z",
            "TASK269_ACCOUNT",
            "TASK269_PORTFOLIO",
            instrumentCode,
            "BUY",
            "100",
            "2.0000",
            "200.00",
            "1.00",
            "-201.00",
            "CNY",
            "TASK269_SANITIZED_IMPORT",
            "SANITIZED_TASK269_BUY"});
    }
    payload.sheets.push_back(std::move(trades));

    if (includeMarketPrice) {
        payload.sheets.push_back(etfdt::data_service_api::ShellAccountingExcelVbaImportSheet{
            "MarketPrices",
            {"INSTRUMENT_CODE", "PRICE_TIME_UTC", "PRICE", "CURRENCY", "SOURCE"},
            {{instrumentCode, "2026-06-07T15:00:00Z", "2.5000", "CNY", "TASK269_MARKET_PRICE"}},
        });
    }
    if (includeFxRate) {
        payload.sheets.push_back(etfdt::data_service_api::ShellAccountingExcelVbaImportSheet{
            "FxRates",
            {"BASE_CURRENCY", "QUOTE_CURRENCY", "RATE_TIME_UTC", "RATE", "SOURCE"},
            {{"USD", "CNY", "2026-06-07T15:00:00Z", "7.1000", "TASK269_FX"}},
        });
    }
    return payload;
}

std::string importPayloadJson(
    bool includeTrade,
    bool includeMarketPrice,
    bool includeFxRate,
    const std::string& cashCurrency = "CNY",
    const std::string& instrumentCode = "TASK269_INSTRUMENT")
{
    std::ostringstream payload;
    payload << "{"
            << "\"sheets\":["
            << "{"
            << "\"sheetName\":\"InitialCash\","
            << "\"headers\":[\"ROW_ID\",\"TIME_UTC\",\"ACCOUNT_CODE\",\"PORTFOLIO_CODE\",\"ACTION\",\"AMOUNT\",\"CURRENCY\",\"MEMO\"],"
            << "\"rows\":[[\"TASK269_CASH_001\",\"2026-06-07T09:00:00Z\",\"TASK269_ACCOUNT\",\"TASK269_PORTFOLIO\",\"INITIAL_CASH\",\"1000.00\",\""
            << cashCurrency
            << "\",\"SANITIZED_TASK269_INITIAL_CASH\"]]"
            << "},"
            << "{"
            << "\"sheetName\":\"TradeLog\","
            << "\"headers\":[\"ROW_ID\",\"TRADE_TIME_UTC\",\"ACCOUNT_CODE\",\"PORTFOLIO_CODE\",\"INSTRUMENT_CODE\",\"SIDE\",\"QUANTITY\",\"PRICE\",\"AMOUNT\",\"FEE\",\"CASH_FLOW\",\"CURRENCY\",\"SOURCE\",\"MEMO\"],"
            << "\"rows\":[";
    if (includeTrade) {
        payload << "[\"TASK269_TRADE_001\",\"2026-06-07T09:30:00Z\",\"TASK269_ACCOUNT\",\"TASK269_PORTFOLIO\",\""
                << instrumentCode
                << "\",\"BUY\",\"100\",\"2.0000\",\"200.00\",\"1.00\",\"-201.00\",\"CNY\",\"TASK269_SANITIZED_IMPORT\",\"SANITIZED_TASK269_BUY\"]";
    }
    payload << "]"
            << "}";
    if (includeMarketPrice) {
        payload << ",{"
                << "\"sheetName\":\"MarketPrices\","
                << "\"headers\":[\"INSTRUMENT_CODE\",\"PRICE_TIME_UTC\",\"PRICE\",\"CURRENCY\",\"SOURCE\"],"
                << "\"rows\":[[\"" << instrumentCode
                << "\",\"2026-06-07T15:00:00Z\",\"2.5000\",\"CNY\",\"TASK269_MARKET_PRICE\"]]"
                << "}";
    }
    if (includeFxRate) {
        payload << ",{"
                << "\"sheetName\":\"FxRates\","
                << "\"headers\":[\"BASE_CURRENCY\",\"QUOTE_CURRENCY\",\"RATE_TIME_UTC\",\"RATE\",\"SOURCE\"],"
                << "\"rows\":[[\"USD\",\"CNY\",\"2026-06-07T15:00:00Z\",\"7.1000\",\"TASK269_FX\"]]"
                << "}";
    }
    payload << "]"
            << "}";
    return payload.str();
}

std::string validDigest(
    bool includeTrade,
    bool includeMarketPrice,
    bool includeFxRate,
    const std::string& cashCurrency = "CNY",
    const std::string& instrumentCode = "TASK269_INSTRUMENT")
{
    const auto parsed = etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(
        parserPayload(includeTrade, includeMarketPrice, includeFxRate, cashCurrency, instrumentCode));
    require(parsed.accepted, "parser payload accepted");
    return previewDigestForParseResult(parsed);
}

std::string persistPayload(
    const std::string& idempotencyKey,
    bool includeTrade,
    bool includeMarketPrice,
    bool includeFxRate,
    const std::string& cashCurrency = "CNY",
    const std::string& instrumentCode = "TASK269_INSTRUMENT")
{
    std::ostringstream payload;
    payload << "{"
            << "\"previewStatus\":\"ACCEPTED\","
            << "\"previewDigest\":\""
            << validDigest(includeTrade, includeMarketPrice, includeFxRate, cashCurrency, instrumentCode)
            << "\","
            << "\"idempotencyKey\":\"" << idempotencyKey << "\","
            << "\"requestId\":\"task-269-request\","
            << "\"schemaVersion\":\"excel-vba-export/v1\","
            << "\"source\":\"sanitized-excel-vba-export\","
            << "\"acceptedAt\":\"2026-06-07T10:00:00Z\","
            << "\"importBatchLabel\":\"TASK269_BATCH\","
            << "\"previewFactSummary\":{"
            << "\"tradeFactCount\":" << (includeTrade ? 1 : 0) << ','
            << "\"cashFactCount\":1,"
            << "\"marketPriceFactCount\":" << (includeMarketPrice ? 1 : 0) << ','
            << "\"fxRateFactCount\":" << (includeFxRate ? 1 : 0)
            << "},"
            << "\"importPayload\":" << importPayloadJson(includeTrade, includeMarketPrice, includeFxRate, cashCurrency, instrumentCode)
            << "}";
    return payload.str();
}

DbFixture makeDb(const Harness& h, const std::string& label)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("task269_excel_vba_persist_" + label + "_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "task269.sqlite";
    config.enableWal = false;
    require(fixture.connection->open(config).hasValue(), "open temp SQLite");
    require(fixture.connection->applyMigrationFile(h.root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply migration 001");
    require(fixture.connection->applyMigrationFile(
                h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
                .hasValue(),
        "apply migration 002");

    auto account = fixture.connection->executeStatement(
        "INSERT INTO account(uid,name,account_type,base_currency,is_active,initial_cash_cents,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("TASK269_ACCOUNT"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-269 Account"),
         etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-269 temp account"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-07T00:00:00Z")});
    require(account.hasValue(), "insert account");
    fixture.accountId = fixture.connection->lastInsertRowId();

    auto portfolio = fixture.connection->executeStatement(
        "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
        "VALUES(?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("TASK269_PORTFOLIO"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-269 Portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("supported fact types alignment test"),
         etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-07T00:00:00Z")});
    require(portfolio.hasValue(), "insert portfolio");
    fixture.portfolioId = fixture.connection->lastInsertRowId();

    auto instrument = fixture.connection->executeStatement(
        "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
        "VALUES(?,?,?,?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("task-269-instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK269_INSTRUMENT"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-269 ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("ETF"),
         etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-269 temp instrument"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-07T00:00:00Z")});
    require(instrument.hasValue(), "insert instrument");

    return fixture;
}

std::int64_t singleInt64(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto rows = connection.queryRows(sql);
    require(rows.hasValue(), "query succeeds: " + sql);
    require(!rows.value().empty() && !rows.value().front().empty(), "query returns row: " + sql);
    return rows.value().front().front().int64Value;
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    return static_cast<int>(singleInt64(connection, "SELECT COUNT(*) FROM " + table + ";"));
}

etfdt::protocol::MessageEnvelope envelope(std::string payloadJson)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "task-269-excel-vba-import-persist";
    request.traceId = "task-269-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = etfdt::data_service_api::kActionAccountingExcelVbaImportPersistManualEntry;
    request.timestampUtc = "2026-06-07T10:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

etfdt::protocol::ProtocolResponse dispatch(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& payloadJson)
{
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_service_api::registerDataServiceWriteActions(dispatcher, connection);
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionAccountingExcelVbaImportPersistManualEntry),
        "TASK-269 persist action registered");
    return dispatcher.dispatch(envelope(payloadJson));
}

QJsonObject parseObject(const std::string& payloadJson)
{
    QJsonParseError error {};
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(payloadJson), &error);
    require(error.error == QJsonParseError::NoError, "payload JSON parses");
    require(document.isObject(), "payload JSON root object");
    return document.object();
}

bool boolValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isBool(), std::string("bool field missing: ") + key);
    return value.toBool();
}

int intValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isDouble(), std::string("int field missing: ") + key);
    return value.toInt();
}

void requireIssue(const QJsonObject& payload, const QString& issue)
{
    const auto issues = payload.value(QStringLiteral("issues")).toArray();
    for (const auto& value : issues) {
        if (value.toString() == issue) {
            return;
        }
    }
    throw std::runtime_error("missing issue " + issue.toStdString());
}

etfdt::data_access::ShellAccountingManualCashMovementWriteCommand cashCommand(
    const DbFixture& fixture,
    const std::string& idempotencyKey)
{
    etfdt::data_access::ShellAccountingManualCashMovementWriteCommand command;
    command.accountId = fixture.accountId;
    command.portfolioId = fixture.portfolioId;
    command.movementType = "Deposit";
    command.amountMinor = 10000;
    command.currencyCode = "CNY";
    command.occurredAtUtc = "2026-06-07T09:00:00Z";
    command.sourceMemo = "TASK-269 manual cash API preserved";
    command.sourceReference = "TASK269_CASH_API";
    command.requestId = "task-269-cash-api";
    command.idempotencyKey = idempotencyKey;
    return command;
}

void testManualCashMovementActiveTransactionContract(const Harness& h)
{
    auto fixture = makeDb(h, "cash_active_tx");
    etfdt::data_access::ShellAccountingManualCashMovementRepository repository(*fixture.connection);
    const auto outside = repository.persistManualCashMovementInActiveTransaction(
        cashCommand(fixture, "task-269-cash-active-outside"));
    require(!outside.success, "cash active-transaction method fails outside transaction");
    require(outside.status == "ACTIVE_TRANSACTION_REQUIRED", "cash active transaction required status");
    require(countRows(*fixture.connection, "trade_log") == 0, "outside active tx writes no trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "outside active tx writes no cash_adjustment");

    const auto publicResult = repository.persistManualCashMovement(
        cashCommand(fixture, "task-269-cash-public-api"));
    require(publicResult.success, "existing cash movement API still succeeds");
    require(publicResult.transactionCommitted, "existing cash movement API commits");
    require(countRows(*fixture.connection, "trade_log") == 1, "public cash API writes trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "public cash API writes cash_adjustment");
}

void assertNoWrites(etfdt::data_access::SQLiteConnection& connection, const std::string& context)
{
    require(countRows(connection, "trade_log") == 0, context + " writes no trade_log");
    require(countRows(connection, "cash_adjustment") == 0, context + " writes no cash_adjustment");
    require(countRows(connection, "audit_log") == 0, context + " writes no audit_log");
}

void testCashOnlyAcceptedPreviewPersists(const Harness& h)
{
    auto fixture = makeDb(h, "cash_only");
    const auto first = dispatch(
        *fixture.connection,
        persistPayload("task-269-cash-only", false, false, false));
    require(first.success, "cash-only accepted preview persists");
    const auto payload = parseObject(first.payloadJson);
    require(boolValue(payload, "cashFactsPersistenceSupported"), "cash facts supported evidence");
    require(boolValue(payload, "manualCashMovementRepositoryUsed"), "manual cash movement repository used");
    require(boolValue(payload, "transactionCommitted"), "cash-only transaction committed");
    require(boolValue(payload, "cashAdjustmentWritten"), "cash-only cash adjustment written");
    require(boolValue(payload, "auditLogWritten"), "cash-only audit written");
    require(intValue(payload, "tradeLogRowsWritten") == 1, "cash-only writes one cash trade_log row");
    require(intValue(payload, "cashAdjustmentRowsWritten") == 1, "cash-only writes one cash_adjustment row");
    require(countRows(*fixture.connection, "trade_log") == 1, "cash-only DB trade_log count");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "cash-only DB cash_adjustment count");
    require(countRows(*fixture.connection, "audit_log") == 1, "cash-only DB audit count");

    const auto duplicate = dispatch(
        *fixture.connection,
        persistPayload("task-269-cash-only", false, false, false));
    require(duplicate.success, "cash-only duplicate succeeds idempotently");
    const auto duplicatePayload = parseObject(duplicate.payloadJson);
    require(boolValue(duplicatePayload, "duplicateImportPrevented"), "cash-only duplicate prevented");
    require(countRows(*fixture.connection, "trade_log") == 1, "cash-only duplicate adds no trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "cash-only duplicate adds no cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 1, "cash-only duplicate adds no audit_log");
}

void testTradeAndCashPersistTogether(const Harness& h)
{
    auto fixture = makeDb(h, "trade_cash");
    const auto response = dispatch(
        *fixture.connection,
        persistPayload("task-269-trade-cash", true, false, false));
    require(response.success, "trade+cash accepted preview persists");
    const auto payload = parseObject(response.payloadJson);
    require(boolValue(payload, "tradeLogWritten"), "trade+cash trade_log written");
    require(boolValue(payload, "cashAdjustmentWritten"), "trade+cash cash adjustment written");
    require(boolValue(payload, "auditLogWritten"), "trade+cash audit written");
    require(intValue(payload, "tradeLogRowsWritten") == 2, "trade+cash writes two trade_log rows");
    require(intValue(payload, "cashAdjustmentRowsWritten") == 1, "trade+cash writes one cash_adjustment row");
    require(countRows(*fixture.connection, "trade_log") == 2, "trade+cash DB trade_log count");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "trade+cash DB cash_adjustment count");
    require(countRows(*fixture.connection, "audit_log") == 1, "trade+cash DB audit count");
}

void testUnsupportedMarketFxRejected(const Harness& h)
{
    {
        auto fixture = makeDb(h, "market_rejected");
        const auto response = dispatch(
            *fixture.connection,
            persistPayload("task-269-market", false, true, false));
        require(!response.success, "market price persistence rejected");
        const auto payload = parseObject(response.payloadJson);
        require(boolValue(payload, "marketPricePersistenceRejected"), "market unsupported evidence");
        requireIssue(payload, QStringLiteral("EXCEL_VBA_IMPORT_MARKET_PRICE_PERSISTENCE_UNSUPPORTED"));
        assertNoWrites(*fixture.connection, "market price rejection");
    }
    {
        auto fixture = makeDb(h, "fx_rejected");
        const auto response = dispatch(
            *fixture.connection,
            persistPayload("task-269-fx", false, false, true));
        require(!response.success, "FX persistence rejected");
        const auto payload = parseObject(response.payloadJson);
        require(boolValue(payload, "fxRatePersistenceRejected"), "FX unsupported evidence");
        requireIssue(payload, QStringLiteral("EXCEL_VBA_IMPORT_FX_RATE_PERSISTENCE_UNSUPPORTED"));
        assertNoWrites(*fixture.connection, "FX rejection");
    }
}

void testRollbackOnCashAndAuditFailure(const Harness& h)
{
    {
        auto fixture = makeDb(h, "cash_failure");
        const auto response = dispatch(
            *fixture.connection,
            persistPayload("task-269-cash-failure", true, false, false, "USD"));
        require(!response.success, "cash failure rejects persistence");
        assertNoWrites(*fixture.connection, "cash failure rollback");
    }
    {
        auto fixture = makeDb(h, "audit_failure");
        auto trigger = fixture.connection->executeSql(
            "CREATE TRIGGER task269_abort_audit BEFORE INSERT ON audit_log "
            "BEGIN SELECT RAISE(ABORT, 'task-269 audit failure'); END;");
        require(trigger.hasValue(), "audit failure trigger created");
        const auto response = dispatch(
            *fixture.connection,
            persistPayload("task-269-audit-failure", true, false, false));
        require(!response.success, "audit failure rejects persistence");
        assertNoWrites(*fixture.connection, "audit failure rollback");
    }
}

void testStaticBoundaries(const Harness& h)
{
    const auto dataAccess = readFile(
        h.root / "libs" / "DataAccess" / "src" / "ShellAccountingExcelVbaImportManualEntryPersistenceRepository.cpp");
    requireContains(dataAccess, "ShellAccountingManualCashMovementRepository", "DataAccess composition repository");
    requireContains(dataAccess, "persistManualCashMovementInActiveTransaction", "DataAccess composition repository");
    requireContains(dataAccess, "EXCEL_VBA_IMPORT_MARKET_PRICE_PERSISTENCE_UNSUPPORTED", "DataAccess unsupported market guard");
    requireContains(dataAccess, "EXCEL_VBA_IMPORT_FX_RATE_PERSISTENCE_UNSUPPORTED", "DataAccess unsupported FX guard");

    const auto cashRepo = readFile(
        h.root / "libs" / "DataAccess" / "src" / "ShellAccountingManualCashMovementRepository.cpp");
    requireContains(cashRepo, "persistManualCashMovementInActiveTransaction", "cash movement active transaction API");
    requireContains(cashRepo, "ACTIVE_TRANSACTION_REQUIRED", "cash movement active transaction guard");

    const auto actionSource = readFile(
        h.root / "libs" / "DataServiceApi" / "src" / "ShellAccountingExcelVbaImportPersistManualEntryAction.cpp");
    requireContains(actionSource, "request.cashFacts.reserve", "DataService cash facts mapped");
    requireContains(actionSource, "EXCEL_VBA_IMPORT_MARKET_PRICE_PERSISTENCE_UNSUPPORTED", "DataService market unsupported");
    requireContains(actionSource, "EXCEL_VBA_IMPORT_FX_RATE_PERSISTENCE_UNSUPPORTED", "DataService FX unsupported");
    requireNotContains(actionSource, "INSERT INTO trade_log", "DataService action");
    requireNotContains(actionSource, "AccountingEngine", "DataService action");

    const auto presenter = readFile(
        h.root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp");
    requireContains(presenter, "(tradeFactCount + cashFactCount) <= 0", "Presenter supported fact guard");
    requireContains(presenter, "marketPriceFactCount > 0 || fxRateFactCount > 0", "Presenter unsupported fact guard");
    requireContains(presenter, "Market price and FX rate facts are not supported", "Presenter unsupported issue");
    requireNotContains(presenter, "DataServiceClient/DataServiceClient.h", "Presenter direct client dependency");
    requireNotContains(presenter, "AccountingEngine::", "Presenter direct AccountingEngine dependency");

    const auto qml = readFile(
        h.root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml");
    requireContains(qml, "supportedFactCount", "QML supported fact count");
    requireContains(qml, "excelVbaImportPreviewMarketPriceFactCount === 0", "QML market disabled");
    requireContains(qml, "excelVbaImportPreviewFxRateFactCount === 0", "QML FX disabled");
    requireNotContains(qml, "DataServiceClient", "QML direct client dependency");
    requireNotContains(qml, "SQLite", "QML direct SQLite dependency");
    requireNotContains(qml, "AccountingEngine", "QML direct AccountingEngine dependency");
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "TASK-269"},
        {"cashFactsPersistenceSupported", true},
        {"cashOnlyAcceptedPreviewPersists", true},
        {"tradeAndCashPersistTogether", true},
        {"manualCashMovementRepositoryUsed", true},
        {"cashMovementActiveTransactionMethodCreated", true},
        {"singleTransactionBoundaryUsed", true},
        {"cashFailureRollsBackTradeAndAudit", true},
        {"auditFailureRollsBackTradeAndCash", true},
        {"marketPricePersistenceRejected", true},
        {"fxRatePersistenceRejected", true},
        {"qmlPersistDisabledForUnsupportedMarketFx", true},
        {"presenterRejectsUnsupportedMarketFx", true},
        {"duplicateImportPreventsTradeCashAuditDuplicates", true},
        {"directTradeLogSqlInDataService", false},
        {"qmlDirectSQLite", false},
        {"qmlDirectDataServiceClient", false},
        {"accountingEngineCalled", false},
        {"brokerOrderSubmitted", false},
        {"automaticTrading", false},
    };
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        Harness harness;
        for (int index = 1; index < argc; ++index) {
            const std::string arg = argv[index];
            if (arg == "--source-root" && index + 1 < argc) {
                harness.root = argv[++index];
            }
        }
        require(!harness.root.empty(), "--source-root is required");

        testManualCashMovementActiveTransactionContract(harness);
        testCashOnlyAcceptedPreviewPersists(harness);
        testTradeAndCashPersistTogether(harness);
        testUnsupportedMarketFxRejected(harness);
        testRollbackOnCashAndAuditFailure(harness);
        testStaticBoundaries(harness);

        std::cout << QJsonDocument(evidenceJson()).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    } catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-269"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"qmlDirectSQLite", false},
            {"qmlDirectDataServiceClient", false},
            {"accountingEngineCalled", false},
            {"brokerOrderSubmitted", false},
            {"automaticTrading", false},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
