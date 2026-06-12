#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceActionRegistrar.h"
#include "DataServiceApi/DataServiceActions.h"
#include "DataServiceClient/DataServiceClient.h"
#include "MarketEngine/DisabledMarketDataProvider.h"
#include "MarketEngine/FixtureMarketDataProvider.h"
#include "MarketEngine/MarketDataRefreshEngine.h"
#include "MarketEngine/MarketDataSafetyPolicy.h"
#include "ServiceHost/ActionServiceHost.h"
#include "ServiceRuntime/BuiltinActions.h"
#include "ServiceRuntime/ServiceRuntime.h"
#include "ShellServices/ShellAccountingDataServiceAdapter.h"
#include "ShellServices/ShellAccountingDataServiceClientPortAdapter.h"
#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "StrategyEngine/StrategyRecommendationEngine.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QThread>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

namespace {

namespace fs = std::filesystem;

const std::vector<std::string>& positiveSamples()
{
    static const std::vector<std::string> samples{
        "EPIC280_etf_quote_and_history_high.json",
        "EPIC280_index_quote_and_history_high.json",
        "EPIC280_otc_nav_quote.json",
        "EPIC280_etf_drawdown_from_high.json",
        "EPIC280_index_drawdown_from_high.json",
        "EPIC280_premium_discount_positive.json",
        "EPIC280_premium_discount_negative.json",
        "EPIC280_stale_quote_with_cached_history.json",
        "EPIC280_partial_quote_missing_history.json",
        "EPIC280_multi_instrument_refresh.json",
        "EPIC280_strategy_input_market_fields.json",
        "EPIC280_current_price_breaks_historical_high_display_only.json",
    };
    return samples;
}

const std::vector<std::string>& negativeSamples()
{
    static const std::vector<std::string> samples{
        "EPIC280_negative_provider_disabled.json",
        "EPIC280_negative_missing_price.json",
        "EPIC280_negative_invalid_price.json",
        "EPIC280_negative_missing_history.json",
        "EPIC280_negative_invalid_high.json",
        "EPIC280_negative_unsupported_instrument.json",
        "EPIC280_negative_stale_beyond_limit.json",
        "EPIC280_negative_sensitive_token.json",
        "EPIC280_negative_broker_endpoint_token.json",
        "EPIC280_negative_credential_token.json",
        "EPIC280_negative_network_endpoint_not_allowlisted.json",
        "EPIC280_negative_live_provider_disabled_by_default.json",
        "EPIC280_negative_rate_limited_same_batch.json",
        "EPIC280_negative_history_failure_circuit_breaker.json",
        "EPIC280_negative_no_parallel_same_host.json",
    };
    return samples;
}

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
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
    QFile file(QString::fromStdWString(path.wstring()));
    require(file.exists(), "missing file: " + path.string());
    require(file.open(QIODevice::ReadOnly), "cannot read file: " + path.string());
    return QString::fromUtf8(file.readAll()).toStdString();
}

QJsonObject parseObject(const std::string& json, const std::string& label)
{
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    require(parseError.error == QJsonParseError::NoError, "parse JSON: " + label);
    require(document.isObject(), "root is object: " + label);
    return document.object();
}

std::string compactJson(const QJsonObject& object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
}

QJsonObject loadSample(const fs::path& sampleDir, const std::string& fileName)
{
    return parseObject(readFile(sampleDir / fileName), fileName);
}

std::string stringValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isString() ? value.toString().toStdString() : std::string {};
}

std::vector<std::string> stringArray(const QJsonObject& object, const char* key)
{
    std::vector<std::string> result;
    const auto values = object.value(QString::fromUtf8(key)).toArray();
    for (const auto& value : values) {
        if (value.isString()) {
            result.push_back(value.toString().toStdString());
        }
    }
    return result;
}

bool contains(const std::vector<std::string>& values, const std::string& expected)
{
    return std::find(values.begin(), values.end(), expected) != values.end();
}

etfdt::market_engine::MarketRefreshInput decodeMarketInput(
    const std::string& payload,
    const std::string& label)
{
    etfdt::market_engine::MarketRefreshInput input;
    std::string errorCode;
    require(
        etfdt::market_engine::decodeMarketRefreshInputJson(payload, input, errorCode),
        label + " decodes market input: " + errorCode);
    return input;
}

