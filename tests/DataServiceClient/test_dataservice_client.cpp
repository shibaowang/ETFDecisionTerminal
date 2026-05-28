#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "DataServiceClient/DataServiceClientApi.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectEqual(std::string_view actual, std::string_view expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectEqual(int actual, int expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

std::filesystem::path optionValue(int argc, char* argv[], const std::string& option)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == option) {
            return argv[i + 1];
        }
    }
    return {};
}

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path()
        / ("etfdt_dataservice_client_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
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

void openMigratedDatabase(
    const std::filesystem::path& migrationPath,
    const std::filesystem::path& dbPath,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath;
    config.migrationFilePath = migrationPath;
    config.enableWal = true;
    config.foreignKeys = true;

    auto openResult = connection.open(config);
    expectTrue(openResult.hasValue(), "SQLiteConnection open succeeds");
    auto migrationResult =
        etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrationResult.hasValue(), "Initial migration succeeds");
    auto health = connection.healthCheck();
    expectTrue(health.hasValue(), "healthCheck succeeds before serving");
}

QJsonObject payloadObject(const etfdt::protocol::ProtocolResponse& response)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(response.payloadJson));
    if (!document.isObject()) {
        ++gFailures;
        std::cerr << "FAILED: payload is not a JSON object: " << response.payloadJson << '\n';
        return {};
    }
    return document.object();
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& tableName)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + tableName + ";");
    expectTrue(count.hasValue(), "count query succeeds for " + tableName);
    return count.hasValue() ? count.value() : -1;
}

void expectSuccessfulResponse(
    const etfdt::data_service_client::DataServiceClientResult<etfdt::protocol::ProtocolResponse>&
        result,
    std::string_view label)
{
    expectTrue(result.hasValue(), std::string(label) + " returns ProtocolResponse");
    if (result) {
        expectTrue(result.value().success, std::string(label) + " success=true");
    }
}

