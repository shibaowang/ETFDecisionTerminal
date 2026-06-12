#include "DataAccess/DatabaseTypes.h"
#include "DataAccess/SQLiteConnection.h"
#include "DataAccess/ShellAccountingTradeDraftRepository.h"
#include "DataServiceApi/DataServiceActionRegistrar.h"
#include "DataServiceApi/DataServiceActions.h"
#include "DataServiceClient/DataServiceClient.h"
#include "DataServiceClient/DataServiceClientJson.h"
#include "ServiceHost/ActionServiceHost.h"
#include "ServiceRuntime/ActionDispatcher.h"
#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "StrategyEngine/OtcMapAcMultiChannelDraftEngine.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

#include <chrono>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;

namespace {

struct DbFixture final {
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
        std::cerr << "FAILED: " << message << '\n';
        std::exit(1);
    }
}

void phase(const std::string& message)
{
    std::cerr << "[EPIC-279] " << message << '\n';
}

std::string sourceRoot(int argc, char** argv)
{
    for (int index = 1; index + 1 < argc; ++index) {
        if (std::string(argv[index]) == "--source-root") {
            return argv[index + 1];
        }
    }
    return ".";
}

std::string readFile(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    require(input.good(), "read file: " + path.string());
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

QJsonObject parseObject(const std::string& text, const std::string& label)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(text));
    require(document.isObject(), "parse object: " + label);
    return document.object();
}

QJsonObject loadSample(const fs::path& sampleDir, const std::string& fileName)
{
    return parseObject(readFile(sampleDir / fileName), fileName);
}

std::string compactJson(const QJsonObject& object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
}

std::string payloadJson(QJsonObject sample, bool userConfirmed)
{
    if (sample.value(QStringLiteral("input")).isObject()) {
        auto input = sample.value(QStringLiteral("input")).toObject();
        input.insert(QStringLiteral("userConfirmed"), userConfirmed);
        sample.insert(QStringLiteral("input"), input);
    }
    sample.insert(QStringLiteral("userConfirmed"), userConfirmed);
    return compactJson(sample);
}

std::string sampleIdempotencyKey(const QJsonObject& sample)
{
    return sample.value(QStringLiteral("input"))
        .toObject()
        .value(QStringLiteral("idempotencyKey"))
        .toString()
        .toStdString();
}

std::vector<std::string> positiveSamples()
{
    return {
        "EPIC279_buy_single_channel_A.json",
        "EPIC279_buy_split_A_C_by_limit.json",
        "EPIC279_buy_cash_capped_multi_channel.json",
        "EPIC279_buy_skip_disabled_channel.json",
        "EPIC279_buy_skip_below_min_amount.json",
        "EPIC279_sell_c_class_first.json",
        "EPIC279_sell_split_c_then_a.json",
        "EPIC279_sell_base_protection_pass.json",
        "EPIC279_sell_partial_due_daily_limit.json",
        "EPIC279_otc_generic_recommendation_to_multileg_draft.json",
        "EPIC279_duplicate_same_key_same_digest.json",
        "EPIC279_decimal_nav_quantity.json",
    };
}

std::vector<std::string> negativeSamples()
{
    return {
        "EPIC279_negative_no_enabled_channels.json",
        "EPIC279_negative_no_buy_supported_channels.json",
        "EPIC279_negative_no_sell_supported_channels.json",
        "EPIC279_negative_invalid_nav.json",
        "EPIC279_negative_invalid_limit.json",
        "EPIC279_negative_invalid_cash.json",
        "EPIC279_negative_no_holding_to_sell.json",
        "EPIC279_negative_base_protection_blocks_sell.json",
        "EPIC279_negative_below_min_amount.json",
        "EPIC279_negative_missing_strategy_code.json",
        "EPIC279_negative_sensitive_token.json",
        "EPIC279_negative_broker_order_token.json",
        "EPIC279_negative_credential_endpoint_token.json",
        "EPIC279_negative_automatic_trading_token.json",
        "EPIC279_negative_idempotency_conflict.json",
    };
}