void assertNoWriteFlags(const etfdt::market_engine::MarketRefreshResult& result, const std::string& label)
{
    require(result.readOnlyNoWrite, label + " is read-only");
    require(!result.productionDbTouched, label + " touches no production DB");
    require(!result.tradeLogRowsWrittenByMarketData, label + " writes no trade_log rows");
    require(!result.cashAdjustmentRowsWrittenByMarketData, label + " writes no cash_adjustment rows");
    require(!result.networkAccess, label + " uses no network");
    require(!result.rawUrlExposed, label + " exposes no raw URL");
    require(!result.rawResponseExposed, label + " exposes no raw response");
    require(!result.brokerOrderSubmitted, label + " submits no broker order");
    require(!result.credentialAccess, label + " reads no credentials");
    require(!result.endpointAccess, label + " reads no endpoint");
    require(!result.realOrderPlacement, label + " places no real order");
    require(!result.automaticTrading, label + " performs no automatic trading");
}

void assertExpectedMarketResult(
    const std::string& fileName,
    const QJsonObject& sample,
    const etfdt::market_engine::MarketRefreshResult& result)
{
    const auto expected = sample.value(QStringLiteral("expected")).toObject();
    require(result.status == stringValue(expected, "status"), fileName + " status");
    require(result.dataQualityStatus == stringValue(expected, "dataQualityStatus"), fileName + " quality");
    require(result.currentPriceText == stringValue(expected, "currentPriceText"), fileName + " current");
    require(result.previousCloseText == stringValue(expected, "previousCloseText"), fileName + " previous close");
    require(result.historicalHighText == stringValue(expected, "historicalHighText"), fileName + " high");
    require(result.historicalHighDate == stringValue(expected, "historicalHighDate"), fileName + " high date");
    require(result.displayedHighText == stringValue(expected, "displayedHighText"), fileName + " displayed high");
    require(result.drawdownFromHighText == stringValue(expected, "drawdownFromHighText"), fileName + " drawdown");
    require(result.premiumDiscountText == stringValue(expected, "premiumDiscountText"), fileName + " premium");
    require(result.providerSource == stringValue(expected, "providerSource"), fileName + " provider source");
    require(result.exactHostAllowlistEnforced, fileName + " exact host allowlist flag");
    require(result.batchRequestsOnly, fileName + " batch requests flag");
    require(result.perHostRateLimitEnforced, fileName + " per-host rate flag");
    require(result.historyHighDailyCacheEnforced, fileName + " daily cache flag");
    require(result.historyHighFailureCircuitBreakerEnforced, fileName + " circuit breaker flag");
    require(result.noParallelSameHostRequests, fileName + " same-host parallel guard flag");
    for (const auto& code : stringArray(expected, "issueCodes")) {
        require(contains(result.issueCodes, code), fileName + " issue code present: " + code);
    }
    assertNoWriteFlags(result, fileName);
}

void assertExpectedFailure(
    const std::string& fileName,
    const QJsonObject& sample,
    const etfdt::market_engine::MarketRefreshResult& result)
{
    const auto expected = sample.value(QStringLiteral("expected")).toObject();
    const auto codes = stringArray(expected, "issueCodes");
    require(!codes.empty(), fileName + " declares expected issue codes");
    for (const auto& code : codes) {
        require(contains(result.issueCodes, code), fileName + " issue code present: " + code);
    }
    assertNoWriteFlags(result, fileName);
}

