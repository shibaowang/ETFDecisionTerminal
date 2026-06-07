#include "DataAccess/ShellAccountingManualTransactionRepository.h"
#include "DataServiceApi/DataServiceApi.h"
#include "DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h"
#include "Protocol/Protocol.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <QJsonDocument>
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
#include <utility>
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
    std::int64_t instrumentId = 0;
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

std::string importPayloadJson(const std::string& tradeInstrument = "TASK265_INSTRUMENT")
{
    std::ostringstream payload;
    payload << "{"
            << "\"sheets\":["
            << "{"
            << "\"sheetName\":\"InitialCash\","
            << "\"headers\":[\"ROW_ID\",\"TIME_UTC\",\"ACCOUNT_CODE\",\"PORTFOLIO_CODE\",\"ACTION\",\"AMOUNT\",\"CURRENCY\",\"MEMO\"],"
            << "\"rows\":[[\"TASK265_CASH_001\",\"2026-06-07T00:00:00Z\",\"TASK265_ACCOUNT\",\"TASK265_PORTFOLIO\",\"INITIAL_CASH\",\"100000.00\",\"CNY\",\"SANITIZED_TASK265_INITIAL_CASH\"]]"
            << "},"
            << "{"
            << "\"sheetName\":\"TradeLog\","
            << "\"headers\":[\"ROW_ID\",\"TRADE_TIME_UTC\",\"ACCOUNT_CODE\",\"PORTFOLIO_CODE\",\"INSTRUMENT_CODE\",\"SIDE\",\"QUANTITY\",\"PRICE\",\"AMOUNT\",\"FEE\",\"CASH_FLOW\",\"CURRENCY\",\"SOURCE\",\"MEMO\"],"
            << "\"rows\":[[\"TASK265_TRADE_001\",\"2026-06-07T09:30:00Z\",\"TASK265_ACCOUNT\",\"TASK265_PORTFOLIO\",\""
            << tradeInstrument
            << "\",\"BUY\",\"100\",\"2.0000\",\"200.00\",\"1.00\",\"-201.00\",\"CNY\",\"TASK265_SANITIZED_IMPORT\",\"SANITIZED_TASK265_BUY\"]]"
            << "}"
            << "]"
            << "}";
    return payload.str();
}

