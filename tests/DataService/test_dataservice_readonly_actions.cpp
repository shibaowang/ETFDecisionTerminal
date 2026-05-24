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

void expectSuccess(const QJsonObject& response, std::string_view label)
{
    expectTrue(response.value("success").toBool(false), std::string(label) + " success=true");
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