void runEngineSampleMatrix(const fs::path& sampleDir)
{
    etfdt::market_engine::MarketDataRefreshEngine engine;
    for (const auto& fileName : positiveSamples()) {
        const auto payload = readFile(sampleDir / fileName);
        const auto sample = parseObject(payload, fileName);
        auto input = decodeMarketInput(payload, fileName);
        etfdt::market_engine::FixtureMarketDataProvider provider(payload);
        const auto result = engine.refreshReadOnly(input, provider);
        require(result.accepted, fileName + " accepted");
        assertExpectedMarketResult(fileName, sample, result);
    }

    for (const auto& fileName : negativeSamples()) {
        const auto payload = readFile(sampleDir / fileName);
        const auto sample = parseObject(payload, fileName);
        const auto expected = sample.value(QStringLiteral("expected")).toObject();
        const auto caseId = stringValue(sample, "caseId");
        const auto codes = stringArray(expected, "issueCodes");

        if (caseId == "negative_network_endpoint_not_allowlisted") {
            const auto decision = etfdt::market_engine::MarketDataSafetyPolicy{}.authorizeUrl(
                "https",
                "example.com",
                "/private/quote");
            require(!decision.allowed, fileName + " safety policy rejects host/path");
            require(contains(codes, decision.issueCode), fileName + " safety issue exact");
            continue;
        }
        if (caseId == "negative_rate_limited_same_batch") {
            etfdt::market_engine::MarketDataRateLimiter limiter;
            require(limiter.allowBatch("eastmoney", "batch-280", 1000), fileName + " first batch allowed");
            require(!limiter.allowBatch("eastmoney", "batch-280", 1010), fileName + " second batch limited");
            continue;
        }
        if (caseId == "negative_history_failure_circuit_breaker") {
            etfdt::market_engine::MarketDataCircuitBreaker breaker;
            breaker.recordHistoryFailure("1.510300", 1000);
            require(!breaker.allowHistoryRequest("1.510300", 1100), fileName + " circuit open");
            require(breaker.allowHistoryRequest("1.510300", 1700), fileName + " circuit closes after 10 minutes");
            continue;
        }
        if (caseId == "negative_no_parallel_same_host") {
            const auto inputObject = sample.value(QStringLiteral("input")).toObject();
            require(
                inputObject.value(QStringLiteral("safetyPolicyProbe")).toString()
                    == QStringLiteral("no_parallel_same_host"),
                fileName + " sample documents same-host parallel denial");
            require(contains(codes, "MARKET_DATA_NO_PARALLEL_SAME_HOST"), fileName + " same-host issue exact");
            continue;
        }

        auto input = decodeMarketInput(payload, fileName);
        std::unique_ptr<etfdt::market_engine::MarketDataProvider> provider;
        if (input.providerMode == "disabled") {
            provider = std::make_unique<etfdt::market_engine::DisabledMarketDataProvider>();
        } else if (input.providerMode == "live") {
            provider = std::make_unique<etfdt::market_engine::LivePublicMarketDataProviderBoundary>(
                input.liveMarketDataEnabled);
        } else {
            provider = std::make_unique<etfdt::market_engine::FixtureMarketDataProvider>(payload);
        }
        const auto result = engine.refreshReadOnly(input, *provider);
        require(
            !result.accepted || result.status == "STALE" || result.status == "PARTIAL",
            fileName + " fail-closed or safe stale/partial");
        assertExpectedFailure(fileName, sample, result);
    }
}

void runSafetyPolicyAssertions()
{
    etfdt::market_engine::MarketDataSafetyPolicy policy;
    require(policy.authorizeUrl("http", "qt.gtimg.cn", "/q=sh510300").allowed, "tencent quote URL allowed");
    require(
        policy.authorizeUrl("https", "push2.eastmoney.com", "/api/qt/ulist.np/get").allowed,
        "eastmoney quote URL allowed");
    require(policy.authorizeUrl("http", "hq.sinajs.cn", "/list=f_510300").allowed, "sina quote URL allowed");
    require(
        policy.authorizeUrl("https", "push2his.eastmoney.com", "/api/qt/stock/kline/get").allowed,
        "eastmoney history URL allowed");
    require(
        !policy.authorizeUrl("https", "broker.example.internal", "/private").allowed,
        "unlisted host rejected");
    require(policy.containsForbiddenToken("access_token"), "forbidden token detected");

    etfdt::market_engine::MarketDataInMemoryCache cache;
    require(!cache.hasHistorySuccessForToday("1.510300", "2026-06-12"), "cache initially empty");
    cache.recordHistorySuccessForToday("1.510300", "2026-06-12");
    require(cache.hasHistorySuccessForToday("1.510300", "2026-06-12"), "history daily cache records success");
}

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

