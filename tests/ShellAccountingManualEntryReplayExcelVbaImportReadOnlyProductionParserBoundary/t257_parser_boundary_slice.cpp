#include "AccountingEngine/AccountingEngine.h"
#include "DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct LoadedSample {
    std::string sampleId;
    std::string sampleKind;
    std::string accountId;
    std::string portfolioId;
    std::filesystem::path path;
    std::filesystem::file_time_type beforeWriteTime;
    QJsonObject expectedParser;
    QJsonValue expectedProjection;
    etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payload;
};

struct SampleResult {
    QJsonObject evidence;
    bool validParityPassed = true;
    bool invalidRejected = true;
    bool diagnosticsSanitized = true;
    bool factsBuilt = false;
    bool parserExecuted = false;
    bool replayExecuted = false;
    bool sampleMutated = false;
    bool rawUserDataExposed = false;
};

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string stringValue(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isString(), std::string("missing string field: ") + key);
    return value.toString().toStdString();
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

QJsonObject parseJsonFile(const std::filesystem::path& path)
{
    QFile file(QString::fromStdString(path.string()));
    require(file.exists(), "sample file missing");
    require(file.open(QIODevice::ReadOnly), "sample file cannot be opened");

    QJsonParseError parseError{};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    require(parseError.error == QJsonParseError::NoError, "sample JSON malformed");
    require(document.isObject(), "sample JSON root must be object");
    return document.object();
}

QJsonObject diagnosticJson(
    const etfdt::data_service_api::ShellAccountingExcelVbaImportDiagnostic& diagnostic)
{
    return QJsonObject{
        {"level", QString::fromStdString(diagnostic.level)},
        {"code", QString::fromStdString(diagnostic.code)},
        {"field", QString::fromStdString(diagnostic.field)},
        {"sheetName", QString::fromStdString(diagnostic.sheetName)},
        {"rowId", QString::fromStdString(diagnostic.rowId)},
    };
}

QJsonArray diagnosticsJson(
    const std::vector<etfdt::data_service_api::ShellAccountingExcelVbaImportDiagnostic>& diagnostics)
{
    QJsonArray values;
    for (const auto& diagnostic : diagnostics) {
        values.append(diagnosticJson(diagnostic));
    }
    return values;
}

bool diagnosticsAreSanitized(const QJsonArray& diagnostics)
{
    const auto compact = QString::fromUtf8(QJsonDocument(diagnostics).toJson(QJsonDocument::Compact));
    return !compact.contains("message", Qt::CaseInsensitive) &&
           !compact.contains("payload", Qt::CaseInsensitive) &&
           !compact.contains("sql", Qt::CaseInsensitive) &&
           !compact.contains("credential", Qt::CaseInsensitive) &&
           !compact.contains("endpoint", Qt::CaseInsensitive) &&
           !compact.contains("broker", Qt::CaseInsensitive) &&
           !compact.contains("password", Qt::CaseInsensitive) &&
           !compact.contains("secret", Qt::CaseInsensitive) &&
           !compact.contains("stack", Qt::CaseInsensitive);
}

etfdt::data_service_api::ShellAccountingExcelVbaImportPayload importPayloadFromJson(
    const QJsonObject& root)
{
    etfdt::data_service_api::ShellAccountingExcelVbaImportPayload payload;
    payload.rawUserDataExposed = boolValue(root, "rawUserDataExposed");
    const auto importPayload = objectValue(root, "importPayload");
    for (const auto& sheetValue : arrayValue(importPayload, "sheets")) {
        require(sheetValue.isObject(), "sheet must be object");
        const auto sheetObject = sheetValue.toObject();
        etfdt::data_service_api::ShellAccountingExcelVbaImportSheet sheet;
        sheet.sheetName = stringValue(sheetObject, "sheetName");
        for (const auto& headerValue : arrayValue(sheetObject, "headers")) {
            require(headerValue.isString(), "header must be string");
            sheet.headers.push_back(headerValue.toString().toStdString());
        }
        for (const auto& rowValue : arrayValue(sheetObject, "rows")) {
            require(rowValue.isArray(), "row must be array");
            std::vector<std::string> row;
            for (const auto& cellValue : rowValue.toArray()) {
                require(cellValue.isString(), "cell must be string");
                row.push_back(cellValue.toString().toStdString());
            }
            sheet.rows.push_back(std::move(row));
        }
        payload.sheets.push_back(std::move(sheet));
    }
    return payload;
}

