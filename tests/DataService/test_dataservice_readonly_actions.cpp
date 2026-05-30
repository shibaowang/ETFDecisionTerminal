#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "Protocol/Protocol.h"
#include "ServiceHost/ServiceHost.h"
#include "ServiceRuntime/ServiceRuntime.h"
#include "Transport/Transport.h"

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
#include <utility>
#include <vector>

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
        / ("etfdt_dataservice_actions_" + std::to_string(now));
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

etfdt::protocol::MessageEnvelope envelope(std::string action, std::string payloadJson = "{}")
{
    etfdt::protocol::MessageEnvelope request;
    request.msgId = "dataservice-action-msg-" + action;
    request.traceId = "dataservice-action-trace-" + action;
    request.from = etfdt::protocol::ServiceName::ETFDecisionShell;
    request.to = etfdt::protocol::ServiceName::ETFDataService;
    request.action = std::move(action);
    request.timestampUtc = "2026-05-24T14:00:00Z";
    request.payloadJson = std::move(payloadJson);
    return request;
}

QJsonObject parseJsonObject(const std::string& json)
{
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(json));
    if (!document.isObject()) {
        ++gFailures;
        std::cerr << "FAILED: response is not a JSON object: " << json << '\n';
        return {};
    }
    return document.object();
}

bool sendAndWait(
    etfdt::transport::LocalSocketClient& client,
    const std::string& json,
    std::vector<std::string>& responses)
{
    const auto before = responses.size();
    auto sent = client.send(json);
    expectTrue(sent.hasValue(), "client sends request frame");
    if (!sent) {
        return false;
    }
    return waitUntil([&]() { return responses.size() > before; }, 2000);
}

QJsonObject sendEnvelope(
    etfdt::transport::LocalSocketClient& client,
    const etfdt::protocol::MessageEnvelope& request,
    std::vector<std::string>& responses,
    std::string_view label)
{
    const bool received = sendAndWait(client, etfdt::protocol::toJsonString(request), responses);
    expectTrue(received, std::string(label) + " response received");
    if (!received) {
        return {};
    }
    return parseJsonObject(responses.back());
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& tableName)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + tableName + ";");
    expectTrue(count.hasValue(), "count query succeeds for " + tableName);
    return count.hasValue() ? count.value() : -1;
}

std::vector<std::pair<std::string, int>> protectedTableCounts(
    etfdt::data_access::SQLiteConnection& connection)
{
    return {
        {"trade_log", countRows(connection, "trade_log")},
        {"trade_execution_group", countRows(connection, "trade_execution_group")},
        {"trade_draft", countRows(connection, "trade_draft")},
        {"cash_snapshot", countRows(connection, "cash_snapshot")},
        {"position_snapshot", countRows(connection, "position_snapshot")},
        {"portfolio_summary", countRows(connection, "portfolio_summary")},
        {"audit_log", countRows(connection, "audit_log")},
    };
}

void expectProtectedTableCountsUnchanged(
    etfdt::data_access::SQLiteConnection& connection,
    const std::vector<std::pair<std::string, int>>& before,
    std::string_view label)
{
    for (const auto& [tableName, beforeCount] : before) {
        expectEqual(
            countRows(connection, tableName),
            beforeCount,
            std::string(label) + " keeps " + tableName + " row count unchanged");
    }
}

void expectSuccess(const QJsonObject& response, std::string_view label)
{
    expectTrue(response.value("success").toBool(false), std::string(label) + " success=true");
}

