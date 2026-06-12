#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceActions.h"
#include "DataServiceApi/DataServiceApi.h"
#include "DataServiceClient/DataServiceClient.h"
#include "ServiceHost/ServiceHost.h"
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

#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

namespace fs = std::filesystem;

const std::vector<std::string>& positiveSamples()
{
    static const std::vector<std::string> samples{
        "EPIC277_buy_tier1_etf_direct.json",
        "EPIC277_buy_tier3_cash_capped_etf.json",
        "EPIC277_buy_tier6_cumulative_etf.json",
        "EPIC277_hold_no_signal.json",
        "EPIC277_wait_missing_market_data.json",
        "EPIC277_sell_take_profit_base_safe.json",
        "EPIC277_sell_extreme_premium_base_safe.json",
        "EPIC277_hold_base_protected_sell_blocked.json",
        "EPIC277_otc_replacement_generic_buy.json",
        "EPIC277_zero_fee_quantization.json",
        "EPIC277_decimal_price_quantity.json",
        "EPIC277_existing_position_hold.json",
    };
    return samples;
}

const std::vector<std::string>& negativeSamples()
{
    static const std::vector<std::string> samples{
        "EPIC277_negative_missing_instrument.json",
        "EPIC277_negative_invalid_price.json",
        "EPIC277_negative_invalid_cash.json",
        "EPIC277_negative_invalid_position_quantity.json",
        "EPIC277_negative_invalid_principal_base.json",
        "EPIC277_negative_unsupported_trade_source.json",
        "EPIC277_negative_sensitive_token.json",
        "EPIC277_negative_broker_order_token.json",
        "EPIC277_negative_unsupervised_execution_token.json",
        "EPIC277_negative_sell_over_position.json",
        "EPIC277_negative_no_cash_for_min_lot.json",
        "EPIC277_negative_invalid_thresholds.json",
    };
    return samples;
}

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string readFile(const fs::path& path)
{
    QFile file(QString::fromStdWString(path.wstring()));
    require(file.exists(), "missing file: " + path.string());
    require(file.open(QIODevice::ReadOnly), "cannot read file: " + path.string());
    return QString::fromUtf8(file.readAll()).toStdString();
}

QJsonObject parseObject(const std::string& json, const std::string& context)
{
    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json), &parseError);
    require(parseError.error == QJsonParseError::NoError, context + " parses");
    require(document.isObject(), context + " root is object");
    return document.object();
}

std::string stringValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isString() ? value.toString().toStdString() : std::string {};
}

bool boolValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isBool(), std::string("missing bool: ") + key);
    return value.toBool();
}

bool hasIssueCode(
    const etfdt::strategy_engine::StrategyRecommendationResult& result,
    const std::string& code)
{
    for (const auto& issue : result.issues) {
        if (issue.code == code) {
            return true;
        }
    }
    return false;
}

bool hasIssueCode(
    const etfdt::data_service_client::StrategyRecommendationReadOnlySummaryResult& result,
    const std::string& code)
{
    for (const auto& issueCode : result.issueCodes) {
        if (issueCode == code) {
            return true;
        }
    }
    return false;
}

etfdt::strategy_engine::StrategyRecommendationInput inputFromPayload(const std::string& payload)
{
    etfdt::strategy_engine::StrategyRecommendationInput input;
    std::string errorCode;
    require(
        etfdt::strategy_engine::decodeStrategyRecommendationInputJson(payload, input, errorCode),
        "strategy recommendation input decodes: " + errorCode);
    return input;
}

void assertNoWriteFlags(const etfdt::strategy_engine::StrategyRecommendationResult& result)
{
    require(result.readOnlyNoWrite, "engine result is read-only");
    require(!result.tradeDraftCreated, "engine does not create TradeDraft");
    require(!result.tradeLogRowsWrittenByRecommendation, "engine writes no trade_log rows");
    require(!result.cashAdjustmentRowsWrittenByRecommendation, "engine writes no cash rows");
    require(!result.auditLogRowsWrittenByRecommendation, "engine writes no audit rows");
    require(!result.productionDbTouched, "engine touches no production DB");
    require(!result.networkAccess, "engine uses no network");
    require(!result.credentialAccess, "engine uses no credentials");
    require(!result.endpointAccess, "engine uses no endpoint");
    require(!result.brokerOrderSubmitted, "engine submits no broker order");
    require(!result.automaticTrading, "engine does no automatic trading");
}

