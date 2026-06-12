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
#include "StrategyEngine/StrategyRecommendationEngine.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>

#include <chrono>
#include <filesystem>
#include <functional>
#include <fstream>
#include <iostream>
#include <memory>
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
    std::cerr << "[EPIC-278] " << message << '\n';
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

std::string compactJson(const QJsonObject& object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
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

void execute(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto result = connection.executeSql(sql);
    if (!result.hasValue()) {
        std::cerr << "FAILED: execute SQL: " << result.error().message << "\nSQL: " << sql << '\n';
        std::exit(1);
    }
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    auto result = connection.querySingleInt("SELECT COUNT(*) FROM " + table + ";");
    require(result.hasValue(), "count rows in " + table);
    return result.value();
}

DbFixture makeDb(const fs::path& root)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("epic278_tradedraft_manual_recommendation_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "epic278.sqlite";
    config.enableWal = false;
    require(fixture.connection->open(config).hasValue(), "open temp SQLite");
    require(
        fixture.connection->applyMigrationFile(root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply migration 001");
    return fixture;
}

void seedTradeDraftRefs(etfdt::data_access::SQLiteConnection& connection)
{
    execute(connection, "INSERT INTO account(id,uid,name,account_type,is_active,initial_cash_cents,created_at_utc) "
                        "VALUES(1001,'epic-278-account','EPIC 278 Account','SIMULATED',0,1000000,'2026-06-12T00:00:00Z');");
    execute(connection, "INSERT INTO portfolio(id,uid,name,is_active,created_at_utc) "
                        "VALUES(1001,'epic-278-portfolio','EPIC 278 Portfolio',0,'2026-06-12T00:00:00Z');");
    execute(connection, "INSERT INTO instrument(id,uid,code,name,instrument_type,market_code,currency,enabled,created_at_utc) "
                        "VALUES(1001,'epic-278-instrument','159509','EPIC 278 ETF','ETF','CN_A','CNY',1,'2026-06-12T00:00:00Z');");
    execute(connection, "INSERT INTO strategy(id,uid,strategy_code,instrument_id,name,enabled,created_at_utc) "
                        "VALUES(1001,'epic-278-strategy','TASK139',1001,'EPIC 278 Strategy',1,'2026-06-12T00:00:00Z');");
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

std::vector<std::string> positiveSamples()
{
    return {
        "EPIC278_buy_tier1_to_draft.json",
        "EPIC278_buy_cash_capped_to_draft.json",
        "EPIC278_sell_take_profit_to_draft.json",
        "EPIC278_sell_extreme_premium_to_draft.json",
        "EPIC278_otc_generic_buy_to_draft.json",
        "EPIC278_zero_fee_to_draft.json",
        "EPIC278_decimal_quantity_to_draft.json",
        "EPIC278_duplicate_same_key_same_digest.json",
    };
}

std::vector<std::string> negativeSamples()
{
    return {
        "EPIC278_negative_hold_not_eligible.json",
        "EPIC278_negative_wait_not_eligible.json",
        "EPIC278_negative_rejected_recommendation.json",
        "EPIC278_negative_base_protected_sell_blocked.json",
        "EPIC278_negative_missing_confirmation.json",
        "EPIC278_negative_missing_idempotency_key.json",
        "EPIC278_negative_idempotency_conflict.json",
        "EPIC278_negative_broker_order_token.json",
        "EPIC278_negative_credential_token.json",
        "EPIC278_negative_automatic_trading_token.json",
        "EPIC278_negative_zero_quantity.json",
        "EPIC278_negative_missing_amount.json",
    };
}

etfdt::data_service_client::TradeDraftCreateFromRecommendationRequest requestFromSample(
    const QJsonObject& sample)
{
    auto recommendation = sample.value(QStringLiteral("recommendationPayload")).toObject();
    auto input = recommendation.value(QStringLiteral("input")).toObject();
    etfdt::data_service_client::TradeDraftCreateFromRecommendationRequest request;
    request.recommendationPayloadJson = compactJson(recommendation);
    request.idempotencyKey = sample.value(QStringLiteral("idempotencyKey")).toString().toStdString();
    request.recommendationDigest = sample.value(QStringLiteral("recommendationDigest")).toString().toStdString();
    request.accountId = input.value(QStringLiteral("accountId")).toString().toStdString();
    request.portfolioId = input.value(QStringLiteral("portfolioId")).toString().toStdString();
    request.strategyId = input.value(QStringLiteral("strategyId")).toString(QStringLiteral("1001")).toStdString();
    request.instrumentId = input.value(QStringLiteral("instrumentId")).toString(QStringLiteral("1001")).toStdString();
    request.strategyCode = input.value(QStringLiteral("strategyCode")).toString().toStdString();
    request.instrumentCode = input.value(QStringLiteral("instrumentCode")).toString().toStdString();
    request.instrumentType = input.value(QStringLiteral("instrumentType")).toString().toStdString();
    request.tradeSource = input.value(QStringLiteral("tradeSource")).toString().toStdString();
    request.expiresAtUtc = "2026-06-13T00:00:00Z";
    request.userConfirmed = sample.value(QStringLiteral("userConfirmed")).toBool(false);
    return request;
}

QJsonObject loadSample(const fs::path& sampleDir, const std::string& fileName)
{
    return parseObject(readFile(sampleDir / fileName), fileName);
}

void assertNoTradeOrCashWrites(etfdt::data_access::SQLiteConnection& connection)
{
    require(countRows(connection, "trade_log") == 0, "TradeDraft writes no trade_log rows");
    require(countRows(connection, "cash_adjustment") == 0, "TradeDraft writes no cash_adjustment rows");
}

void assertResultBoundaries(
    const etfdt::data_service_client::TradeDraftCreateFromRecommendationResult& result)
{
    require(result.tradeDraftIsNotOrder, "TradeDraft is not order");
    require(!result.tradeLogRowsWrittenByTradeDraft, "TradeDraft result maps no trade_log write");
    require(!result.cashAdjustmentRowsWrittenByTradeDraft, "TradeDraft result maps no cash write");
    require(!result.positionChangedByTradeDraft, "TradeDraft result maps no position change");
    require(!result.cashChangedByTradeDraft, "TradeDraft result maps no cash change");
    require(!result.brokerOrderSubmitted, "TradeDraft submits no broker order");
    require(!result.networkAccess, "TradeDraft uses no network");
    require(!result.credentialAccess, "TradeDraft uses no credentials");
    require(!result.endpointAccess, "TradeDraft uses no endpoint");
    require(!result.automaticTrading, "TradeDraft uses no automatic trading");
}

void runPositiveSamples(
    const fs::path& sampleDir,
    etfdt::data_service_client::DataServiceClient& client,
    etfdt::data_access::SQLiteConnection& connection)
{
    for (const auto& fileName : positiveSamples()) {
        if (fileName == "EPIC278_duplicate_same_key_same_digest.json") {
            continue;
        }
        phase("positive sample " + fileName);
        const auto sample = loadSample(sampleDir, fileName);
        auto request = requestFromSample(sample);
        const auto beforeDrafts = countRows(connection, "trade_draft");
        const auto beforeAudit = countRows(connection, "audit_log");
        phase("create " + fileName);
        auto created = client.accountingTradeDraftCreateFromRecommendation(request, 1000);
        phase("created result " + fileName);
        require(created.hasValue(), fileName + " client create succeeds");
        assertResultBoundaries(created.value());
        require(created.value().draftWritten, fileName + " writes draft");
        require(created.value().legWritten, fileName + " writes draft leg");
        require(created.value().auditWritten, fileName + " writes audit");
        require(created.value().transactionCommitted, fileName + " commits transaction");
        require(created.value().status == "DRAFT_CREATED", fileName + " returns DRAFT_CREATED");
        require(countRows(connection, "trade_draft") == beforeDrafts + 1, fileName + " draft row grows once");
        require(countRows(connection, "audit_log") == beforeAudit + 1, fileName + " audit row grows once");
        assertNoTradeOrCashWrites(connection);

        etfdt::data_service_client::TradeDraftReadOnlySummaryRequest summaryRequest;
        summaryRequest.idempotencyKey = request.idempotencyKey;
        phase("summary " + fileName);
        const auto summary = client.accountingTradeDraftReadOnlySummary(summaryRequest, 1000);
        require(summary.hasValue(), fileName + " summary call succeeds");
        require(summary.value().found, fileName + " summary found");
        require(summary.value().draftId == created.value().draftId, fileName + " summary draft id matches");
        require(!summary.value().productionWrite, fileName + " summary is read-only");
    }
}

void runDuplicateAndConflict(
    const fs::path& sampleDir,
    etfdt::data_service_client::DataServiceClient& client,
    etfdt::data_access::SQLiteConnection& connection)
{
    const auto duplicateSample = loadSample(sampleDir, "EPIC278_duplicate_same_key_same_digest.json");
    auto duplicateRequest = requestFromSample(duplicateSample);
    const auto first = client.accountingTradeDraftCreateFromRecommendation(duplicateRequest, 1000);
    require(first.hasValue(), "duplicate first create succeeds");
    require(first.value().status == "DRAFT_CREATED", "duplicate first create writes draft");
    const auto draftsAfterFirst = countRows(connection, "trade_draft");
    const auto auditAfterFirst = countRows(connection, "audit_log");
    const auto duplicate = client.accountingTradeDraftCreateFromRecommendation(duplicateRequest, 1000);
    require(duplicate.hasValue(), "duplicate second call succeeds");
    require(duplicate.value().duplicateDraft, "duplicate maps duplicateDraft");
    require(duplicate.value().status == "DUPLICATE", "duplicate status");
    require(countRows(connection, "trade_draft") == draftsAfterFirst, "duplicate no draft growth");
    require(countRows(connection, "audit_log") == auditAfterFirst, "duplicate no audit growth");
    assertNoTradeOrCashWrites(connection);

    const auto conflictSample = loadSample(sampleDir, "EPIC278_negative_idempotency_conflict.json");
    auto conflictFirst = requestFromSample(conflictSample);
    conflictFirst.recommendationDigest = "digest-conflict-first";
    const auto firstConflictSeed = client.accountingTradeDraftCreateFromRecommendation(conflictFirst, 1000);
    require(firstConflictSeed.hasValue(), "conflict seed succeeds");
    require(firstConflictSeed.value().status == "DRAFT_CREATED", "conflict seed writes draft");
    const auto draftsBeforeConflict = countRows(connection, "trade_draft");
    const auto auditBeforeConflict = countRows(connection, "audit_log");
    auto conflictSecond = requestFromSample(conflictSample);
    const auto conflict = client.accountingTradeDraftCreateFromRecommendation(conflictSecond, 1000);
    require(conflict.hasValue(), "conflict call succeeds");
    require(conflict.value().idempotencyConflict, "conflict maps idempotency conflict");
    require(conflict.value().status == "IDEMPOTENCY_CONFLICT", "conflict status");
    require(countRows(connection, "trade_draft") == draftsBeforeConflict, "conflict no draft growth");
    require(countRows(connection, "audit_log") == auditBeforeConflict, "conflict no audit growth");
    assertNoTradeOrCashWrites(connection);
}

void runNegativeSamples(
    const fs::path& sampleDir,
    etfdt::data_service_client::DataServiceClient& client,
    etfdt::data_access::SQLiteConnection& connection)
{
    for (const auto& fileName : negativeSamples()) {
        if (fileName == "EPIC278_negative_idempotency_conflict.json") {
            continue;
        }
        const auto beforeDrafts = countRows(connection, "trade_draft");
        const auto beforeAudit = countRows(connection, "audit_log");
        const auto sample = loadSample(sampleDir, fileName);
        const auto result =
            client.accountingTradeDraftCreateFromRecommendation(requestFromSample(sample), 1000);
        if (!result.hasValue()) {
            require(
                fileName == "EPIC278_negative_missing_idempotency_key.json",
                fileName + " only missing idempotency may fail at client validation");
            require(countRows(connection, "trade_draft") == beforeDrafts, fileName + " no draft growth");
            require(countRows(connection, "audit_log") == beforeAudit, fileName + " no audit growth");
            assertNoTradeOrCashWrites(connection);
            continue;
        }
        require(result.hasValue(), fileName + " returns sanitized protocol result");
        assertResultBoundaries(result.value());
        require(!result.value().draftWritten, fileName + " writes no draft");
        require(!result.value().auditWritten, fileName + " writes no audit");
        require(result.value().status != "DRAFT_CREATED", fileName + " fails closed");
        require(countRows(connection, "trade_draft") == beforeDrafts, fileName + " no draft growth");
        require(countRows(connection, "audit_log") == beforeAudit, fileName + " no audit growth");
        assertNoTradeOrCashWrites(connection);
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

    const auto sample = loadSample(sampleDir, "EPIC278_buy_tier1_to_draft.json");
    const auto recommendationPayload = compactJson(sample.value(QStringLiteral("recommendationPayload")).toObject());
    require(
        presenter.previewStrategyRecommendationReadOnlySummary(QString::fromStdString(recommendationPayload)),
        "Presenter recommendation preview succeeds");
    require(
        presenter.previewTradeDraftFromLastRecommendation(),
        "Presenter previewTradeDraftFromLastRecommendation succeeds");
    require(
        presenter.lastTradeDraftUserConfirmationRequired(),
        "Presenter requires explicit TradeDraft confirmation");
    require(
        !presenter.createTradeDraftFromLastRecommendation(false),
        "Presenter fails closed without user confirmation");
    require(
        presenter.lastTradeDraftStatus() == QStringLiteral("INPUT_ERROR"),
        "Presenter marks missing confirmation as input error");
    require(
        presenter.previewTradeDraftFromLastRecommendation(),
        "Presenter preview can be restored");
    require(
        presenter.createTradeDraftFromLastRecommendation(true),
        "Presenter creates TradeDraft after confirmation");
    require(
        !presenter.lastTradeDraftId().isEmpty(),
        "Presenter maps TradeDraft id");
    require(
        presenter.lastTradeDraftSummary().contains(QStringLiteral("TradeDraft"))
            || presenter.lastTradeDraftSummary().contains(QStringLiteral("draftIsNotOrder")),
        "Presenter maps TradeDraft summary");
}

void assertDocsAndSamples(const fs::path& root)
{
    const auto sampleDir = root / "samples" / "trade_draft";
    for (const auto& fileName : positiveSamples()) {
        require(fs::exists(sampleDir / fileName), "positive sample exists: " + fileName);
    }
    for (const auto& fileName : negativeSamples()) {
        require(fs::exists(sampleDir / fileName), "negative sample exists: " + fileName);
    }
    for (const auto& fileName : {
             "369_tradedraft_manual_recommendation_flow_full_delivery.md",
             "370_tradedraft_vba_parity_matrix.md",
             "371_tradedraft_contract.md",
             "372_tradedraft_user_visible_flow.md",
             "373_tradedraft_test_plan.md",
         }) {
        const auto text = readFile(root / "docs" / fileName);
        require(text.find("EPIC-278") != std::string::npos, std::string(fileName) + " references EPIC-278");
        require(text.find("TradeDraft is not an order") != std::string::npos, std::string(fileName) + " says not order");
        require(text.find("broker") != std::string::npos, std::string(fileName) + " documents broker boundary");
    }
}

void assertSourceBoundaries(const fs::path& root)
{
    const auto actionText =
        readFile(root / "libs" / "DataServiceApi" / "src" / "ShellAccountingTradeDraftRecommendationAction.cpp");
    require(
        actionText.find("INSERT INTO trade_log") == std::string::npos,
        "DataService TradeDraft action has no direct trade_log SQL");
    const auto presenterText =
        readFile(root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp");
    require(
        presenterText.find("DataServiceClient") == std::string::npos,
        "Presenter does not call DataServiceClient directly");
}

void probeFirstSampleDirect(const fs::path& root)
{
    phase("direct handler probe");
    auto fixture = makeDb(root);
    seedTradeDraftRefs(*fixture.connection);
    const auto sample = loadSample(root / "samples" / "trade_draft", "EPIC278_buy_tier1_to_draft.json");
    const auto request = requestFromSample(sample);
    phase("direct decode");
    etfdt::strategy_engine::StrategyRecommendationInput input;
    std::string decodeError;
    require(
        etfdt::strategy_engine::decodeStrategyRecommendationInputJson(
            request.recommendationPayloadJson,
            input,
            decodeError),
        "direct decode succeeds");
    phase("direct recommend");
    const auto recommendation =
        etfdt::strategy_engine::StrategyRecommendationEngine{}.recommendReadOnly(input);
    require(recommendation.accepted, "direct recommendation accepted");
    phase("direct convert");
    etfdt::strategy_engine::TradeDraftConversionRequest conversionRequest;
    conversionRequest.recommendation = recommendation;
    conversionRequest.accountId = request.accountId;
    conversionRequest.portfolioId = request.portfolioId;
    conversionRequest.strategyCode = request.strategyCode;
    conversionRequest.instrumentCode = request.instrumentCode;
    conversionRequest.instrumentType = request.instrumentType;
    conversionRequest.tradeSource = request.tradeSource;
    conversionRequest.idempotencyKey = request.idempotencyKey;
    conversionRequest.recommendationDigest = request.recommendationDigest;
    conversionRequest.userConfirmed = request.userConfirmed;
    const auto conversion =
        etfdt::strategy_engine::StrategyRecommendationEngine{}.convertRecommendationToTradeDraft(
            conversionRequest);
    require(conversion.eligible, "direct conversion eligible");
    phase("direct repository create");
    etfdt::data_access::ShellAccountingTradeDraftCreateRequest repoRequest;
    repoRequest.accountId = 1001;
    repoRequest.portfolioId = 1001;
    repoRequest.strategyId = 1001;
    repoRequest.strategyCode = conversion.strategyCode;
    repoRequest.instrumentId = 1001;
    repoRequest.instrumentCode = conversion.instrumentCode;
    repoRequest.instrumentType = conversion.instrumentType;
    repoRequest.side = conversion.side;
    repoRequest.quantity1e6 = 400000000;
    repoRequest.quantityText = conversion.quantityText;
    repoRequest.amountText = conversion.amountText;
    repoRequest.priceText = conversion.priceText;
    repoRequest.feeEstimateText = conversion.feeEstimateText;
    repoRequest.netCashImpactText = conversion.netCashImpactText;
    repoRequest.tradeSource = conversion.tradeSource;
    repoRequest.reason = "EPIC-278 direct probe";
    repoRequest.authorizationToken = "EPIC-278_TRADEDRAFT_RECOMMENDATION_WRITE";
    repoRequest.createdAtUtc = "2026-06-12T00:00:00Z";
    repoRequest.expiresAtUtc = "2026-06-13T00:00:00Z";
    repoRequest.idempotencyKey = conversionRequest.idempotencyKey;
    repoRequest.recommendationDigest = conversionRequest.recommendationDigest;
    repoRequest.sourceRecommendationAction = conversion.sourceRecommendationAction;
    repoRequest.sourceRecommendationReason = conversion.sourceRecommendationReason;
    repoRequest.userConfirmationRequired = true;
    repoRequest.userConfirmed = true;
    repoRequest.baseProtectionPassed = conversion.baseProtectionPassed;
    repoRequest.cashLimitApplied = conversion.cashLimitApplied;
    repoRequest.otcGenericDraft = conversion.otcGenericDraft;
    etfdt::data_access::ShellAccountingTradeDraftRepository repository(*fixture.connection);
    const auto repoResult = repository.createTradeDraft(repoRequest);
    require(repoResult.hasValue(), "direct repository create succeeds");
    phase("direct repository duplicate");
    const auto duplicateRepoResult = repository.createTradeDraft(repoRequest);
    require(duplicateRepoResult.hasValue(), "direct repository duplicate succeeds");
    require(duplicateRepoResult.value().duplicateDraft, "direct repository duplicate maps duplicate");
    phase("direct handler call");
    etfdt::protocol::MessageEnvelope envelope;
    envelope.msgId = "epic278-direct-probe";
    envelope.traceId = "epic278-direct-probe";
    envelope.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    envelope.to = etfdt::protocol::ServiceName::ETFDataService;
    envelope.action = etfdt::data_service_api::kActionAccountingTradeDraftCreateFromRecommendation;
    envelope.timestampUtc = "2026-06-12T00:00:00Z";
    envelope.payloadJson =
        etfdt::data_service_client::tradeDraftCreateFromRecommendationPayloadJson(request);
    etfdt::service_runtime::ActionContext context;
    context.request = envelope;
    context.serviceName = etfdt::protocol::ServiceName::ETFDataService;
    context.receivedAtUtc = envelope.timestampUtc;
    context.traceId = envelope.traceId;
    const auto response =
        etfdt::data_service_api::handleAccountingTradeDraftCreateFromRecommendation(
            context,
            *fixture.connection);
    require(response.success, "direct handler probe returns protocol success");
    require(
        response.payloadJson.find("\"draftWritten\":true") != std::string::npos
            || response.payloadJson.find("\"duplicateDraft\":true") != std::string::npos,
        "direct handler probe writes or deduplicates draft");
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    const fs::path root = sourceRoot(argc, argv);
    const fs::path sampleDir = root / "samples" / "trade_draft";
    phase("assert docs and samples");
    assertDocsAndSamples(root);
    phase("assert source boundaries");
    assertSourceBoundaries(root);
    probeFirstSampleDirect(root);

    phase("make database");
    auto fixture = makeDb(root);
    phase("seed references");
    seedTradeDraftRefs(*fixture.connection);
    require(countRows(*fixture.connection, "trade_log") == 0, "initial trade_log empty");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "initial cash_adjustment empty");

    phase("register actions");
    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, *fixture.connection);
    etfdt::data_service_api::registerDataServiceWriteActions(dispatcher, *fixture.connection);
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionAccountingTradeDraftCreateFromRecommendation),
        "TradeDraft create-from-recommendation action registered");
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionAccountingTradeDraftReadOnlySummary),
        "TradeDraft read-only summary action registered");

    phase("start service host");
    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalEpic278TradeDraft_"
        + std::to_string(QCoreApplication::applicationPid()) + "_"
        + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    require(host.listen(socketName).hasValue(), "ActionServiceHost listen succeeds");

    auto client = std::make_shared<etfdt::data_service_client::DataServiceClient>();
    phase("connect client");
    require(client->connect(socketName, 1000).hasValue(), "DataServiceClient connects");
    require(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "service host accepts client");

    phase("run positive samples");
    runPositiveSamples(sampleDir, *client, *fixture.connection);
    phase("run duplicate and conflict");
    runDuplicateAndConflict(sampleDir, *client, *fixture.connection);
    phase("run negative samples");
    runNegativeSamples(sampleDir, *client, *fixture.connection);
    phase("run presenter path");
    runPresenterPath(sampleDir, client);
    phase("assert no trade or cash writes");
    assertNoTradeOrCashWrites(*fixture.connection);

    phase("close host");
    client->disconnect();
    host.close();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    require(!host.isListening(), "local service host closed");

    std::cout
        << "{\"task\":\"EPIC-278\","
        << "\"mode\":\"tradedraft-manual-recommendation-flow-full-delivery\","
        << "\"schemaPreflightPassed\":true,"
        << "\"tradeDraftSchemaFound\":true,"
        << "\"tradeDraftRepositoryFound\":true,"
        << "\"tradeDraftManualRecommendationFlowCreated\":true,"
        << "\"strategyRecommendationToTradeDraftConversionCreated\":true,"
        << "\"dataServiceWriteActionCreated\":true,"
        << "\"dataServiceReadOnlySummaryCreated\":true,"
        << "\"dataServiceClientAdapterCreated\":true,"
        << "\"shellServicesPresenterContractCreated\":true,"
        << "\"qmlPanelWired\":false,"
        << "\"tradeDraftRequiresExplicitUserConfirmation\":true,"
        << "\"positiveSampleCount\":8,"
        << "\"negativeSampleCount\":12,"
        << "\"buyRecommendationCreatesDraft\":true,"
        << "\"sellRecommendationCreatesDraft\":true,"
        << "\"holdRecommendationRejected\":true,"
        << "\"waitRecommendationRejected\":true,"
        << "\"rejectedRecommendationRejected\":true,"
        << "\"baseProtectedSellBlockedRejected\":true,"
        << "\"missingConfirmationRejected\":true,"
        << "\"duplicateSameKeyNoGrowth\":true,"
        << "\"idempotencyConflictNoGrowth\":true,"
        << "\"realDataServiceClientUsed\":true,"
        << "\"presenterTradeDraftMapped\":true,"
        << "\"tradeDraftIsNotOrder\":true,"
        << "\"tradeLogRowsWrittenByTradeDraft\":false,"
        << "\"cashAdjustmentRowsWrittenByTradeDraft\":false,"
        << "\"positionChangedByTradeDraft\":false,"
        << "\"cashChangedByTradeDraft\":false,"
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