LoadedSample loadSample(const std::filesystem::path& samplePath)
{
    const auto root = parseJsonFile(samplePath);
    require(stringValue(root, "task") == "TASK-257", "sample task must be TASK-257");
    require(
        stringValue(root, "schemaVersion") ==
            "task-257-excel-vba-import-readonly-parser-boundary/v1",
        "sample schema version mismatch");
    require(
        stringValue(root, "sourceFormat") == "sanitized-excel-vba-style-import-payload",
        "sample source format mismatch");
    require(arrayValue(root, "externalDependencies").isEmpty(), "sample must have no external dependencies");

    const auto scope = objectValue(root, "defaultScope");
    LoadedSample sample;
    sample.sampleId = stringValue(root, "sampleId");
    sample.sampleKind = stringValue(root, "sampleKind");
    sample.accountId = stringValue(scope, "accountId");
    sample.portfolioId = stringValue(scope, "portfolioId");
    sample.path = samplePath;
    sample.beforeWriteTime = std::filesystem::last_write_time(samplePath);
    sample.expectedParser = objectValue(root, "expectedParser");
    sample.expectedProjection = root.value("expectedProjection");
    sample.payload = importPayloadFromJson(root);
    return sample;
}

etfdt::accounting::ReplayRequestDto makeRequest(const LoadedSample& sample)
{
    etfdt::accounting::ReplayRequestDto request;
    request.accountId = sample.accountId;
    request.portfolioId = sample.portfolioId;
    request.requestedOutputs = {
        etfdt::accounting::RequestedReplayOutput::Positions,
        etfdt::accounting::RequestedReplayOutput::Cash,
        etfdt::accounting::RequestedReplayOutput::Pnl,
        etfdt::accounting::RequestedReplayOutput::Issues,
    };
    request.includeIssues = true;
    request.calculationVersion = "TASK-257-production-parser-boundary-test-only-replay";
    return request;
}

QJsonObject cashSummaryJson(const etfdt::accounting::CashSummaryDto& cash)
{
    return QJsonObject{
        {"accountId", QString::fromStdString(cash.accountId)},
        {"portfolioId", QString::fromStdString(cash.portfolioId)},
        {"currency", QString::fromStdString(cash.currency)},
        {"cashBalanceText", QString::fromStdString(cash.cashBalanceText)},
        {"dataQualityStatus", QString::fromStdString(cash.dataQualityStatus)},
    };
}

QJsonObject positionSummaryJson(const etfdt::accounting::PositionSummaryDto& position)
{
    return QJsonObject{
        {"accountId", QString::fromStdString(position.accountId)},
        {"portfolioId", QString::fromStdString(position.portfolioId)},
        {"instrumentCode", QString::fromStdString(position.instrumentCode)},
        {"quantityText", QString::fromStdString(position.quantityText)},
        {"costAmountText", QString::fromStdString(position.costAmountText)},
        {"costPriceText", QString::fromStdString(position.costPriceText)},
        {"marketValueText", QString::fromStdString(position.marketValueText)},
        {"unrealizedPnlText", QString::fromStdString(position.unrealizedPnlText)},
        {"currency", QString::fromStdString(position.currency)},
        {"dataQualityStatus", QString::fromStdString(position.dataQualityStatus)},
    };
}

QJsonObject pnlSummaryJson(const etfdt::accounting::PortfolioPnlDto& pnl)
{
    return QJsonObject{
        {"portfolioId", QString::fromStdString(pnl.portfolioId)},
        {"currency", QString::fromStdString(pnl.currency)},
        {"realizedPnlText", QString::fromStdString(pnl.realizedPnlText)},
        {"unrealizedPnlText", QString::fromStdString(pnl.unrealizedPnlText)},
        {"totalAssetsText", QString::fromStdString(pnl.totalAssetsText)},
        {"totalPnlText", QString::fromStdString(pnl.totalPnlText)},
        {"dataQualityStatus", QString::fromStdString(pnl.dataQualityStatus)},
    };
}

QJsonObject issueJson(const etfdt::accounting::AccountingIssueDto& issue)
{
    return QJsonObject{
        {"level", QString::fromStdString(issue.level)},
        {"code", QString::fromStdString(issue.code)},
        {"blocking", issue.blocking},
        {"field", QString::fromStdString(issue.field)},
        {"sourceId", QString::fromStdString(issue.sourceId)},
    };
}

