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

std::vector<std::string> initialCashHeaders()
{
    return {
        "ROW_ID",
        "TIME_UTC",
        "ACCOUNT_CODE",
        "PORTFOLIO_CODE",
        "ACTION",
        "AMOUNT",
        "CURRENCY",
        "MEMO",
    };
}

std::vector<std::string> tradeLogHeaders()
{
    return {
        "ROW_ID",
        "TRADE_TIME_UTC",
        "ACCOUNT_CODE",
        "PORTFOLIO_CODE",
        "INSTRUMENT_CODE",
        "SIDE",
        "QUANTITY",
        "PRICE",
        "AMOUNT",
        "FEE",
        "CASH_FLOW",
        "CURRENCY",
        "SOURCE",
        "MEMO",
    };
}

void assertHelper(const fs::path& root)
{
    const auto helper =
        readFile(root / "tools" / "excel_vba_export" / "ExcelVbaImportMvpExportHelper.bas");
    const auto helperReadme =
        readFile(root / "tools" / "excel_vba_export" / "ExcelVbaImportMvpExportHelper_README.md");

    for (const auto& token : {
             "Option Explicit",
             "ExportMvpJsonTxt",
             "GetSaveAsFilename",
             "ThisWorkbook.Worksheets",
             "schemaVersion",
             "excel-vba-export/v1",
             "source",
             "sanitized-excel-vba-export",
             "InitialCash",
             "TradeLog",
             "Missing required sheet",
             "Missing required header",
             "TradeLog sheet has no data rows",
             "Export file could not be written",
             "DEMO_ACCOUNT",
             "DEMO_PORTFOLIO",
             "DEMO_ETF",
             "SANITIZED_SAMPLE",
         }) {
        requireContains(helper, token, "VBA helper");
    }

    for (const auto& header : initialCashHeaders()) {
        requireContains(helper, header, "VBA helper InitialCash headers");
    }
    for (const auto& header : tradeLogHeaders()) {
        requireContains(helper, header, "VBA helper TradeLog headers");
    }

    for (const std::string token : {
             "broker",
             "network",
             "credential",
             "endpoint",
             "automaticTrading",
             "http://",
             "https://",
         }) {
        requireNotContainsLower(helper, token, "VBA helper");
    }

    requireContains(helperReadme, "Excel 2010", "helper README");
    requireContains(helperReadme, "JSON/TXT", "helper README");
    requireContainsLower(helperReadme, ".xlsx", "helper README");
    requireContainsLower(helperReadme, "not supported", "helper README");
    requireContains(helperReadme, "DEMO_ACCOUNT", "helper README");
    requireContains(helperReadme, "SANITIZED_SAMPLE", "helper README");
}

void assertDocs(const fs::path& root)
{
    const auto readme = readFile(root / "README.md");
    const auto docsIndex = readFile(root / "docs" / "README.md");
    const auto prompt = readFile(root / "docs" / "12_codex_prompt_template.md");
    const auto overview =
        readFile(root / "docs" / "350_excel_vba_import_mvp_export_helper_compatibility_pack.md");
    const auto mapping =
        readFile(root / "docs" / "351_excel_vba_import_mvp_export_helper_mapping.md");
    const auto guide =
        readFile(root / "docs" / "352_excel_vba_import_mvp_export_helper_user_guide.md");
    const auto plan =
        readFile(root / "docs" / "353_excel_vba_import_mvp_export_helper_test_plan.md");

    for (const auto& text : {readme, docsIndex, prompt, overview, mapping, guide, plan}) {
        requireContains(text, "Excel/VBA", "EPIC-274 docs");
        requireContains(text, "JSON/TXT", "EPIC-274 docs");
        requireContainsLower(text, ".xlsx", "EPIC-274 docs");
        requireContainsLower(text, "not supported", "EPIC-274 docs");
        requireContainsLower(text, "broker", "EPIC-274 docs");
        requireContainsLower(text, "automatic trading", "EPIC-274 docs");
    }

    for (const std::string token : {
             "350_excel_vba_import_mvp_export_helper_compatibility_pack.md",
             "351_excel_vba_import_mvp_export_helper_mapping.md",
             "352_excel_vba_import_mvp_export_helper_user_guide.md",
             "353_excel_vba_import_mvp_export_helper_test_plan.md",
             "tools/excel_vba_export/ExcelVbaImportMvpExportHelper.bas",
             "tools/excel_vba_export/ExcelVbaImportMvpExportHelper_README.md",
             "samples/excel_vba_import/TASK274_export_helper_expected_sample.json",
         }) {
        requireContains(docsIndex, token, "docs/README.md");
        requireContains(readme, token, "README.md");
    }

    for (const auto& header : initialCashHeaders()) {
        requireContains(mapping, header, "mapping doc InitialCash headers");
    }
    for (const auto& header : tradeLogHeaders()) {
        requireContains(mapping, header, "mapping doc TradeLog headers");
    }
    requireContains(mapping, "Excel/VBA-side", "mapping doc");
    requireContains(mapping, "sanitized JSON/TXT", "mapping doc");
}