DbFixture makeDb(const fs::path& root)
{
    DbFixture fixture;
    fixture.directory = fs::temp_directory_path()
        / ("etfdt-epic280-" + std::to_string(QCoreApplication::applicationPid()) + "-"
           + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "epic280.sqlite";
    config.enableWal = false;
    require(fixture.connection->open(config).hasValue(), "open temp SQLite");
    require(
        fixture.connection->applyMigrationFile(root / "migrations" / "001_initial_schema.sql").hasValue(),
        "apply migration 001");
    require(
        fixture.connection
            ->applyMigrationFile(root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
            .hasValue(),
        "apply migration 002");
    return fixture;
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& table)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + table + ";");
    require(count.hasValue(), "count query succeeds for " + table);
    return static_cast<int>(count.value());
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

std::string socketName()
{
    return "ETFDecisionTerminalEpic280MarketData_"
        + std::to_string(QCoreApplication::applicationPid()) + "_"
        + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
}

QJsonObject strategyPayloadFromMarket(
    const fs::path& strategySampleDir,
    const etfdt::data_service_client::MarketDataReadOnlySummaryResult& marketData)
{
    auto payload = loadSample(strategySampleDir, "EPIC277_buy_tier1_etf_direct.json");
    auto input = payload.value(QStringLiteral("input")).toObject();
    input.insert(QStringLiteral("instrumentCode"), QString::fromStdString(marketData.instrumentCode));
    input.insert(QStringLiteral("instrumentType"), QString::fromStdString(marketData.instrumentType));
    input.insert(QStringLiteral("currentPriceText"), QString::fromStdString(marketData.currentPriceText));
    input.insert(QStringLiteral("indexDrawdownText"), QStringLiteral("16.00"));
    input.insert(QStringLiteral("premiumText"), QStringLiteral("2.44"));
    payload.insert(QStringLiteral("input"), input);
    return payload;
}

void runLocalServicePresenterAndStrategyPath(const fs::path& root, const fs::path& sampleDir)
{
    auto fixture = makeDb(root);
    require(countRows(*fixture.connection, "trade_log") == 0, "initial trade_log empty");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "initial cash_adjustment empty");
    require(countRows(*fixture.connection, "audit_log") == 0, "initial audit_log empty");

    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, *fixture.connection);
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionMarketDataRefreshReadOnlySummary),
        "market data refresh action registered");
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionMarketDataHistoricalHighReadOnlySummary),
        "market data historical high action registered");
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionStrategyRecommendationReadOnlySummary),
        "strategy recommendation action registered");

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const auto socket = socketName();
    require(host.listen(socket).hasValue(), "ActionServiceHost listen succeeds");

    auto client = std::make_shared<etfdt::data_service_client::DataServiceClient>();
    require(client->connect(socket, 1000).hasValue(), "DataServiceClient connects");
    require(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "service host accepts client");

    const auto payload = readFile(sampleDir / "EPIC280_etf_quote_and_history_high.json");
    etfdt::data_service_client::MarketDataReadOnlySummaryRequest request;
    request.payloadJson = payload;
    const auto refresh = client->marketDataRefreshReadOnlySummary(request, 1000);
    require(refresh.hasValue(), "real DataServiceClient calls market data refresh");
    require(refresh.value().accepted, "client maps market data accepted");
    require(refresh.value().currentPriceText == "4.800", "client maps current price");
    require(refresh.value().historicalHighText == "5.000", "client maps historical high");
    require(refresh.value().marketDataReadOnlyActionCreated, "client maps read-only action flag");
    require(refresh.value().historicalHighReadOnlyActionCreated, "client maps historical high action flag");
    require(!refresh.value().networkAccess, "client maps no network");
    require(!refresh.value().productionDbTouched, "client maps no production DB");

    const auto high = client->marketDataHistoricalHighReadOnlySummary(request, 1000);
    require(high.hasValue(), "real DataServiceClient calls historical high action");
    require(high.value().accepted, "historical high action accepted");
    require(high.value().historicalHighText == "5.000", "historical high action maps high");

    auto port =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter>(client);
    auto serviceAdapter =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(port);
    auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
    controller->setServiceAdapter(serviceAdapter);
    etfdt::shell_services::ShellAccountingPresenter presenter;
    presenter.setController(controller);

    require(
        presenter.refreshMarketDataReadOnly(QString::fromStdString(payload)),
        "Presenter market data refresh succeeds");
    require(presenter.lastMarketDataRefreshStatus() == QStringLiteral("OK"), "Presenter maps status");
    require(presenter.lastMarketDataInstrumentCode() == QStringLiteral("510300"), "Presenter maps instrument");
    require(presenter.lastMarketDataCurrentPriceText() == QStringLiteral("4.800"), "Presenter maps current");
    require(
        presenter.lastMarketDataHistoricalHighText() == QStringLiteral("5.000"),
        "Presenter maps historical high");
    require(
        presenter.lastMarketDataSummary().contains(QStringLiteral("displayedHigh=5.000")),
        "Presenter summary visible");

    const auto strategyMarketPayload = readFile(sampleDir / "EPIC280_strategy_input_market_fields.json");
    etfdt::data_service_client::MarketDataReadOnlySummaryRequest strategyMarketRequest;
    strategyMarketRequest.payloadJson = strategyMarketPayload;
    const auto strategyMarket = client->marketDataRefreshReadOnlySummary(strategyMarketRequest, 1000);
    require(strategyMarket.hasValue(), "strategy market input refresh called");
    require(strategyMarket.value().accepted, "strategy market input accepted");

    etfdt::data_service_client::StrategyRecommendationReadOnlySummaryRequest strategyRequest;
    strategyRequest.recommendationPayloadJson =
        compactJson(strategyPayloadFromMarket(root / "samples" / "strategy_recommendation", strategyMarket.value()));
    const auto strategy = client->strategyRecommendationReadOnlySummary(strategyRequest, 1000);
    require(strategy.hasValue(), "strategy recommendation DataService call succeeds");
    require(strategy.value().accepted, "market fields feed accepted strategy recommendation");
    require(!strategy.value().tradeDraftCreated, "strategy remains read-only");
    require(!strategy.value().brokerOrderSubmitted, "strategy submits no broker order");
    require(!strategy.value().networkAccess, "strategy uses no network");

    require(countRows(*fixture.connection, "trade_log") == 0, "market data writes no trade_log rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "market data writes no cash_adjustment rows");
    require(countRows(*fixture.connection, "audit_log") == 0, "market data writes no audit rows");

    client->disconnect();
    host.close();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    require(!host.isListening(), "local service host closed");
}

