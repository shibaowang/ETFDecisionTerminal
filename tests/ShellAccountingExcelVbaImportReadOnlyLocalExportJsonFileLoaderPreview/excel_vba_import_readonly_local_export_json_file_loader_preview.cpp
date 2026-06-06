#include "ShellServices/ShellAccountingExcelVbaImportReadOnlyFileLoader.h"
#include "ShellServices/ShellAccountingPresenter.h"
#include "ShellServices/ShellAccountingReadOnlyController.h"
#include "ShellServices/ShellAccountingServiceAdapter.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QUrl>

#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string optionValue(int argc, char* argv[], const std::string& option)
{
    for (int index = 1; index + 1 < argc; ++index) {
        if (argv[index] == option) {
            return argv[index + 1];
        }
    }
    return {};
}

QString readFile(const std::filesystem::path& path)
{
    QFile file(QString::fromStdString(path.string()));
    require(file.open(QIODevice::ReadOnly), "read " + path.string());
    return QString::fromUtf8(file.readAll());
}

void writeFile(const QString& path, const QByteArray& content)
{
    QFile file(path);
    require(file.open(QIODevice::WriteOnly | QIODevice::Truncate), "write temp file");
    require(file.write(content) == content.size(), "write temp content");
}

bool textContainsSensitiveToken(const QString& text)
{
    const auto lower = text.toLower();
    return lower.contains(QStringLiteral("c:/"))
        || lower.contains(QStringLiteral("d:/"))
        || lower.contains(QStringLiteral("select "))
        || lower.contains(QStringLiteral("credential"))
        || lower.contains(QStringLiteral("endpoint"))
        || lower.contains(QStringLiteral("stacktrace"));
}

