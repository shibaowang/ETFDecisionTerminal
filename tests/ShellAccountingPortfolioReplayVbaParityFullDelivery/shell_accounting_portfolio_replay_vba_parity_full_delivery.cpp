#include "AccountingEngine/ShellAccountingPortfolioReplayEngine.h"
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
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

namespace fs = std::filesystem;

using etfdt::accounting::ShellAccountingReplayCashFact;
using etfdt::accounting::ShellAccountingReplayInput;
using etfdt::accounting::ShellAccountingReplayMarketPriceFact;
using etfdt::accounting::ShellAccountingReplayTradeFact;

const std::vector<std::string>& positiveSamples()
{
    static const std::vector<std::string> samples{
        "EPIC276_buy_only.json",
        "EPIC276_buy_partial_sell.json",
        "EPIC276_multi_buy_average_cost.json",
        "EPIC276_sell_all_position.json",
        "EPIC276_cash_in_cash_out.json",
        "EPIC276_multi_instrument.json",
        "EPIC276_zero_fee.json",
        "EPIC276_decimal_quantity_price_fee.json",
        "EPIC276_etf_and_otc_split.json",
        "EPIC276_unrealized_pnl_with_market_price.json",
    };
    return samples;
}

const std::vector<std::string>& negativeSamples()
{
    static const std::vector<std::string> samples{
        "EPIC276_negative_sell_over_position.json",
        "EPIC276_negative_duplicate_fact_id.json",
        "EPIC276_negative_invalid_quantity.json",
        "EPIC276_negative_invalid_price.json",
        "EPIC276_negative_unsupported_side.json",
        "EPIC276_negative_unsupported_cash_action.json",
        "EPIC276_negative_missing_instrument.json",
        "EPIC276_negative_currency_mismatch.json",
        "EPIC276_negative_sensitive_token.json",
        "EPIC276_negative_broker_order_token.json",
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

QJsonObject parseObjectFile(const fs::path& path)
{
    return parseObject(readFile(path), path.string());
}

std::string stringValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isString() ? value.toString().toStdString() : std::string {};
}

QJsonArray arrayValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isArray(), std::string("missing array: ") + key);
    return value.toArray();
}

ShellAccountingReplayTradeFact parseTradeFact(const QJsonObject& object)
{
    return ShellAccountingReplayTradeFact{
        stringValue(object, "factId"),
        stringValue(object, "tradeTime"),
        stringValue(object, "accountId"),
        stringValue(object, "portfolioId"),
        stringValue(object, "instrumentCode"),
        stringValue(object, "instrumentType"),
        stringValue(object, "side"),
        stringValue(object, "quantityText"),
        stringValue(object, "priceText"),
        stringValue(object, "amountText"),
        stringValue(object, "feeText"),
        stringValue(object, "cashFlowText"),
        stringValue(object, "currency"),
        stringValue(object, "source"),
        stringValue(object, "note"),
    };
}

ShellAccountingReplayCashFact parseCashFact(const QJsonObject& object)
{
    return ShellAccountingReplayCashFact{
        stringValue(object, "factId"),
        stringValue(object, "time"),
        stringValue(object, "accountId"),
        stringValue(object, "portfolioId"),
        stringValue(object, "action"),
        stringValue(object, "amountText"),
        stringValue(object, "currency"),
        stringValue(object, "note"),
    };
}

ShellAccountingReplayMarketPriceFact parseMarketPriceFact(const QJsonObject& object)
{
    return ShellAccountingReplayMarketPriceFact{
        stringValue(object, "instrumentCode"),
        stringValue(object, "priceTime"),
        stringValue(object, "priceText"),
        stringValue(object, "currency"),
        stringValue(object, "source"),
    };
}

ShellAccountingReplayInput inputFromFixture(const QJsonObject& fixture)
{
    ShellAccountingReplayInput input;
    input.schemaVersion = stringValue(fixture, "schemaVersion");
    const auto facts = fixture.value(QStringLiteral("inputFacts")).toObject();
    for (const auto& value : arrayValue(facts, "cashFacts")) {
        require(value.isObject(), "cash fact is object");
        input.cashFacts.push_back(parseCashFact(value.toObject()));
    }
    for (const auto& value : arrayValue(facts, "tradeFacts")) {
        require(value.isObject(), "trade fact is object");
        input.tradeFacts.push_back(parseTradeFact(value.toObject()));
    }
    for (const auto& value : arrayValue(facts, "marketPriceFacts")) {
        require(value.isObject(), "market price fact is object");
        input.marketPriceFacts.push_back(parseMarketPriceFact(value.toObject()));
    }
    return input;
}

