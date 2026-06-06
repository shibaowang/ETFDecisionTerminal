#include "DataAccess/DataAccess.h"
#include "DataServiceApi/DataServiceApi.h"
#include "DataServiceClient/DataServiceClient.h"
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

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

struct PreviewFixture final {
    std::string sampleId;
    std::filesystem::path path;
    std::filesystem::file_time_type beforeWriteTime;
    QJsonObject requestPayload;
    QJsonObject expectedPreview;
};

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::filesystem::path optionValue(int argc, char* argv[], const std::string& option)
{
    for (int index = 1; index + 1 < argc; ++index) {
        if (argv[index] == option) {
            return argv[index + 1];
        }
    }
    return {};
}

std::vector<std::filesystem::path> optionValues(int argc, char* argv[], const std::string& option)
{
    std::vector<std::filesystem::path> values;
    for (int index = 1; index + 1 < argc; ++index) {
        if (argv[index] == option) {
            values.emplace_back(argv[index + 1]);
        }
    }
    return values;
}

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path()
        / ("etfdt_task260_shellservices_preview_" + std::to_string(now));
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

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    require(input.good(), "file exists and can be read: " + path.string());
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

QJsonObject parseJsonFile(const std::filesystem::path& path)
{
    QFile file(QString::fromStdString(path.string()));
    require(file.exists(), "fixture file missing: " + path.string());
    require(file.open(QIODevice::ReadOnly), "fixture file cannot be opened: " + path.string());

    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    require(parseError.error == QJsonParseError::NoError, "fixture JSON malformed: " + path.string());
    require(document.isObject(), "fixture JSON root must be object: " + path.string());
    return document.object();
}

std::string stringValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isString(), std::string("missing string field: ") + key);
    return value.toString().toStdString();
}

int intValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isDouble(), std::string("missing integer field: ") + key);
    return value.toInt();
}

bool boolValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isBool(), std::string("missing bool field: ") + key);
    return value.toBool();
}

QJsonObject objectValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isObject(), std::string("missing object field: ") + key);
    return value.toObject();
}

QJsonArray arrayValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isArray(), std::string("missing array field: ") + key);
    return value.toArray();
}

std::string compactJson(const QJsonObject& object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
}

PreviewFixture loadFixture(const std::filesystem::path& path)
{
    const auto root = parseJsonFile(path);
    require(stringValue(root, "task") == "TASK-260", "fixture task mismatch");
    require(
        stringValue(root, "schemaVersion") ==
            "task-260-excel-vba-import-readonly-preview-shellservices-presenter-contract/v1",
        "fixture schema version mismatch");
    require(
        stringValue(root, "sourceFormat") == "sanitized-excel-vba-style-import-payload",
        "fixture source format mismatch");
    require(!boolValue(root, "rawUserDataExposed"), "fixture raw user data flag false");
    require(arrayValue(root, "externalDependencies").isEmpty(), "fixture has no external dependencies");

    PreviewFixture fixture;
    fixture.sampleId = stringValue(root, "sampleId");
    fixture.path = path;
    fixture.beforeWriteTime = std::filesystem::last_write_time(path);
    fixture.requestPayload = objectValue(root, "requestPayload");
    fixture.expectedPreview = objectValue(root, "expectedPreview");
    return fixture;
}

void openMigratedDatabase(
    const std::filesystem::path& migrationPath,
    const std::filesystem::path& dbPath,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath.string();
    config.migrationFilePath = migrationPath.string();
    config.enableWal = true;
    config.foreignKeys = true;

    auto openResult = connection.open(config);
    require(openResult.hasValue(), "SQLiteConnection open succeeds");
    auto migrationResult =
        etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    require(migrationResult.hasValue(), "initial migration succeeds");
}

int countRows(etfdt::data_access::SQLiteConnection& connection, const std::string& tableName)
{
    auto count = connection.querySingleInt("SELECT COUNT(*) FROM " + tableName + ";");
    require(count.hasValue(), "count query succeeds for " + tableName);
    return count.value();
}