bool hasIssueCode(
    const std::vector<std::string>& issueCodes,
    const std::string& expected)
{
    for (const auto& code : issueCodes) {
        if (code == expected) {
            return true;
        }
    }
    return false;
}

void assertExpectedLegs(
    const std::string& fileName,
    const QJsonObject& sample,
    const etfdt::strategy_engine::OtcMapAcMultiChannelDraftResult& result)
{
    const auto expected = sample.value(QStringLiteral("expected")).toObject();
    require(result.status == expected.value(QStringLiteral("status")).toString().toStdString(),
            fileName + " status");
    const auto legs = expected.value(QStringLiteral("legs")).toArray();
    require(static_cast<int>(result.legs.size()) == legs.size(), fileName + " leg count");

    for (int index = 0; index < legs.size(); ++index) {
        const auto expectedLeg = legs.at(index).toObject();
        const auto& actual = result.legs[static_cast<std::size_t>(index)];
        require(
            actual.instrumentCode == expectedLeg.value(QStringLiteral("fundCode")).toString().toStdString(),
            fileName + " leg instrument code");
        require(
            actual.fundClass == expectedLeg.value(QStringLiteral("fundClass")).toString().toStdString(),
            fileName + " leg fund class");
        require(
            actual.side == expectedLeg.value(QStringLiteral("action")).toString().toStdString(),
            fileName + " leg action");
        require(
            actual.amountText == expectedLeg.value(QStringLiteral("amountText")).toString().toStdString(),
            fileName + " leg amount");
        require(
            actual.quantityText == expectedLeg.value(QStringLiteral("quantityText")).toString().toStdString(),
            fileName + " leg quantity");
        require(
            actual.navText == expectedLeg.value(QStringLiteral("navText")).toString().toStdString(),
            fileName + " leg nav");
        require(
            actual.priority == expectedLeg.value(QStringLiteral("priority")).toInt(),
            fileName + " leg priority");
        require(
            actual.netCashImpactText
                == expectedLeg.value(QStringLiteral("netCashImpactText")).toString().toStdString(),
            fileName + " leg net cash impact");
    }
}

void runEngineSamples(const fs::path& sampleDir)
{
    etfdt::strategy_engine::OtcMapAcMultiChannelDraftEngine engine;
    for (const auto& fileName : positiveSamples()) {
        const auto sample = loadSample(sampleDir, fileName);
        etfdt::strategy_engine::OtcMapAcMultiChannelDraftRequest request;
        std::string errorCode;
        require(
            etfdt::strategy_engine::decodeOtcMapAcMultiChannelDraftRequestJson(
                payloadJson(sample, false),
                request,
                errorCode),
            fileName + " decodes");
        const auto result = engine.buildReadOnlyDraft(request);
        require(result.accepted, fileName + " accepted");
        require(result.eligibleForTradeDraft, fileName + " eligible");
        require(!result.brokerOrderSubmitted, fileName + " submits no broker order");
        require(!result.networkAccess, fileName + " has no network access");
        require(!result.credentialAccess, fileName + " has no credential access");
        require(!result.endpointAccess, fileName + " has no endpoint access");
        require(!result.automaticTrading, fileName + " has no automatic trading");
        assertExpectedLegs(fileName, sample, result);
    }

    for (const auto& fileName : negativeSamples()) {
        if (fileName == "EPIC279_negative_idempotency_conflict.json") {
            continue;
        }
        const auto sample = loadSample(sampleDir, fileName);
        etfdt::strategy_engine::OtcMapAcMultiChannelDraftRequest request;
        std::string errorCode;
        require(
            etfdt::strategy_engine::decodeOtcMapAcMultiChannelDraftRequestJson(
                payloadJson(sample, false),
                request,
                errorCode),
            fileName + " decodes");
        const auto result = engine.buildReadOnlyDraft(request);
        const auto expectedIssues =
            sample.value(QStringLiteral("expected")).toObject().value(QStringLiteral("issueCodes")).toArray();
        require(!result.accepted, fileName + " fails closed");
        require(result.status == "REJECTED", fileName + " rejected");
        require(!result.issueCodes.empty(), fileName + " has issue code");
        for (const auto& value : expectedIssues) {
            require(
                hasIssueCode(result.issueCodes, value.toString().toStdString()),
                fileName + " expected issue code");
        }
    }
}