std::vector<std::string> expectedIssueCodes(const QJsonObject& fixture)
{
    std::vector<std::string> codes;
    const auto projection = fixture.value(QStringLiteral("expectedProjection")).toObject();
    for (const auto& value : arrayValue(projection, "expectedIssueCodes")) {
        require(value.isString(), "expected issue code is string");
        codes.push_back(value.toString().toStdString());
    }
    return codes;
}

std::optional<etfdt::accounting::ShellAccountingReplayPosition> findPosition(
    const etfdt::accounting::ShellAccountingReplayResult& result,
    const std::string& instrumentCode)
{
    for (const auto& position : result.positions) {
        if (position.instrumentCode == instrumentCode) {
            return position;
        }
    }
    return std::nullopt;
}

std::optional<etfdt::accounting::ShellAccountingReplayCashSummary> findCashSummary(
    const etfdt::accounting::ShellAccountingReplayResult& result,
    const std::string& accountId,
    const std::string& portfolioId,
    const std::string& currency)
{
    for (const auto& cash : result.cashSummaries) {
        if (cash.accountId == accountId && cash.portfolioId == portfolioId && cash.currency == currency) {
            return cash;
        }
    }
    return std::nullopt;
}

void assertExpectedProjection(
    const QJsonObject& fixture,
    const etfdt::accounting::ShellAccountingReplayResult& result)
{
    const auto projection = fixture.value(QStringLiteral("expectedProjection")).toObject();
    for (const auto& value : arrayValue(projection, "cashSummaries")) {
        require(value.isObject(), "expected cash summary is object");
        const auto expected = value.toObject();
        const auto actual = findCashSummary(
            result,
            stringValue(expected, "accountId"),
            stringValue(expected, "portfolioId"),
            stringValue(expected, "currency"));
        require(actual.has_value(), "expected cash summary present");
        require(actual->cashBalanceText == stringValue(expected, "cashBalanceText"),
                "cash balance exact match for " + stringValue(expected, "currency"));
    }

    const auto expectedPositions = arrayValue(projection, "positions");
    require(result.positions.size() == static_cast<std::size_t>(expectedPositions.size()),
            "position count exact match for " + stringValue(fixture, "fixtureId"));
    for (const auto& value : expectedPositions) {
        require(value.isObject(), "expected position is object");
        const auto expected = value.toObject();
        const auto actual = findPosition(result, stringValue(expected, "instrumentCode"));
        require(actual.has_value(), "expected position present: " + stringValue(expected, "instrumentCode"));
        require(actual->positionBucket == stringValue(expected, "positionBucket"), "position bucket exact match");
        require(actual->quantityText == stringValue(expected, "quantityText"), "quantity exact match");
        require(actual->averageCostText == stringValue(expected, "averageCostText"), "average cost exact match");
        require(actual->costBasisText == stringValue(expected, "costBasisText"), "cost basis exact match");
        require(actual->realizedPnlText == stringValue(expected, "realizedPnlText"), "realized pnl exact match");
        require(actual->unrealizedPnlText == stringValue(expected, "unrealizedPnlText"), "unrealized pnl exact match");
        require(actual->totalFeeText == stringValue(expected, "totalFeeText"), "total fee exact match");
        if (expected.contains(QStringLiteral("marketValueText"))) {
            require(actual->marketValueText == stringValue(expected, "marketValueText"), "market value exact match");
        }
    }

    const auto pnl = projection.value(QStringLiteral("pnlSummary")).toObject();
    require(result.pnlSummary.realizedPnlText == stringValue(pnl, "realizedPnlText"), "portfolio realized pnl exact match");
    require(result.pnlSummary.unrealizedPnlText == stringValue(pnl, "unrealizedPnlText"), "portfolio unrealized pnl exact match");
    require(result.pnlSummary.totalFeeText == stringValue(pnl, "totalFeeText"), "portfolio fee exact match");
    require(result.pnlSummary.totalMarketValueText == stringValue(pnl, "totalMarketValueText"), "portfolio market value exact match");
    require(result.pnlSummary.dataQualityStatus == stringValue(pnl, "dataQualityStatus"), "portfolio data quality exact match");
}

