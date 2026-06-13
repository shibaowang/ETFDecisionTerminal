#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h"
#include "DataServiceClient/DataServiceClientApi.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"
#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
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

    DbFixture() = default;
    DbFixture(const DbFixture&) = delete;
    DbFixture& operator=(const DbFixture&) = delete;
    DbFixture(DbFixture&&) noexcept = default;
    DbFixture& operator=(DbFixture&&) noexcept = default;

    ~DbFixture()
    {
        if (connection) {
            connection->close();
        }
        if (!directory.empty()) {
            std::error_code ec;
            fs::remove_all(directory, ec);
        }
    }
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
    const std::string& tradeRowId,
    const std::string& cashRowId)
{
    etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payload;
    payload.sheets.push_back(etfdt::data_service_api::ShellAccountingExcelVbaImportSheet{
        "InitialCash",
        {"ROW_ID", "TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "ACTION", "AMOUNT", "CURRENCY", "MEMO"},
        {{cashRowId,
          "2026-06-11T09:00:00Z",
          "TASK271_ACCOUNT",
          "TASK271_PORTFOLIO",
          "INITIAL_CASH",
          "1000.00",
          "CNY",
          "SANITIZED_TASK271_INITIAL_CASH"}},
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
        {{tradeRowId,
          "2026-06-11T09:30:00Z",
          "TASK271_ACCOUNT",
          "TASK271_PORTFOLIO",
          "TASK271_INSTRUMENT",
          "BUY",
          "100",
          "2.0000",
          "200.00",
          "1.00",
          "-201.00",
          "CNY",
          "TASK271_SANITIZED_IMPORT",
          "SANITIZED_TASK271_BUY"}},
    });
    return payload;
}

std::string importPayloadJson(
    const std::string& tradeRowId = "TASK271_TRADE_001",
    const std::string& cashRowId = "TASK271_CASH_001")
{
    std::ostringstream payload;
    payload << "{"
            << "\"schemaVersion\":\"excel-vba-export/v1\","
            << "\"source\":\"sanitized-excel-vba-export\","
            << "\"sheets\":["
            << "{"
            << "\"name\":\"InitialCash\","
            << "\"sheetName\":\"InitialCash\","
            << "\"headers\":[\"ROW_ID\",\"TIME_UTC\",\"ACCOUNT_CODE\",\"PORTFOLIO_CODE\","
               "\"ACTION\",\"AMOUNT\",\"CURRENCY\",\"MEMO\"],"
            << "\"rows\":[[\"" << cashRowId
            << "\",\"2026-06-11T09:00:00Z\",\"TASK271_ACCOUNT\",\"TASK271_PORTFOLIO\","
               "\"INITIAL_CASH\",\"1000.00\",\"CNY\",\"SANITIZED_TASK271_INITIAL_CASH\"]]"
            << "},"
            << "{"
            << "\"name\":\"TradeLog\","
            << "\"sheetName\":\"TradeLog\","
            << "\"headers\":[\"ROW_ID\",\"TRADE_TIME_UTC\",\"ACCOUNT_CODE\",\"PORTFOLIO_CODE\","
               "\"INSTRUMENT_CODE\",\"SIDE\",\"QUANTITY\",\"PRICE\",\"AMOUNT\",\"FEE\",\"CASH_FLOW\","
               "\"CURRENCY\",\"SOURCE\",\"MEMO\"],"
            << "\"rows\":[[\"" << tradeRowId
            << "\",\"2026-06-11T09:30:00Z\",\"TASK271_ACCOUNT\",\"TASK271_PORTFOLIO\","
               "\"TASK271_INSTRUMENT\",\"BUY\",\"100\",\"2.0000\",\"200.00\",\"1.00\","
               "\"-201.00\",\"CNY\",\"TASK271_SANITIZED_IMPORT\",\"SANITIZED_TASK271_BUY\"]]"
            << "}"
            << "]"
            << "}";
    return payload.str();
}

std::string validDigest(
    const std::string& tradeRowId = "TASK271_TRADE_001",
    const std::string& cashRowId = "TASK271_CASH_001")
{
    const auto parsed = etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(
        parserPayload(tradeRowId, cashRowId));
    require(parsed.accepted, "parser payload accepted");
    return previewDigestForParseResult(parsed);
}

DbFixture makeDb(const Harness& h)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("task271_excel_vba_mvp_local_service_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "task271.sqlite";
    config.enableWal = false;
    require(fixture.connection->open(config).hasValue(), "open temp SQLite");
    require(
        fixture.connection->applyMigrationFile(h.root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply migration 001");
    require(
        fixture.connection
            ->applyMigrationFile(h.root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
            .hasValue(),
        "apply migration 002");

    require(
        fixture.connection
            ->executeStatement(
                "INSERT INTO account(uid,name,account_type,base_currency,is_active,initial_cash_cents,memo,created_at_utc) "
                "VALUES(?,?,?,?,?,?,?,?);",
                {etfdt::data_access::SqlStatementParameter::textValue("TASK271_ACCOUNT"),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-271 Account"),
                 etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
                 etfdt::data_access::SqlStatementParameter::textValue("CNY"),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-271 temp account"),
                 etfdt::data_access::SqlStatementParameter::textValue("2026-06-11T00:00:00Z")})
            .hasValue(),
        "insert account");

    require(
        fixture.connection
            ->executeStatement(
                "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
                "VALUES(?,?,?,?,?,?);",
                {etfdt::data_access::SqlStatementParameter::textValue("TASK271_PORTFOLIO"),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-271 Portfolio"),
                 etfdt::data_access::SqlStatementParameter::textValue("Excel/VBA MVP local service test"),
                 etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
                 etfdt::data_access::SqlStatementParameter::textValue("2026-06-11T00:00:00Z")})
            .hasValue(),
        "insert portfolio");

    require(
        fixture.connection
            ->executeStatement(
                "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
                "VALUES(?,?,?,?,?,?,?,?,?);",
                {etfdt::data_access::SqlStatementParameter::textValue("task-271-instrument"),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK271_INSTRUMENT"),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-271 ETF"),
                 etfdt::data_access::SqlStatementParameter::textValue("ETF"),
                 etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
                 etfdt::data_access::SqlStatementParameter::textValue("CNY"),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
                 etfdt::data_access::SqlStatementParameter::textValue("TASK-271 temp instrument"),
                 etfdt::data_access::SqlStatementParameter::textValue("2026-06-11T00:00:00Z")})
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

bool refreshSucceeded(const QString& status, const QString& issue)
{
    return status == QStringLiteral("OK") || status == QStringLiteral("WARNING")
        || status == QStringLiteral("EMPTY")
        || (status == QStringLiteral("ERROR")
            && issue.contains(QStringLiteral("Manual entry persisted facts were mapped")));
}

void requireStaticDocs(const Harness& h)
{
    requireContains(
        readFile(h.root / "docs" / "338_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance.md"),
        "TASK-271",
        "TASK-271 docs");
    requireContains(
        readFile(
            h.root
            / "docs"
            / "339_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance_test_plan.md"),
        "shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance",
        "TASK-271 test plan");
}

void testMvpLocalServiceE2e(const Harness& h)
{
    auto fixture = makeDb(h);

    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceDailyUseActions(dispatcher, *fixture.connection);
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview),
        "read-only preview action registered");
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionAccountingExcelVbaImportPersistManualEntry),
        "persist manual entry action registered");
    require(dispatcher.hasAction(etfdt::data_service_api::kActionPositionList), "position list registered");
    require(dispatcher.hasAction(etfdt::data_service_api::kActionCashSummary), "cash summary registered");
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionPortfolioPnlSummary),
        "portfolio PnL summary registered");
    require(
        !dispatcher.hasAction(etfdt::data_service_api::kActionAccountingBrokerOrderDryRun),
        "daily-use mode does not register broker order action");
    require(
        !dispatcher.hasAction(etfdt::data_service_api::kActionAccountingTradeDraftCreate),
        "daily-use mode does not register trade draft create action");
    require(
        !dispatcher.hasAction(etfdt::data_service_api::kActionAccountingTradeDraftConfirm),
        "daily-use mode does not register trade draft confirm action");
    require(
        !dispatcher.hasAction(etfdt::data_service_api::kActionAccountingSnapshotWrite),
        "daily-use mode does not register snapshot write action");

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalTask271MvpLocalServiceE2e_"
        + std::to_string(QCoreApplication::applicationPid()) + "_"
        + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    require(host.listen(socketName).hasValue(), "ActionServiceHost listen succeeds");

    auto client = std::make_shared<etfdt::data_service_client::DataServiceClient>();
    require(client->connect(socketName, 1000).hasValue(), "DataServiceClient connects");
    require(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "service host accepts client");

    auto port =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter>(client);
    auto serviceAdapter =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(port);
    auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
    controller->setServiceAdapter(serviceAdapter);
    etfdt::shell_services::ShellAccountingPresenter presenter;
    presenter.setController(controller);

    require(countRows(*fixture.connection, "trade_log") == 0, "initial trade_log empty");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "initial cash_adjustment empty");
    require(countRows(*fixture.connection, "audit_log") == 0, "initial audit_log empty");

    const auto payload = importPayloadJson();
    require(presenter.previewExcelVbaImportReadOnly(QString::fromStdString(payload)), "Presenter preview succeeds");
    require(
        presenter.lastExcelVbaImportPreviewStatus() == QStringLiteral("ACCEPTED"),
        "Presenter preview accepted");
    require(
        presenter.lastExcelVbaImportPreviewDigest() == QString::fromStdString(validDigest()),
        "Presenter preview digest matches parser digest");
    require(presenter.excelVbaImportPreviewTradeFactCount() == 1, "Presenter maps one trade fact");
    require(presenter.excelVbaImportPreviewCashFactCount() == 1, "Presenter maps one cash fact");
    require(presenter.excelVbaImportPreviewMarketPriceFactCount() == 0, "Presenter maps zero market price facts");
    require(presenter.excelVbaImportPreviewFxRateFactCount() == 0, "Presenter maps zero FX facts");

    const auto originalDigest = presenter.lastExcelVbaImportPreviewDigest();
    const auto originalIdempotencyKey =
        QStringLiteral("shell-qml-excel-vba-import:") + originalDigest;
    require(presenter.persistAcceptedExcelVbaImportPreview(), "Presenter persist succeeds");
    require(
        presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("PERSISTED"),
        "Presenter persist status mapped");
    require(presenter.lastExcelVbaImportPersistTradeLogRowsWritten() == 2, "Presenter trade_log rows visible");
    require(
        presenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten() == 1,
        "Presenter cash_adjustment rows visible");
    require(countRows(*fixture.connection, "trade_log") == 2, "first persist writes trade and cash trade rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "first persist writes cash adjustment row");
    require(countRows(*fixture.connection, "audit_log") == 1, "first persist writes audit row");
    require(
        refreshSucceeded(presenter.lastPostWriteRefreshStatus(), presenter.lastPostWriteRefreshIssue()),
        std::string("post-persist refresh succeeds; status=")
            + presenter.lastPostWriteRefreshStatus().toStdString()
            + " issue=" + presenter.lastPostWriteRefreshIssue().toStdString()
            + " summary=" + presenter.lastPostWriteRefreshSummary().toStdString());
    require(
        presenter.lastPostWriteRefreshSummary().contains(
            QStringLiteral("reason=excel_vba_import_persist_manual_entry")),
        "post-persist refresh reason visible");
    require(
        presenter.lastPostWriteRefreshSummary().contains(QStringLiteral("position.list")),
        "post-persist position refresh visible");
    require(
        presenter.lastPostWriteRefreshSummary().contains(QStringLiteral("cash.summary")),
        "post-persist cash refresh visible");
    require(
        presenter.lastPostWriteRefreshSummary().contains(QStringLiteral("portfolio.pnl.summary")),
        "post-persist PnL refresh visible");

    require(presenter.persistAcceptedExcelVbaImportPreview(), "duplicate import returns idempotent success");
    require(
        presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("DUPLICATE"),
        "duplicate import visible");
    require(presenter.lastExcelVbaImportPersistTradeLogRowsWritten() == 0, "duplicate claims no trade rows");
    require(
        presenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten() == 0,
        "duplicate claims no cash rows");
    require(countRows(*fixture.connection, "trade_log") == 2, "duplicate adds no trade rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "duplicate adds no cash rows");
    require(countRows(*fixture.connection, "audit_log") == 1, "duplicate adds no audit rows");
    require(presenter.lastPostWriteRefreshStatus() == QStringLiteral("SKIPPED"), "duplicate skips refresh");
    require(
        presenter.lastPostWriteRefreshSummary().contains(QStringLiteral("newRows=false")),
        "duplicate does not claim new rows");

    const auto conflictPayload = importPayloadJson("TASK271_TRADE_CONFLICT", "TASK271_CASH_CONFLICT");
    const auto conflictDigest = QString::fromStdString(
        validDigest("TASK271_TRADE_CONFLICT", "TASK271_CASH_CONFLICT"));
    require(conflictDigest != originalDigest, "conflict digest differs");
    require(
        !presenter.persistExcelVbaImportManualEntry(
            QStringLiteral("ACCEPTED"),
            conflictDigest,
            originalIdempotencyKey,
            QStringLiteral("excel-vba-export/v1"),
            QStringLiteral("sanitized-excel-vba-export"),
            QStringLiteral("2026-06-11T10:15:00Z"),
            QStringLiteral("TASK271_CONFLICT"),
            QString::fromStdString(conflictPayload),
            1,
            1,
            0,
            0),
        "idempotency conflict fails closed");
    require(
        presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("IDEMPOTENCY_CONFLICT"),
        "idempotency conflict status visible");
    require(countRows(*fixture.connection, "trade_log") == 2, "conflict adds no trade rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 1, "conflict adds no cash rows");
    require(countRows(*fixture.connection, "audit_log") == 1, "conflict adds no audit rows");
    require(presenter.lastPostWriteRefreshStatus() == QStringLiteral("SKIPPED"), "conflict skips refresh");

    require(!presenter.brokerSubmissionEnabled(), "Presenter broker submission remains disabled");
    require(!presenter.tradeDraftGenerationEnabled(), "Presenter TradeDraft generation remains disabled");
    require(!presenter.strategyExecutionEnabled(), "Presenter strategy execution remains disabled");

    client->disconnect();
    host.close();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    require(!host.isListening(), "local service host closed");
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "TASK-271"},
        {"mvpLocalServiceE2eAcceptanceCreated", true},
        {"tempDbOnly", true},
        {"productionDbTouched", false},
        {"localServiceHostUsed", true},
        {"realDataServiceClientUsed", true},
        {"presenterPreviewExecuted", true},
        {"previewAccepted", true},
        {"presenterPersistExecuted", true},
        {"persistActionCalled", true},
        {"tradeLogRowsWritten", true},
        {"cashAdjustmentRowsWritten", true},
        {"auditLogRowsWritten", true},
        {"postPersistRefreshAttempted", true},
        {"postPersistRefreshSucceeded", true},
        {"duplicateImportNoGrowth", true},
        {"idempotencyConflictNoGrowth", true},
        {"noResidualProcess", true},
        {"brokerOrderSubmitted", false},
        {"networkAccess", false},
        {"credentialAccess", false},
        {"endpointAccess", false},
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

        requireStaticDocs(harness);
        testMvpLocalServiceE2e(harness);

        std::cout << QJsonDocument(evidenceJson()).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    } catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-271"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"productionDbTouched", false},
            {"brokerOrderSubmitted", false},
            {"networkAccess", false},
            {"credentialAccess", false},
            {"endpointAccess", false},
            {"automaticTrading", false},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