void testDataServiceClient(const std::filesystem::path& migrationPath)
{
    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "dataservice_client.db";

    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);

    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
    etfdt::data_service_api::registerDataServiceWriteActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    const std::string socketName = "ETFDecisionTerminalDataServiceClientTest_"
        + std::to_string(QCoreApplication::applicationPid());
    auto listenResult = host.listen(socketName);
    expectTrue(listenResult.hasValue(), "ActionServiceHost listen succeeds");

    etfdt::data_service_client::DataServiceClient client;
    auto connected = client.connect(socketName, 1000);
    expectTrue(connected.hasValue(), "DataServiceClient connects");
    expectTrue(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "host accepts client");

    auto ping = client.ping();
    expectSuccessfulResponse(ping, "client.ping");
    if (ping) {
        expectTrue(payloadObject(ping.value()).value("pong").toBool(false), "ping payload pong=true");
    }

    auto health = client.health();
    expectSuccessfulResponse(health, "client.health");
    if (health) {
        expectTrue(
            payloadObject(health.value()).value("healthy").toBool(false),
            "health payload healthy=true");
    }

    auto summary = client.summary();
    expectSuccessfulResponse(summary, "client.summary");
    if (summary) {
        expectTrue(payloadObject(summary.value()).contains("activeAccount"), "summary has activeAccount");
    }

    auto accounts = client.listAccounts();
    expectSuccessfulResponse(accounts, "client.listAccounts");
    if (accounts) {
        expectTrue(
            !payloadObject(accounts.value()).value("accounts").toArray().isEmpty(),
            "listAccounts contains default account");
    }

    auto portfolios = client.listPortfolios();
    expectSuccessfulResponse(portfolios, "client.listPortfolios");
    if (portfolios) {
        expectTrue(
            !payloadObject(portfolios.value()).value("portfolios").toArray().isEmpty(),
            "listPortfolios contains default portfolio");
    }

    auto instruments = client.listInstruments();
    expectSuccessfulResponse(instruments, "client.listInstruments");
    if (instruments) {
        const auto values = payloadObject(instruments.value()).value("instruments").toArray();
        const bool hasCash = std::any_of(values.begin(), values.end(), [](const auto& value) {
            return value.toObject().value("instrumentType").toString() == "CASH";
        });
        expectTrue(hasCash, "listInstruments contains CASH");
    }

    auto strategies = client.listStrategies();
    expectSuccessfulResponse(strategies, "client.listStrategies");
    if (strategies) {
        expectTrue(payloadObject(strategies.value()).value("strategies").isArray(), "strategies is array");
    }

    auto otc = client.listOtc("not_exists");
    expectSuccessfulResponse(otc, "client.listOtc not_exists");
    if (otc) {
        expectTrue(
            payloadObject(otc.value()).value("channels").toArray().isEmpty(),
            "listOtc not_exists returns empty channels");
    }

    const int auditCountBefore = countRows(connection, "audit_log");
    auto accountingHealth = client.accountingHealth();
    expectSuccessfulResponse(accountingHealth, "client.accountingHealth");
    if (accountingHealth) {
        const auto payload = payloadObject(accountingHealth.value());
        expectEqual(payload.value("module").toString().toStdString(), "accounting", "accountingHealth module");
        expectTrue(payload.value("readOnly").toBool(false), "accountingHealth readOnly=true");
        expectTrue(
            !payload.value("replayImplemented").toBool(true),
            "accountingHealth replayImplemented=false");
        expectTrue(!payload.value("writeEnabled").toBool(true), "accountingHealth writeEnabled=false");
        const auto warnings = payload.value("warnings").toArray();
        const bool hasReplayWarning = std::any_of(warnings.begin(), warnings.end(), [](const auto& value) {
            return value.toObject().value("code").toString() == "REPLAY_NOT_IMPLEMENTED";
        });
        expectTrue(hasReplayWarning, "accountingHealth warnings include REPLAY_NOT_IMPLEMENTED");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore,
        "accountingHealth does not insert audit_log row");

    auto replayPreview = client.accountingReplayPreview(
        R"({"accountId":"ACC-DEMO-001","portfolioId":"PF-DEMO-001","requestedOutputs":["positions","cash"]})");
    expectSuccessfulResponse(replayPreview, "client.accountingReplayPreview");
    if (replayPreview) {
        const auto payload = payloadObject(replayPreview.value());
        expectTrue(!payload.value("implemented").toBool(true), "replay preview implemented=false");
        expectTrue(!payload.value("replayExecuted").toBool(true), "replay preview replayExecuted=false");
        expectTrue(!payload.value("writeEnabled").toBool(true), "replay preview writeEnabled=false");
        expectEqual(
            payload.value("status").toString().toStdString(),
            "REPLAY_NOT_AVAILABLE",
            "replay preview status");
        const auto warnings = payload.value("warnings").toArray();
        const bool hasReplayWarning = std::any_of(warnings.begin(), warnings.end(), [](const auto& value) {
            return value.toObject().value("code").toString() == "REPLAY_NOT_IMPLEMENTED";
        });
        expectTrue(hasReplayWarning, "replay preview warnings include REPLAY_NOT_IMPLEMENTED");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore,
        "accountingReplayPreview does not insert audit_log row");

    auto positionList = client.positionList();
    expectSuccessfulResponse(positionList, "client.positionList");
    if (positionList) {
        const auto payload = payloadObject(positionList.value());
        expectEqual(payload.value("action").toString().toStdString(), "position.list", "positionList action");
        expectEqual(payload.value("module").toString().toStdString(), "accounting", "positionList module");
        expectTrue(!payload.value("implemented").toBool(true), "positionList implemented=false");
        expectTrue(payload.value("readOnly").toBool(false), "positionList readOnly=true");
        expectTrue(!payload.value("writeEnabled").toBool(true), "positionList writeEnabled=false");
        expectTrue(!payload.value("replayExecuted").toBool(true), "positionList replayExecuted=false");
        expectTrue(!payload.value("sqliteAccessed").toBool(true), "positionList sqliteAccessed=false");
        expectTrue(
            !payload.value("accountingEngineCalled").toBool(true),
            "positionList accountingEngineCalled=false");
        expectEqual(
            payload.value("status").toString().toStdString(),
            "POSITION_LIST_NOT_AVAILABLE",
            "positionList status");
        expectTrue(
            payload.value("futureOutput").toObject().value("positions").toArray().isEmpty(),
            "positionList wrapper returns no real positions");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore,
        "positionList does not insert audit_log row");

    auto cashSummary = client.cashSummary();
    expectSuccessfulResponse(cashSummary, "client.cashSummary");
    if (cashSummary) {
        const auto payload = payloadObject(cashSummary.value());
        expectEqual(payload.value("action").toString().toStdString(), "cash.summary", "cashSummary action");
        expectEqual(payload.value("module").toString().toStdString(), "accounting", "cashSummary module");
        expectTrue(!payload.value("implemented").toBool(true), "cashSummary implemented=false");
        expectTrue(payload.value("readOnly").toBool(false), "cashSummary readOnly=true");
        expectTrue(!payload.value("writeEnabled").toBool(true), "cashSummary writeEnabled=false");
        expectTrue(!payload.value("replayExecuted").toBool(true), "cashSummary replayExecuted=false");
        expectTrue(!payload.value("sqliteAccessed").toBool(true), "cashSummary sqliteAccessed=false");
        expectTrue(!payload.value("cashFactsAccessed").toBool(true), "cashSummary cashFactsAccessed=false");
        expectTrue(!payload.value("snapshotAccessed").toBool(true), "cashSummary snapshotAccessed=false");
        expectTrue(
            !payload.value("portfolioSummaryAccessed").toBool(true),
            "cashSummary portfolioSummaryAccessed=false");
        expectTrue(
            !payload.value("accountingEngineCalled").toBool(true),
            "cashSummary accountingEngineCalled=false");
        expectEqual(
            payload.value("status").toString().toStdString(),
            "CASH_SUMMARY_NOT_AVAILABLE",
            "cashSummary status");
        const auto futureOutput = payload.value("futureOutput").toObject();
        expectTrue(futureOutput.value("cashSummary").isNull(), "cashSummary wrapper returns no real summary");
        expectTrue(
            futureOutput.value("accountCashSummaries").toArray().isEmpty(),
            "cashSummary wrapper returns no account cash summaries");
        expectTrue(!payload.contains("cashBalance"), "cashSummary wrapper returns no real cashBalance");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore,
        "cashSummary does not insert audit_log row");

    auto portfolioPnlSummary = client.portfolioPnlSummary();
    expectSuccessfulResponse(portfolioPnlSummary, "client.portfolioPnlSummary");
    if (portfolioPnlSummary) {
        const auto payload = payloadObject(portfolioPnlSummary.value());
        expectEqual(
            payload.value("action").toString().toStdString(),
            "portfolio.pnl.summary",
            "portfolioPnlSummary action");
        expectEqual(
            payload.value("module").toString().toStdString(),
            "accounting",
            "portfolioPnlSummary module");
        expectTrue(!payload.value("implemented").toBool(true), "portfolioPnlSummary implemented=false");
        expectTrue(payload.value("readOnly").toBool(false), "portfolioPnlSummary readOnly=true");
        expectTrue(!payload.value("writeEnabled").toBool(true), "portfolioPnlSummary writeEnabled=false");
        expectTrue(!payload.value("replayExecuted").toBool(true), "portfolioPnlSummary replayExecuted=false");
        expectTrue(!payload.value("sqliteAccessed").toBool(true), "portfolioPnlSummary sqliteAccessed=false");
        expectTrue(!payload.value("tradeFactsAccessed").toBool(true), "portfolioPnlSummary tradeFactsAccessed=false");
        expectTrue(!payload.value("cashFactsAccessed").toBool(true), "portfolioPnlSummary cashFactsAccessed=false");
        expectTrue(
            !payload.value("marketPriceFactsAccessed").toBool(true),
            "portfolioPnlSummary marketPriceFactsAccessed=false");
        expectTrue(!payload.value("snapshotAccessed").toBool(true), "portfolioPnlSummary snapshotAccessed=false");
        expectTrue(
            !payload.value("portfolioSummaryAccessed").toBool(true),
            "portfolioPnlSummary portfolioSummaryAccessed=false");
        expectTrue(
            !payload.value("accountingEngineCalled").toBool(true),
            "portfolioPnlSummary accountingEngineCalled=false");
        expectEqual(
            payload.value("status").toString().toStdString(),
            "PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE",
            "portfolioPnlSummary status");
        const auto futureOutput = payload.value("futureOutput").toObject();
        expectTrue(
            futureOutput.value("portfolioPnl").isNull(),
            "portfolioPnlSummary wrapper returns no real pnl summary");
        expectTrue(!payload.contains("totalAssets"), "portfolioPnlSummary wrapper returns no real totalAssets");
        expectTrue(!payload.contains("realizedPnl"), "portfolioPnlSummary wrapper returns no real realizedPnl");
        expectTrue(!payload.contains("unrealizedPnl"), "portfolioPnlSummary wrapper returns no real unrealizedPnl");
        expectTrue(!payload.contains("totalPnl"), "portfolioPnlSummary wrapper returns no real totalPnl");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore,
        "portfolioPnlSummary does not insert audit_log row");

    auto basePositionSummary = client.basePositionSummary();
    expectSuccessfulResponse(basePositionSummary, "client.basePositionSummary");
    if (basePositionSummary) {
        const auto payload = payloadObject(basePositionSummary.value());
        expectEqual(
            payload.value("action").toString().toStdString(),
            "base_position.summary",
            "basePositionSummary action");
        expectEqual(
            payload.value("module").toString().toStdString(),
            "accounting",
            "basePositionSummary module");
        expectTrue(!payload.value("implemented").toBool(true), "basePositionSummary implemented=false");
        expectTrue(payload.value("readOnly").toBool(false), "basePositionSummary readOnly=true");
        expectTrue(!payload.value("writeEnabled").toBool(true), "basePositionSummary writeEnabled=false");
        expectTrue(!payload.value("replayExecuted").toBool(true), "basePositionSummary replayExecuted=false");
        expectTrue(!payload.value("sqliteAccessed").toBool(true), "basePositionSummary sqliteAccessed=false");
        expectTrue(!payload.value("tradeFactsAccessed").toBool(true), "basePositionSummary tradeFactsAccessed=false");
        expectTrue(!payload.value("snapshotAccessed").toBool(true), "basePositionSummary snapshotAccessed=false");
        expectTrue(
            !payload.value("positionSnapshotAccessed").toBool(true),
            "basePositionSummary positionSnapshotAccessed=false");
        expectTrue(
            !payload.value("portfolioSummaryAccessed").toBool(true),
            "basePositionSummary portfolioSummaryAccessed=false");
        expectTrue(
            !payload.value("accountingEngineCalled").toBool(true),
            "basePositionSummary accountingEngineCalled=false");
        expectTrue(
            !payload.value("tradeDraftGenerated").toBool(true),
            "basePositionSummary tradeDraftGenerated=false");
        expectTrue(
            !payload.value("tradeSuggestionGenerated").toBool(true),
            "basePositionSummary tradeSuggestionGenerated=false");
        expectTrue(!payload.value("strategyExecuted").toBool(true), "basePositionSummary strategyExecuted=false");
        expectEqual(
            payload.value("status").toString().toStdString(),
            "BASE_POSITION_SUMMARY_NOT_AVAILABLE",
            "basePositionSummary status");
        const auto futureOutput = payload.value("futureOutput").toObject();
        expectTrue(
            futureOutput.value("basePosition").isNull(),
            "basePositionSummary wrapper returns no real base position");
        expectTrue(!payload.contains("targetBaseRatioText"), "basePositionSummary wrapper returns no target base ratio");
        expectTrue(
            !payload.contains("lockedBaseAmountText"),
            "basePositionSummary wrapper returns no locked base amount");
        expectTrue(
            !payload.contains("sellableAboveBaseAmountText"),
            "basePositionSummary wrapper returns no sellable amount");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore,
        "basePositionSummary does not insert audit_log row");

    auto sniperPoolSummary = client.sniperPoolSummary();
    expectSuccessfulResponse(sniperPoolSummary, "client.sniperPoolSummary");
    if (sniperPoolSummary) {
        const auto payload = payloadObject(sniperPoolSummary.value());
        expectEqual(
            payload.value("action").toString().toStdString(),
            "sniper_pool.summary",
            "sniperPoolSummary action");
        expectEqual(
            payload.value("module").toString().toStdString(),
            "accounting",
            "sniperPoolSummary module");
        expectTrue(!payload.value("implemented").toBool(true), "sniperPoolSummary implemented=false");
        expectTrue(payload.value("readOnly").toBool(false), "sniperPoolSummary readOnly=true");
        expectTrue(!payload.value("writeEnabled").toBool(true), "sniperPoolSummary writeEnabled=false");
        expectTrue(!payload.value("replayExecuted").toBool(true), "sniperPoolSummary replayExecuted=false");
        expectTrue(!payload.value("sqliteAccessed").toBool(true), "sniperPoolSummary sqliteAccessed=false");
        expectTrue(!payload.value("tradeFactsAccessed").toBool(true), "sniperPoolSummary tradeFactsAccessed=false");
        expectTrue(!payload.value("snapshotAccessed").toBool(true), "sniperPoolSummary snapshotAccessed=false");
        expectTrue(
            !payload.value("positionSnapshotAccessed").toBool(true),
            "sniperPoolSummary positionSnapshotAccessed=false");
        expectTrue(
            !payload.value("cashSnapshotAccessed").toBool(true),
            "sniperPoolSummary cashSnapshotAccessed=false");
        expectTrue(
            !payload.value("portfolioSummaryAccessed").toBool(true),
            "sniperPoolSummary portfolioSummaryAccessed=false");
        expectTrue(
            !payload.value("accountingEngineCalled").toBool(true),
            "sniperPoolSummary accountingEngineCalled=false");
        expectTrue(
            !payload.value("sniperPoolCalculated").toBool(true),
            "sniperPoolSummary sniperPoolCalculated=false");
        expectTrue(
            !payload.value("tierSummaryCalculated").toBool(true),
            "sniperPoolSummary tierSummaryCalculated=false");
        expectTrue(
            !payload.value("tradeDraftGenerated").toBool(true),
            "sniperPoolSummary tradeDraftGenerated=false");
        expectTrue(
            !payload.value("tradeSuggestionGenerated").toBool(true),
            "sniperPoolSummary tradeSuggestionGenerated=false");
        expectTrue(!payload.value("strategyExecuted").toBool(true), "sniperPoolSummary strategyExecuted=false");
        expectEqual(
            payload.value("status").toString().toStdString(),
            "SNIPER_POOL_SUMMARY_NOT_AVAILABLE",
            "sniperPoolSummary status");
        const auto futureOutput = payload.value("futureOutput").toObject();
        expectTrue(
            futureOutput.value("sniperPool").isNull(),
            "sniperPoolSummary wrapper returns no real sniper pool");
        expectTrue(
            futureOutput.value("tierSummary").toArray().isEmpty(),
            "sniperPoolSummary wrapper returns no real tier summary");
        expectTrue(!payload.contains("poolAmountText"), "sniperPoolSummary wrapper returns no pool amount");
        expectTrue(!payload.contains("remainingAmountText"), "sniperPoolSummary wrapper returns no remaining amount");
        expectTrue(!payload.contains("T1"), "sniperPoolSummary wrapper returns no T1 tier");
        expectTrue(!payload.contains("T6"), "sniperPoolSummary wrapper returns no T6 tier");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore,
        "sniperPoolSummary does not insert audit_log row");

    etfdt::data_service_client::AuditAppendRequest auditRequest;
    auditRequest.entityType = "SYSTEM";
    auditRequest.entityId = "1";
    auditRequest.action = "CLIENT_DEMO_AUDIT";
    auditRequest.reason = "client compatibility test";
    auditRequest.operatorName = "client-test";
    auditRequest.newValueJson = R"({"client":true})";
    auto append = client.appendAuditDemo(auditRequest);
    expectSuccessfulResponse(append, "client.appendAuditDemo");
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore + 1,
        "appendAuditDemo inserts one audit_log row");

    auto missingReasonRequest = client.makeRequest(
        "data.audit.append",
        R"({"entityType":"SYSTEM","action":"CLIENT_MISSING_REASON"})");
    auto missingReason = client.sendRaw(missingReasonRequest, 2000);
    expectTrue(missingReason.hasValue(), "missing reason returns ProtocolResponse");
    if (missingReason) {
        expectTrue(!missingReason.value().success, "missing reason success=false");
        expectEqual(
            etfdt::protocol::toString(*missingReason.value().errorCode),
            "E1002_MISSING_REQUIRED_FIELD",
            "missing reason returns E1002");
    }
    expectEqual(
        countRows(connection, "audit_log"),
        auditCountBefore + 1,
        "missing reason does not insert audit_log row");

    auto unknown = client.sendRaw(client.makeRequest("data.trade_log.append", R"({"reason":"blocked"})"), 2000);
    expectTrue(unknown.hasValue(), "unknown action returns ProtocolResponse");
    if (unknown) {
        expectTrue(!unknown.value().success, "unknown action success=false");
        expectEqual(
            etfdt::protocol::toString(*unknown.value().errorCode),
            "E1004_INVALID_ACTION",
            "unknown action returns E1004");
    }

    expectEqual(countRows(connection, "trade_log"), 0, "trade_log remains empty");
    expectEqual(countRows(connection, "trade_execution_group"), 0, "trade_execution_group remains empty");
    expectEqual(countRows(connection, "trade_draft"), 0, "trade_draft remains empty");
    expectEqual(countRows(connection, "position_snapshot"), 0, "position_snapshot remains empty");
    expectEqual(countRows(connection, "cash_snapshot"), 0, "cash_snapshot remains empty");
    expectEqual(countRows(connection, "portfolio_summary"), 0, "portfolio_summary remains empty");

    client.disconnect();
    expectTrue(!client.isConnected(), "client disconnects");
    auto afterDisconnect = client.accountingHealth(100);
    expectTrue(!afterDisconnect.hasValue(), "accountingHealth after disconnect returns error");
    expectTrue(
        afterDisconnect.error().errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "accountingHealth after disconnect returns E9001");
    auto replayAfterDisconnect = client.accountingReplayPreview("{}", 100);
    expectTrue(!replayAfterDisconnect.hasValue(), "accountingReplayPreview after disconnect returns error");
    expectTrue(
        replayAfterDisconnect.error().errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "accountingReplayPreview after disconnect returns E9001");

    host.close();

    etfdt::data_service_client::DataServiceClient missingServerClient;
    auto missingServer = missingServerClient.connect(
        "ETFDecisionTerminalMissingSocket_" + std::to_string(QCoreApplication::applicationPid()),
        50);
    expectTrue(!missingServer.hasValue(), "missing socket connect fails");
    expectTrue(
        missingServer.error().errorCode == etfdt::protocol::ErrorCode::E9001_SERVICE_UNAVAILABLE,
        "missing socket connect returns E9001");

    connection.close();
    std::filesystem::remove_all(tempDir);
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto migrationPath = optionValue(argc, argv, "--migration");
    if (migrationPath.empty()) {
        std::cerr << "--migration <path> is required\n";
        return 1;
    }

    testDataServiceClient(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " DataServiceClient test failure(s)" << '\n';
        return 1;
    }

    std::cout << "DataServiceClient tests passed." << '\n';
    return 0;
}