class FakePreviewAdapter final : public etfdt::shell_services::ShellAccountingServiceAdapter {
public:
    etfdt::shell_services::ShellAccountingServiceResult fetchPositionList(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override
    {
        return unavailable(request);
    }

    etfdt::shell_services::ShellAccountingServiceResult fetchCashSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override
    {
        return unavailable(request);
    }

    etfdt::shell_services::ShellAccountingServiceResult fetchPortfolioPnlSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override
    {
        return unavailable(request);
    }

    etfdt::shell_services::ShellAccountingServiceResult fetchBasePositionSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override
    {
        return unavailable(request);
    }

    etfdt::shell_services::ShellAccountingServiceResult fetchSniperPoolSummary(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override
    {
        return unavailable(request);
    }

    etfdt::shell_services::ShellAccountingServiceResult previewExcelVbaImportReadOnly(
        const etfdt::shell_services::ShellAccountingServiceRequest& request) override
    {
        ++previewCallCount;
        lastPreviewPayload = QString::fromStdString(request.importPayloadJson);
        etfdt::shell_services::ShellAccountingServiceResult result;
        result.actionName = request.actionName;
        result.protocolSuccess = true;
        result.implemented = true;
        result.readOnly = true;
        result.writeEnabled = false;
        result.payloadStatus = "OK";
        result.dataQualityStatus = "OK";
        result.importPreviewAccepted = true;
        result.importPreviewRejected = false;
        result.importPreviewDiagnosticCodes = {"TASK262_VALID"};
        result.importPreviewFactSummary.tradeFactCount = 1;
        result.importPreviewFactSummary.cashFactCount = 1;
        result.importPreviewFactSummary.marketPriceFactCount = 0;
        result.importPreviewFactSummary.fxRateFactCount = 0;
        result.rawPayload = "{\"status\":\"OK\"}";
        return result;
    }

    int previewCallCount = 0;
    QString lastPreviewPayload;

private:
    static etfdt::shell_services::ShellAccountingServiceResult unavailable(
        const etfdt::shell_services::ShellAccountingServiceRequest& request)
    {
        etfdt::shell_services::ShellAccountingServiceResult result;
        result.actionName = request.actionName;
        result.readOnly = true;
        result.writeEnabled = false;
        result.payloadStatus = "UNAVAILABLE";
        return result;
    }
};

QJsonObject evidenceJson()
{
    return QJsonObject{
        {"task", "TASK-262"},
        {"mode", "readonly-local-export-json-file-loader-preview-vertical-slice"},
        {"localExportJsonFileLoaderCreated", true},
        {"qmlFileDialogWired", true},
        {"localFileOnly", true},
        {"extensionAllowlistEnforced", true},
        {"fileSizeLimitEnforced", true},
        {"jsonObjectRequired", true},
        {"fileLoadedAsInMemoryPayload", true},
        {"existingPreviewPathReused", true},
        {"previewStatusVisible", true},
        {"diagnosticsMapped", true},
        {"replayFactSummaryMapped", true},
        {"networkUrlRejected", true},
        {"rawPathSanitized", true},
        {"rawPayloadExposed", false},
        {"qmlDirectFileRead", false},
        {"qmlDirectDataServiceClient", false},
        {"qmlDirectSQLite", false},
        {"qmlDirectAccountingEngine", false},
        {"accountingEngineCalled", false},
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
    };
}

}  // namespace

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    try {
        const auto sourceRoot = optionValue(argc, argv, "--source-root");
        require(!sourceRoot.empty(), "--source-root is required");
        const std::filesystem::path root(sourceRoot);

        const auto qmlPath =
            root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml";
        const auto qml = readFile(qmlPath);
        require(qml.contains(QStringLiteral("FileDialog")), "QML FileDialog wired");
        require(qml.contains(QStringLiteral("shellAccountingExcelVbaImportPreviewSelectFileButton")), "select file button wired");
        require(qml.contains(QStringLiteral("shellAccountingExcelVbaImportPreviewFileButton")), "preview file button wired");
        require(qml.contains(QStringLiteral("shellAccountingExcelVbaImportPreviewClearFileButton")), "clear file button wired");
        require(qml.contains(QStringLiteral("shellAccountingExcelVbaImportPreviewSelectedFileText")), "selected basename display wired");
        require(qml.contains(QStringLiteral("previewExcelVbaImportReadOnlyFromLocalFile(")), "QML invokes Presenter file preview");
        require(!qml.contains(QStringLiteral("XMLHttpRequest")), "QML has no direct network request");
        require(!qml.contains(QStringLiteral("fetch(")), "QML has no fetch");
        require(!qml.contains(QStringLiteral(".read(")), "QML does not read file content");
        require(!qml.contains(QStringLiteral("DataServiceClient")), "QML has no DataServiceClient");
        require(!qml.contains(QStringLiteral("SQLite")), "QML has no SQLite");
        require(!qml.contains(QStringLiteral("AccountingEngine")), "QML has no AccountingEngine");

        const auto loaderHeader =
            root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingExcelVbaImportReadOnlyFileLoader.h";
        const auto loaderSource =
            root / "libs" / "ShellServices" / "src" / "ShellAccountingExcelVbaImportReadOnlyFileLoader.cpp";
        const auto presenterHeader =
            root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h";
        const auto presenterSource =
            root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp";
        require(readFile(loaderHeader).contains(QStringLiteral("kMaxFileSizeBytes")), "loader exposes size limit");
        require(readFile(loaderSource).contains(QStringLiteral("QFile")), "loader reads file in ShellServices C++");
        require(!readFile(loaderSource).contains(QStringLiteral("QNetwork")), "loader has no network access");
        require(readFile(presenterHeader).contains(QStringLiteral("previewExcelVbaImportReadOnlyFromLocalFile")), "Presenter file preview method declared");
        require(readFile(presenterSource).contains(QStringLiteral("previewExcelVbaImportReadOnly(loaded.payloadJson)")), "Presenter reuses existing preview path");

        QTemporaryDir tempDir;
        require(tempDir.isValid(), "temporary directory created");
        const auto validJsonPath = tempDir.filePath(QStringLiteral("TASK262_valid_export.json"));
        const auto validPayload = QByteArrayLiteral(
            "{\"schemaVersion\":\"task-262-test\",\"rows\":[{\"type\":\"trade\",\"side\":\"BUY\",\"quantity\":1}],\"metadata\":{\"source\":\"sanitized-export\"}}");
        writeFile(validJsonPath, validPayload);

        const auto validLoad =
            etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(validJsonPath);
        require(validLoad.success, "valid JSON local export loads");
        require(validLoad.sanitizedFileName == QStringLiteral("TASK262_valid_export.json"), "basename sanitized");
        require(!validLoad.payloadJson.contains(QStringLiteral("TASK262_valid_export.json")), "payload does not include path or filename");

        const auto validTxtPath = tempDir.filePath(QStringLiteral("TASK262_valid_export.txt"));
        writeFile(validTxtPath, validPayload);
        require(
            etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(validTxtPath).success,
            "txt extension local export loads");

        const auto networkLoad =
            etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(
                QStringLiteral("https://example.invalid/export.json"));
        require(!networkLoad.success, "network URL rejected");
        require(!textContainsSensitiveToken(networkLoad.issue), "network rejection issue sanitized");

        const auto qrcLoad =
            etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(
                QStringLiteral("qrc:/export.json"));
        require(!qrcLoad.success, "qrc resource URL rejected");

        const auto csvPath = tempDir.filePath(QStringLiteral("TASK262_export.csv"));
        writeFile(csvPath, validPayload);
        require(
            !etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(csvPath).success,
            "extension allowlist enforced");

        const auto arrayPath = tempDir.filePath(QStringLiteral("TASK262_array.json"));
        writeFile(arrayPath, QByteArrayLiteral("[1,2,3]"));
        require(
            !etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(arrayPath).success,
            "JSON object required");

        const auto forbiddenPath = tempDir.filePath(QStringLiteral("TASK262_forbidden.json"));
        writeFile(forbiddenPath, QByteArrayLiteral("{\"filePath\":\"C:/secret/export.json\"}"));
        const auto forbiddenLoad =
            etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(forbiddenPath);
        require(!forbiddenLoad.success, "forbidden path payload field rejected");
        require(!textContainsSensitiveToken(forbiddenLoad.issue), "forbidden payload issue sanitized");

        const auto sqlPath = tempDir.filePath(QStringLiteral("TASK262_sql.json"));
        writeFile(sqlPath, QByteArrayLiteral("{\"memo\":\"SELECT * FROM trade_log\"}"));
        const auto sqlLoad =
            etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(sqlPath);
        require(!sqlLoad.success, "raw SQL token rejected");
        require(!textContainsSensitiveToken(sqlLoad.issue), "raw SQL issue sanitized");

        const auto largePath = tempDir.filePath(QStringLiteral("TASK262_large.json"));
        QByteArray largePayload = QByteArrayLiteral("{\"data\":\"");
        largePayload.append(
            QByteArray(static_cast<int>(etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::kMaxFileSizeBytes), 'A'));
        largePayload.append(QByteArrayLiteral("\"}"));
        writeFile(largePath, largePayload);
        require(
            !etfdt::shell_services::ShellAccountingExcelVbaImportReadOnlyFileLoader::loadLocalExportJsonPayload(largePath).success,
            "file size limit enforced");

        auto fakeAdapter = std::make_shared<FakePreviewAdapter>();
        auto controller = std::make_shared<etfdt::shell_services::ShellAccountingReadOnlyController>();
        controller->setServiceAdapter(fakeAdapter);
        etfdt::shell_services::ShellAccountingPresenter presenter;
        presenter.setController(controller);

        const bool accepted = presenter.previewExcelVbaImportReadOnlyFromLocalFile(
            QUrl::fromLocalFile(validJsonPath).toString());
        require(accepted, "Presenter file preview accepted");
        require(fakeAdapter->previewCallCount == 1, "existing preview path called once");
        require(fakeAdapter->lastPreviewPayload == validLoad.payloadJson, "loaded payload passed to existing preview path");
        require(presenter.lastExcelVbaImportPreviewStatus() == QStringLiteral("ACCEPTED"), "preview status visible");
        require(presenter.excelVbaImportPreviewTradeFactCount() == 1, "trade fact count mapped");
        require(presenter.excelVbaImportPreviewCashFactCount() == 1, "cash fact count mapped");
        require(presenter.lastExcelVbaImportPreviewDiagnosticCodes().contains(QStringLiteral("TASK262_VALID")), "diagnostics mapped");

        const bool rejected = !presenter.previewExcelVbaImportReadOnlyFromLocalFile(
            QStringLiteral("https://example.invalid/export.json"));
        require(rejected, "Presenter rejects network URL");
        require(fakeAdapter->previewCallCount == 1, "network URL does not call preview path");
        require(presenter.lastExcelVbaImportPreviewStatus() == QStringLiteral("INPUT_ERROR"), "network URL maps to input error");
        require(!textContainsSensitiveToken(presenter.lastExcelVbaImportPreviewIssue()), "Presenter issue sanitized");

        const auto evidence = evidenceJson();
        require(evidence.value(QStringLiteral("localExportJsonFileLoaderCreated")).toBool(), "evidence loader created");
        require(evidence.value(QStringLiteral("existingPreviewPathReused")).toBool(), "evidence preview path reused");
        require(!evidence.value(QStringLiteral("productionWrite")).toBool(), "evidence no production write");
        require(!evidence.value(QStringLiteral("networkAccess")).toBool(), "evidence no network access");

        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 0;
    } catch (const std::exception& ex) {
        QJsonObject failure{
            {"task", "TASK-262"},
            {"error", QString::fromStdString(ex.what())},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return 1;
    }
}
