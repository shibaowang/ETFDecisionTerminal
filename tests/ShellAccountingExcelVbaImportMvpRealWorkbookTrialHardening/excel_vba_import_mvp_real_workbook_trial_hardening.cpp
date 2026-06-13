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
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QThread>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
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

struct SampleSpec {
    const char* fileName;
    int tradeFacts;
    int cashFacts;
    int marketPriceFacts;
    int fxRateFacts;
};

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string lower(std::string value)
{
    for (auto& ch : value) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = static_cast<char>(ch - 'A' + 'a');
        }
    }
    return value;
}

std::string readFile(const fs::path& path)
{
    QFile file(QString::fromStdString(path.string()));
    require(file.exists(), "missing file: " + path.string());
    require(file.open(QIODevice::ReadOnly), "cannot read file: " + path.string());
    return QString::fromUtf8(file.readAll()).toStdString();
}

void requireContains(const std::string& text, std::string_view token, std::string_view context)
{
    require(text.find(std::string(token)) != std::string::npos,
            std::string(context) + " missing `" + std::string(token) + "`");
}

void requireNotContainsLower(
    const std::string& text,
    std::string_view token,
    std::string_view context)
{
    require(lower(text).find(lower(std::string(token))) == std::string::npos,
            std::string(context) + " unexpectedly contains `" + std::string(token) + "`");
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

QJsonObject parseJsonObjectText(const std::string& text, const std::string& context)
{
    QJsonParseError parseError {};
    const auto document =
        QJsonDocument::fromJson(QByteArray::fromStdString(text), &parseError);
    require(parseError.error == QJsonParseError::NoError, context + " parses as JSON");
    require(document.isObject(), context + " root is object");
    return document.object();
}

QJsonObject parseJsonObject(const fs::path& path)
{
    return parseJsonObjectText(readFile(path), path.string());
}

std::string stringField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isString(), std::string("missing string field: ") + key);
    return value.toString().toStdString();
}

std::string optionalStringField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isString() ? value.toString().toStdString() : std::string();
}

QJsonArray arrayField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isArray(), std::string("missing array field: ") + key);
    return value.toArray();
}

QJsonObject objectField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isObject(), std::string("missing object field: ") + key);
    return value.toObject();
}

int intField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isDouble(), std::string("missing numeric field: ") + key);
    return value.toInt();
}

bool boolField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isBool(), std::string("missing bool field: ") + key);
    return value.toBool();
}

etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payloadFromSample(
    const QJsonObject& root)
{
    etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payload;
    payload.rawUserDataExposed = false;
    require(stringField(root, "schemaVersion") == "excel-vba-export/v1", "schemaVersion exact");
    require(stringField(root, "source") == "sanitized-excel-vba-export", "source exact");

    for (const auto& sheetValue : arrayField(root, "sheets")) {
        require(sheetValue.isObject(), "sheet is object");
        const auto sheetObject = sheetValue.toObject();
        etfdt::data_service_api::ShellAccountingExcelVbaImportSheet sheet;
        sheet.sheetName = optionalStringField(sheetObject, "sheetName");
        if (sheet.sheetName.empty()) {
            sheet.sheetName = stringField(sheetObject, "name");
        }
        const auto compatibleName = optionalStringField(sheetObject, "name");
        if (!compatibleName.empty()) {
            require(compatibleName == sheet.sheetName, "sheet name compatibility");
        }
        for (const auto& headerValue : arrayField(sheetObject, "headers")) {
            require(headerValue.isString(), "header is string");
            sheet.headers.push_back(headerValue.toString().toStdString());
        }
        for (const auto& rowValue : arrayField(sheetObject, "rows")) {
            require(rowValue.isArray(), "row is array");
            std::vector<std::string> row;
            for (const auto& cellValue : rowValue.toArray()) {
                if (cellValue.isString()) {
                    row.push_back(cellValue.toString().toStdString());
                } else if (cellValue.isDouble()) {
                    row.push_back(QString::number(cellValue.toDouble(), 'g', 15).toStdString());
                } else if (cellValue.isBool()) {
                    row.push_back(cellValue.toBool() ? "true" : "false");
                } else if (cellValue.isNull() || cellValue.isUndefined()) {
                    row.emplace_back();
                } else {
                    require(false, "cell is scalar");
                }
            }
            sheet.rows.push_back(std::move(row));
        }
        payload.sheets.push_back(std::move(sheet));
    }
    return payload;
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

void assertDiagnosticsSanitized(
    const etfdt::data_service_api::ShellAccountingExcelVbaImportReadOnlyParseResult& result)
{
    for (const auto& diagnostic : result.diagnostics) {
        const std::string combined =
            diagnostic.level + "|" + diagnostic.code + "|" + diagnostic.field + "|"
            + diagnostic.sheetName + "|" + diagnostic.rowId;
        for (const std::string token : {
                 "c:/",
                 "d:/",
                 "\\users\\",
                 "select ",
                 "insert ",
                 "update ",
                 "delete ",
                 "password",
                 "secret=",
                 "api_key=",
                 "http://",
                 "https://",
                 "broker payload",
                 "real order",
                 "stack trace",
                 "traceback",
             }) {
            requireNotContainsLower(combined, token, "diagnostic payload");
        }
    }
}

fs::path samplePath(const Harness& h, const char* fileName)
{
    return h.root / "samples" / "excel_vba_import" / fileName;
}

QJsonObject previewPayloadViaAction(const Harness& h, const char* fileName)
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "epic289-fix5-preview-msg";
    request.traceId = "epic289-fix5-preview-trace";
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview;
    request.timestampUtc = "2026-06-13T15:06:12Z";
    request.payloadJson = readFile(samplePath(h, fileName));

    auto context = etfdt::service_runtime::makeActionContext(
        request,
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::data_access::SQLiteConnection connection;
    const auto response =
        etfdt::data_service_api::handleAccountingExcelVbaImportReadOnlyPreview(context, connection);
    require(response.success, std::string(fileName) + " preview action succeeds");
    return parseJsonObjectText(response.payloadJson, std::string(fileName) + " preview response");
}

void assertWorkbookLevelSample(const Harness& h)
{
    constexpr const char* fileName = "EPIC289_FIX5_workbook_level_trade_log.json";
    const auto root = parseJsonObject(samplePath(h, fileName));
    const auto result =
        etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(payloadFromSample(root));

    require(result.accepted, "workbook-level TradeLog sample accepted");
    require(result.tradeFacts.size() == 3U, "workbook-level sample trade fact count");
    require(result.cashFacts.size() == 1U, "workbook-level sample cash fact count");
    require(result.configFactCount > 0, "workbook-level sample config facts counted");
    require(result.strategyFactCount > 0, "workbook-level sample strategy facts counted");
    require(result.skippedRows >= 2, "workbook-level sample trailing rows skipped");
    require(result.sensitiveHeadersIgnored == 1, "sensitive header ignored once");
    require(!result.tradeFacts.empty() && result.tradeFacts.back().instrumentCode == "17091",
            "numeric off-market code preserved");
    require(result.tradeFacts.back().source == "场外替代", "off-market source normalized");
    require(!result.diagnostics.empty(), "sensitive header warning emitted");
    bool sawSensitiveHeaderWarning = false;
    for (const auto& diagnostic : result.diagnostics) {
        if (diagnostic.code == "SENSITIVE_HEADER_IGNORED" && diagnostic.level == "WARNING") {
            sawSensitiveHeaderWarning = true;
        }
    }
    require(sawSensitiveHeaderWarning, "SENSITIVE_HEADER_IGNORED warning present");
    assertDiagnosticsSanitized(result);
    const auto workbookDigest = previewDigestForParseResult(result);
    require(workbookDigest.rfind("fnv64-", 0) == 0, "workbook-level persistence digest generated");

    const auto actionPayload = previewPayloadViaAction(h, fileName);
    require(boolField(actionPayload, "accepted"), "workbook-level preview action accepted");
    const auto summary = objectField(actionPayload, "replayFactSummary");
    require(intField(summary, "tradeFactCount") == 3, "preview action trade fact count");
    require(intField(summary, "cashFactCount") == 1, "preview action cash fact count");
    require(intField(summary, "configFactCount") > 0, "preview action config fact count");
    require(intField(summary, "strategyFactCount") > 0, "preview action strategy fact count");
    require(intField(summary, "skippedRows") >= 2, "preview action skipped rows count");
    require(intField(summary, "sensitiveHeadersIgnored") == 1, "preview action sensitive header count");
    const auto responseText = QString::fromUtf8(QJsonDocument(actionPayload).toJson(QJsonDocument::Compact))
                                  .toStdString();
    requireNotContainsLower(responseText, "should_not_appear", "preview action sanitized response");
    requireNotContainsLower(responseText, "pushplus token", "preview action sanitized response");
    require(!boolField(actionPayload, "sqliteProductionWrite"), "preview action remains read-only");
    require(!boolField(actionPayload, "brokerOrderSubmitted"), "preview action does not broker");
    require(!boolField(actionPayload, "automaticTrading"), "preview action does not auto trade");
}

void assertSamples(const Harness& h)
{
    const std::vector<SampleSpec> positives{
        {"EPIC275_positive_single_cash_single_buy.json", 1, 1, 0, 0},
        {"EPIC275_positive_buy_partial_sell.json", 2, 1, 0, 0},
        {"EPIC275_positive_multi_trade_multi_cash.json", 3, 3, 0, 0},
        {"EPIC275_positive_chinese_memo_whitespace_headers.json", 1, 1, 0, 0},
    };

    for (const auto& spec : positives) {
        const auto root = parseJsonObject(samplePath(h, spec.fileName));
        const auto result =
            etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(
                payloadFromSample(root));
        require(result.accepted, std::string(spec.fileName) + " accepted");
        require(static_cast<int>(result.tradeFacts.size()) == spec.tradeFacts,
                std::string(spec.fileName) + " trade fact count");
        require(static_cast<int>(result.cashFacts.size()) == spec.cashFacts,
                std::string(spec.fileName) + " cash fact count");
        require(static_cast<int>(result.marketPriceFacts.size()) == spec.marketPriceFacts,
                std::string(spec.fileName) + " market price fact count");
        require(static_cast<int>(result.fxRateFacts.size()) == spec.fxRateFacts,
                std::string(spec.fileName) + " fx fact count");
        require(result.diagnostics.empty(), std::string(spec.fileName) + " diagnostics empty");
    }

    const std::vector<const char*> rejected{
        "EPIC275_negative_missing_initialcash_sheet.json",
        "EPIC275_negative_missing_tradelog_sheet.json",
        "EPIC275_negative_missing_required_header.json",
        "EPIC275_negative_empty_tradelog.json",
        "EPIC275_negative_duplicate_row_id.json",
        "EPIC275_negative_invalid_numeric.json",
        "EPIC275_negative_invalid_date.json",
        "EPIC275_negative_unsupported_side_action.json",
        "EPIC275_negative_sensitive_tokens.json",
    };

    for (const auto* fileName : rejected) {
        const auto root = parseJsonObject(samplePath(h, fileName));
        const auto result =
            etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(
                payloadFromSample(root));
        require(!result.accepted, std::string(fileName) + " rejected");
        require(!result.diagnostics.empty(), std::string(fileName) + " diagnostics present");
        assertDiagnosticsSanitized(result);
    }

    const auto marketFxRoot =
        parseJsonObject(samplePath(h, "EPIC275_negative_market_fx_persistence_not_supported.json"));
    const auto marketFxResult =
        etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(
            payloadFromSample(marketFxRoot));
    require(marketFxResult.accepted, "market/fx sample preview can be accepted");
    require(!marketFxResult.marketPriceFacts.empty(), "market price facts parsed");
    require(!marketFxResult.fxRateFacts.empty(), "fx facts parsed");

    assertWorkbookLevelSample(h);
}

void assertMarketFxNotPersistable(const Harness& h)
{
    const auto root =
        parseJsonObject(samplePath(h, "EPIC275_negative_market_fx_persistence_not_supported.json"));
    const auto result =
        etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(payloadFromSample(root));
    require(result.accepted, "market/fx preview accepted before persist eligibility check");
    require(!result.marketPriceFacts.empty(), "market price fact present");
    require(!result.fxRateFacts.empty(), "fx fact present");
    const bool persistEligible =
        result.accepted && result.marketPriceFacts.empty() && result.fxRateFacts.empty()
        && (!result.tradeFacts.empty() || !result.cashFacts.empty());
    require(!persistEligible, "market/fx persistence remains unsupported");
}

DbFixture makeDb(const Harness& h)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("epic275_excel_vba_real_workbook_trial_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "epic275.sqlite";
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
                {etfdt::data_access::SqlStatementParameter::textValue("EPIC275_ACCOUNT"),
                 etfdt::data_access::SqlStatementParameter::textValue("EPIC-275 Account"),
                 etfdt::data_access::SqlStatementParameter::textValue("SIMULATED"),
                 etfdt::data_access::SqlStatementParameter::textValue("CNY"),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
                 etfdt::data_access::SqlStatementParameter::textValue("EPIC-275 temp account"),
                 etfdt::data_access::SqlStatementParameter::textValue("2026-06-11T00:00:00Z")})
            .hasValue(),
        "insert account");

    require(
        fixture.connection
            ->executeStatement(
                "INSERT INTO portfolio(uid,name,description,risk_level,is_active,created_at_utc) "
                "VALUES(?,?,?,?,?,?);",
                {etfdt::data_access::SqlStatementParameter::textValue("EPIC275_PORTFOLIO"),
                 etfdt::data_access::SqlStatementParameter::textValue("EPIC-275 Portfolio"),
                 etfdt::data_access::SqlStatementParameter::textValue("real workbook trial hardening"),
                 etfdt::data_access::SqlStatementParameter::textValue("DEFAULT"),
                 etfdt::data_access::SqlStatementParameter::int64ValueOf(0),
                 etfdt::data_access::SqlStatementParameter::textValue("2026-06-11T00:00:00Z")})
            .hasValue(),
        "insert portfolio");

    for (const auto& instrument : {
             std::pair<const char*, const char*>{"epic-275-etf-a", "EPIC275_ETF_A"},
             std::pair<const char*, const char*>{"epic-275-etf-b", "EPIC275_ETF_B"},
         }) {
        require(
            fixture.connection
                ->executeStatement(
                    "INSERT INTO instrument(uid,code,name,instrument_type,market_code,currency,enabled,memo,created_at_utc) "
                    "VALUES(?,?,?,?,?,?,?,?,?);",
                    {etfdt::data_access::SqlStatementParameter::textValue(instrument.first),
                     etfdt::data_access::SqlStatementParameter::textValue(instrument.second),
                     etfdt::data_access::SqlStatementParameter::textValue(instrument.second),
                     etfdt::data_access::SqlStatementParameter::textValue("ETF"),
                     etfdt::data_access::SqlStatementParameter::textValue("CN_A"),
                     etfdt::data_access::SqlStatementParameter::textValue("CNY"),
                     etfdt::data_access::SqlStatementParameter::int64ValueOf(1),
                     etfdt::data_access::SqlStatementParameter::textValue("EPIC-275 temp instrument"),
                     etfdt::data_access::SqlStatementParameter::textValue("2026-06-11T00:00:00Z")})
                .hasValue(),
            "insert instrument");
    }

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

