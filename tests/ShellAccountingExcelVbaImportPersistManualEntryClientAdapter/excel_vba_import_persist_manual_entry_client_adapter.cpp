#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h"
#include "DataServiceClient/DataServiceClientApi.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

namespace fs = std::filesystem;

struct Harness {
    fs::path root;
};

struct DbFixture {
    fs::path directory;
    std::unique_ptr<etfdt::data_access::SQLiteConnection> connection =
        std::make_unique<etfdt::data_access::SQLiteConnection>();
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

bool waitUntil(const std::function<bool()>& predicate, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        if (predicate()) {
            return true;
        }
        QThread::msleep(5);
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    return predicate();
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
    const std::string& instrumentCode = "TASK266_INSTRUMENT")
{
    etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payload;
    payload.sheets.push_back(etfdt::data_service_api::ShellAccountingExcelVbaImportSheet{
        "InitialCash",
        {"ROW_ID", "TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "ACTION", "AMOUNT", "CURRENCY", "MEMO"},
        {{"TASK266_CASH_001",
          "2026-06-07T00:00:00Z",
          "TASK266_ACCOUNT",
          "TASK266_PORTFOLIO",
          "INITIAL_CASH",
          "100000.00",
          "CNY",
          "SANITIZED_TASK266_INITIAL_CASH"}},
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
        {{"TASK266_TRADE_001",
          "2026-06-07T09:30:00Z",
          "TASK266_ACCOUNT",
          "TASK266_PORTFOLIO",
          instrumentCode,
          "BUY",
          "100",
          "2.0000",
          "200.00",
          "1.00",
          "-201.00",
          "CNY",
          "TASK266_SANITIZED_IMPORT",
          "SANITIZED_TASK266_BUY"}},
    });
    return payload;
}

std::string importPayloadJson(const std::string& instrumentCode = "TASK266_INSTRUMENT")
{
    std::ostringstream payload;
    payload << "{"
            << "\"sheets\":["
            << "{"
            << "\"sheetName\":\"InitialCash\","
            << "\"headers\":[\"ROW_ID\",\"TIME_UTC\",\"ACCOUNT_CODE\",\"PORTFOLIO_CODE\",\"ACTION\",\"AMOUNT\",\"CURRENCY\",\"MEMO\"],"
            << "\"rows\":[[\"TASK266_CASH_001\",\"2026-06-07T00:00:00Z\",\"TASK266_ACCOUNT\",\"TASK266_PORTFOLIO\",\"INITIAL_CASH\",\"100000.00\",\"CNY\",\"SANITIZED_TASK266_INITIAL_CASH\"]]"
            << "},"
            << "{"
            << "\"sheetName\":\"TradeLog\","
            << "\"headers\":[\"ROW_ID\",\"TRADE_TIME_UTC\",\"ACCOUNT_CODE\",\"PORTFOLIO_CODE\",\"INSTRUMENT_CODE\",\"SIDE\",\"QUANTITY\",\"PRICE\",\"AMOUNT\",\"FEE\",\"CASH_FLOW\",\"CURRENCY\",\"SOURCE\",\"MEMO\"],"
            << "\"rows\":[[\"TASK266_TRADE_001\",\"2026-06-07T09:30:00Z\",\"TASK266_ACCOUNT\",\"TASK266_PORTFOLIO\",\""
            << instrumentCode
            << "\",\"BUY\",\"100\",\"2.0000\",\"200.00\",\"1.00\",\"-201.00\",\"CNY\",\"TASK266_SANITIZED_IMPORT\",\"SANITIZED_TASK266_BUY\"]]"
            << "}"
            << "]"
            << "}";
    return payload.str();
}

std::string validDigest(const std::string& instrumentCode = "TASK266_INSTRUMENT")
{
    const auto parsed = etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(
        parserPayload(instrumentCode));
    require(parsed.accepted, "parser payload accepted");
    return previewDigestForParseResult(parsed);
}

etfdt::data_service_client::ExcelVbaImportPersistManualEntryRequest persistRequest(
    const std::string& idempotencyKey,
    const std::string& previewStatus = "ACCEPTED",
    const std::string& digestOverride = {},
    const std::string& instrumentCode = "TASK266_INSTRUMENT")
{
    etfdt::data_service_client::ExcelVbaImportPersistManualEntryRequest request;
    request.previewStatus = previewStatus;
    request.previewDigest = digestOverride.empty() ? validDigest(instrumentCode) : digestOverride;
    request.idempotencyKey = idempotencyKey;
    request.schemaVersion = "excel-vba-export/v1";
    request.source = "sanitized-excel-vba-export";
    request.acceptedAt = "2026-06-07T10:00:00Z";
    request.importBatchLabel = "TASK266_BATCH";
    request.requestId = "task-266-request";
    request.factSummary.tradeFactCount = 1;
    request.factSummary.cashFactCount = 1;
    request.factSummary.marketPriceFactCount = 0;
    request.factSummary.fxRateFactCount = 0;
    request.sanitizedImportPayloadJson = importPayloadJson(instrumentCode);
    return request;
}

DbFixture makeDb(const Harness& h)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("task266_excel_vba_persist_client_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "task266.sqlite";
    config.enableWal = false;
    require(fixture.connection->open(config).hasValue(), "open temp SQLite");
    require(
        fixture.connection->applyMigrationFile(h.root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply migration 001");
    require(
        fixture.connection->applyMigrationFile(
            h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
            .hasValue(),
        "apply migration 002");

    require(
        fixture.connection
            ->executeStatement(
                "INSERT INTO account(uid,name,account_type,base_currency,is_active,initial_cash_cents,memo,created_at_utc) "
                "VALUES(?,?,?,?,?,?,?,?);",
                {etfdt::data_access::SqlStatementParameter::textValue("TASK266_ACCOUNT"),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-266 Account"),
                 etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
                 etfdt::data_access::SqlStatementParameter::textValue("CNY"),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-266 temp account"),
                 etfdt::data_access::SqlStatementParameter::textValue("2026-06-07T00:00:00Z")})
            .hasValue(),
        "insert account");

    require(
        fixture.connection
            ->executeStatement(
                "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
                "VALUES(?,?,?,?,?,?);",
                {etfdt::data_access::SqlStatementParameter::textValue("TASK266_PORTFOLIO"),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-266 Portfolio"),
                 etfdt::data_access::SqlStatementParameter::textValue("Excel/VBA client adapter test"),
                 etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
                 etfdt::data_access::SqlStatementParameter::textValue("2026-06-07T00:00:00Z")})
            .hasValue(),
        "insert portfolio");

    require(
        fixture.connection
            ->executeStatement(
                "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
                "VALUES(?,?,?,?,?,?,?,?,?);",
                {etfdt::data_access::SqlStatementParameter::textValue("task-266-instrument"),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK266_INSTRUMENT"),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-266 ETF"),
                 etfdt::data_access::SqlStatementParameter::textValue("ETF"),
                 etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
                 etfdt::data_access::SqlStatementParameter::textValue("CNY"),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-266 temp instrument"),
                 etfdt::data_access::SqlStatementParameter::textValue("2026-06-07T00:00:00Z")})
            .hasValue(),
        "insert instrument");
    return fixture;
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + table + ";");
    require(count.hasValue(), "count query succeeds for " + table);
    return static_cast<int>(count.value());
}

void requireMappedSuccess(
    const etfdt::data_service_client::ExcelVbaImportPersistManualEntryResult& result)
{
    require(result.protocolSuccess, "protocol success mapped");
    require(result.action == etfdt::data_service_api::kActionAccountingExcelVbaImportPersistManualEntry,
        "persist action name mapped");
    require(result.task == "TASK-265", "service task mapped");
    require(result.status == "OK", "success status mapped");
    require(result.dataServicePersistActionRegistered, "registered flag mapped");
    require(result.acceptedPreviewRequired, "accepted preview required mapped");
    require(result.parserBoundaryReused, "parser boundary reused mapped");
    require(result.manualTransactionRepositoryUsed, "manual transaction repository mapped");
    require(!result.directTradeLogSqlInDataService, "direct trade_log SQL flag false");
    require(result.transactionCommitted, "transaction committed mapped");
    require(result.tradeLogWritten, "trade_log written mapped");
    require(result.auditLogWritten, "audit log written mapped");
    require(result.idempotencyRequired, "idempotency required mapped");
    require(result.singleTransactionBoundaryUsed, "single transaction boundary mapped");
    require(result.manualFactsAndAuditCommittedTogether, "facts and audit committed together mapped");
    require(!result.nestedTransactionAttempted, "nested transaction flag false");
    require(!result.qmlWiringChanged, "QML unchanged mapped");
    require(!result.importButtonAdded, "no import button mapped");
    require(!result.accountingEngineCalled, "AccountingEngine not called mapped");
    require(!result.brokerOrderSubmitted, "broker not submitted mapped");
    require(!result.networkAccess, "network access false mapped");
    require(!result.credentialAccess, "credential access false mapped");
    require(!result.endpointAccess, "endpoint access false mapped");
    require(!result.automaticTrading, "automatic trading false mapped");
    require(result.tradeLogRowsWritten == 2, "trade row count mapped");
    require(result.auditLogId > 0, "audit id mapped");
}

void testPersistClientAdapter(const Harness& h)
{
    auto fixture = makeDb(h);

    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_service_api::registerDataServiceWriteActions(dispatcher, *fixture.connection);
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionAccountingExcelVbaImportPersistManualEntry),
        "TASK-265 DataService persist action registered");

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalTask266PersistClientAdapter_"
        + std::to_string(QCoreApplication::applicationPid());
    require(host.listen(socketName).hasValue(), "ActionServiceHost listen succeeds");

    etfdt::data_service_client::DataServiceClient client;
    require(client.connect(socketName, 1000).hasValue(), "DataServiceClient connects");
    require(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "service host accepts client");

    auto mappedPayload = etfdt::data_service_client::excelVbaImportPersistManualEntryPayloadJson(
        persistRequest("task-266-payload-shape"));
    requireContains(mappedPayload, "\"previewStatus\"", "typed request payload");
    requireContains(mappedPayload, "\"previewDigest\"", "typed request payload");
    requireContains(mappedPayload, "\"idempotencyKey\"", "typed request payload");
    requireContains(mappedPayload, "\"schemaVersion\"", "typed request payload");
    requireContains(mappedPayload, "\"source\"", "typed request payload");
    requireContains(mappedPayload, "\"acceptedAt\"", "typed request payload");
    requireContains(mappedPayload, "\"importBatchLabel\"", "typed request payload");
    requireContains(mappedPayload, "\"previewFactSummary\"", "typed request payload");
    requireContains(mappedPayload, "\"importPayload\"", "typed request payload");

    const auto first = client.accountingExcelVbaImportPersistManualEntry(
        persistRequest("task-266-import-key"),
        2000);
    require(first.hasValue(), "typed persist client adapter returns first result");
    requireMappedSuccess(first.value());
    require(countRows(*fixture.connection, "trade_log") == 2, "first client call writes trade and cash rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "first client call writes one cash adjustment row");
    require(countRows(*fixture.connection, "audit_log") == 1, "first client call writes one audit row");

    const auto duplicate = client.accountingExcelVbaImportPersistManualEntry(
        persistRequest("task-266-import-key"),
        2000);
    require(duplicate.hasValue(), "typed duplicate result maps to value");
    require(duplicate.value().protocolSuccess, "duplicate protocol success mapped");
    require(duplicate.value().duplicateImportPrevented, "duplicate import prevented mapped");
    require(duplicate.value().status == "IDEMPOTENT_REPLAY", "duplicate status mapped");
    require(countRows(*fixture.connection, "trade_log") == 2, "duplicate adds no trade or cash trade row");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "duplicate adds no cash adjustment row");
    require(countRows(*fixture.connection, "audit_log") == 1, "duplicate adds no audit row");

    const auto conflict = client.accountingExcelVbaImportPersistManualEntry(
        persistRequest(
            "task-266-import-key",
            "ACCEPTED",
            validDigest("TASK266_OTHER_INSTRUMENT"),
            "TASK266_OTHER_INSTRUMENT"),
        2000);
    require(conflict.hasValue(), "typed conflict result maps to value");
    require(!conflict.value().protocolSuccess, "conflict protocol failure mapped");
    require(conflict.value().idempotencyConflictRejected, "idempotency conflict mapped");
    require(conflict.value().status == "IDEMPOTENCY_CONFLICT", "conflict status mapped");
    require(!conflict.value().issues.empty(), "conflict issue codes mapped");
    require(conflict.value().issues.front() == "EXCEL_VBA_IMPORT_IDEMPOTENCY_CONFLICT",
        "conflict sanitized issue mapped");
    require(countRows(*fixture.connection, "trade_log") == 2, "conflict adds no trade or cash trade row");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "conflict adds no cash adjustment row");
    require(countRows(*fixture.connection, "audit_log") == 1, "conflict adds no audit row");