void expectReadOnlyFactsEmptyPayload(
    const QJsonObject& payload,
    std::string_view action,
    std::string_view emptyIssueCode,
    std::string_view label)
{
    expectEqual(payload.value("action").toString().toStdString(), action, std::string(label) + " action");
    expectEqual(payload.value("module").toString().toStdString(), "accounting", std::string(label) + " module");
    expectTrue(payload.value("implemented").toBool(false), std::string(label) + " implemented=true");
    expectTrue(payload.value("readOnly").toBool(false), std::string(label) + " readOnly=true");
    expectTrue(!payload.value("writeEnabled").toBool(true), std::string(label) + " writeEnabled=false");
    expectTrue(!payload.value("replayExecuted").toBool(true), std::string(label) + " replayExecuted=false");
    expectTrue(!payload.value("snapshotRebuilt").toBool(true), std::string(label) + " snapshotRebuilt=false");
    expectTrue(payload.value("dataSourceAccessed").toBool(false), std::string(label) + " dataSourceAccessed=true");
    expectTrue(payload.value("sqliteAccessed").toBool(false), std::string(label) + " sqliteAccessed=true");
    expectTrue(
        !payload.value("accountingEngineCalled").toBool(true),
        std::string(label) + " accountingEngineCalled=false");
    expectTrue(
        !payload.value("tradeDraftGenerated").toBool(true),
        std::string(label) + " tradeDraftGenerated=false");
    expectTrue(
        !payload.value("tradeSuggestionGenerated").toBool(true),
        std::string(label) + " tradeSuggestionGenerated=false");
    expectTrue(
        !payload.value("strategyExecuted").toBool(true),
        std::string(label) + " strategyExecuted=false");
    expectEqual(payload.value("status").toString().toStdString(), "EMPTY", std::string(label) + " status");
    expectEqual(
        payload.value("dataQualityStatus").toString().toStdString(),
        "OK",
        std::string(label) + " dataQualityStatus");
    expectTrue(!payload.value("hasRows").toBool(true), std::string(label) + " hasRows=false");
    const auto issues = payload.value("issues").toArray();
    const bool hasEmptyIssue = std::any_of(issues.begin(), issues.end(), [&](const auto& value) {
        const auto issue = value.toObject();
        return issue.value("code").toString().toStdString() == emptyIssueCode
            && !issue.value("blocking").toBool(true);
    });
    expectTrue(hasEmptyIssue, std::string(label) + " issues contains non-blocking empty issue");
    const auto privacy = payload.value("privacy").toObject();
    expectTrue(!privacy.value("rawSqlExposed").toBool(true), std::string(label) + " rawSqlExposed=false");
    expectTrue(
        !privacy.value("rawTradeLogPayloadExposed").toBool(true),
        std::string(label) + " rawTradeLogPayloadExposed=false");
}