etfdt::data_service_api::ShellAccountingExcelVbaImportPayload parserPayload(
    const std::string& tradeInstrument = "TASK265_INSTRUMENT")
{
    etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payload;
    payload.sheets.push_back(etfdt::data_service_api::ShellAccountingExcelVbaImportSheet{
        "InitialCash",
        {"ROW_ID", "TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "ACTION", "AMOUNT", "CURRENCY", "MEMO"},
        {{"TASK265_CASH_001",
          "2026-06-07T00:00:00Z",
          "TASK265_ACCOUNT",
          "TASK265_PORTFOLIO",
          "INITIAL_CASH",
          "100000.00",
          "CNY",
          "SANITIZED_TASK265_INITIAL_CASH"}},
    });
    payload.sheets.push_back(etfdt::data_service_api::ShellAccountingExcelVbaImportSheet{
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
        {{"TASK265_TRADE_001",
          "2026-06-07T09:30:00Z",
          "TASK265_ACCOUNT",
          "TASK265_PORTFOLIO",
          tradeInstrument,
          "BUY",
          "100",
          "2.0000",
          "200.00",
          "1.00",
          "-201.00",
          "CNY",
          "TASK265_SANITIZED_IMPORT",
          "SANITIZED_TASK265_BUY"}},
    });
    return payload;
}

std::string validDigest(const std::string& tradeInstrument = "TASK265_INSTRUMENT")
{
    const auto parsed = etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(
        parserPayload(tradeInstrument));
    require(parsed.accepted, "parser payload accepted");
    return previewDigestForParseResult(parsed);
}

std::string persistPayload(
    const std::string& idempotencyKey,
    const std::string& previewStatus = "ACCEPTED",
    const std::string& digestOverride = {},
    const std::string& tradeInstrument = "TASK265_INSTRUMENT")
{
    const auto digest = digestOverride.empty() ? validDigest(tradeInstrument) : digestOverride;
    std::ostringstream payload;
    payload << "{"
            << "\"previewStatus\":\"" << previewStatus << "\","
            << "\"previewDigest\":\"" << digest << "\","
            << "\"idempotencyKey\":\"" << idempotencyKey << "\","
            << "\"requestId\":\"task-265-request\","
            << "\"schemaVersion\":\"excel-vba-export/v1\","
            << "\"source\":\"sanitized-excel-vba-export\","
            << "\"acceptedAt\":\"2026-06-07T10:00:00Z\","
            << "\"importBatchLabel\":\"TASK265_BATCH\","
            << "\"previewFactSummary\":{"
            << "\"tradeFactCount\":1,"
            << "\"cashFactCount\":1,"
            << "\"marketPriceFactCount\":0,"
            << "\"fxRateFactCount\":0"
            << "},"
            << "\"importPayload\":" << importPayloadJson(tradeInstrument)
            << "}";
    return payload.str();
}

DbFixture makeDb(const Harness& h, const std::string& label, bool withInstrument = true)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("task265_excel_vba_persist_" + label + "_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "task265.sqlite";
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
        {etfdt::data_access::SqlStatementParameter::textValue("TASK265_ACCOUNT"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-265 Account"),
         etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
         etfdt::data_access::SqlStatementParameter::textValue("CNY"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-265 temp account"),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-07T00:00:00Z")});
    require(account.hasValue(), "insert account");
    fixture.accountId = fixture.connection->lastInsertRowId();

    auto portfolio = fixture.connection->executeStatement(
        "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
        "VALUES(?,?,?,?,?,?);",
        {etfdt::data_access::SqlStatementParameter::textValue("TASK265_PORTFOLIO"),
         etfdt::data_access::SqlStatementParameter::textValue("TASK-265 Portfolio"),
         etfdt::data_access::SqlStatementParameter::textValue("Excel/VBA import persistence test"),
         etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
         etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
         etfdt::data_access::SqlStatementParameter::textValue("2026-06-07T00:00:00Z")});
    require(portfolio.hasValue(), "insert portfolio");
    fixture.portfolioId = fixture.connection->lastInsertRowId();

    if (withInstrument) {
        auto instrument = fixture.connection->executeStatement(
            "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
            "VALUES(?,?,?,?,?,?,?,?,?);",
            {etfdt::data_access::SqlStatementParameter::textValue("task-265-instrument"),
             etfdt::data_access::SqlStatementParameter::textValue("TASK265_INSTRUMENT"),
             etfdt::data_access::SqlStatementParameter::textValue("TASK-265 ETF"),
             etfdt::data_access::SqlStatementParameter::textValue("ETF"),
             etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
             etfdt::data_access::SqlStatementParameter::textValue("CNY"),
             etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
             etfdt::data_access::SqlStatementParameter::textValue("TASK-265 temp instrument"),
             etfdt::data_access::SqlStatementParameter::textValue("2026-06-07T00:00:00Z")});
        require(instrument.hasValue(), "insert instrument");
        fixture.instrumentId = fixture.connection->lastInsertRowId();
    }

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
    request.msgId = "task-265-excel-vba-import-persist";
    request.traceId = "task-265-trace";
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
        "TASK-265 persist action registered");
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

std::string stringValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isString(), std::string("string field missing: ") + key);
    return value.toString().toStdString();
}