    const auto rejected = client.accountingExcelVbaImportPersistManualEntry(
        persistRequest("task-266-rejected-key", "REJECTED"),
        2000);
    require(rejected.hasValue(), "typed non-accepted result maps to value");
    require(!rejected.value().protocolSuccess, "non-accepted protocol failure mapped");
    require(rejected.value().nonAcceptedPreviewRejected, "non-accepted flag mapped");
    require(rejected.value().status == "REJECTED", "non-accepted status mapped");
    require(!rejected.value().issues.empty(), "non-accepted issue mapped");

    auto invalidRequest = persistRequest("task-266-invalid-client-request");
    invalidRequest.sanitizedImportPayloadJson = "[]";
    const auto invalid = client.accountingExcelVbaImportPersistManualEntry(invalidRequest, 100);
    require(!invalid.hasValue(), "typed client rejects non-object sanitized payload before send");

    client.disconnect();
    host.close();
    fixture.connection->close();
    fs::remove_all(fixture.directory);
}

void testStaticBoundaries(const Harness& h)
{
    const auto clientHeader = readFile(
        h.root / "libs" / "DataServiceClient" / "include" / "DataServiceClient" / "DataServiceClient.h");
    const auto clientSource = readFile(h.root / "libs" / "DataServiceClient" / "src" / "DataServiceClient.cpp");
    const auto clientJson = readFile(h.root / "libs" / "DataServiceClient" / "src" / "DataServiceClientJson.cpp");
    requireContains(clientHeader, "accountingExcelVbaImportPersistManualEntry", "DataServiceClient header");
    requireContains(clientSource, "kActionAccountingExcelVbaImportPersistManualEntry", "DataServiceClient source");
    requireContains(clientJson, "parseExcelVbaImportPersistManualEntryPayloadJson", "DataServiceClient JSON");
    requireNotContains(clientSource, "SQLiteConnection", "DataServiceClient source");
    requireNotContains(clientSource, "DataAccess", "DataServiceClient source");
    requireNotContains(clientSource, "AccountingEngine", "DataServiceClient source");
    requireNotContains(clientSource, "Broker", "DataServiceClient source");
    requireNotContains(clientSource, "INSERT INTO", "DataServiceClient source");
    requireNotContains(clientHeader, "ShellAccountingPresenter", "DataServiceClient header");
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "TASK-266"},
        {"persistClientAdapterCreated", true},
        {"dataServicePersistActionCalled", true},
        {"requestMapped", true},
        {"responseParsed", true},
        {"transactionCommittedMapped", true},
        {"tradeLogWrittenMapped", true},
        {"auditLogWrittenMapped", true},
        {"duplicateImportMapped", true},
        {"idempotencyConflictMapped", true},
        {"sanitizedIssuesMapped", true},
        {"clientDirectSQLite", false},
        {"clientDirectDataAccess", false},
        {"clientDirectAccountingEngine", false},
        {"qmlWiringChanged", false},
        {"presenterWiringChanged", false},
        {"productionDbTouched", false},
        {"brokerOrderSubmitted", false},
        {"networkAccessBeyondLocalDataServiceTransport", false},
        {"automaticTrading", false},
    };
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    try {
        Harness harness;
        for (int index = 1; index < argc; ++index) {
            const std::string arg = argv[index];
            if (arg == "--source-root" && index + 1 < argc) {
                harness.root = argv[++index];
            }
        }
        require(!harness.root.empty(), "--source-root is required");

        testPersistClientAdapter(harness);
        testStaticBoundaries(harness);

        std::cout << QJsonDocument(evidenceJson()).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    } catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-266"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"productionDbTouched", false},
            {"brokerOrderSubmitted", false},
            {"networkAccessBeyondLocalDataServiceTransport", false},
            {"automaticTrading", false},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