std::string conflictPayloadFor(const std::string& payload)
{
    std::string conflict = payload;
    const auto pos = conflict.find("SANITIZED_");
    require(pos != std::string::npos, "conflict payload sentinel found");
    conflict.insert(pos + std::string("SANITIZED_").size(), "CONFLICT_");
    return conflict;
}

void runLocalServiceE2e(
    const Harness& h,
    const char* fileName,
    int expectedTradeFacts,
    int expectedCashFacts,
    bool verifyDuplicateAndConflict)
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
    require(
        !dispatcher.hasAction(etfdt::data_service_api::kActionAccountingBrokerOrderDryRun),
        "daily-use mode does not register broker order action");
    require(
        !dispatcher.hasAction(etfdt::data_service_api::kActionAccountingTradeDraftCreate),
        "daily-use mode does not register trade draft create action");
    require(
        !dispatcher.hasAction(etfdt::data_service_api::kActionAccountingTradeDraftConfirm),
        "daily-use mode does not register trade draft confirm action");

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalEpic275Trial_"
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

    const auto payloadText = readFile(samplePath(h, fileName));
    const auto parsed =
        etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(
            payloadFromSample(parseJsonObjectText(payloadText, fileName)));
    require(parsed.accepted, std::string(fileName) + " parser accepted before E2E");
    require(static_cast<int>(parsed.tradeFacts.size()) == expectedTradeFacts, "expected trade facts");
    require(static_cast<int>(parsed.cashFacts.size()) == expectedCashFacts, "expected cash facts");

    require(
        presenter.previewExcelVbaImportReadOnly(QString::fromStdString(payloadText)),
        std::string(fileName) + " Presenter preview succeeds");
    require(presenter.lastExcelVbaImportPreviewStatus() == QStringLiteral("ACCEPTED"), "preview accepted");
    require(presenter.excelVbaImportPreviewTradeFactCount() == expectedTradeFacts, "trade facts mapped");
    require(presenter.excelVbaImportPreviewCashFactCount() == expectedCashFacts, "cash facts mapped");
    require(presenter.excelVbaImportPreviewMarketPriceFactCount() == 0, "market facts zero");
    require(presenter.excelVbaImportPreviewFxRateFactCount() == 0, "fx facts zero");

    const auto originalDigest = presenter.lastExcelVbaImportPreviewDigest();
    const auto originalIdempotencyKey =
        QStringLiteral("shell-qml-excel-vba-import:") + originalDigest;
    require(presenter.persistAcceptedExcelVbaImportPreview(), "Presenter persist succeeds");
    require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("PERSISTED"), "persisted");
    require(
        presenter.lastExcelVbaImportPersistTradeLogRowsWritten()
            == expectedTradeFacts + expectedCashFacts,
        "trade_log rows written visible");
    require(
        presenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten() == expectedCashFacts,
        "cash_adjustment rows written visible");
    require(
        countRows(*fixture.connection, "trade_log") == expectedTradeFacts + expectedCashFacts,
        "trade_log rows exact");
    require(countRows(*fixture.connection, "cash_adjustment") == expectedCashFacts, "cash rows exact");
    require(countRows(*fixture.connection, "audit_log") == 1, "audit row exact");
    require(
        refreshSucceeded(presenter.lastPostWriteRefreshStatus(), presenter.lastPostWriteRefreshIssue()),
        "post-persist readback refresh succeeds or maps persisted facts safely");

    if (verifyDuplicateAndConflict) {
        require(presenter.persistAcceptedExcelVbaImportPreview(), "duplicate import succeeds idempotently");
        require(presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("DUPLICATE"), "duplicate visible");
        require(presenter.lastExcelVbaImportPersistTradeLogRowsWritten() == 0, "duplicate no trade rows");
        require(
            presenter.lastExcelVbaImportPersistCashAdjustmentRowsWritten() == 0,
            "duplicate no cash rows");
        require(
            countRows(*fixture.connection, "trade_log") == expectedTradeFacts + expectedCashFacts,
            "duplicate trade_log no growth");
        require(countRows(*fixture.connection, "cash_adjustment") == expectedCashFacts, "duplicate cash no growth");
        require(countRows(*fixture.connection, "audit_log") == 1, "duplicate audit no growth");

        const auto conflictPayload = conflictPayloadFor(payloadText);
        const auto conflictParsed =
            etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(
                payloadFromSample(parseJsonObjectText(conflictPayload, "conflict payload")));
        require(conflictParsed.accepted, "conflict payload remains accepted");
        const auto conflictDigest = QString::fromStdString(previewDigestForParseResult(conflictParsed));
        require(conflictDigest != originalDigest, "conflict digest differs");
        require(
            !presenter.persistExcelVbaImportManualEntry(
                QStringLiteral("ACCEPTED"),
                conflictDigest,
                originalIdempotencyKey,
                QStringLiteral("excel-vba-export/v1"),
                QStringLiteral("sanitized-excel-vba-export"),
                QStringLiteral("2026-06-11T13:00:00Z"),
                QStringLiteral("EPIC275_CONFLICT"),
                QString::fromStdString(conflictPayload),
                expectedTradeFacts,
                expectedCashFacts,
                0,
                0),
            "idempotency conflict fails closed");
        require(
            presenter.lastExcelVbaImportPersistStatus() == QStringLiteral("IDEMPOTENCY_CONFLICT"),
            "idempotency conflict status visible");
        require(
            countRows(*fixture.connection, "trade_log") == expectedTradeFacts + expectedCashFacts,
            "conflict trade_log no growth");
        require(countRows(*fixture.connection, "cash_adjustment") == expectedCashFacts, "conflict cash no growth");
        require(countRows(*fixture.connection, "audit_log") == 1, "conflict audit no growth");
    }

    require(!presenter.brokerSubmissionEnabled(), "broker submission disabled");
    require(!presenter.tradeDraftGenerationEnabled(), "TradeDraft generation disabled");
    require(!presenter.strategyExecutionEnabled(), "strategy execution disabled");

    client->disconnect();
    host.close();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    require(!host.isListening(), "local service host closed");
}