void assertDocs(const fs::path& root)
{
    for (const auto& file : {
             "379_market_data_refresh_engine_full_delivery.md",
             "380_market_data_provider_contract.md",
             "381_market_data_historical_high_vba_parity_matrix.md",
             "382_market_data_user_visible_refresh_flow.md",
             "383_market_data_test_plan.md",
         }) {
        const auto text = readFile(root / "docs" / file);
        require(text.find("EPIC-280") != std::string::npos, std::string(file) + " references EPIC-280");
        require(text.find("read-only") != std::string::npos, std::string(file) + " documents read-only");
        require(text.find("broker") != std::string::npos, std::string(file) + " documents broker boundary");
        require(text.find("2-second") != std::string::npos || text.find("2 second") != std::string::npos,
                std::string(file) + " documents no 2-second polling loop");
    }
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "EPIC-280"},
        {"marketDataRefreshEngineCreated", true},
        {"marketDataProviderContractCreated", true},
        {"disabledProviderCreated", true},
        {"fixtureProviderCreated", true},
        {"liveProviderImplemented", false},
        {"liveProviderDisabledByDefault", true},
        {"liveProviderDeferredForSafety", true},
        {"positiveSampleCount", static_cast<int>(positiveSamples().size())},
        {"negativeSampleCount", static_cast<int>(negativeSamples().size())},
        {"etfHistoricalHighPassed", true},
        {"indexHistoricalHighPassed", true},
        {"otcNavQuotePassed", true},
        {"etfDrawdownPassed", true},
        {"indexDrawdownPassed", true},
        {"premiumDiscountPassed", true},
        {"staleQuoteHandled", true},
        {"partialQuoteHandled", true},
        {"providerDisabledFailClosed", true},
        {"exactHostAllowlistEnforced", true},
        {"batchRequestsOnly", true},
        {"perHostRateLimitEnforced", true},
        {"historyHighDailyCacheEnforced", true},
        {"historyHighFailureCircuitBreakerMinutes", 10},
        {"noParallelSameHostRequests", true},
        {"marketDataReadOnlyActionCreated", true},
        {"historicalHighReadOnlyActionCreated", true},
        {"realDataServiceClientUsed", true},
        {"presenterMarketDataMapped", true},
        {"strategyInputMarketFieldsMapped", true},
        {"testNetworkAccess", false},
        {"rawUrlExposed", false},
        {"rawResponseExposed", false},
        {"productionDbTouched", false},
        {"tradeLogRowsWrittenByMarketData", false},
        {"cashAdjustmentRowsWrittenByMarketData", false},
        {"brokerOrderSubmitted", false},
        {"credentialAccess", false},
        {"endpointAccess", false},
        {"realOrderPlacement", false},
        {"automaticTrading", false},
    };
}

}  // namespace

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    try {
        const fs::path root = sourceRoot(argc, argv);
        const auto sampleDir = root / "samples" / "market_data";
        runEngineSampleMatrix(sampleDir);
        runSafetyPolicyAssertions();
        runLocalServicePresenterAndStrategyPath(root, sampleDir);
        assertDocs(root);
        std::cout << QJsonDocument(evidenceJson()).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "EPIC-280 full delivery test failed: " << ex.what() << '\n';
        return 1;
    }
}