void assertIssueCodes(
    const QJsonObject& fixture,
    const etfdt::accounting::ShellAccountingReplayResult& result)
{
    const auto expectedCodes = expectedIssueCodes(fixture);
    for (const auto& expectedCode : expectedCodes) {
        bool found = false;
        for (const auto& issue : result.issues) {
            found = found || issue.code == expectedCode;
            require(issue.message.find("password") == std::string::npos, "issue message sanitized");
            require(issue.message.find("broker_order") == std::string::npos, "issue broker token sanitized");
        }
        require(found, "expected issue code present: " + expectedCode);
    }
}

void runSampleMatrix(const fs::path& sampleDir)
{
    etfdt::accounting::ShellAccountingPortfolioReplayEngine engine;
    for (const auto& fileName : positiveSamples()) {
        const auto fixture = parseObjectFile(sampleDir / fileName);
        const auto result = engine.replayReadOnly(inputFromFixture(fixture));
        require(result.accepted, "positive sample accepted: " + fileName);
        require(result.replayExecuted, "positive sample replay executed: " + fileName);
        require(result.readOnly, "positive sample read-only: " + fileName);
        assertExpectedProjection(fixture, result);
        assertIssueCodes(fixture, result);
    }

    for (const auto& fileName : negativeSamples()) {
        const auto fixture = parseObjectFile(sampleDir / fileName);
        const auto result = engine.replayReadOnly(inputFromFixture(fixture));
        require(!result.accepted, "negative sample rejected: " + fileName);
        require(result.replayExecuted, "negative sample replay attempted: " + fileName);
        require(result.readOnly, "negative sample read-only: " + fileName);
        require(result.positions.empty(), "negative sample has no positions: " + fileName);
        require(result.cashSummaries.empty(), "negative sample has no cash summaries: " + fileName);
        assertExpectedProjection(fixture, result);
        assertIssueCodes(fixture, result);
    }
}

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

DbFixture makeDb(const fs::path& root)
{
    DbFixture fixture;
    const auto ticks = std::chrono::steady_clock::now().time_since_epoch().count();
    fixture.directory = fs::temp_directory_path()
        / ("epic276_portfolio_replay_" + std::to_string(ticks));
    fs::remove_all(fixture.directory);
    fs::create_directories(fixture.directory);

    etfdt::data_access::DatabaseConfig config;
    config.databasePath = fixture.directory / "epic276.sqlite";
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
        dispatcher.hasAction(etfdt::data_service_api::kActionAccountingPortfolioReplayReadOnlySummary),
        "portfolio replay read-only action registered");

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalEpic276PortfolioReplay_"
        + std::to_string(QCoreApplication::applicationPid()) + "_"
        + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    require(host.listen(socketName).hasValue(), "ActionServiceHost listen succeeds");

    auto client = std::make_shared<etfdt::data_service_client::DataServiceClient>();
    require(client->connect(socketName, 1000).hasValue(), "DataServiceClient connects");
    require(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "service host accepts client");

    const auto buyPartialPayload = readFile(sampleDir / "EPIC276_buy_partial_sell.json");
    etfdt::data_service_client::PortfolioReplayReadOnlySummaryRequest clientRequest;
    clientRequest.replayPayloadJson = buyPartialPayload;
    const auto replay = client->accountingPortfolioReplayReadOnlySummary(clientRequest, 1000);
    require(replay.hasValue(), "real DataServiceClient calls portfolio replay action");
    require(replay.value().accepted, "client maps accepted replay");
    require(replay.value().positionCount == 1, "client maps one replay position");
    require(replay.value().cashSummaryCount == 1, "client maps one cash summary");
    require(replay.value().pnlSummary.realizedPnlText == "38.60", "client maps realized PnL");
    require(replay.value().readOnlyReplayNoWrite, "client maps read-only no-write flag");
    require(!replay.value().tradeLogRowsWrittenByReplay, "client maps no trade_log writes");
    require(!replay.value().cashAdjustmentRowsWrittenByReplay, "client maps no cash writes");
    require(!replay.value().auditLogRowsWrittenByReplay, "client maps no audit writes");

    auto port =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter>(client);
    auto serviceAdapter =
        std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(port);
    auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
    controller->setServiceAdapter(serviceAdapter);
    etfdt::shell_services::ShellAccountingPresenter presenter;
    presenter.setController(controller);

    require(
        presenter.previewPortfolioReplayReadOnlySummary(QString::fromStdString(buyPartialPayload)),
        "Presenter portfolio replay summary succeeds");
    require(
        presenter.lastPortfolioReplayStatus() == QStringLiteral("COMPLETED"),
        "Presenter replay status mapped");
    require(presenter.portfolioReplayPositionCount() == 1, "Presenter maps position count");
    require(presenter.portfolioReplayCashSummaryCount() == 1, "Presenter maps cash summary count");
    require(presenter.portfolioReplayRealizedPnl() == QStringLiteral("38.60"), "Presenter maps realized PnL");
    require(
        presenter.lastPortfolioReplaySummary().contains(QStringLiteral("positions=1")),
        "Presenter replay summary visible");

    const auto negativePayload = readFile(sampleDir / "EPIC276_negative_sell_over_position.json");
    require(
        !presenter.previewPortfolioReplayReadOnlySummary(QString::fromStdString(negativePayload)),
        "Presenter negative replay fails closed");
    require(
        presenter.lastPortfolioReplayIssueCodes().contains(QStringLiteral("SELL_OVER_POSITION")),
        "Presenter maps negative issue code");

    require(countRows(*fixture.connection, "trade_log") == 0, "read-only replay writes no trade_log rows");
    require(countRows(*fixture.connection, "cash_adjustment") == 0, "read-only replay writes no cash_adjustment rows");
    require(countRows(*fixture.connection, "audit_log") == 0, "read-only replay writes no audit rows");

    client->disconnect();
    host.close();
    QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
    require(!host.isListening(), "local service host closed");
}