void assertExpectedRecommendation(
    const QJsonObject& fixture,
    const etfdt::strategy_engine::StrategyRecommendationResult& result)
{
    const auto expected = fixture.value(QStringLiteral("expectedRecommendation")).toObject();
    require(result.accepted, "positive recommendation accepted: " + stringValue(fixture, "sampleId"));
    require(result.status == stringValue(expected, "status"), "status exact");
    require(result.actionCode == stringValue(expected, "actionCode"), "actionCode exact");
    require(result.reasonCode == stringValue(expected, "reasonCode"), "reasonCode exact");
    require(result.tierLabel == stringValue(expected, "tierLabel"), "tierLabel exact");
    require(result.sourceCode == stringValue(expected, "sourceCode"), "sourceCode exact");
    require(
        result.suggestedQuantityText == stringValue(expected, "suggestedQuantityText"),
        "suggestedQuantityText exact");
    require(
        result.suggestedAmountText == stringValue(expected, "suggestedAmountText"),
        "suggestedAmountText exact");
    require(
        result.netCashImpactText == stringValue(expected, "netCashImpactText"),
        "netCashImpactText exact");
    require(
        result.baseProtectionPassed == boolValue(expected, "baseProtectionPassed"),
        "baseProtectionPassed exact");
    require(
        result.cashLimitApplied == boolValue(expected, "cashLimitApplied"),
        "cashLimitApplied exact");
    assertNoWriteFlags(result);
    for (const auto& issue : result.issues) {
        require(issue.message.find("password") == std::string::npos, "issue omits sensitive token");
        require(issue.message.find("broker_order") == std::string::npos, "issue omits broker token");
    }
}

void assertExpectedIssues(
    const QJsonObject& fixture,
    const etfdt::strategy_engine::StrategyRecommendationResult& result)
{
    require(!result.accepted, "negative recommendation fails closed: " + stringValue(fixture, "sampleId"));
    const auto expectedCodes = fixture.value(QStringLiteral("expectedIssueCodes")).toArray();
    require(!expectedCodes.isEmpty(), "negative sample has issue codes");
    for (const auto& value : expectedCodes) {
        require(value.isString(), "expected issue code is string");
        require(hasIssueCode(result, value.toString().toStdString()), "expected issue code present");
    }
    assertNoWriteFlags(result);
    for (const auto& issue : result.issues) {
        require(issue.message.find("password") == std::string::npos, "negative issue sanitized");
        require(issue.message.find("broker_order") == std::string::npos, "negative issue broker sanitized");
        require(issue.message.find("unsupervised_execution") == std::string::npos, "negative issue token sanitized");
    }
}

void runSampleMatrix(const fs::path& sampleDir)
{
    etfdt::strategy_engine::StrategyRecommendationEngine engine;
    for (const auto& fileName : positiveSamples()) {
        const auto payload = readFile(sampleDir / fileName);
        const auto fixture = parseObject(payload, fileName);
        const auto result = engine.recommendReadOnly(inputFromPayload(payload));
        assertExpectedRecommendation(fixture, result);
    }
    for (const auto& fileName : negativeSamples()) {
        const auto payload = readFile(sampleDir / fileName);
        const auto fixture = parseObject(payload, fileName);
        const auto result = engine.recommendReadOnly(inputFromPayload(payload));
        assertExpectedIssues(fixture, result);
    }
}

struct DbFixture {
    fs::path directory;
    std::unique_ptr<etfdt::data_access::SQLiteConnection> connection =
        std::make_unique<etfdt::data_access::SQLiteConnection>();

    DbFixture() = default;
    DbFixture(const DbFixture&) = delete;
    DbFixture& operator=(const DbFixture&) = delete;

    DbFixture(DbFixture&& other) noexcept
        : directory(std::move(other.directory))
        , connection(std::move(other.connection))
    {
        other.directory.clear();
    }

