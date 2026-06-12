#include "DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace {

namespace fs = std::filesystem;

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string readFile(const fs::path& path)
{
    QFile file(QString::fromStdString(path.string()));
    require(file.exists(), "missing file: " + path.string());
    require(file.open(QIODevice::ReadOnly), "cannot read file: " + path.string());
    return QString::fromUtf8(file.readAll()).toStdString();
}

void requireContains(const std::string& text, std::string_view token, std::string_view context)
{
    require(text.find(std::string(token)) != std::string::npos,
            std::string(context) + " missing `" + std::string(token) + "`");
}

void requireNotContains(
    const std::string& text,
    std::string_view token,
    std::string_view context)
{
    require(text.find(std::string(token)) == std::string::npos,
            std::string(context) + " unexpectedly contains `" + std::string(token) + "`");
}

std::string lower(std::string value)
{
    for (auto& ch : value) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = static_cast<char>(ch - 'A' + 'a');
        }
    }
    return value;
}

QJsonObject parseJsonObject(const fs::path& path)
{
    QFile file(QString::fromStdString(path.string()));
    require(file.exists(), "sample export exists");
    require(file.open(QIODevice::ReadOnly), "sample export readable");

    QJsonParseError parseError {};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    require(parseError.error == QJsonParseError::NoError, "sample export JSON parses");
    require(document.isObject(), "sample export root is object");
    return document.object();
}

std::string stringField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isString(), std::string("missing string field: ") + key);
    return value.toString().toStdString();
}

QJsonArray arrayField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isArray(), std::string("missing array field: ") + key);
    return value.toArray();
}

etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payloadFromSample(
    const QJsonObject& root)
{
    etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payload;
    payload.rawUserDataExposed = false;
    for (const auto& sheetValue : arrayField(root, "sheets")) {
        require(sheetValue.isObject(), "sheet is object");
        const auto sheetObject = sheetValue.toObject();
        etfdt::data_service_api::ShellAccountingExcelVbaImportSheet sheet;
        sheet.sheetName = stringField(sheetObject, "sheetName");
        require(stringField(sheetObject, "name") == sheet.sheetName, "sheet name compatibility");
        for (const auto& headerValue : arrayField(sheetObject, "headers")) {
            require(headerValue.isString(), "header is string");
            sheet.headers.push_back(headerValue.toString().toStdString());
        }
        for (const auto& rowValue : arrayField(sheetObject, "rows")) {
            require(rowValue.isArray(), "row is array");
            std::vector<std::string> row;
            for (const auto& cellValue : rowValue.toArray()) {
                require(cellValue.isString(), "cell is string");
                row.push_back(cellValue.toString().toStdString());
            }
            sheet.rows.push_back(std::move(row));
        }
        payload.sheets.push_back(std::move(sheet));
    }
    return payload;
}

void assertSampleSanitized(const std::string& sampleJson)
{
    const auto sampleLower = lower(sampleJson);
    for (const std::string token : {
             "c:/",
             "d:/",
             "\\users\\",
             "filePath",
             "filename",
             "select ",
             "insert ",
             "update ",
             "delete ",
             "credential",
             "endpoint",
             "password",
             "secret",
             "broker payload",
             "real order",
             "stack trace",
             "traceback",
         }) {
        require(sampleLower.find(lower(token)) == std::string::npos,
                "sample export sanitized token absent: " + token);
    }
}

void assertQmlMvpPanel(const std::string& qml)
{
    for (const std::string token : {
             "shellAccountingExcelVbaImportPreviewPanel",
             "shellAccountingExcelVbaImportMvpStepsPanel",
             "shellAccountingExcelVbaImportMvpStepsText",
             "shellAccountingExcelVbaImportMvpSupportText",
             "shellAccountingExcelVbaImportReleaseReadinessText",
             "shellAccountingExcelVbaImportPreviewPayloadInput",
             "shellAccountingExcelVbaImportPreviewButton",
             "shellAccountingExcelVbaImportPreviewSelectFileButton",
             "shellAccountingExcelVbaImportPreviewFileButton",
             "shellAccountingExcelVbaImportPreviewLoadSampleButton",
             "shellAccountingExcelVbaImportPreviewStatusText",
             "shellAccountingExcelVbaImportPreviewDiagnosticCodesText",
             "shellAccountingExcelVbaImportPersistButton",
             "shellAccountingExcelVbaImportPersistTradeLogRowsText",
             "shellAccountingExcelVbaImportPersistCashAdjustmentRowsText",
             "shellAccountingExcelVbaImportPersistStateGuideText",
             "shellAccountingExcelVbaImportPostPersistRefreshStatusText",
         }) {
        requireContains(qml, token, "ShellAccountingReadOnlyPage.qml");
    }

    for (const std::string token : {
             "ACCEPTED",
             "REJECTED",
             "PERSISTED",
             "DUPLICATE",
             "CONFLICT",
             "\xE4\xB8\x8D\xE6\x94\xAF\xE6\x8C\x81\xE7\x9B\xB4\xE6\x8E\xA5 .xlsx \xE5\xAF\xBC\xE5\x85\xA5",
             "schemaVersion=excel-vba-export/v1",
             "source=sanitized-excel-vba-export",
             "sheetName",
             "previewExcelVbaImportReadOnly(",
             "previewExcelVbaImportReadOnlyFromLocalFile(",
             "persistAcceptedExcelVbaImportPreview()",
         }) {
        requireContains(qml, token, "ShellAccountingReadOnlyPage.qml");
    }

    for (const std::string token : {
             "DataServiceClient",
             "DataServiceApi",
             "SQLite",
             "AccountingEngine",
             "XMLHttpRequest",
             "fetch(",
             "credential",
             "endpoint",
             "automaticTrading",
         }) {
        requireNotContains(qml, token, "ShellAccountingReadOnlyPage.qml");
    }
}