std::vector<std::pair<std::string, int>> protectedCounts(
    etfdt::data_access::SQLiteConnection& connection)
{
    return {
        {"audit_log", countRows(connection, "audit_log")},
        {"trade_log", countRows(connection, "trade_log")},
        {"trade_execution_group", countRows(connection, "trade_execution_group")},
        {"cash_adjustment", countRows(connection, "cash_adjustment")},
        {"cash_snapshot", countRows(connection, "cash_snapshot")},
        {"position_snapshot", countRows(connection, "position_snapshot")},
        {"portfolio_summary", countRows(connection, "portfolio_summary")},
        {"trade_draft", countRows(connection, "trade_draft")},
    };
}

void requireCountsUnchanged(
    etfdt::data_access::SQLiteConnection& connection,
    const std::vector<std::pair<std::string, int>>& before)
{
    for (const auto& [tableName, expected] : before) {
        require(countRows(connection, tableName) == expected, tableName + " row count unchanged");
    }
}

bool textContainsSensitiveToken(const QString& text)
{
    const auto lower = text.toLower();
    return lower.contains("password") || lower.contains("credential")
        || lower.contains("endpoint") || lower.contains("stacktrace")
        || lower.contains("raw sql");
}

void requirePresenterMatchesFixture(
    etfdt::shell_services::ShellAccountingPresenter& presenter,
    const PreviewFixture& fixture)
{
    const auto expected = fixture.expectedPreview;
    const auto expectedSummary = objectValue(expected, "replayFactSummary");
    const bool expectedAccepted = boolValue(expected, "accepted");

    const bool accepted = presenter.previewExcelVbaImportReadOnly(
        QString::fromStdString(compactJson(fixture.requestPayload)));
    require(accepted == expectedAccepted, "presenter preview accepted status mapped");
    require(
        presenter.lastExcelVbaImportPreviewStatus() ==
            (expectedAccepted ? QStringLiteral("ACCEPTED") : QStringLiteral("REJECTED")),
        "presenter preview status mapped");
    require(
        presenter.excelVbaImportPreviewTradeFactCount() ==
            intValue(expectedSummary, "tradeFactCount"),
        "presenter trade fact count mapped");
    require(
        presenter.excelVbaImportPreviewCashFactCount() ==
            intValue(expectedSummary, "cashFactCount"),
        "presenter cash fact count mapped");
    require(
        presenter.excelVbaImportPreviewMarketPriceFactCount() ==
            intValue(expectedSummary, "marketPriceFactCount"),
        "presenter market price fact count mapped");
    require(
        presenter.excelVbaImportPreviewFxRateFactCount() ==
            intValue(expectedSummary, "fxRateFactCount"),
        "presenter FX fact count mapped");
    require(!presenter.excelVbaImportPreviewBusy(), "presenter preview busy cleared");
    require(!textContainsSensitiveToken(presenter.lastExcelVbaImportPreviewIssue()), "issue text sanitized");
    require(!textContainsSensitiveToken(presenter.lastExcelVbaImportPreviewSummary()), "summary text sanitized");

    const auto expectedCodes = arrayValue(expected, "diagnosticCodes");
    for (const auto& value : expectedCodes) {
        require(value.isString(), "expected diagnostic code is string");
        require(
            presenter.lastExcelVbaImportPreviewDiagnosticCodes().contains(value.toString()),
            "presenter diagnostic code mapped");
    }
    require(
        std::filesystem::last_write_time(fixture.path) == fixture.beforeWriteTime,
        "TASK-260 fixture file not mutated");
}