void assertHelper(const Harness& h)
{
    const auto helper =
        readFile(h.root / "tools" / "excel_vba_export" / "ExcelVbaImportMvpExportHelper.bas");
    const auto helperReadme =
        readFile(h.root / "tools" / "excel_vba_export" / "ExcelVbaImportMvpExportHelper_README.md");

    for (const std::string token : {
             "Option Explicit",
             "ExportMvpJsonTxt",
             "GetSaveAsFilename",
             "ThisWorkbook.Worksheets",
             "Trim$(",
             "RowHasAnyValue",
             "TradeLog sheet has no data rows.",
             "RequireAllowedExportExtension",
             "codePoint > 126",
             "\\u",
             "InitialCash",
             "TradeLog",
             "ROW_ID",
             "TRADE_TIME_UTC",
             "CASH_FLOW",
             "DEMO_ACCOUNT",
             "DEMO_PORTFOLIO",
             "DEMO_ETF",
             "SANITIZED_SAMPLE",
         }) {
        requireContains(helper, token, "VBA helper");
    }

    for (const std::string token : {
             "http://",
             "https://",
             "automaticTrading",
             "CreateObject(\"MSXML",
         }) {
        requireNotContainsLower(helper, token, "VBA helper");
    }

    requireContains(helperReadme, "Excel 2010", "helper README");
    requireContains(helperReadme, "JSON/TXT", "helper README");
    requireContains(helperReadme, "Direct `.xlsx` import is not supported", "helper README");
}