void execute(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    const auto result = connection.executeSql(sql);
    if (!result) {
        std::cerr << "FAILED: SQL: " << result.error().message << "\n" << sql << '\n';
        std::exit(1);
    }
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    const auto result = connection.querySingleInt("SELECT COUNT(*) FROM " + table + ";");
    require(result.hasValue(), "count rows: " + table);
    return result.value();
}

DbFixture makeDb(const fs::path& root)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("epic279_otcmap_multichannel_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "epic279.sqlite";
    config.enableWal = false;
    require(fixture.connection->open(config).hasValue(), "open temp SQLite");
    require(
        fixture.connection->applyMigrationFile(root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply migration 001");
    return fixture;
}

std::map<int, std::string> sampleInstruments(const fs::path& sampleDir)
{
    std::map<int, std::string> instruments;
    for (const auto& name : positiveSamples()) {
        const auto input = loadSample(sampleDir, name).value(QStringLiteral("input")).toObject();
        for (const auto& value : input.value(QStringLiteral("channels")).toArray()) {
            const auto channel = value.toObject();
            instruments[channel.value(QStringLiteral("instrumentId")).toInt()] =
                channel.value(QStringLiteral("instrumentCode")).toString().toStdString();
        }
    }
    for (const auto& name : negativeSamples()) {
        const auto input = loadSample(sampleDir, name).value(QStringLiteral("input")).toObject();
        for (const auto& value : input.value(QStringLiteral("channels")).toArray()) {
            const auto channel = value.toObject();
            instruments[channel.value(QStringLiteral("instrumentId")).toInt()] =
                channel.value(QStringLiteral("instrumentCode")).toString().toStdString();
        }
    }
    return instruments;
}

void seedDatabase(
    etfdt::data_access::SQLiteConnection& connection,
    const fs::path& sampleDir)
{
    execute(connection, "INSERT OR IGNORE INTO account(id,uid,name,account_type,is_active,initial_cash_cents,created_at_utc) "
                        "VALUES(1,'epic-279-account','EPIC 279 Account','SIMULATED',0,1000000,'2026-06-12T00:00:00Z');");
    execute(connection, "INSERT OR IGNORE INTO portfolio(id,uid,name,is_active,created_at_utc) "
                        "VALUES(1,'epic-279-portfolio','EPIC 279 Portfolio',0,'2026-06-12T00:00:00Z');");
    execute(connection, "INSERT OR IGNORE INTO instrument(id,uid,code,name,instrument_type,market_code,currency,enabled,created_at_utc) "
                        "VALUES(1000,'epic-279-anchor','OTCMAP_ANCHOR','EPIC 279 Anchor','OTC_FUND','FUND_OTC','CNY',1,'2026-06-12T00:00:00Z');");
    for (const auto& [id, code] : sampleInstruments(sampleDir)) {
        std::ostringstream sql;
        sql << "INSERT OR IGNORE INTO instrument(id,uid,code,name,instrument_type,market_code,currency,enabled,created_at_utc) "
            << "VALUES(" << id << ",'epic-279-" << code << "','" << code << "','EPIC 279 "
            << code << "','OTC_FUND','FUND_OTC','CNY',1,'2026-06-12T00:00:00Z');";
        execute(connection, sql.str());
    }
    execute(connection, "INSERT OR IGNORE INTO strategy(id,uid,strategy_code,instrument_id,name,enabled,created_at_utc) "
                        "VALUES(279,'epic-279-strategy','OTC_A_C',1000,'EPIC 279 OTCMap',1,'2026-06-12T00:00:00Z');");
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

void assertNoTradeOrCashWrites(etfdt::data_access::SQLiteConnection& connection)
{
    require(countRows(connection, "trade_log") == 0, "OTCMap writes no trade_log rows");
    require(countRows(connection, "cash_adjustment") == 0, "OTCMap writes no cash_adjustment rows");
}

void assertSafeClientResult(
    const etfdt::data_service_client::OtcMapMultiChannelDraftResult& result,
    const std::string& label)
{
    require(result.protocolSuccess, label + " protocol success");
    require(!result.tradeLogRowsWrittenByOtcMapDraft, label + " no trade_log write flag");
    require(!result.cashAdjustmentRowsWrittenByOtcMapDraft, label + " no cash_adjustment write flag");
    require(!result.positionChangedByOtcMapDraft, label + " no position change");
    require(!result.cashChangedByOtcMapDraft, label + " no cash change");
    require(!result.brokerOrderSubmitted, label + " no broker order");
    require(!result.networkAccess, label + " no network");
    require(!result.credentialAccess, label + " no credential");
    require(!result.endpointAccess, label + " no endpoint");
    require(!result.realOrderPlacement, label + " no real order");
    require(!result.automaticTrading, label + " no automatic trading");
    require(!result.productionDbTouched, label + " no production DB");
}

void assertSummary(
    etfdt::data_access::SQLiteConnection& connection,
    const std::string& idempotencyKey,
    int expectedLegCount)
{
    etfdt::data_access::ShellAccountingTradeDraftRepository repository(connection);
    const auto summary = repository.readTradeDraftSummary(idempotencyKey);
    require(summary.hasValue(), "read TradeDraft summary succeeds");
    require(summary.value().found, "read TradeDraft summary found");
    require(summary.value().readOnly, "read TradeDraft summary is read-only");
    require(summary.value().legCount == expectedLegCount, "multi-leg summary leg count");
    require(static_cast<int>(summary.value().legs.size()) == expectedLegCount, "multi-leg readback legs");
}

void previewAndCreate(
    etfdt::data_service_client::DataServiceClient& client,
    etfdt::data_access::SQLiteConnection& connection,
    const QJsonObject& sample,
    const std::string& label,
    int expectedLegCount)
{
    etfdt::data_service_client::OtcMapMultiChannelReadOnlyPreviewRequest previewRequest;
    previewRequest.payloadJson = payloadJson(sample, false);
    const auto preview = client.accountingOtcMapMultiChannelReadOnlyPreview(previewRequest, 1000);
    require(preview.hasValue(), label + " preview maps");
    assertSafeClientResult(preview.value(), label + " preview");
    require(preview.value().dataServiceReadOnlyActionCreated, label + " preview action registered");
    require(preview.value().previewActionExecuted, label + " preview executed");
    require(preview.value().accepted, label + " preview accepted");
    require(preview.value().legCount == expectedLegCount, label + " preview leg count");

    const auto draftsBefore = countRows(connection, "trade_draft");
    const auto legsBefore = countRows(connection, "trade_draft_leg");
    const auto auditBefore = countRows(connection, "audit_log");

    etfdt::data_service_client::OtcMapTradeDraftCreateRequest createRequest;
    createRequest.payloadJson = payloadJson(sample, true);
    const auto created = client.accountingTradeDraftCreateOtcMapMultiChannel(createRequest, 1000);
    require(created.hasValue(), label + " create maps");
    assertSafeClientResult(created.value(), label + " create");
    require(created.value().dataServiceWriteActionCreated, label + " write action registered");
    require(created.value().draftWritten, label + " draft written");
    require(created.value().legWritten, label + " legs written");
    require(created.value().auditWritten, label + " audit written");
    require(created.value().transactionCommitted, label + " transaction committed");
    require(created.value().legCount == expectedLegCount, label + " created leg count");
    require(countRows(connection, "trade_draft") == draftsBefore + 1, label + " draft row growth");
    require(countRows(connection, "trade_draft_leg") == legsBefore + expectedLegCount, label + " leg row growth");
    require(countRows(connection, "audit_log") == auditBefore + 1, label + " audit row growth");
    assertSummary(connection, sampleIdempotencyKey(sample), expectedLegCount);
    assertNoTradeOrCashWrites(connection);
}

void runLocalServicePath(
    const fs::path& sampleDir,
    etfdt::data_service_client::DataServiceClient& client,
    etfdt::data_access::SQLiteConnection& connection)
{
    previewAndCreate(
        client,
        connection,
        loadSample(sampleDir, "EPIC279_buy_split_A_C_by_limit.json"),
        "BUY split",
        2);
    previewAndCreate(
        client,
        connection,
        loadSample(sampleDir, "EPIC279_sell_c_class_first.json"),
        "SELL C first",
        2);

    const auto duplicateSample = loadSample(sampleDir, "EPIC279_duplicate_same_key_same_digest.json");
    etfdt::data_service_client::OtcMapTradeDraftCreateRequest duplicateRequest;
    duplicateRequest.payloadJson = payloadJson(duplicateSample, true);
    const auto first = client.accountingTradeDraftCreateOtcMapMultiChannel(duplicateRequest, 1000);
    require(first.hasValue(), "duplicate first create maps");
    require(first.value().draftWritten, "duplicate first create writes draft");
    const auto draftsAfterFirst = countRows(connection, "trade_draft");
    const auto legsAfterFirst = countRows(connection, "trade_draft_leg");
    const auto auditAfterFirst = countRows(connection, "audit_log");
    const auto duplicate = client.accountingTradeDraftCreateOtcMapMultiChannel(duplicateRequest, 1000);
    require(duplicate.hasValue(), "duplicate second create maps");
    require(duplicate.value().duplicateDraft, "duplicate same key maps duplicate");
    require(duplicate.value().status == "DUPLICATE", "duplicate status");
    require(countRows(connection, "trade_draft") == draftsAfterFirst, "duplicate no draft growth");
    require(countRows(connection, "trade_draft_leg") == legsAfterFirst, "duplicate no leg growth");
    require(countRows(connection, "audit_log") == auditAfterFirst, "duplicate no audit growth");

    auto conflictFirstSample = loadSample(sampleDir, "EPIC279_negative_idempotency_conflict.json");
    auto input = conflictFirstSample.value(QStringLiteral("input")).toObject();
    input.insert(QStringLiteral("otcMapDigest"), QStringLiteral("digest-negative-conflict-first"));
    conflictFirstSample.insert(QStringLiteral("input"), input);
    etfdt::data_service_client::OtcMapTradeDraftCreateRequest conflictRequest;
    conflictRequest.payloadJson = payloadJson(conflictFirstSample, true);
    const auto conflictSeed = client.accountingTradeDraftCreateOtcMapMultiChannel(conflictRequest, 1000);
    require(conflictSeed.hasValue(), "conflict seed maps");
    require(conflictSeed.value().draftWritten, "conflict seed writes draft");
    const auto draftsBeforeConflict = countRows(connection, "trade_draft");
    const auto legsBeforeConflict = countRows(connection, "trade_draft_leg");
    const auto auditBeforeConflict = countRows(connection, "audit_log");
    conflictRequest.payloadJson = payloadJson(
        loadSample(sampleDir, "EPIC279_negative_idempotency_conflict.json"),
        true);
    const auto conflict = client.accountingTradeDraftCreateOtcMapMultiChannel(conflictRequest, 1000);
    require(conflict.hasValue(), "conflict maps");
    require(conflict.value().idempotencyConflict, "conflict flag");
    require(conflict.value().status == "IDEMPOTENCY_CONFLICT", "conflict status");
    require(countRows(connection, "trade_draft") == draftsBeforeConflict, "conflict no draft growth");
    require(countRows(connection, "trade_draft_leg") == legsBeforeConflict, "conflict no leg growth");
    require(countRows(connection, "audit_log") == auditBeforeConflict, "conflict no audit growth");

    for (const auto& fileName : negativeSamples()) {
        if (fileName == "EPIC279_negative_idempotency_conflict.json") {
            continue;
        }
        const auto sample = loadSample(sampleDir, fileName);
        const auto draftsBefore = countRows(connection, "trade_draft");
        const auto legsBefore = countRows(connection, "trade_draft_leg");
        const auto auditBefore = countRows(connection, "audit_log");
        etfdt::data_service_client::OtcMapTradeDraftCreateRequest request;
        request.payloadJson = payloadJson(sample, true);
        const auto result = client.accountingTradeDraftCreateOtcMapMultiChannel(request, 1000);
        require(result.hasValue(), fileName + " create returns sanitized result");
        assertSafeClientResult(result.value(), fileName + " create");
        require(!result.value().draftWritten, fileName + " writes no draft");
        require(result.value().status == "REJECTED", fileName + " rejected");
        require(countRows(connection, "trade_draft") == draftsBefore, fileName + " no draft growth");
        require(countRows(connection, "trade_draft_leg") == legsBefore, fileName + " no leg growth");
        require(countRows(connection, "audit_log") == auditBefore, fileName + " no audit growth");
    }
}

void runPresenterPath(
    const fs::path& sampleDir,
    std::shared_ptr<etfdt::data_service_client::DataServiceClient> client)
{
    auto port =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter>(client);
    auto serviceAdapter =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(port);
    auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
    controller->setServiceAdapter(serviceAdapter);
    etfdt::shell_services::ShellAccountingPresenter presenter;
    presenter.setController(controller);

    const auto sample = loadSample(sampleDir, "EPIC279_decimal_nav_quantity.json");
    require(
        presenter.previewOtcMapMultiChannelDraft(QString::fromStdString(payloadJson(sample, false))),
        "Presenter OTCMap preview succeeds");
    require(presenter.lastOtcMapPreviewStatus() == QStringLiteral("ACCEPTED"), "Presenter preview accepted");
    require(presenter.lastOtcMapPreviewLegCount() == 1, "Presenter preview leg count");
    require(presenter.lastOtcMapPreviewTotalAmountText() == QStringLiteral("100.00"), "Presenter preview amount");
    require(!presenter.createOtcMapMultiChannelTradeDraft(false), "Presenter fails closed without confirmation");
    require(
        presenter.lastOtcMapDraftStatus() == QStringLiteral("INPUT_ERROR")
            || presenter.lastTradeDraftStatus() == QStringLiteral("INPUT_ERROR"),
        "Presenter records missing confirmation");
    require(
        presenter.previewOtcMapMultiChannelDraft(QString::fromStdString(payloadJson(sample, false))),
        "Presenter OTCMap preview can be restored");
    require(
        presenter.createOtcMapMultiChannelTradeDraft(true),
        "Presenter creates OTCMap TradeDraft after explicit confirmation");
    require(
        presenter.lastOtcMapDraftStatus() == QStringLiteral("DRAFT_CREATED")
            || presenter.lastOtcMapDraftStatus() == QStringLiteral("DUPLICATE"),
        "Presenter maps create status");
    require(presenter.lastOtcMapDraftLegCount() == 1, "Presenter maps draft leg count");
    require(!presenter.lastOtcMapDraftSummary().isEmpty(), "Presenter maps draft summary");
}

void assertDocsAndSources(const fs::path& root)
{
    for (const auto& doc : {
             "374_otcmap_ac_multichannel_draft_engine_full_delivery.md",
             "375_otcmap_ac_multichannel_contract.md",
             "376_otcmap_ac_multichannel_vba_parity_matrix.md",
             "377_otcmap_ac_multichannel_user_visible_flow.md",
             "378_otcmap_ac_multichannel_test_plan.md",
         }) {
        const auto text = readFile(root / "docs" / doc);
        require(text.find("EPIC-279") != std::string::npos, std::string(doc) + " references EPIC-279");
        require(text.find("broker") != std::string::npos, std::string(doc) + " documents broker boundary");
        require(text.find("trade_log") != std::string::npos, std::string(doc) + " documents trade_log boundary");
    }

    const auto actionText = readFile(
        root / "libs" / "DataServiceApi" / "src" / "ShellAccountingOtcMapMultiChannelDraftAction.cpp");
    require(
        actionText.find("INSERT INTO trade_log") == std::string::npos,
        "OTCMap DataService action has no direct trade_log SQL");
    require(
        actionText.find("INSERT INTO cash_adjustment") == std::string::npos,
        "OTCMap DataService action has no direct cash_adjustment SQL");
    const auto presenterText = readFile(root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp");
    require(
        presenterText.find("previewOtcMapMultiChannelDraft") != std::string::npos,
        "Presenter exposes OTCMap preview");
    require(
        presenterText.find("createOtcMapMultiChannelTradeDraft") != std::string::npos,
        "Presenter exposes OTCMap explicit draft create");
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    const fs::path root = sourceRoot(argc, argv);
    const fs::path sampleDir = root / "samples" / "otcmap_multichannel";

    phase("docs and source boundaries");
    assertDocsAndSources(root);
    phase("sample matrix");
    for (const auto& fileName : positiveSamples()) {
        require(fs::exists(sampleDir / fileName), "positive sample exists: " + fileName);
    }
    for (const auto& fileName : negativeSamples()) {
        require(fs::exists(sampleDir / fileName), "negative sample exists: " + fileName);
    }
    runEngineSamples(sampleDir);

    phase("temp DB");
    auto fixture = makeDb(root);
    seedDatabase(*fixture.connection, sampleDir);
    require(countRows(*fixture.connection, "trade_log") == 0, "initial trade_log empty");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "initial cash_adjustment empty");

    phase("local service host");
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, *fixture.connection);
    etfdt::data_service_api::registerDataServiceWriteActions(dispatcher, *fixture.connection);
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionAccountingOtcMapMultiChannelReadOnlyPreview),
        "OTCMap preview action registered");
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionAccountingTradeDraftCreateOtcMapMultiChannel),
        "OTCMap create action registered");

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalEpic279OtcMap_"
        + std::to_string(QCoreApplication::applicationPid()) + "_"
        + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    require(host.listen(socketName).hasValue(), "ActionServiceHost listen succeeds");

    auto client = std::make_shared<etfdt::data_service_client::DataServiceClient>();
    require(client->connect(socketName, 1000).hasValue(), "DataServiceClient connects");
    require(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "service host accepts client");

    phase("local service path");
    runLocalServicePath(sampleDir, *client, *fixture.connection);
    phase("presenter path");
    runPresenterPath(sampleDir, client);
    assertNoTradeOrCashWrites(*fixture.connection);

    client->disconnect();
    host.close();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    require(!host.isListening(), "local service host closed");

    std::cout
        << "{\"task\":\"EPIC-279\","
        << "\"otcMapAcMultiChannelEngineCreated\":true,"
        << "\"positiveSampleCount\":12,"
        << "\"negativeSampleCount\":15,"
        << "\"buySplitByLimitPassed\":true,"
        << "\"buyCashCappedPassed\":true,"
        << "\"buyDisabledChannelSkipped\":true,"
        << "\"buyBelowMinSkipped\":true,"
        << "\"sellCClassFirstPassed\":true,"
        << "\"sellSplitCThenAPassed\":true,"
        << "\"sellBaseProtectionPassed\":true,"
        << "\"sellBaseProtectionBlocked\":true,"
        << "\"dailyLimitPartialSellPassed\":true,"
        << "\"multiLegTradeDraftCreated\":true,"
        << "\"multiLegTradeDraftReadbackPassed\":true,"
        << "\"duplicateSameKeyNoGrowth\":true,"
        << "\"idempotencyConflictNoGrowth\":true,"
        << "\"tradeLogRowsWrittenByOtcMapDraft\":false,"
        << "\"cashAdjustmentRowsWrittenByOtcMapDraft\":false,"
        << "\"positionChangedByOtcMapDraft\":false,"
        << "\"cashChangedByOtcMapDraft\":false,"
        << "\"brokerOrderSubmitted\":false,"
        << "\"networkAccess\":false,"
        << "\"credentialAccess\":false,"
        << "\"endpointAccess\":false,"
        << "\"realOrderPlacement\":false,"
        << "\"automaticTrading\":false,"
        << "\"productionDbTouched\":false}"
        << std::endl;
    return 0;
}