void requireStaticBoundaries(const std::filesystem::path& root)
{
    const auto presenterHeader =
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h";
    const auto presenterSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp";
    const auto controllerHeader =
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h";
    const auto controllerSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp";
    const auto portAdapterSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceClientPortAdapter.cpp";

    for (const auto& path : {presenterHeader, presenterSource, controllerHeader, controllerSource}) {
        const auto text = readFile(path);
        require(text.find("DataServiceClient") == std::string::npos, "Presenter / Controller has no DataServiceClient dependency");
        require(text.find("DataServiceApi") == std::string::npos, "Presenter / Controller has no DataServiceApi dependency");
        require(text.find("DataAccess") == std::string::npos, "Presenter / Controller has no DataAccess dependency");
        require(text.find("AccountingEngine") == std::string::npos, "Presenter / Controller has no AccountingEngine dependency");
        require(text.find("SQLite") == std::string::npos, "Presenter / Controller has no SQLite dependency");
    }

    const auto portText = readFile(portAdapterSource);
    require(
        portText.find("accountingExcelVbaImportReadOnlyPreview") != std::string::npos,
        "PortAdapter calls typed preview client adapter");
    require(
        portText.find("INSERT ") == std::string::npos &&
            portText.find("UPDATE ") == std::string::npos &&
            portText.find("DELETE ") == std::string::npos &&
            portText.find("REPLACE ") == std::string::npos,
        "PortAdapter does not contain write SQL tokens");
}