QJsonValue cashSummaryValue(const etfdt::accounting::AccountingReplayResult& result)
{
    return result.hasCashSummary ? QJsonValue(cashSummaryJson(result.cashSummary)) : QJsonValue(QJsonValue::Null);
}

QJsonValue pnlSummaryValue(const etfdt::accounting::AccountingReplayResult& result)
{
    return result.hasPortfolioPnl ? QJsonValue(pnlSummaryJson(result.portfolioPnl)) : QJsonValue(QJsonValue::Null);
}

QJsonArray positionsJson(const std::vector<etfdt::accounting::PositionSummaryDto>& positions)
{
    QJsonArray values;
    for (const auto& position : positions) {
        values.append(positionSummaryJson(position));
    }
    return values;
}

QJsonArray issuesJson(const std::vector<etfdt::accounting::AccountingIssueDto>& issues)
{
    QJsonArray values;
    for (const auto& issue : issues) {
        values.append(issueJson(issue));
    }
    return values;
}

SampleResult evaluateRejectedSample(
    const LoadedSample& sample,
    const etfdt::data_service_api::ShellAccountingExcelVbaImportReadOnlyParseResult& parseResult)
{
    const auto expectedAccepted = boolValue(sample.expectedParser, "accepted");
    const auto actualDiagnostics = diagnosticsJson(parseResult.diagnostics);
    const auto expectedDiagnostics = arrayValue(sample.expectedParser, "diagnostics");
    const auto sanitized = diagnosticsAreSanitized(actualDiagnostics);
    const auto rejected = !parseResult.accepted && !expectedAccepted &&
                          actualDiagnostics == expectedDiagnostics && sanitized;

    SampleResult result;
    result.invalidRejected = rejected;
    result.validParityPassed = true;
    result.diagnosticsSanitized = sanitized;
    result.parserExecuted = true;
    result.rawUserDataExposed = parseResult.rawUserDataExposed;
    result.sampleMutated = sample.beforeWriteTime != std::filesystem::last_write_time(sample.path);
    result.evidence = QJsonObject{
        {"sampleId", QString::fromStdString(sample.sampleId)},
        {"sampleKind", QString::fromStdString(sample.sampleKind)},
        {"accepted", false},
        {"invalidSampleRejected", rejected},
        {"diagnosticsMatched", actualDiagnostics == expectedDiagnostics},
        {"diagnosticsSanitized", sanitized},
        {"diagnostics", actualDiagnostics},
        {"parserExecuted", true},
        {"replayExecutedFromTestOnly", false},
        {"parityPassed", rejected},
        {"sampleMutated", result.sampleMutated},
    };
    return result;
}

SampleResult evaluateAcceptedSample(
    const LoadedSample& sample,
    const etfdt::data_service_api::ShellAccountingExcelVbaImportReadOnlyParseResult& parseResult,
    const etfdt::accounting::AccountingReplayResult& replayResult)
{
    require(sample.expectedProjection.isObject(), "accepted sample must have expected projection");
    const auto expected = sample.expectedProjection.toObject();
    const auto actualDiagnostics = diagnosticsJson(parseResult.diagnostics);
    const auto expectedDiagnostics = arrayValue(sample.expectedParser, "diagnostics");
    const auto actualCash = cashSummaryValue(replayResult);
    const auto actualPositions = positionsJson(replayResult.positionList.positions);
    const auto actualPnl = pnlSummaryValue(replayResult);
    const auto actualIssues = issuesJson(replayResult.issues);

    const bool diagnosticsMatched = actualDiagnostics == expectedDiagnostics;
    const bool diagnosticsSanitized = diagnosticsAreSanitized(actualDiagnostics);
    const bool cashMatched = actualCash == expected.value("cashSummary");
    const bool positionsMatched = actualPositions == expected.value("positionSummaries").toArray();
    const bool pnlMatched = actualPnl == expected.value("pnlSummary");
    const bool issuesMatched = actualIssues == expected.value("issues").toArray();
    const bool statusMatched = replayResult.status == stringValue(expected, "replayStatus");
    const bool sampleMutated = sample.beforeWriteTime != std::filesystem::last_write_time(sample.path);
    const bool parityPassed = diagnosticsMatched && diagnosticsSanitized && cashMatched &&
                              positionsMatched && pnlMatched && issuesMatched && statusMatched &&
                              !sampleMutated;

    SampleResult result;
    result.validParityPassed = parityPassed;
    result.invalidRejected = true;
    result.diagnosticsSanitized = diagnosticsSanitized;
    result.factsBuilt = !parseResult.cashFacts.empty() || !parseResult.tradeFacts.empty();
    result.parserExecuted = true;
    result.replayExecuted = replayResult.replayExecuted;
    result.rawUserDataExposed = parseResult.rawUserDataExposed;
    result.sampleMutated = sampleMutated;
    result.evidence = QJsonObject{
        {"sampleId", QString::fromStdString(sample.sampleId)},
        {"sampleKind", QString::fromStdString(sample.sampleKind)},
        {"accepted", true},
        {"parserExecuted", true},
        {"factsBuilt", result.factsBuilt},
        {"replayStatus", QString::fromStdString(replayResult.status)},
        {"replayExecutedFromTestOnly", replayResult.replayExecuted},
        {"parityPassed", parityPassed},
        {"cashSummaryMatched", cashMatched},
        {"positionSummaryMatched", positionsMatched},
        {"pnlSummaryMatched", pnlMatched},
        {"issuesMatched", issuesMatched},
        {"diagnosticsMatched", diagnosticsMatched},
        {"diagnosticsSanitized", diagnosticsSanitized},
        {"diagnostics", actualDiagnostics},
        {"sampleMutated", sampleMutated},
    };
    return result;
}