void assertDocsIndexed(const fs::path& root)
{
    const auto readme = readFile(root / "README.md");
    const auto docsIndex = readFile(root / "docs" / "README.md");
    const auto prompt = readFile(root / "docs" / "12_codex_prompt_template.md");
    const auto release = readFile(root / "docs" / "340_excel_vba_import_mvp_release_readiness.md");
    const auto userGuide = readFile(root / "docs" / "341_excel_vba_import_mvp_user_guide.md");
    const auto format = readFile(root / "docs" / "342_excel_vba_import_mvp_sample_export_format.md");
    const auto runbook = readFile(root / "docs" / "343_excel_vba_import_mvp_local_runbook.md");
    const auto limits = readFile(root / "docs" / "344_excel_vba_import_mvp_known_limits.md");
    const auto testPlan = readFile(root / "docs" / "345_excel_vba_import_mvp_release_readiness_test_plan.md");
    const auto testsCmake = readFile(root / "tests" / "CMakeLists.txt");

    for (const auto& text : {readme, docsIndex, prompt, release, userGuide, format, runbook, limits, testPlan}) {
        requireContains(text, "Excel/VBA", "release docs");
        requireContains(text, "sanitized", "release docs");
        requireContains(text, "JSON/TXT", "release docs");
    }

    for (const std::string token : {
             "340_excel_vba_import_mvp_release_readiness.md",
             "341_excel_vba_import_mvp_user_guide.md",
             "342_excel_vba_import_mvp_sample_export_format.md",
             "343_excel_vba_import_mvp_local_runbook.md",
             "344_excel_vba_import_mvp_known_limits.md",
             "345_excel_vba_import_mvp_release_readiness_test_plan.md",
             "TASK272_sanitized_excel_vba_import_mvp_sample.json",
         }) {
        requireContains(docsIndex, token, "docs/README.md");
    }

    requireContains(readme, "shell_accounting_excel_vba_import_mvp_release_readiness", "README.md");
    requireContains(testsCmake, "ShellAccountingExcelVbaImportMvpReleaseReadiness", "tests/CMakeLists.txt");
    requireContains(testsCmake, "ShellAccountingExcelVbaImportMvpLocalServiceE2eAcceptance", "tests/CMakeLists.txt");
    requireContains(runbook, "shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance", "runbook");
    requireContains(limits, "Direct `.xlsx` import is not supported", "known limits");
    requireContains(release, "Broker, network, credentials, endpoints", "release checklist");
    requireContains(release, "automatic", "release checklist");
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        fs::path root;
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--source-root" && i + 1 < argc) {
                root = argv[++i];
            }
        }
        require(!root.empty(), "--source-root is required");

        const auto samplePath =
            root / "samples" / "excel_vba_import" / "TASK272_sanitized_excel_vba_import_mvp_sample.json";
        const auto sampleJson = readFile(samplePath);
        assertSampleSanitized(sampleJson);

        const auto sample = parseJsonObject(samplePath);
        require(stringField(sample, "schemaVersion") == "excel-vba-export/v1", "sample schemaVersion");
        require(stringField(sample, "source") == "sanitized-excel-vba-export", "sample source");
        auto payload = payloadFromSample(sample);
        auto parseResult = etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(payload);
        require(parseResult.accepted, "sample preview accepted");
        require(parseResult.parserReadOnly, "parser reports read-only");
        require(parseResult.parserNoSqliteAccess, "parser reports no SQLite access");
        require(parseResult.parserNoWrite, "parser reports no write");
        require(!parseResult.rawUserDataExposed, "parser reports no raw user data exposure");
        require(parseResult.tradeFacts.size() == 1, "sample trade fact count");
        require(parseResult.cashFacts.size() == 1, "sample cash fact count");
        require(parseResult.marketPriceFacts.empty(), "sample market facts absent");
        require(parseResult.fxRateFacts.empty(), "sample fx facts absent");

        const auto qml = readFile(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml");
        assertQmlMvpPanel(qml);
        assertDocsIndexed(root);

        const QJsonObject evidence {
            {"task", "EPIC-272"},
            {"excelVbaImportMvpReleaseReadinessCreated", true},
            {"userGuideCreated", true},
            {"sampleExportCreated", true},
            {"sampleExportSanitized", true},
            {"samplePreviewAccepted", true},
            {"localRunbookCreated", true},
            {"knownLimitsDocumented", true},
            {"releaseChecklistCreated", true},
            {"smokeTestCreated", true},
            {"qmlMvpUxPolished", true},
            {"directXlsxImportSupported", false},
            {"productionDbTouched", false},
            {"brokerOrderSubmitted", false},
            {"networkAccess", false},
            {"credentialAccess", false},
            {"endpointAccess", false},
            {"automaticTrading", false},
        };
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 0;
    } catch (const std::exception& ex) {
        const QJsonObject evidence {
            {"task", "EPIC-272"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"productionDbTouched", false},
            {"brokerOrderSubmitted", false},
            {"networkAccess", false},
            {"credentialAccess", false},
            {"endpointAccess", false},
            {"automaticTrading", false},
        };
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 1;
    }
}