QJsonObject evidenceJson(
    const std::vector<PreviewFixture>& fixtures,
    bool malformedInputFailsClosed,
    bool filePathInputFailsClosed)
{
    return QJsonObject{
        {"task", "TASK-260"},
        {"mode", "readonly-import-preview-shellservices-presenter-contract-vertical-slice"},
        {"fixtureCount", static_cast<int>(fixtures.size())},
        {"shellServicesPreviewContractCreated", true},
        {"presenterPreviewMethodCreated", true},
        {"controllerPreviewMethodCreated", true},
        {"serviceAdapterPreviewMapped", true},
        {"dataServiceClientPortExtended", true},
        {"typedClientPreviewAdapterCalled", true},
        {"previewViewModelMapped", true},
        {"diagnosticsMapped", true},
        {"replayFactSummaryMapped", true},
        {"malformedInputFailsClosed", malformedInputFailsClosed},
        {"filePathInputFailsClosed", filePathInputFailsClosed},
        {"dataServiceAction", etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview},
        {"accountingEngineCalled", false},
        {"qmlWiringChanged", false},
        {"startupWiringChanged", false},
        {"productionFileLoading", false},
        {"productionRuntimeIntegrationImplemented", false},
        {"productionWrite", false},
        {"sqliteProductionWrite", false},
        {"auditWritten", false},
        {"ledgerWritten", false},
        {"snapshotWritten", false},
        {"tradeLogWritten", false},
        {"readModelPersistentWrite", false},
        {"tradeDraftGenerated", false},
        {"strategyExecuted", false},
        {"brokerOrderSubmitted", false},
        {"networkAccess", false},
        {"credentialAccess", false},
        {"endpointAccess", false},
        {"automaticTrading", false},
        {"rawUserDataExposed", false},
    };
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    try {
        const auto sourceRoot = optionValue(argc, argv, "--source-root");
        const auto migrationPath = optionValue(argc, argv, "--migration");
        const auto fixturePaths = optionValues(argc, argv, "--fixture");
        require(!sourceRoot.empty(), "--source-root <path> is required");
        require(!migrationPath.empty(), "--migration <path> is required");
        require(fixturePaths.size() >= 2U, "at least two fixtures are required");
        requireStaticBoundaries(sourceRoot);

        const auto tempDir = createTempDirectory();
        const auto dbPath = tempDir / "task260_shellservices_preview.db";

        etfdt::data_access::SQLiteConnection connection;
        openMigratedDatabase(migrationPath, dbPath, connection);

        etfdt::service_runtime::ActionDispatcher dispatcher(
            etfdt::protocol::ServiceName::ETFDataService);
        etfdt::service_runtime::registerBuiltinActions(dispatcher);
        etfdt::data_service_api::registerDataServiceReadOnlyActions(dispatcher, connection);
        require(
            dispatcher.hasAction(etfdt::data_service_api::kActionAccountingExcelVbaImportReadOnlyPreview),
            "read-only preview action is registered");

        etfdt::service_host::ActionServiceHost host(dispatcher);
        const std::string socketName = "ETFDecisionTerminalTask260ShellServicesPreview_"
            + std::to_string(QCoreApplication::applicationPid());
        auto listenResult = host.listen(socketName);
        require(listenResult.hasValue(), "ActionServiceHost listen succeeds");

        auto client = std::make_shared<etfdt::data_service_client::DataServiceClient>();
        auto connected = client->connect(socketName, 1000);
        require(connected.hasValue(), "DataServiceClient connects to local service host");
        require(waitUntil([&]() { return host.clientCount() == 1; }, 1000), "service host accepts client");

        auto port = std::make_shared<etfdt::shell_services::ShellAccountingDataServiceClientPortAdapter>(
            client);
        auto serviceAdapter =
            std::make_shared<etfdt::shell_services::ShellAccountingDataServiceAdapter>(port);
        auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
        controller->setServiceAdapter(serviceAdapter);
        etfdt::shell_services::ShellAccountingPresenter presenter;
        presenter.setController(controller);

        const auto countsBefore = protectedCounts(connection);
        std::vector<PreviewFixture> fixtures;
        for (const auto& fixturePath : fixturePaths) {
            auto fixture = loadFixture(fixturePath);
            requirePresenterMatchesFixture(presenter, fixture);
            fixtures.push_back(std::move(fixture));
        }

        const bool malformedInputFailsClosed =
            !presenter.previewExcelVbaImportReadOnly(QStringLiteral("{"));
        require(malformedInputFailsClosed, "malformed JSON input fails closed");
        require(
            presenter.lastExcelVbaImportPreviewStatus() == QStringLiteral("INPUT_ERROR"),
            "malformed input maps to input error");
        const bool filePathInputFailsClosed =
            !presenter.previewExcelVbaImportReadOnly(QStringLiteral("{\"filePath\":\"sanitized.xlsx\"}"));
        require(filePathInputFailsClosed, "file path input fails closed");
        require(
            presenter.lastExcelVbaImportPreviewStatus() == QStringLiteral("INPUT_ERROR"),
            "file path input maps to input error");

        requireCountsUnchanged(connection, countsBefore);
        require(presenter.readOnly(), "presenter remains read-only");
        require(!presenter.writeEnabled(), "presenter remains write disabled");
        require(!presenter.tradeDraftGenerationEnabled(), "no TradeDraft generation");
        require(!presenter.tradeSuggestionEnabled(), "no trade suggestion");
        require(!presenter.strategyExecutionEnabled(), "no strategy execution");
        require(!presenter.brokerSubmissionEnabled(), "no broker submission");

        const auto evidence =
            evidenceJson(fixtures, malformedInputFailsClosed, filePathInputFailsClosed);
        require(boolValue(evidence, "shellServicesPreviewContractCreated"), "evidence contract created");
        require(boolValue(evidence, "presenterPreviewMethodCreated"), "evidence presenter method");
        require(boolValue(evidence, "controllerPreviewMethodCreated"), "evidence controller method");
        require(boolValue(evidence, "typedClientPreviewAdapterCalled"), "evidence typed client call");
        require(!boolValue(evidence, "productionWrite"), "evidence no production write");
        require(!boolValue(evidence, "brokerOrderSubmitted"), "evidence no broker order");
        require(!boolValue(evidence, "networkAccess"), "evidence no network access");

        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';

        client->disconnect();
        host.close();
        connection.close();
        std::filesystem::remove_all(tempDir);
        return 0;
    }
    catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-260"},
            {"mode", "readonly-import-preview-shellservices-presenter-contract-vertical-slice"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"productionWrite", false},
            {"sqliteProductionWrite", false},
            {"brokerOrderSubmitted", false},
            {"networkAccess", false},
            {"automaticTrading", false},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