    DbFixture& operator=(DbFixture&& other) noexcept
    {
        if (this != &other) {
            if (connection) {
                connection->close();
            }
            if (!directory.empty()) {
                std::error_code ec;
                fs::remove_all(directory, ec);
            }
            directory = std::move(other.directory);
            connection = std::move(other.connection);
            other.directory.clear();
        }
        return *this;
    }

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
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("epic277_strategy_recommendation_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "epic277.sqlite";
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

void assertClientNoWrite(
    const etfdt::data_service_client::StrategyRecommendationReadOnlySummaryResult& result)
{
    require(result.readOnlyRecommendationNoWrite, "client maps read-only no-write");
    require(!result.tradeDraftCreated, "client maps no TradeDraft");
    require(!result.tradeLogRowsWrittenByRecommendation, "client maps no trade_log write");
    require(!result.cashAdjustmentRowsWrittenByRecommendation, "client maps no cash write");
    require(!result.auditLogRowsWrittenByRecommendation, "client maps no audit write");
    require(!result.productionDbTouched, "client maps no production DB touch");
    require(!result.brokerOrderSubmitted, "client maps no broker order");
    require(!result.networkAccess, "client maps no network");
    require(!result.credentialAccess, "client maps no credentials");
    require(!result.endpointAccess, "client maps no endpoint");
    require(!result.automaticTrading, "client maps no automatic trading");
}

void runLocalServicePresenterPath(const fs::path& root, const fs::path& sampleDir)
{
    auto fixture = makeDb(root);
    require(countRows(*fixture.connection, "trade_log") == 0, "initial trade_log empty");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "initial cash_adjustment empty");
    require(countRows(*fixture.connection, "audit_log") == 0, "initial audit_log empty");

    etfdt::service_runtime::ActionDispatcher dispatcher(etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, *fixture.connection);
    require(
        dispatcher.hasAction(etfdt::data_service_api::kActionStrategyRecommendationReadOnlySummary),
        "strategy recommendation read-only action registered");

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalEpic277StrategyRecommendation_"
        + std::to_string(QCoreApplication::applicationPid()) + "_"
        + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    require(host.listen(socketName).hasValue(), "ActionServiceHost listen succeeds");

    auto client = std::make_shared<etfdt::data_service_client::DataServiceClient>();
    require(client->connect(socketName, 1000).hasValue(), "DataServiceClient connects");
    require(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "service host accepts client");

    etfdt::data_service_client::StrategyRecommendationReadOnlySummaryRequest request;
    request.recommendationPayloadJson = readFile(sampleDir / "EPIC277_buy_tier1_etf_direct.json");
    const auto buy = client->strategyRecommendationReadOnlySummary(request, 1000);
    require(buy.hasValue(), "real DataServiceClient calls strategy recommendation action");
    require(buy.value().accepted, "client maps buy accepted");
    require(buy.value().actionCode == "BUY", "client maps buy action");
    require(buy.value().tierLabel == "Tier1", "client maps buy tier");
    assertClientNoWrite(buy.value());

    request.recommendationPayloadJson = readFile(sampleDir / "EPIC277_sell_take_profit_base_safe.json");
    const auto sell = client->strategyRecommendationReadOnlySummary(request, 1000);
    require(sell.hasValue(), "client maps sell response");
    require(sell.value().accepted, "client maps sell accepted");
    require(sell.value().reasonCode == "TAKE_PROFIT", "client maps take profit");
    require(sell.value().suggestedQuantityText == "900", "client maps sell quantity");
    assertClientNoWrite(sell.value());

    request.recommendationPayloadJson = readFile(sampleDir / "EPIC277_hold_base_protected_sell_blocked.json");
    const auto baseProtected = client->strategyRecommendationReadOnlySummary(request, 1000);
    require(baseProtected.hasValue(), "client maps base-protected response");
    require(baseProtected.value().accepted, "client maps base-protected accepted");
    require(baseProtected.value().actionCode == "HOLD", "client maps base-protected hold");
    require(!baseProtected.value().baseProtectionPassed, "client maps base protection block");
    assertClientNoWrite(baseProtected.value());

    request.recommendationPayloadJson = readFile(sampleDir / "EPIC277_negative_sensitive_token.json");
    const auto sensitive = client->strategyRecommendationReadOnlySummary(request, 1000);
    require(sensitive.hasValue(), "client maps sensitive-token response");
    require(!sensitive.value().accepted, "client maps sensitive-token fail closed");
    require(hasIssueCode(sensitive.value(), "SENSITIVE_TOKEN"), "client maps sensitive issue");
    assertClientNoWrite(sensitive.value());

    auto port =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter>(client);
    auto serviceAdapter =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(port);
    auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
    controller->setServiceAdapter(serviceAdapter);
    etfdt::shell_services::ShellAccountingPresenter presenter;
    presenter.setController(controller);

    require(
        presenter.previewStrategyRecommendationReadOnlySummary(
            QString::fromStdString(readFile(sampleDir / "EPIC277_buy_tier1_etf_direct.json"))),
        "Presenter strategy recommendation succeeds");
    require(
        presenter.lastStrategyRecommendationStatus() == QStringLiteral("OK"),
        "Presenter recommendation status mapped");
    require(
        presenter.lastStrategyRecommendationAction() == QStringLiteral("Buy"),
        "Presenter recommendation action mapped");
    require(
        presenter.lastStrategyRecommendationTier() == QStringLiteral("Tier1"),
        "Presenter recommendation tier mapped");
    require(
        presenter.lastStrategyRecommendationSummary().contains(QStringLiteral("quantity=400")),
        "Presenter recommendation summary visible");

    require(
        !presenter.previewStrategyRecommendationReadOnlySummary(
            QString::fromStdString(readFile(sampleDir / "EPIC277_negative_sensitive_token.json"))),
        "Presenter negative recommendation fails closed");
    require(
        presenter.lastStrategyRecommendationIssueCodes().contains(QStringLiteral("SENSITIVE_TOKEN")),
        "Presenter maps sensitive issue code");

    require(countRows(*fixture.connection, "trade_log") == 0, "recommendation writes no trade_log rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "recommendation writes no cash rows");
    require(countRows(*fixture.connection, "audit_log") == 0, "recommendation writes no audit rows");

    client->disconnect();
    host.close();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    require(!host.isListening(), "local service host closed");
}

void assertDocs(const fs::path& root)
{
    for (const auto& file : {
             "364_strategy_recommendation_engine_vba_parity_full_delivery.md",
             "365_strategy_recommendation_engine_vba_parity_matrix.md",
             "366_strategy_recommendation_engine_contract.md",
             "367_strategy_recommendation_engine_user_visible_summary.md",
             "368_strategy_recommendation_engine_test_plan.md",
         }) {
        const auto text = readFile(root / "docs" / file);
        require(text.find("EPIC-277") != std::string::npos, std::string(file) + " references EPIC-277");
        require(text.find("read-only") != std::string::npos, std::string(file) + " documents read-only boundary");
        require(text.find("broker") != std::string::npos, std::string(file) + " documents broker boundary");
    }
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "EPIC-277"},
        {"strategyRecommendationEngineCreated", true},
        {"vbaParityMatrixCreated", true},
        {"positiveSampleCount", static_cast<int>(positiveSamples().size())},
        {"negativeSampleCount", static_cast<int>(negativeSamples().size())},
        {"positiveRecommendationsExact", true},
        {"negativeRecommendationsFailClosed", true},
        {"buyTier1EtfDirectPassed", true},
        {"buyTier3CashCappedPassed", true},
        {"buyTier6CumulativePassed", true},
        {"holdNoSignalPassed", true},
        {"waitMissingMarketDataPassed", true},
        {"sellTakeProfitBaseSafePassed", true},
        {"sellExtremePremiumBaseSafePassed", true},
        {"holdBaseProtectedSellBlockedPassed", true},
        {"otcReplacementGenericBuyPassed", true},
        {"quantizationParityPassed", true},
        {"cashCappedBuyParityPassed", true},
        {"baseProtectionParityPassed", true},
        {"dataServiceReadOnlyActionCreated", true},
        {"realDataServiceClientUsed", true},
        {"presenterRecommendationMapped", true},
        {"readOnlyRecommendationNoWrite", true},
        {"tradeDraftCreated", false},
        {"tradeLogRowsWrittenByRecommendation", false},
        {"cashAdjustmentRowsWrittenByRecommendation", false},
        {"auditLogRowsWrittenByRecommendation", false},
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
        fs::path root;
        for (int index = 1; index < argc; ++index) {
            const std::string arg = argv[index];
            if (arg == "--source-root" && index + 1 < argc) {
                root = argv[++index];
            }
        }
        require(!root.empty(), "--source-root is required");

        const auto sampleDir = root / "samples" / "strategy_recommendation";
        assertDocs(root);
        runSampleMatrix(sampleDir);
        runLocalServicePresenterPath(root, sampleDir);

        std::cout << QJsonDocument(evidenceJson()).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    } catch (const std::exception& ex) {
        const QJsonObject failure{
            {"task", "EPIC-277"},
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