void assertDocs(const fs::path& root)
{
    for (const auto& file : {
             "359_shell_accounting_portfolio_replay_vba_parity_full_delivery.md",
             "360_shell_accounting_portfolio_replay_vba_parity_matrix.md",
             "361_shell_accounting_portfolio_replay_read_model_contract.md",
             "362_shell_accounting_portfolio_replay_user_visible_summary.md",
             "363_shell_accounting_portfolio_replay_test_plan.md",
         }) {
        const auto text = readFile(root / "docs" / file);
        require(text.find("EPIC-276") != std::string::npos, std::string(file) + " references EPIC-276");
        require(text.find("read-only") != std::string::npos, std::string(file) + " documents read-only boundary");
        require(text.find("broker") != std::string::npos, std::string(file) + " documents broker boundary");
    }
}

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "EPIC-276"},
        {"portfolioReplayEngineCreated", true},
        {"vbaParityMatrixCreated", true},
        {"positiveSampleCount", static_cast<int>(positiveSamples().size())},
        {"negativeSampleCount", static_cast<int>(negativeSamples().size())},
        {"positiveReplayExactAssertionsPassed", true},
        {"negativeReplayFailClosed", true},
        {"buyOnlyParityPassed", true},
        {"buyPartialSellParityPassed", true},
        {"multiBuyAverageCostParityPassed", true},
        {"sellAllPositionParityPassed", true},
        {"cashInCashOutParityPassed", true},
        {"multiInstrumentParityPassed", true},
        {"etfAndOtcSplitRepresented", true},
        {"realizedPnlComputed", true},
        {"unrealizedPnlComputedWhenMarketPriceProvided", true},
        {"cashAuditFailClosed", true},
        {"dataServiceReadOnlyActionCreated", true},
        {"realDataServiceClientUsed", true},
        {"presenterReplaySummaryMapped", true},
        {"readOnlyReplayNoWrite", true},
        {"tradeLogRowsWrittenByReplay", false},
        {"cashAdjustmentRowsWrittenByReplay", false},
        {"auditLogRowsWrittenByReplay", false},
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

        const auto sampleDir = root / "samples" / "shell_accounting_replay";
        assertDocs(root);
        runSampleMatrix(sampleDir);
        runLocalServicePresenterPath(root, sampleDir);

        std::cout << QJsonDocument(evidenceJson()).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    } catch (const std::exception& ex) {
        const QJsonObject failure{
            {"task", "EPIC-276"},
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