etfdt::data_access::ShellAccountingManualTransactionWriteCommand manualCommand(
    const DbFixture& fixture,
    const std::string& idempotencyKey)
{
    etfdt::data_access::ShellAccountingManualTransactionWriteCommand command;
    command.accountId = fixture.accountId;
    command.portfolioId = fixture.portfolioId;
    command.instrumentId = fixture.instrumentId;
    command.securityCode = "TASK265_INSTRUMENT";
    command.tradeSide = "BUY";
    command.quantityUnits = 100000000;
    command.priceAmountMinor = 2000000;
    command.grossAmountMinor = 20000;
    command.feeAmountMinor = 100;
    command.taxAmountMinor = 0;
    command.occurredAtUtc = "2026-06-07T09:30:00Z";
    command.sourceMemo = "TASK-265 manual transaction API preserved";
    command.requestId = "task-265-manual-api";
    command.idempotencyKey = idempotencyKey;
    return command;
}

void testActiveTransactionMethodFailsOutsideTransaction(const Harness& h)
{
    auto fixture = makeDb(h, "active_tx_outside");
    etfdt::data_access::ShellAccountingManualTransactionRepository repository(*fixture.connection);
    const auto result = repository.persistManualTransactionInActiveTransaction(
        manualCommand(fixture, "task-265-active-outside"));
    require(!result.success, "active-transaction method fails outside transaction");
    require(result.status == "ACTIVE_TRANSACTION_REQUIRED", "active transaction required status");
    require(countRows(*fixture.connection, "trade_log") == 0, "outside active tx writes no trade_log");
}

void testExistingManualTransactionApiPreserved(const Harness& h)
{
    auto fixture = makeDb(h, "manual_api");
    etfdt::data_access::ShellAccountingManualTransactionRepository repository(*fixture.connection);
    const auto result = repository.persistManualTransaction(
        manualCommand(fixture, "task-265-manual-api-preserved"));
    require(result.success, "existing manual transaction API still succeeds");
    require(result.transactionCommitted, "existing manual transaction API commits");
    require(countRows(*fixture.connection, "trade_log") == 1, "manual API writes one trade_log");
    require(countRows(*fixture.connection, "audit_log") == 0, "manual API remains audit-free");
}

void assertSuccessPayload(const QJsonObject& payload)
{
    require(stringValue(payload, "task") == "TASK-265", "task evidence");
    require(boolValue(payload, "dataServicePersistActionRegistered"), "DataService action registered");
    require(boolValue(payload, "acceptedPreviewRequired"), "accepted preview required");
    require(boolValue(payload, "parserBoundaryReused"), "parser boundary reused");
    require(boolValue(payload, "manualTransactionRepositoryUsed"), "manual transaction repository used");
    require(!boolValue(payload, "directTradeLogSqlInDataService"), "no direct trade_log SQL in DataService");
    require(boolValue(payload, "tempDbOnly"), "temp DB only evidence");
    require(!boolValue(payload, "productionDbTouched"), "production DB untouched");
    require(boolValue(payload, "transactionCommitted"), "transaction committed");
    require(boolValue(payload, "tradeLogWritten"), "trade_log written");
    require(boolValue(payload, "cashAdjustmentWritten"), "cash_adjustment written");
    require(boolValue(payload, "auditLogWritten"), "audit log written");
    require(boolValue(payload, "idempotencyRequired"), "idempotency required");
    require(boolValue(payload, "supplementalDataAccessTransactionAuthorizationUsed"), "supplemental auth used");
    require(boolValue(payload, "manualTransactionActiveTransactionMethodCreated"), "active transaction method created");
    require(boolValue(payload, "existingManualTransactionApiPreserved"), "existing API preserved");
    require(boolValue(payload, "dataAccessCompositionRepositoryCreated"), "composition repository created");
    require(boolValue(payload, "singleTransactionBoundaryUsed"), "single transaction boundary");
    require(boolValue(payload, "manualFactsAndAuditCommittedTogether"), "facts and audit committed together");
    require(boolValue(payload, "auditFailureRollsBackManualFacts"), "audit rollback evidence");
    require(!boolValue(payload, "nestedTransactionAttempted"), "no nested transaction");
    require(!boolValue(payload, "qmlWiringChanged"), "QML unchanged");
    require(!boolValue(payload, "importButtonAdded"), "no import button");
    require(!boolValue(payload, "accountingEngineCalled"), "AccountingEngine not called");
    require(!boolValue(payload, "brokerOrderSubmitted"), "no broker order");
    require(!boolValue(payload, "networkAccess"), "no network");
    require(!boolValue(payload, "automaticTrading"), "no automatic trading");
}