void testReadOnlyActions(const std::filesystem::path& migrationPath)
{
    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "readonly_actions.db";

    etfdt::data_access::SQLiteConnection connection;
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
    auto healthCheck = connection.healthCheck();
    expectTrue(healthCheck.hasValue(), "healthCheck succeeds before serving");
    const int auditLogBefore = countRows(connection, "audit_log");

    etfdt::service_runtime::ActionDispatcher dispatcher(
        etfdt::protocol::ServiceName::ETFDataService);
    etfdt::service_runtime::registerBuiltinActions(dispatcher);
    etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);

    etfdt::service_host::ActionServiceHost host(dispatcher);
    etfdt::transport::LocalSocketClient client;
    std::vector<std::string> responses;
    client.setMessageCallback([&](const std::string& json) {
        responses.push_back(json);
    });

    const std::string socketName = "ETFDecisionTerminalDataServiceActionsTest_"
        + std::to_string(QCoreApplication::applicationPid());

    auto listenResult = host.listen(socketName);
    expectTrue(listenResult.hasValue(), "ActionServiceHost listen succeeds");

    auto connected = client.connectToServer(socketName, 1000);
    expectTrue(connected.hasValue(), "LocalSocketClient connects");
    expectTrue(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "host accepts client");

    auto ping = sendEnvelope(
        client,
        envelope(etfdt::service_runtime::kActionSystemPing),
        responses,
        "system.ping");
    expectSuccess(ping, "system.ping");

    auto dataHealth = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionDataHealth),
        responses,
        "data.health");
    expectSuccess(dataHealth, "data.health");
    expectTrue(
        dataHealth.value("payload").toObject().value("healthy").toBool(false),
        "data.health payload healthy=true");

    auto summary = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionDataSummary),
        responses,
        "data.summary");
    expectSuccess(summary, "data.summary");
    expectTrue(
        summary.value("payload").toObject().contains("activeAccount"),
        "data.summary contains activeAccount");

    auto accounts = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionDataAccountsList),
        responses,
        "data.accounts.list");
    expectSuccess(accounts, "data.accounts.list");
    expectTrue(
        !accounts.value("payload").toObject().value("accounts").toArray().isEmpty(),
        "data.accounts.list returns default account");

    auto portfolios = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionDataPortfoliosList),
        responses,
        "data.portfolios.list");
    expectSuccess(portfolios, "data.portfolios.list");
    expectTrue(
        !portfolios.value("payload").toObject().value("portfolios").toArray().isEmpty(),
        "data.portfolios.list returns default portfolio");

    auto instruments = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionDataInstrumentsList),
        responses,
        "data.instruments.list");
    expectSuccess(instruments, "data.instruments.list");
    const auto instrumentArray = instruments.value("payload").toObject().value("instruments").toArray();
    const bool hasCash = std::any_of(instrumentArray.begin(), instrumentArray.end(), [](const auto& value) {
        return value.toObject().value("instrumentType").toString() == "CASH";
    });
    expectTrue(hasCash, "data.instruments.list returns CASH instrument");

    auto strategies = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionDataStrategiesList),
        responses,
        "data.strategies.list");
    expectSuccess(strategies, "data.strategies.list");
    expectTrue(
        strategies.value("payload").toObject().value("strategies").isArray(),
        "data.strategies.list returns array");

    auto missingStrategyCode = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionDataOtcList, "{}"),
        responses,
        "data.otc.list missing strategyCode");
    expectTrue(
        !missingStrategyCode.value("success").toBool(true),
        "data.otc.list missing strategyCode success=false");
    expectEqual(
        missingStrategyCode.value("errorCode").toString().toStdString(),
        "E1002_MISSING_REQUIRED_FIELD",
        "data.otc.list missing strategyCode returns E1002");

    auto missingOtc = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionDataOtcList, R"({"strategyCode":"NO_SUCH"})"),
        responses,
        "data.otc.list nonexistent strategyCode");
    expectSuccess(missingOtc, "data.otc.list nonexistent strategyCode");
    expectTrue(
        missingOtc.value("payload").toObject().value("channels").toArray().isEmpty(),
        "data.otc.list nonexistent strategyCode returns empty channels");

    auto accountingHealth = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionAccountingHealth),
        responses,
        "accounting.health");
    expectSuccess(accountingHealth, "accounting.health");
    const auto accountingPayload = accountingHealth.value("payload").toObject();
    expectTrue(!accountingPayload.isEmpty(), "accounting.health payload is JSON object");
    expectEqual(
        accountingPayload.value("module").toString().toStdString(),
        "accounting",
        "accounting.health module");
    expectTrue(accountingPayload.value("healthy").toBool(false), "accounting.health healthy=true");
    expectTrue(accountingPayload.value("readOnly").toBool(false), "accounting.health readOnly=true");
    expectTrue(
        !accountingPayload.value("replayImplemented").toBool(true),
        "accounting.health replayImplemented=false");
    expectTrue(
        !accountingPayload.value("snapshotImplemented").toBool(true),
        "accounting.health snapshotImplemented=false");
    expectTrue(
        !accountingPayload.value("writeEnabled").toBool(true),
        "accounting.health writeEnabled=false");

    const auto implementedActions = accountingPayload.value("implementedActions").toArray();
    const auto implementedActionExists = [&implementedActions](const QString& action) {
        return std::any_of(implementedActions.begin(), implementedActions.end(), [&action](const auto& value) {
            return value.toString() == action;
        });
    };
    const bool hasAccountingHealth = implementedActionExists("accounting.health");
    expectTrue(hasAccountingHealth, "accounting.health implementedActions contains accounting.health");
    expectTrue(
        implementedActionExists("position.list"),
        "accounting.health implementedActions contains position.list");
    expectTrue(
        implementedActionExists("cash.summary"),
        "accounting.health implementedActions contains cash.summary");
    expectTrue(
        implementedActionExists("portfolio.pnl.summary"),
        "accounting.health implementedActions contains portfolio.pnl.summary");
    expectTrue(
        implementedActionExists("base_position.summary"),
        "accounting.health implementedActions contains base_position.summary");
    expectTrue(
        implementedActionExists("sniper_pool.summary"),
        "accounting.health implementedActions contains sniper_pool.summary");

    const auto futureActions = accountingPayload.value("futureActions").toArray();
    const bool hasReplayPreview =
        std::any_of(futureActions.begin(), futureActions.end(), [](const auto& value) {
            return value.toString() == "accounting.replay.preview";
        });
    expectTrue(
        hasReplayPreview,
        "accounting.health futureActions contains accounting.replay.preview");

    const auto warnings = accountingPayload.value("warnings").toArray();
    const bool hasReplayWarning = std::any_of(warnings.begin(), warnings.end(), [](const auto& value) {
        return value.toObject().value("code").toString() == "REPLAY_NOT_IMPLEMENTED";
    });
    expectTrue(hasReplayWarning, "accounting.health warnings contains REPLAY_NOT_IMPLEMENTED");
    expectTrue(
        accountingPayload.value("errors").toArray().isEmpty(),
        "accounting.health errors is empty array");

    auto replayPreview = sendEnvelope(
        client,
        envelope(
            etfdt::data_service_api::kActionAccountingReplayPreview,
            R"({"accountId":"ACC-DEMO-001","portfolioId":"PF-DEMO-001","sourceFromTime":"2026-01-01T00:00:00Z","sourceToTime":"2026-01-31T23:59:59Z","fixtureMode":false,"requestedOutputs":["positions","cash","pnl"]})"),
        responses,
        "accounting.replay.preview");
    expectSuccess(replayPreview, "accounting.replay.preview guard");
    const auto replayPayload = replayPreview.value("payload").toObject();
    expectTrue(!replayPayload.value("implemented").toBool(true), "replay preview implemented=false");
    expectTrue(!replayPayload.value("replayExecuted").toBool(true), "replay preview replayExecuted=false");
    expectTrue(!replayPayload.value("writeEnabled").toBool(true), "replay preview writeEnabled=false");
    expectEqual(
        replayPayload.value("status").toString().toStdString(),
        "REPLAY_NOT_AVAILABLE",
        "replay preview status");
    expectEqual(
        replayPayload.value("requestedScope").toObject().value("accountId").toString().toStdString(),
        "ACC-DEMO-001",
        "replay preview echoes accountId");

    const auto requiredFixtures = replayPayload.value("requiredFixtures").toArray();
    const bool hasFirstFixture =
        std::any_of(requiredFixtures.begin(), requiredFixtures.end(), [](const auto& value) {
            return value.toString() == "FX001_EMPTY_LEDGER";
        });
    const bool hasLastFixture =
        std::any_of(requiredFixtures.begin(), requiredFixtures.end(), [](const auto& value) {
            return value.toString() == "FX013_MULTI_CURRENCY_UNSUPPORTED";
        });
    expectTrue(hasFirstFixture, "replay preview requiredFixtures contains FX001");
    expectTrue(hasLastFixture, "replay preview requiredFixtures contains FX013");

    const auto futureOutputs = replayPayload.value("futureOutputs").toArray();
    const bool hasPositionOutput =
        std::any_of(futureOutputs.begin(), futureOutputs.end(), [](const auto& value) {
            return value.toString() == "PositionListResponse";
        });
    const bool hasCashOutput =
        std::any_of(futureOutputs.begin(), futureOutputs.end(), [](const auto& value) {
            return value.toString() == "CashSummaryDto";
        });
    expectTrue(hasPositionOutput, "replay preview futureOutputs contains PositionListResponse");
    expectTrue(hasCashOutput, "replay preview futureOutputs contains CashSummaryDto");

    const auto replayErrors = replayPayload.value("errors").toArray();
    const bool hasUnavailableError =
        std::any_of(replayErrors.begin(), replayErrors.end(), [](const auto& value) {
            return value.toObject().value("code").toString() == "REPLAY_NOT_AVAILABLE";
        });
    expectTrue(hasUnavailableError, "replay preview errors contains REPLAY_NOT_AVAILABLE");

    const auto replayWarnings = replayPayload.value("warnings").toArray();
    const bool hasNotImplementedWarning =
        std::any_of(replayWarnings.begin(), replayWarnings.end(), [](const auto& value) {
            return value.toObject().value("code").toString() == "REPLAY_NOT_IMPLEMENTED";
        });
    expectTrue(hasNotImplementedWarning, "replay preview warnings contains REPLAY_NOT_IMPLEMENTED");

    const auto forbiddenWrites = replayPayload.value("forbiddenWrites").toArray();
    const bool forbidsTradeLog =
        std::any_of(forbiddenWrites.begin(), forbiddenWrites.end(), [](const auto& value) {
            return value.toString() == "trade_log";
        });
    const bool forbidsPositionSnapshot =
        std::any_of(forbiddenWrites.begin(), forbiddenWrites.end(), [](const auto& value) {
            return value.toString() == "position_snapshot";
        });
    expectTrue(forbidsTradeLog, "replay preview forbiddenWrites contains trade_log");
    expectTrue(forbidsPositionSnapshot, "replay preview forbiddenWrites contains position_snapshot");

    const auto boundaries = replayPayload.value("boundaries").toArray();
    const bool statesNoReplay =
        std::any_of(boundaries.begin(), boundaries.end(), [](const auto& value) {
            return value.toString().contains("does not replay accounting");
        });
    expectTrue(statesNoReplay, "replay preview boundaries state no replay");

    const auto positionListCountsBefore = protectedTableCounts(connection);
    auto positionList = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionPositionList),
        responses,
        "position.list");
    expectSuccess(positionList, "position.list guard");
    const auto positionPayload = positionList.value("payload").toObject();
    expectReadOnlyFactsEmptyPayload(
        positionPayload,
        "position.list",
        "POSITION_LIST_EMPTY",
        "position.list");
    expectTrue(positionPayload.value("positions").toArray().isEmpty(), "position.list returns empty positions");
    expectProtectedTableCountsUnchanged(connection, positionListCountsBefore, "position.list guard");

    auto positionListInvalidPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionPositionList, "[]"),
        responses,
        "position.list invalid payload");
    expectTrue(
        !positionListInvalidPayload.value("success").toBool(true),
        "position.list invalid payload success=false");
    expectEqual(
        positionListInvalidPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "position.list invalid payload returns E1001");

    auto positionListMalformedPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionPositionList, "{ invalid }"),
        responses,
        "position.list malformed payload");
    expectTrue(
        !positionListMalformedPayload.value("success").toBool(true),
        "position.list malformed payload success=false");
    expectEqual(
        positionListMalformedPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "position.list malformed payload returns E1001");

    const auto cashSummaryCountsBefore = protectedTableCounts(connection);
    auto cashSummary = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionCashSummary),
        responses,
        "cash.summary");
    expectSuccess(cashSummary, "cash.summary guard");
    const auto cashPayload = cashSummary.value("payload").toObject();
    expectReadOnlyFactsEmptyPayload(
        cashPayload,
        "cash.summary",
        "CASH_SUMMARY_EMPTY",
        "cash.summary");
    expectTrue(
        cashPayload.value("accountCashSummaries").toArray().isEmpty(),
        "cash.summary returns empty accountCashSummaries");
    expectProtectedTableCountsUnchanged(connection, cashSummaryCountsBefore, "cash.summary guard");

    auto cashSummaryInvalidPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionCashSummary, "[]"),
        responses,
        "cash.summary invalid payload");
    expectTrue(
        !cashSummaryInvalidPayload.value("success").toBool(true),
        "cash.summary invalid payload success=false");
    expectEqual(
        cashSummaryInvalidPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "cash.summary invalid payload returns E1001");

    auto cashSummaryMalformedPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionCashSummary, "{ invalid }"),
        responses,
        "cash.summary malformed payload");
    expectTrue(
        !cashSummaryMalformedPayload.value("success").toBool(true),
        "cash.summary malformed payload success=false");
    expectEqual(
        cashSummaryMalformedPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "cash.summary malformed payload returns E1001");

    const auto portfolioPnlCountsBefore = protectedTableCounts(connection);
    auto portfolioPnlSummary = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionPortfolioPnlSummary),
        responses,
        "portfolio.pnl.summary");
    expectSuccess(portfolioPnlSummary, "portfolio.pnl.summary guard");
    const auto portfolioPnlPayload = portfolioPnlSummary.value("payload").toObject();
    expectReadOnlyFactsEmptyPayload(
        portfolioPnlPayload,
        "portfolio.pnl.summary",
        "PORTFOLIO_PNL_SUMMARY_EMPTY",
        "portfolio.pnl.summary");
    expectTrue(
        portfolioPnlPayload.value("portfolioPnlSummaries").toArray().isEmpty(),
        "portfolio.pnl.summary returns empty portfolioPnlSummaries");
    expectProtectedTableCountsUnchanged(connection, portfolioPnlCountsBefore, "portfolio.pnl.summary guard");

    auto portfolioPnlInvalidPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionPortfolioPnlSummary, "[]"),
        responses,
        "portfolio.pnl.summary invalid payload");
    expectTrue(
        !portfolioPnlInvalidPayload.value("success").toBool(true),
        "portfolio.pnl.summary invalid payload success=false");
    expectEqual(
        portfolioPnlInvalidPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "portfolio.pnl.summary invalid payload returns E1001");

    auto portfolioPnlMalformedPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionPortfolioPnlSummary, "{ invalid }"),
        responses,
        "portfolio.pnl.summary malformed payload");
    expectTrue(
        !portfolioPnlMalformedPayload.value("success").toBool(true),
        "portfolio.pnl.summary malformed payload success=false");
    expectEqual(
        portfolioPnlMalformedPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "portfolio.pnl.summary malformed payload returns E1001");

    const auto basePositionCountsBefore = protectedTableCounts(connection);
    auto basePositionSummary = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionBasePositionSummary),
        responses,
        "base_position.summary");
    expectSuccess(basePositionSummary, "base_position.summary guard");
    const auto basePositionPayload = basePositionSummary.value("payload").toObject();
    expectReadOnlyFactsEmptyPayload(
        basePositionPayload,
        "base_position.summary",
        "BASE_POSITION_SUMMARY_EMPTY",
        "base_position.summary");
    expectTrue(
        basePositionPayload.value("basePositions").toArray().isEmpty(),
        "base_position.summary returns empty basePositions");
    expectProtectedTableCountsUnchanged(connection, basePositionCountsBefore, "base_position.summary guard");

    auto basePositionInvalidPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionBasePositionSummary, "[]"),
        responses,
        "base_position.summary invalid payload");
    expectTrue(
        !basePositionInvalidPayload.value("success").toBool(true),
        "base_position.summary invalid payload success=false");
    expectEqual(
        basePositionInvalidPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "base_position.summary invalid payload returns E1001");

    auto basePositionMalformedPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionBasePositionSummary, "{ invalid }"),
        responses,
        "base_position.summary malformed payload");
    expectTrue(
        !basePositionMalformedPayload.value("success").toBool(true),
        "base_position.summary malformed payload success=false");
    expectEqual(
        basePositionMalformedPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "base_position.summary malformed payload returns E1001");

    const auto sniperPoolCountsBefore = protectedTableCounts(connection);
    auto sniperPoolSummary = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionSniperPoolSummary),
        responses,
        "sniper_pool.summary");
    expectSuccess(sniperPoolSummary, "sniper_pool.summary guard");
    const auto sniperPoolPayload = sniperPoolSummary.value("payload").toObject();
    expectReadOnlyFactsEmptyPayload(
        sniperPoolPayload,
        "sniper_pool.summary",
        "SNIPER_POOL_SUMMARY_EMPTY",
        "sniper_pool.summary");
    expectTrue(
        sniperPoolPayload.value("tierSummary").toArray().isEmpty(),
        "sniper_pool.summary returns empty tierSummary");
    expectProtectedTableCountsUnchanged(connection, sniperPoolCountsBefore, "sniper_pool.summary guard");

    auto sniperPoolInvalidPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionSniperPoolSummary, "[]"),
        responses,
        "sniper_pool.summary invalid payload");
    expectTrue(
        !sniperPoolInvalidPayload.value("success").toBool(true),
        "sniper_pool.summary invalid payload success=false");
    expectEqual(
        sniperPoolInvalidPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "sniper_pool.summary invalid payload returns E1001");

    auto sniperPoolMalformedPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionSniperPoolSummary, "{ invalid }"),
        responses,
        "sniper_pool.summary malformed payload");
    expectTrue(
        !sniperPoolMalformedPayload.value("success").toBool(true),
        "sniper_pool.summary malformed payload success=false");
    expectEqual(
        sniperPoolMalformedPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "sniper_pool.summary malformed payload returns E1001");

    auto replayPreviewMissingPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionAccountingReplayPreview),
        responses,
        "accounting.replay.preview missing payload");
    expectSuccess(replayPreviewMissingPayload, "accounting.replay.preview missing payload guard");
    expectTrue(
        !replayPreviewMissingPayload.value("payload").toObject().value("implemented").toBool(true),
        "replay preview missing payload implemented=false");

    auto replayPreviewInvalidPayload = sendEnvelope(
        client,
        envelope(etfdt::data_service_api::kActionAccountingReplayPreview, "[]"),
        responses,
        "accounting.replay.preview invalid payload");
    expectTrue(
        !replayPreviewInvalidPayload.value("success").toBool(true),
        "accounting.replay.preview invalid payload success=false");
    expectEqual(
        replayPreviewInvalidPayload.value("errorCode").toString().toStdString(),
        "E1001_INVALID_JSON",
        "accounting.replay.preview invalid payload returns E1001");

    auto unknownAccounting =
        sendEnvelope(client, envelope("accounting.unknown"), responses, "unknown accounting action");
    expectTrue(
        !unknownAccounting.value("success").toBool(true),
        "unknown accounting action success=false");
    expectEqual(
        unknownAccounting.value("errorCode").toString().toStdString(),
        "E1004_INVALID_ACTION",
        "unknown accounting action returns E1004");

    auto unknown = sendEnvelope(client, envelope("data.unknown"), responses, "unknown data action");
    expectTrue(!unknown.value("success").toBool(true), "unknown data action success=false");
    expectEqual(
        unknown.value("errorCode").toString().toStdString(),
        "E1004_INVALID_ACTION",
        "unknown data action returns E1004");

    const bool invalidReceived = sendAndWait(client, "{ invalid }", responses);
    expectTrue(invalidReceived, "invalid JSON response received");
    if (invalidReceived) {
        auto invalid = parseJsonObject(responses.back());
        expectTrue(!invalid.value("success").toBool(true), "invalid JSON success=false");
        const auto code = invalid.value("errorCode").toString().toStdString();
        expectTrue(
            code == "E1001_INVALID_JSON" || code == "E1002_MISSING_REQUIRED_FIELD",
            "invalid JSON returns protocol error");
    }

    expectEqual(countRows(connection, "audit_log"), auditLogBefore, "audit_log does not increase");
    expectEqual(countRows(connection, "trade_log"), 0, "trade_log remains empty");
    expectEqual(countRows(connection, "trade_execution_group"), 0, "trade_execution_group remains empty");
    expectEqual(countRows(connection, "trade_draft"), 0, "trade_draft remains empty");
    expectEqual(countRows(connection, "position_snapshot"), 0, "position_snapshot remains empty");
    expectEqual(countRows(connection, "cash_snapshot"), 0, "cash_snapshot remains empty");
    expectEqual(countRows(connection, "portfolio_summary"), 0, "portfolio_summary remains empty");

    client.disconnect();
    expectTrue(waitUntil([&]() { return !client.isConnected(); }, 1000), "client disconnects");
    host.close();
    expectTrue(!host.isListening(), "host closes");
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

    testReadOnlyActions(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " DataService action test failure(s)" << '\n';
        return 1;
    }

    std::cout << "DataService read-only action tests passed." << '\n';
    return 0;
}