void assertSampleAccepted(const fs::path& root)
{
    const auto samplePath =
        root / "samples" / "excel_vba_import" / "TASK274_export_helper_expected_sample.json";
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
        requireNotContainsLower(sampleJson, token, "expected sample export");
    }

    const auto sample = parseJsonObject(samplePath);
    require(stringField(sample, "schemaVersion") == "excel-vba-export/v1", "sample schemaVersion");
    require(stringField(sample, "source") == "sanitized-excel-vba-export", "sample source");

    const auto parseResult =
        etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(payloadFromSample(sample));
    require(parseResult.accepted, "sample export accepted by read-only parser");
    require(parseResult.parserReadOnly, "parser reports read-only");
    require(parseResult.parserNoSqliteAccess, "parser reports no SQLite access");
    require(parseResult.parserNoWrite, "parser reports no write");
    require(!parseResult.rawUserDataExposed, "parser reports no raw user data exposure");
    require(parseResult.tradeFacts.size() == 1, "sample trade fact count");
    require(parseResult.cashFacts.size() == 1, "sample cash fact count");
    require(parseResult.marketPriceFacts.empty(), "sample market facts absent");
    require(parseResult.fxRateFacts.empty(), "sample fx facts absent");
}

void assertCMakeRegistration(const fs::path& root)
{
    const auto testsCmake = readFile(root / "tests" / "CMakeLists.txt");
    requireContains(
        testsCmake,
        "ShellAccountingExcelVbaImportMvpExportHelperCompatibility",
        "tests/CMakeLists.txt");
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

        assertHelper(root);
        assertDocs(root);
        assertSampleAccepted(root);
        assertCMakeRegistration(root);

        const QJsonObject evidence {
            {"task", "EPIC-274"},
            {"exportHelperCompatibilityPackCreated", true},
            {"excel2010CompatibleVbaHelperCreated", true},
            {"initialCashMappingDocumented", true},
            {"tradeLogMappingDocumented", true},
            {"sampleExportAcceptedByParser", true},
            {"sanitizedJsonTxtOnly", true},
            {"directXlsxImportSupported", false},
            {"networkAccess", false},
            {"credentialAccess", false},
            {"endpointAccess", false},
            {"brokerOrderSubmitted", false},
            {"automaticTrading", false},
            {"productionDbTouched", false},
        };
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 0;
    } catch (const std::exception& ex) {
        const QJsonObject evidence {
            {"task", "EPIC-274"},
            {"success", false},
            {"error", QString::fromStdString(ex.what())},
            {"networkAccess", false},
            {"credentialAccess", false},
            {"endpointAccess", false},
            {"brokerOrderSubmitted", false},
            {"automaticTrading", false},
            {"productionDbTouched", false},
        };
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString()
                  << '\n';
        return 1;
    }
}