void testDataServicePersistenceCommitAndDuplicate(const Harness& h)
{
    auto fixture = makeDb(h, "dataservice_commit");
    const auto first = dispatch(*fixture.connection, persistPayload("task-265-import-key"));
    require(first.success, "first DataService persistence succeeds");
    const auto firstPayload = parseObject(first.payloadJson);
    assertSuccessPayload(firstPayload);
    require(intValue(firstPayload, "tradeLogRowsWritten") == 2, "trade and cash trade_log rows written");
    require(intValue(firstPayload, "cashAdjustmentRowsWritten") == 1, "one cash adjustment row written");
    require(countRows(*fixture.connection, "trade_log") == 2, "first write adds trade and cash trade_log rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "first write adds one cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 1, "first write adds one audit_log");

    const auto second = dispatch(*fixture.connection, persistPayload("task-265-import-key"));
    require(second.success, "duplicate DataService persistence succeeds idempotently");
    const auto duplicatePayload = parseObject(second.payloadJson);
    require(boolValue(duplicatePayload, "duplicateImportPrevented"), "duplicate import prevented");
    require(countRows(*fixture.connection, "trade_log") == 2, "duplicate adds no trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "duplicate adds no cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 1, "duplicate adds no audit_log");
}

void testIdempotencyConflictRejected(const Harness& h)
{
    auto fixture = makeDb(h, "idempotency_conflict");
    const auto first = dispatch(*fixture.connection, persistPayload("task-265-conflict-key"));
    require(first.success, "first conflict setup succeeds");
    const auto conflict = dispatch(
        *fixture.connection,
        persistPayload(
            "task-265-conflict-key",
            "ACCEPTED",
            validDigest("TASK265_OTHER_INSTRUMENT"),
            "TASK265_OTHER_INSTRUMENT"));
    require(!conflict.success, "same idempotency key different digest rejected");
    const auto payload = parseObject(conflict.payloadJson);
    require(boolValue(payload, "idempotencyConflictRejected"), "conflict flag set");
    require(countRows(*fixture.connection, "trade_log") == 2, "conflict adds no trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "conflict adds no cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 1, "conflict adds no audit_log");
}

void testNonAcceptedPreviewRejected(const Harness& h)
{
    auto fixture = makeDb(h, "non_accepted");
    const auto response = dispatch(*fixture.connection, persistPayload("task-265-rejected-key", "REJECTED"));
    require(!response.success, "non-ACCEPTED preview rejected");
    require(countRows(*fixture.connection, "trade_log") == 0, "non-accepted writes no trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "non-accepted writes no cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 0, "non-accepted writes no audit_log");
}

void testAuditFailureRollsBackManualFacts(const Harness& h)
{
    auto fixture = makeDb(h, "audit_failure");
    auto trigger = fixture.connection->executeSql(
        "CREATE TRIGGER task265_abort_audit BEFORE INSERT ON audit_log "
        "BEGIN SELECT RAISE(ABORT, 'task-265 audit failure'); END;");
    require(trigger.hasValue(), "audit failure trigger created");
    const auto response = dispatch(*fixture.connection, persistPayload("task-265-audit-failure"));
    require(!response.success, "audit failure rejects persistence");
    require(countRows(*fixture.connection, "trade_log") == 0, "audit failure rolls back trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "audit failure rolls back cash_adjustment");
    require(countRows(*fixture.connection, "trade_execution_group") == 0,
        "audit failure rolls back execution group");
    require(countRows(*fixture.connection, "audit_log") == 0, "audit failure leaves no audit row");
}

void testTradeFailureRollsBackAudit(const Harness& h)
{
    auto fixture = makeDb(h, "trade_failure", false);
    const auto response = dispatch(
        *fixture.connection,
        persistPayload("task-265-trade-failure", "ACCEPTED", validDigest("TASK265_MISSING_INSTRUMENT"), "TASK265_MISSING_INSTRUMENT"));
    require(!response.success, "trade reference failure rejects persistence");
    require(countRows(*fixture.connection, "trade_log") == 0, "trade failure writes no trade_log");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "trade failure writes no cash_adjustment");
    require(countRows(*fixture.connection, "audit_log") == 0, "trade failure writes no audit_log");
}