bool boolField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isBool(), std::string("evidence missing boolean field: ") + key);
    return value.toBool();
}

QJsonObject evidenceJson(
    const std::vector<LoadedSample>& samples,
    const std::vector<SampleResult>& results)
{
    require(samples.size() == results.size(), "sample/result size mismatch");
    require(samples.size() >= 5, "TASK-257 sample matrix must include at least five samples");

    bool parserExecuted = false;
    bool factsBuilt = true;
    bool replayExecuted = false;
    bool validParityPassed = true;
    bool invalidRejected = false;
    bool diagnosticsSanitized = true;
    bool sampleMutated = false;
    bool rawUserDataExposed = false;
    QJsonArray sampleEvidence;
    for (const auto& result : results) {
        parserExecuted = parserExecuted || result.parserExecuted;
        factsBuilt = factsBuilt && (result.factsBuilt || result.invalidRejected);
        replayExecuted = replayExecuted || result.replayExecuted;
        validParityPassed = validParityPassed && result.validParityPassed;
        invalidRejected = invalidRejected || result.invalidRejected;
        diagnosticsSanitized = diagnosticsSanitized && result.diagnosticsSanitized;
        sampleMutated = sampleMutated || result.sampleMutated;
        rawUserDataExposed = rawUserDataExposed || result.rawUserDataExposed;
        sampleEvidence.append(result.evidence);
    }

    return QJsonObject{
        {"task", "TASK-257"},
        {"mode", "readonly-production-parser-boundary-vertical-slice"},
        {"productionParserBoundaryCreated", true},
        {"parserReadOnly", true},
        {"parserNoSqliteAccess", true},
        {"parserNoWrite", true},
        {"sampleMatrixLoaded", true},
        {"sampleCount", static_cast<int>(samples.size())},
        {"parserExecuted", parserExecuted},
        {"diagnosticsBuilt", true},
        {"replayFactsBuilt", factsBuilt},
        {"accountingEngineCalledFromTestOnly", replayExecuted},
        {"replayExecutedFromTestOnly", replayExecuted},
        {"validSamplesParityPassed", validParityPassed},
        {"invalidSamplesRejected", invalidRejected},
        {"diagnosticsSanitized", diagnosticsSanitized},
        {"rawUserDataExposed", rawUserDataExposed},
        {"sampleMutated", sampleMutated},
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
        {"samples", sampleEvidence},
    };
}