void assertDocsIndexed(const Harness& h)
{
    const auto readme = readFile(h.root / "README.md");
    const auto docsIndex = readFile(h.root / "docs" / "README.md");
    const auto prompt = readFile(h.root / "docs" / "12_codex_prompt_template.md");
    const auto overview =
        readFile(h.root / "docs" / "354_excel_vba_import_mvp_real_workbook_trial_hardening.md");
    const auto matrix =
        readFile(h.root / "docs" / "355_excel_vba_import_mvp_compatibility_matrix.md");
    const auto runbook =
        readFile(h.root / "docs" / "356_excel_vba_import_mvp_real_workbook_trial_runbook.md");
    const auto checklist =
        readFile(h.root / "docs" / "357_excel_vba_import_mvp_release_candidate_checklist.md");
    const auto plan =
        readFile(h.root / "docs" / "358_excel_vba_import_mvp_real_workbook_trial_test_plan.md");
    const auto testsCmake = readFile(h.root / "tests" / "CMakeLists.txt");

    for (const auto& text : {readme, docsIndex, prompt, overview, matrix, runbook, checklist, plan}) {
        requireContains(text, "EPIC-275", "EPIC-275 docs");
        requireContains(text, "Excel/VBA", "EPIC-275 docs");
        requireContains(text, "sanitized", "EPIC-275 docs");
        requireContains(text, "JSON/TXT", "EPIC-275 docs");
        requireContains(text, "Direct `.xlsx`", "EPIC-275 docs");
        requireContains(text, "not supported", "EPIC-275 docs");
    }

    for (const std::string token : {
             "354_excel_vba_import_mvp_real_workbook_trial_hardening.md",
             "355_excel_vba_import_mvp_compatibility_matrix.md",
             "356_excel_vba_import_mvp_real_workbook_trial_runbook.md",
             "357_excel_vba_import_mvp_release_candidate_checklist.md",
             "358_excel_vba_import_mvp_real_workbook_trial_test_plan.md",
             "EPIC275_positive_single_cash_single_buy.json",
             "EPIC275_negative_market_fx_persistence_not_supported.json",
         }) {
        requireContains(docsIndex, token, "docs/README.md");
        requireContains(readme, token, "README.md");
    }

    for (const std::string token : {
             "single initial cash + single BUY",
             "BUY + partial SELL",
             "Multiple cash movements",
             "Missing `InitialCash` sheet",
             "Duplicate `ROW_ID`",
             "Market price / FX persistence attempt",
             "Direct `.xlsx` import is not supported",
         }) {
        requireContains(matrix, token, "compatibility matrix");
    }

    requireContains(testsCmake, "ShellAccountingExcelVbaImportMvpRealWorkbookTrialHardening", "tests/CMakeLists.txt");
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "EPIC-275"},
        {"realWorkbookTrialHardeningCreated", true},
        {"compatibilityMatrixCreated", true},
        {"positiveSampleCount", 5},
        {"workbookLevelTradeLogAccepted", true},
        {"workbookLevelSensitiveHeaderIgnored", true},
        {"workbookLevelTrailingRowsSkipped", true},
        {"negativeSampleCount", 10},
        {"positiveSamplesAccepted", true},
        {"negativeSamplesFailClosed", true},
        {"diagnosticsSanitized", true},
        {"helperHeaderWhitespaceSupported", true},
        {"helperTrailingBlankRowsHandled", true},
        {"helperChineseMemoEscaped", true},
        {"singleCashSingleBuyE2ePassed", true},
        {"buyPartialSellE2ePassed", true},
        {"multiTradeMultiCashE2ePassed", true},
        {"duplicateImportNoGrowth", true},
        {"idempotencyConflictNoGrowth", true},
        {"releaseCandidateChecklistCreated", true},
        {"manualTrialRunbookCreated", true},
        {"directXlsxImportSupported", false},
        {"productionDbTouched", false},
        {"networkAccess", false},
        {"credentialAccess", false},
        {"endpointAccess", false},
        {"brokerOrderSubmitted", false},
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

        assertDocsIndexed(harness);
        assertHelper(harness);
        assertSamples(harness);
        assertMarketFxNotPersistable(harness);
        runLocalServiceE2e(
            harness,
            "EPIC275_positive_single_cash_single_buy.json",
            1,
            1,
            false);
        runLocalServiceE2e(
            harness,
            "EPIC275_positive_buy_partial_sell.json",
            2,
            1,
            true);
        runLocalServiceE2e(
            harness,
            "EPIC275_positive_multi_trade_multi_cash.json",
            3,
            3,
            false);

        std::cout << QJsonDocument(evidenceJson()).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 0;
    } catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "EPIC-275"},
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