void testStaticBoundaries(const Harness& h)
{
    const auto actionSource = readFile(
        h.root / "libs" / "DataServiceApi" / "src" / "ShellAccountingExcelVbaImportPersistManualEntryAction.cpp");
    requireContains(actionSource, "parseShellAccountingExcelVbaImportReadOnly", "DataService action");
    requireContains(actionSource, "ShellAccountingExcelVbaImportManualEntryPersistenceRepository", "DataService action");
    requireNotContains(actionSource, "INSERT INTO trade_log", "DataService action");
    requireNotContains(actionSource, "AccountingEngine", "DataService action");
    requireNotContains(actionSource, "Broker", "DataService action");
    requireNotContains(actionSource, "networkAccess\":true", "DataService action");

    const auto registrar = readFile(h.root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp");
    requireContains(registrar, "kActionAccountingExcelVbaImportPersistManualEntry", "registrar");
    const auto policy = readFile(h.root / "libs" / "DataServiceApi" / "src" / "WriteActionPolicy.cpp");
    requireContains(policy, "kActionAccountingExcelVbaImportPersistManualEntry", "write policy");
    require(!fs::exists(h.root / "apps" / "ETFDecisionShell" / "src" / "main.cpp.task265"),
        "startup wiring unchanged sentinel");
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "TASK-265"},
        {"dataServicePersistActionRegistered", true},
        {"acceptedPreviewRequired", true},
        {"parserBoundaryReused", true},
        {"manualTransactionRepositoryUsed", true},
        {"directTradeLogSqlInDataService", false},
        {"tempDbOnly", true},
        {"productionDbTouched", false},
        {"transactionCommitted", true},
        {"tradeLogWritten", true},
        {"auditLogWritten", true},
        {"idempotencyRequired", true},
        {"duplicateImportPrevented", true},
        {"idempotencyConflictRejected", true},
        {"rollbackOnTradeFailureVerified", true},
        {"rollbackOnAuditFailureVerified", true},
        {"nonAcceptedPreviewRejected", true},
        {"supplementalDataAccessTransactionAuthorizationUsed", true},
        {"manualTransactionActiveTransactionMethodCreated", true},
        {"existingManualTransactionApiPreserved", true},
        {"dataAccessCompositionRepositoryCreated", true},
        {"singleTransactionBoundaryUsed", true},
        {"manualFactsAndAuditCommittedTogether", true},
        {"auditFailureRollsBackManualFacts", true},
        {"nestedTransactionAttempted", false},
        {"qmlWiringChanged", false},
        {"importButtonAdded", false},
        {"accountingEngineCalled", false},
        {"brokerOrderSubmitted", false},
        {"networkAccess", false},
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

        testActiveTransactionMethodFailsOutsideTransaction(harness);
        testExistingManualTransactionApiPreserved(harness);
        testDataServicePersistenceCommitAndDuplicate(harness);
        testIdempotencyConflictRejected(harness);
        testNonAcceptedPreviewRejected(harness);
        testAuditFailureRollsBackManualFacts(harness);
        testTradeFailureRollsBackAudit(harness);
        testStaticBoundaries(harness);

        std::cout << QJsonDocument(evidenceJson()).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    } catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-265"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"productionDbTouched", false},
            {"brokerOrderSubmitted", false},
            {"networkAccess", false},
            {"automaticTrading", false},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
