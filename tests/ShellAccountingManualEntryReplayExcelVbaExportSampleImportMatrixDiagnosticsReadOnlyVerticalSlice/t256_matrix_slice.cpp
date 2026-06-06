#include "AccountingEngine/AccountingEngine.h"

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

struct SheetRows {
    std::string sheetName;
    std::vector<std::string> headers;
    QJsonArray rows;
    bool found = false;
};

struct ImportDiagnostic {
    std::string level;
    std::string code;
    std::string field;
    std::string sheetName;
    std::string rowId;
};

struct ImportedSample {
    std::string sampleId;
    std::string sampleKind;
    std::string accountId;
    std::string portfolioId;
    std::filesystem::path path;
    std::filesystem::file_time_type beforeWriteTime;
    bool rawUserDataExposed = true;
    bool accepted = false;
    std::vector<ImportDiagnostic> diagnostics;
    std::vector<etfdt::accounting::TradeFactDto> tradeFacts;
    std::vector<etfdt::accounting::CashFactDto> cashFacts;
    std::vector<etfdt::accounting::MarketPriceFactDto> marketPriceFacts;
    std::vector<etfdt::accounting::FxRateFactDto> fxRateFacts;
    QJsonObject expectedImport;
    QJsonValue expectedProjection;
};

struct SampleResult {
    QJsonObject evidence;
    bool validParityPassed = true;
    bool invalidRejected = true;
    bool diagnosticsSanitized = true;
    bool factsBuilt = false;
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

QJsonObject diagnosticJson(const ImportDiagnostic& diagnostic)
{
    return QJsonObject{
        {"level", QString::fromStdString(diagnostic.level)},
        {"code", QString::fromStdString(diagnostic.code)},
        {"field", QString::fromStdString(diagnostic.field)},
        {"sheetName", QString::fromStdString(diagnostic.sheetName)},
        {"rowId", QString::fromStdString(diagnostic.rowId)},
    };
}

QJsonArray diagnosticsJson(const std::vector<ImportDiagnostic>& diagnostics)
{
    QJsonArray values;
    for (const auto& diagnostic : diagnostics) {
        values.append(diagnosticJson(diagnostic));
    }
    return values;
}

void addDiagnostic(std::vector<ImportDiagnostic>& diagnostics,
                   std::string level,
                   std::string code,
                   std::string field,
                   std::string sheetName,
                   std::string rowId = "")
{
    diagnostics.push_back(ImportDiagnostic{
        std::move(level),
        std::move(code),
        std::move(field),
        std::move(sheetName),
        std::move(rowId),
    });
}

bool diagnosticsAreSanitized(const QJsonArray& diagnostics)
{
    const auto compact = QString::fromUtf8(QJsonDocument(diagnostics).toJson(QJsonDocument::Compact));
    return !compact.contains("message", Qt::CaseInsensitive) && !compact.contains("payload", Qt::CaseInsensitive) &&
           !compact.contains("sql", Qt::CaseInsensitive) && !compact.contains("credential", Qt::CaseInsensitive) &&
           !compact.contains("endpoint", Qt::CaseInsensitive) && !compact.contains("broker", Qt::CaseInsensitive) &&
           !compact.contains("password", Qt::CaseInsensitive) && !compact.contains("secret", Qt::CaseInsensitive);
}

void requireSanitizedToken(const std::string& value)
{
    const auto text = QString::fromStdString(value);
    require(!text.contains("credential", Qt::CaseInsensitive), "sample contains credential-like token");
    require(!text.contains("endpoint", Qt::CaseInsensitive), "sample contains endpoint-like token");
    require(!text.contains("password", Qt::CaseInsensitive), "sample contains password-like token");
    require(!text.contains("secret", Qt::CaseInsensitive), "sample contains secret-like token");
    require(!text.contains("account_number", Qt::CaseInsensitive), "sample contains raw account token");
}

SheetRows findSheet(const QJsonObject& exportSample, const char* sheetName)
{
    const auto sheets = arrayValue(exportSample, "sheets");
    for (const auto& value : sheets) {
        require(value.isObject(), "Excel/VBA export sheet entry must be object");
        const auto sheet = value.toObject();
        if (stringValue(sheet, "sheetName") != sheetName) {
            continue;
        }

        SheetRows parsed;
        parsed.sheetName = sheetName;
        parsed.found = true;
        for (const auto& headerValue : arrayValue(sheet, "headers")) {
            require(headerValue.isString(), "Excel/VBA export header must be string");
            parsed.headers.push_back(headerValue.toString().toStdString());
        }
        parsed.rows = arrayValue(sheet, "rows");
        return parsed;
    }
    return SheetRows{sheetName, {}, {}, false};
}

bool hasHeader(const SheetRows& sheet, const char* header)
{
    return std::find(sheet.headers.begin(), sheet.headers.end(), std::string(header)) != sheet.headers.end();
}

void validateHeaders(const SheetRows& sheet,
                     const std::vector<const char*>& requiredHeaders,
                     std::vector<ImportDiagnostic>& diagnostics)
{
    if (!sheet.found) {
        addDiagnostic(diagnostics, "ERROR", "MISSING_REQUIRED_SHEET", sheet.sheetName, sheet.sheetName);
        return;
    }
    for (const char* header : requiredHeaders) {
        if (!hasHeader(sheet, header)) {
            addDiagnostic(diagnostics, "ERROR", "MISSING_REQUIRED_HEADER", header, sheet.sheetName);
        }
    }
}

std::string cellValue(const QJsonArray& row, const SheetRows& sheet, const char* header)
{
    const std::string expectedHeader(header);
    for (std::size_t index = 0; index < sheet.headers.size(); ++index) {
        if (sheet.headers[index] == expectedHeader) {
            require(static_cast<int>(index) < row.size(), "Excel/VBA export row is missing a cell");
            const auto value = row.at(static_cast<int>(index));
            require(value.isString(), "Excel/VBA export cell must be string");
            return value.toString().toStdString();
        }
    }
    throw std::runtime_error("Excel/VBA export sheet missing expected header");
}

std::vector<etfdt::accounting::CashFactDto> importCashFacts(const SheetRows& sheet)
{
    std::vector<etfdt::accounting::CashFactDto> facts;
    facts.reserve(static_cast<std::size_t>(sheet.rows.size()));
    for (const auto& rowValue : sheet.rows) {
        require(rowValue.isArray(), "InitialCash row must be an array");
        const auto row = rowValue.toArray();
        const auto note = cellValue(row, sheet, "MEMO");
        requireSanitizedToken(note);
        facts.push_back(etfdt::accounting::CashFactDto{
            cellValue(row, sheet, "ROW_ID"),
            cellValue(row, sheet, "TIME_UTC"),
            cellValue(row, sheet, "ACCOUNT_CODE"),
            cellValue(row, sheet, "PORTFOLIO_CODE"),
            cellValue(row, sheet, "ACTION"),
            cellValue(row, sheet, "AMOUNT"),
            cellValue(row, sheet, "CURRENCY"),
            note,
        });
    }
    return facts;
}

std::vector<etfdt::accounting::TradeFactDto> importTradeFacts(const SheetRows& sheet)
{
    std::vector<etfdt::accounting::TradeFactDto> facts;
    facts.reserve(static_cast<std::size_t>(sheet.rows.size()));
    for (const auto& rowValue : sheet.rows) {
        require(rowValue.isArray(), "Trades row must be an array");
        const auto row = rowValue.toArray();
        const auto side = cellValue(row, sheet, "SIDE");
        require(side == etfdt::accounting::TradeAction::Buy || side == etfdt::accounting::TradeAction::Sell,
                "trade side must be BUY or SELL");
        const auto source = cellValue(row, sheet, "SOURCE");
        const auto memo = cellValue(row, sheet, "MEMO");
        requireSanitizedToken(source);
        requireSanitizedToken(memo);
        facts.push_back(etfdt::accounting::TradeFactDto{
            cellValue(row, sheet, "ROW_ID"),
            cellValue(row, sheet, "TRADE_TIME_UTC"),
            cellValue(row, sheet, "ACCOUNT_CODE"),
            cellValue(row, sheet, "PORTFOLIO_CODE"),
            cellValue(row, sheet, "INSTRUMENT_CODE"),
            side,
            cellValue(row, sheet, "QUANTITY"),
            cellValue(row, sheet, "PRICE"),
            cellValue(row, sheet, "AMOUNT"),
            cellValue(row, sheet, "FEE"),
            cellValue(row, sheet, "CASH_FLOW"),
            cellValue(row, sheet, "CURRENCY"),
            source,
            memo,
        });
    }
    return facts;
}

std::vector<etfdt::accounting::MarketPriceFactDto> importMarketPriceFacts(const SheetRows& sheet)
{
    std::vector<etfdt::accounting::MarketPriceFactDto> facts;
    facts.reserve(static_cast<std::size_t>(sheet.rows.size()));
    for (const auto& rowValue : sheet.rows) {
        require(rowValue.isArray(), "MarketPrices row must be an array");
        const auto row = rowValue.toArray();
        facts.push_back(etfdt::accounting::MarketPriceFactDto{
            cellValue(row, sheet, "INSTRUMENT_CODE"),
            cellValue(row, sheet, "PRICE_TIME_UTC"),
            cellValue(row, sheet, "PRICE"),
            cellValue(row, sheet, "CURRENCY"),
            cellValue(row, sheet, "SOURCE"),
        });
    }
    return facts;
}

std::vector<etfdt::accounting::FxRateFactDto> importFxRateFacts(const SheetRows& sheet)
{
    std::vector<etfdt::accounting::FxRateFactDto> facts;
    facts.reserve(static_cast<std::size_t>(sheet.rows.size()));
    for (const auto& rowValue : sheet.rows) {
        require(rowValue.isArray(), "FxRates row must be an array");
        const auto row = rowValue.toArray();
        facts.push_back(etfdt::accounting::FxRateFactDto{
            cellValue(row, sheet, "BASE_CURRENCY"),
            cellValue(row, sheet, "QUOTE_CURRENCY"),
            cellValue(row, sheet, "RATE_TIME_UTC"),
            cellValue(row, sheet, "RATE"),
            cellValue(row, sheet, "SOURCE"),
        });
    }
    return facts;
}

ImportedSample importSample(const std::filesystem::path& samplePath)
{
    const auto root = parseJsonFile(samplePath);
    require(stringValue(root, "task") == "TASK-256", "sample task must be TASK-256");
    require(stringValue(root, "schemaVersion") == "task-256-excel-vba-export-sample-matrix/v1",
            "sample schema version mismatch");
    require(stringValue(root, "sourceFormat") == "sanitized-excel-vba-style-json-export",
            "sample source format mismatch");
    require(arrayValue(root, "externalDependencies").isEmpty(), "sample must have no external dependencies");

    const auto scope = objectValue(root, "defaultScope");
    const auto exportSample = objectValue(root, "exportSample");

    ImportedSample imported;
    imported.sampleId = stringValue(root, "sampleId");
    imported.sampleKind = stringValue(root, "sampleKind");
    imported.accountId = stringValue(scope, "accountId");
    imported.portfolioId = stringValue(scope, "portfolioId");
    imported.path = samplePath;
    imported.beforeWriteTime = std::filesystem::last_write_time(samplePath);
    imported.rawUserDataExposed = boolValue(root, "rawUserDataExposed");
    imported.expectedImport = objectValue(root, "expectedImport");
    imported.expectedProjection = root.value("expectedProjection");

    if (imported.rawUserDataExposed) {
        addDiagnostic(imported.diagnostics, "ERROR", "RAW_USER_DATA_EXPOSED", "rawUserDataExposed", "Workbook");
        return imported;
    }

    const auto cashSheet = findSheet(exportSample, "InitialCash");
    const auto tradeSheet = findSheet(exportSample, "Trades");
    const auto priceSheet = findSheet(exportSample, "MarketPrices");
    const auto fxSheet = findSheet(exportSample, "FxRates");

    validateHeaders(cashSheet, {"ROW_ID", "TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "ACTION", "AMOUNT", "CURRENCY", "MEMO"}, imported.diagnostics);
    validateHeaders(tradeSheet, {"ROW_ID", "TRADE_TIME_UTC", "ACCOUNT_CODE", "PORTFOLIO_CODE", "INSTRUMENT_CODE", "SIDE", "QUANTITY", "PRICE", "AMOUNT", "FEE", "CASH_FLOW", "CURRENCY", "SOURCE", "MEMO"}, imported.diagnostics);
    validateHeaders(priceSheet, {"INSTRUMENT_CODE", "PRICE_TIME_UTC", "PRICE", "CURRENCY", "SOURCE"}, imported.diagnostics);
    validateHeaders(fxSheet, {"BASE_CURRENCY", "QUOTE_CURRENCY", "RATE_TIME_UTC", "RATE", "SOURCE"}, imported.diagnostics);

    if (!imported.diagnostics.empty()) {
        return imported;
    }

    imported.cashFacts = importCashFacts(cashSheet);
    imported.tradeFacts = importTradeFacts(tradeSheet);
    imported.marketPriceFacts = importMarketPriceFacts(priceSheet);
    imported.fxRateFacts = importFxRateFacts(fxSheet);
    imported.accepted = true;
    return imported;
}

etfdt::accounting::ReplayRequestDto makeRequest(const ImportedSample& imported)
{
    etfdt::accounting::ReplayRequestDto request;
    request.accountId = imported.accountId;
    request.portfolioId = imported.portfolioId;
    request.requestedOutputs = {
        etfdt::accounting::RequestedReplayOutput::Positions,
        etfdt::accounting::RequestedReplayOutput::Cash,
        etfdt::accounting::RequestedReplayOutput::Pnl,
        etfdt::accounting::RequestedReplayOutput::Issues,
    };
    request.includeIssues = true;
    request.calculationVersion = "TASK-256-test-only-excel-vba-export-sample-import-matrix-diagnostics";
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

QJsonValue actualCashSummaryValue(const etfdt::accounting::AccountingReplayResult& result)
{
    return result.hasCashSummary ? QJsonValue(cashSummaryJson(result.cashSummary)) : QJsonValue(QJsonValue::Null);
}

QJsonValue actualPnlSummaryValue(const etfdt::accounting::AccountingReplayResult& result)
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

SampleResult evaluateInvalidSample(const ImportedSample& imported)
{
    const auto expectedAccepted = boolValue(imported.expectedImport, "accepted");
    const auto actualDiagnostics = diagnosticsJson(imported.diagnostics);
    const auto expectedDiagnostics = arrayValue(imported.expectedImport, "diagnostics");
    const auto diagnosticsSanitized = diagnosticsAreSanitized(actualDiagnostics);
    const auto rejected = !imported.accepted && !expectedAccepted && actualDiagnostics == expectedDiagnostics && diagnosticsSanitized;

    SampleResult result;
    result.validParityPassed = true;
    result.invalidRejected = rejected;
    result.diagnosticsSanitized = diagnosticsSanitized;
    result.rawUserDataExposed = imported.rawUserDataExposed;
    result.sampleMutated = imported.beforeWriteTime != std::filesystem::last_write_time(imported.path);
    result.evidence = QJsonObject{
        {"sampleId", QString::fromStdString(imported.sampleId)},
        {"sampleKind", QString::fromStdString(imported.sampleKind)},
        {"samplePath", QString::fromStdString(imported.path.generic_string())},
        {"accepted", false},
        {"invalidSampleRejected", rejected},
        {"diagnosticsMatched", actualDiagnostics == expectedDiagnostics},
        {"diagnosticsSanitized", diagnosticsSanitized},
        {"diagnostics", actualDiagnostics},
        {"replayExecuted", false},
        {"parityPassed", rejected},
        {"sampleMutated", result.sampleMutated},
    };
    return result;
}

SampleResult evaluateAcceptedSample(const ImportedSample& imported,
                                    const etfdt::accounting::AccountingReplayResult& replayResult)
{
    require(imported.expectedProjection.isObject(), "accepted sample must have expected projection");
    const auto expected = imported.expectedProjection.toObject();
    const auto actualCash = actualCashSummaryValue(replayResult);
    const auto actualPositions = positionsJson(replayResult.positionList.positions);
    const auto actualPnl = actualPnlSummaryValue(replayResult);
    const auto actualIssues = issuesJson(replayResult.issues);
    const auto actualDiagnostics = diagnosticsJson(imported.diagnostics);
    const auto expectedDiagnostics = arrayValue(imported.expectedImport, "diagnostics");
    const auto diagnosticsSanitized = diagnosticsAreSanitized(actualDiagnostics);

    const bool cashMatched = actualCash == expected.value("cashSummary");
    const bool positionsMatched = actualPositions == expected.value("positionSummaries").toArray();
    const bool pnlMatched = actualPnl == expected.value("pnlSummary");
    const bool issuesMatched = actualIssues == expected.value("issues").toArray();
    const bool statusMatched = replayResult.status == stringValue(expected, "replayStatus");
    const bool diagnosticsMatched = actualDiagnostics == expectedDiagnostics;
    const bool sampleMutated = imported.beforeWriteTime != std::filesystem::last_write_time(imported.path);
    const bool parityPassed =
        cashMatched && positionsMatched && pnlMatched && issuesMatched && statusMatched && diagnosticsMatched &&
        diagnosticsSanitized && !sampleMutated;

    SampleResult result;
    result.validParityPassed = parityPassed;
    result.invalidRejected = true;
    result.diagnosticsSanitized = diagnosticsSanitized;
    result.factsBuilt = !imported.cashFacts.empty() || !imported.tradeFacts.empty();
    result.replayExecuted = replayResult.replayExecuted;
    result.sampleMutated = sampleMutated;
    result.rawUserDataExposed = imported.rawUserDataExposed;
    result.evidence = QJsonObject{
        {"sampleId", QString::fromStdString(imported.sampleId)},
        {"sampleKind", QString::fromStdString(imported.sampleKind)},
        {"samplePath", QString::fromStdString(imported.path.generic_string())},
        {"accepted", true},
        {"replayStatus", QString::fromStdString(replayResult.status)},
        {"replayExecuted", replayResult.replayExecuted},
        {"factsBuilt", result.factsBuilt},
        {"parityPassed", parityPassed},
        {"cashSummaryMatched", cashMatched},
        {"positionSummaryMatched", positionsMatched},
        {"pnlSummaryMatched", pnlMatched},
        {"issuesMatched", issuesMatched},
        {"diagnosticsMatched", diagnosticsMatched},
        {"diagnosticsSanitized", diagnosticsSanitized},
        {"diagnostics", actualDiagnostics},
        {"sampleMutated", sampleMutated},
        {"actualProjection",
            QJsonObject{
                {"cashSummary", actualCash},
                {"positionSummaries", actualPositions},
                {"pnlSummary", actualPnl},
                {"issues", actualIssues},
            }},
    };
    return result;
}

QJsonObject matrixEvidence(const std::vector<ImportedSample>& samples,
                           const std::vector<SampleResult>& sampleResults)
{
    require(samples.size() == sampleResults.size(), "sample and result sizes must match");
    require(samples.size() >= 5, "sample matrix must include at least five samples");

    bool validSamplesParityPassed = true;
    bool invalidSamplesRejected = false;
    bool diagnosticsSanitized = true;
    bool factsBuilt = true;
    bool replayExecuted = false;
    bool sampleMutated = false;
    bool rawUserDataExposed = false;
    QJsonArray sampleEvidence;
    for (const auto& result : sampleResults) {
        validSamplesParityPassed = validSamplesParityPassed && result.validParityPassed;
        invalidSamplesRejected = invalidSamplesRejected || result.invalidRejected;
        diagnosticsSanitized = diagnosticsSanitized && result.diagnosticsSanitized;
        factsBuilt = factsBuilt && (result.factsBuilt || result.invalidRejected);
        replayExecuted = replayExecuted || result.replayExecuted;
        sampleMutated = sampleMutated || result.sampleMutated;
        rawUserDataExposed = rawUserDataExposed || result.rawUserDataExposed;
        sampleEvidence.append(result.evidence);
    }

    return QJsonObject{
        {"task", "TASK-256"},
        {"mode", "test-only-excel-vba-export-sample-import-matrix-diagnostics-readonly-vertical-slice"},
        {"sampleMatrixLoaded", true},
        {"sampleCount", static_cast<int>(samples.size())},
        {"importerExecuted", true},
        {"diagnosticsBuilt", true},
        {"replayFactsBuilt", factsBuilt},
        {"replayExecuted", replayExecuted},
        {"accountingEngineCalled", replayExecuted},
        {"validSamplesParityPassed", validSamplesParityPassed},
        {"invalidSamplesRejected", invalidSamplesRejected},
        {"diagnosticsSanitized", diagnosticsSanitized},
        {"rawUserDataExposed", rawUserDataExposed},
        {"sampleMutated", sampleMutated},
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

bool boolField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isBool(), std::string("evidence missing boolean field: ") + key);
    return value.toBool();
}

void assertEvidence(const QJsonObject& evidence)
{
    require(stringValue(evidence, "task") == "TASK-256", "evidence task mismatch");
    require(stringValue(evidence, "mode") ==
                "test-only-excel-vba-export-sample-import-matrix-diagnostics-readonly-vertical-slice",
            "evidence mode mismatch");
    require(evidence.value("sampleCount").toInt() >= 5, "sample count must be at least five");
    for (const char* key : {
             "sampleMatrixLoaded",
             "importerExecuted",
             "diagnosticsBuilt",
             "replayFactsBuilt",
             "replayExecuted",
             "accountingEngineCalled",
             "validSamplesParityPassed",
             "invalidSamplesRejected",
             "diagnosticsSanitized",
         }) {
        require(boolField(evidence, key), std::string(key) + " must be true");
    }
    for (const char* key : {
             "rawUserDataExposed",
             "sampleMutated",
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

} // namespace

int main(int argc, char** argv)
{
    try {
        require(argc >= 6, "usage: T256ExcelVbaExportSampleImportMatrixDiagnosticsSlice <sample-json>...");

        std::vector<ImportedSample> samples;
        samples.reserve(static_cast<std::size_t>(argc - 1));
        for (int index = 1; index < argc; ++index) {
            samples.push_back(importSample(std::filesystem::path(argv[index])));
        }

        std::vector<std::string> sampleKinds;
        sampleKinds.reserve(samples.size());
        for (const auto& sample : samples) {
            sampleKinds.push_back(sample.sampleKind);
        }
        for (const char* requiredKind :
             {"buy_only_export_sample", "buy_partial_sell_export_sample", "cash_adjustment_export_sample",
              "missing_required_header_sample", "unsupported_or_issue_sample"}) {
            require(std::find(sampleKinds.begin(), sampleKinds.end(), requiredKind) != sampleKinds.end(),
                    std::string("sample matrix missing kind: ") + requiredKind);
        }

        etfdt::accounting::AccountingReplayEngine engine;
        std::vector<SampleResult> sampleResults;
        sampleResults.reserve(samples.size());
        for (const auto& sample : samples) {
            if (!sample.accepted) {
                sampleResults.push_back(evaluateInvalidSample(sample));
                continue;
            }
            const auto replayResult = engine.replayReadOnly(
                makeRequest(sample),
                sample.tradeFacts,
                sample.cashFacts,
                sample.marketPriceFacts,
                sample.fxRateFacts);
            sampleResults.push_back(evaluateAcceptedSample(sample, replayResult));
        }

        const auto evidence = matrixEvidence(samples, sampleResults);
        assertEvidence(evidence);
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return EXIT_SUCCESS;
    } catch (const std::exception&) {
        QJsonObject failure{
            {"task", "TASK-256"},
            {"mode", "test-only-excel-vba-export-sample-import-matrix-diagnostics-readonly-vertical-slice"},
            {"sampleMatrixLoaded", false},
            {"sampleCount", 0},
            {"importerExecuted", false},
            {"diagnosticsBuilt", false},
            {"replayFactsBuilt", false},
            {"replayExecuted", false},
            {"accountingEngineCalled", false},
            {"validSamplesParityPassed", false},
            {"invalidSamplesRejected", false},
            {"diagnosticsSanitized", false},
            {"rawUserDataExposed", false},
            {"productionWrite", false},
            {"sqliteProductionWrite", false},
            {"auditWritten", false},
            {"ledgerWritten", false},
            {"snapshotWritten", false},
            {"tradeLogWritten", false},
            {"readModelPersistentWrite", false},
            {"brokerOrderSubmitted", false},
            {"networkAccess", false},
            {"credentialAccess", false},
            {"endpointAccess", false},
            {"automaticTrading", false},
            {"sanitizedFailure", "TASK256_EXCEL_VBA_EXPORT_SAMPLE_IMPORT_MATRIX_DIAGNOSTICS_FAILURE"},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        std::cerr << "TASK-256 failure: sanitized sample import matrix diagnostics assertion failed\n";
        return EXIT_FAILURE;
    }
}