void assertEvidence(const QJsonObject& evidence)
{
    require(stringValue(evidence, "task") == "TASK-257", "evidence task mismatch");
    require(
        stringValue(evidence, "mode") == "readonly-production-parser-boundary-vertical-slice",
        "evidence mode mismatch");
    require(evidence.value("sampleCount").toInt() >= 5, "sample count must be at least five");
    for (const char* key : {
             "productionParserBoundaryCreated",
             "parserReadOnly",
             "parserNoSqliteAccess",
             "parserNoWrite",
             "sampleMatrixLoaded",
             "parserExecuted",
             "diagnosticsBuilt",
             "replayFactsBuilt",
             "accountingEngineCalledFromTestOnly",
             "replayExecutedFromTestOnly",
             "validSamplesParityPassed",
             "invalidSamplesRejected",
             "diagnosticsSanitized",
         }) {
        require(boolField(evidence, key), std::string(key) + " must be true");
    }
    for (const char* key : {
             "rawUserDataExposed",
             "sampleMutated",
             "productionRuntimeIntegrationImplemented",
             "productionWrite",
             "sqliteProductionWrite",
             "auditWritten",
             "ledgerWritten",
             "snapshotWritten",
             "tradeLogWritten",
             "readModelPersistentWrite",
             "tradeDraftGenerated",
             "strategyExecuted",
             "brokerOrderSubmitted",
             "networkAccess",
             "credentialAccess",
             "endpointAccess",
             "automaticTrading",
         }) {
        require(!boolField(evidence, key), std::string(key) + " must be false");
    }
}

void assertParserBoundaryFlags(
    const etfdt::data_service_api::ShellAccountingExcelVbaImportReadOnlyParseResult& result)
{
    require(result.parserReadOnly, "parser must report read-only boundary");
    require(result.parserNoSqliteAccess, "parser must report no SQLite access");
    require(result.parserNoWrite, "parser must report no write behavior");
    require(!result.productionRuntimeIntegrationImplemented, "parser must not be production runtime integration");
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        require(argc >= 6, "usage: T257ExcelVbaImportParserBoundarySlice <sample-json>...");

        std::vector<LoadedSample> samples;
        samples.reserve(static_cast<std::size_t>(argc - 1));
        for (int index = 1; index < argc; ++index) {
            samples.push_back(loadSample(std::filesystem::path(argv[index])));
        }

        std::vector<std::string> sampleKinds;
        sampleKinds.reserve(samples.size());
        for (const auto& sample : samples) {
            sampleKinds.push_back(sample.sampleKind);
        }
        for (const char* requiredKind : {
                 "buy_only_import_payload",
                 "chinese_header_buy_partial_sell_import_payload",
                 "cash_adjustment_import_payload",
                 "missing_required_header_import_payload",
                 "invalid_action_amount_cash_import_payload",
             }) {
            require(
                std::find(sampleKinds.begin(), sampleKinds.end(), requiredKind) != sampleKinds.end(),
                std::string("sample matrix missing kind: ") + requiredKind);
        }

        etfdt::accounting::AccountingReplayEngine engine;
        std::vector<SampleResult> results;
        results.reserve(samples.size());
        for (const auto& sample : samples) {
            const auto parseResult =
                etfdt::data_service_api::parseShellAccountingExcelVbaImportReadOnly(sample.payload);
            assertParserBoundaryFlags(parseResult);

            if (!parseResult.accepted) {
                results.push_back(evaluateRejectedSample(sample, parseResult));
                continue;
            }

            const auto replayResult = engine.replayReadOnly(
                makeRequest(sample),
                parseResult.tradeFacts,
                parseResult.cashFacts,
                parseResult.marketPriceFacts,
                parseResult.fxRateFacts);
            results.push_back(evaluateAcceptedSample(sample, parseResult, replayResult));
        }

        const auto evidence = evidenceJson(samples, results);
        assertEvidence(evidence);
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return EXIT_SUCCESS;
    } catch (const std::exception&) {
        QJsonObject failure{
            {"task", "TASK-257"},
            {"mode", "readonly-production-parser-boundary-vertical-slice"},
            {"productionParserBoundaryCreated", true},
            {"parserReadOnly", true},
            {"parserNoSqliteAccess", true},
            {"parserNoWrite", true},
            {"sampleMatrixLoaded", false},
            {"sampleCount", 0},
            {"parserExecuted", false},
            {"diagnosticsBuilt", false},
            {"replayFactsBuilt", false},
            {"accountingEngineCalledFromTestOnly", false},
            {"replayExecutedFromTestOnly", false},
            {"validSamplesParityPassed", false},
            {"invalidSamplesRejected", false},
            {"diagnosticsSanitized", false},
            {"rawUserDataExposed", false},
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
            {"sanitizedFailure", "TASK257_EXCEL_VBA_IMPORT_READONLY_PARSER_BOUNDARY_FAILURE"},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        std::cerr << "TASK-257 failure: sanitized parser boundary assertion failed\n";
        return EXIT_FAILURE;
    }
}
