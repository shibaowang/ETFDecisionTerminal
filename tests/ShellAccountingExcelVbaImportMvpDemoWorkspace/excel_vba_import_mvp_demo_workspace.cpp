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
#include <vector>

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

std::string lower(std::string value)
{
    for (auto& ch : value) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = static_cast<char>(ch - 'A' + 'a');
        }
    }
    return value;
}

void requireContains(const std::string& text, std::string_view token, std::string_view context)
{
    require(text.find(std::string(token)) != std::string::npos,
            std::string(context) + " missing `" + std::string(token) + "`");
}

void requireContainsLower(const std::string& text, std::string_view token, std::string_view context)
{
    requireContains(lower(text), lower(std::string(token)), context);
}

void requireNotContainsLower(
    const std::string& text,
    std::string_view token,
    std::string_view context)
{
    require(lower(text).find(lower(std::string(token))) == std::string::npos,
            std::string(context) + " unexpectedly contains `" + std::string(token) + "`");
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

void assertSampleAccepted(const fs::path& root)
{
    const auto samplePath =
        root / "samples" / "excel_vba_import" / "TASK272_sanitized_excel_vba_import_mvp_sample.json";
    const auto sampleJson = readFile(samplePath);
    for (const std::string token : {
             "c:/",
             "d:/",
             "\\users\\",
             "filepath",
             "filename",
             "credential",
             "endpoint",
             "password",
             "secret",
             "broker payload",
             "real order",
             "stack trace",
             "traceback",
         }) {
        requireNotContainsLower(sampleJson, token, "sample export");
    }

    const auto sample = parseJsonObject(samplePath);
    require(stringField(sample, "schemaVersion") == "excel-vba-export/v1", "sample schemaVersion");
    require(stringField(sample, "source") == "sanitized-excel-vba-export", "sample source");
    const auto parseResult =
        etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(payloadFromSample(sample));
    require(parseResult.accepted, "sample preview accepted");
    require(parseResult.parserReadOnly, "parser reports read-only");
    require(parseResult.parserNoSqliteAccess, "parser reports no SQLite access");
    require(parseResult.parserNoWrite, "parser reports no write");
    require(!parseResult.rawUserDataExposed, "parser reports no raw user data exposure");
    require(parseResult.tradeFacts.size() == 1, "sample trade fact count");
    require(parseResult.cashFacts.size() == 1, "sample cash fact count");
    require(parseResult.marketPriceFacts.empty(), "sample market facts absent");
    require(parseResult.fxRateFacts.empty(), "sample fx facts absent");
}

void assertDocs(const fs::path& root)
{
    const auto readme = readFile(root / "README.md");
    const auto docsIndex = readFile(root / "docs" / "README.md");
    const auto prompt = readFile(root / "docs" / "12_codex_prompt_template.md");
    const auto demo = readFile(root / "docs" / "346_excel_vba_import_mvp_demo_workspace.md");
    const auto runbook = readFile(root / "docs" / "347_excel_vba_import_mvp_trial_startup_runbook.md");
    const auto checklist = readFile(root / "docs" / "348_excel_vba_import_mvp_manual_acceptance_checklist.md");
    const auto plan = readFile(root / "docs" / "349_excel_vba_import_mvp_demo_workspace_test_plan.md");
    const auto testsCmake = readFile(root / "tests" / "CMakeLists.txt");

    for (const auto& text : {readme, docsIndex, prompt, demo, runbook, checklist, plan}) {
        requireContains(text, "Excel/VBA", "EPIC-273 docs");
        requireContains(text, "JSON/TXT", "EPIC-273 docs");
        requireContainsLower(text, ".xlsx", "EPIC-273 docs");
        requireContainsLower(text, "not supported", "EPIC-273 docs");
        requireContainsLower(text, "broker", "EPIC-273 docs");
        requireContainsLower(text, "automatic trading", "EPIC-273 docs");
    }

    for (const std::string token : {
             "346_excel_vba_import_mvp_demo_workspace.md",
             "347_excel_vba_import_mvp_trial_startup_runbook.md",
             "348_excel_vba_import_mvp_manual_acceptance_checklist.md",
             "349_excel_vba_import_mvp_demo_workspace_test_plan.md",
             "scripts/excel_vba_import_mvp/New-ExcelVbaImportMvpDemoWorkspace.ps1",
         }) {
        requireContains(docsIndex, token, "docs/README.md");
        requireContains(readme, token, "README.md");
    }

    requireContains(testsCmake, "ShellAccountingExcelVbaImportMvpDemoWorkspace", "tests/CMakeLists.txt");
    requireContains(runbook, ".demo/excel_vba_import_mvp/data/excel_vba_import_mvp_demo.sqlite", "runbook");
    requireContains(runbook, "TASK272_sanitized_excel_vba_import_mvp_sample.json", "runbook");
    requireContains(runbook, "--serve-dev-audit", "runbook");
    requireContains(runbook, "ETFDataServiceExcelVbaImportMvpDemo", "runbook");
    requireContains(runbook, "ETFDecisionShell.exe --diagnostics-mock", "runbook");

    for (const std::string token : {
             "Load copied sample JSON",
             "Click Preview",
             "ACCEPTED",
             "persisted row counts",
             "post-write readback refresh",
             "duplicate import",
             "row counts do not grow",
         }) {
        requireContainsLower(checklist, token, "manual checklist");
    }
}

void assertScriptBoundary(const fs::path& root)
{
    const auto script =
        readFile(root / "scripts" / "excel_vba_import_mvp" / "New-ExcelVbaImportMvpDemoWorkspace.ps1");
    for (const std::string token : {
             ".demo\\excel_vba_import_mvp",
             "excel_vba_import_mvp_demo.sqlite",
             "TASK272_sanitized_excel_vba_import_mvp_sample.json",
             "--init-db",
             "--db",
             "productionDbTouched = $false",
             "networkAccess = $false",
             "credentialAccess = $false",
             "endpointAccess = $false",
             "brokerOrderSubmitted = $false",
             "automaticTrading = $false",
             "directXlsxImportSupported = $false",
             "sanitizedJsonTxtOnly = $true",
         }) {
        requireContains(script, token, "demo bootstrap script");
    }

    for (const std::string token : {
             "Invoke-WebRequest",
             "Invoke-RestMethod",
             "Start-BitsTransfer",
             "http://",
             "https://",
             "brokerOrderSubmitted = $true",
             "automaticTrading = $true",
             "data\\ETFDecision.db",
             "data/ETFDecision.db",
         }) {
        requireNotContainsLower(script, token, "demo bootstrap script");
    }
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

        assertSampleAccepted(root);
        assertDocs(root);
        assertScriptBoundary(root);

        const QJsonObject evidence {
            {"task", "EPIC-273"},
            {"demoWorkspaceCreated", true},
            {"trialStartupRunbookCreated", true},
            {"manualAcceptanceChecklistCreated", true},
            {"sampleImportPathDocumented", true},
            {"demoDbPathExplicit", true},
            {"productionDbTouched", false},
            {"networkAccess", false},
            {"credentialAccess", false},
            {"endpointAccess", false},
            {"brokerOrderSubmitted", false},
            {"automaticTrading", false},
            {"directXlsxImportSupported", false},
            {"sanitizedJsonTxtOnly", true},
        };
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 0;
    } catch (const std::exception& ex) {
        const QJsonObject evidence {
            {"task", "EPIC-273"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"productionDbTouched", false},
            {"networkAccess", false},
            {"credentialAccess", false},
            {"endpointAccess", false},
            {"brokerOrderSubmitted", false},
            {"automaticTrading", false},
        };
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 1;
    }
}
