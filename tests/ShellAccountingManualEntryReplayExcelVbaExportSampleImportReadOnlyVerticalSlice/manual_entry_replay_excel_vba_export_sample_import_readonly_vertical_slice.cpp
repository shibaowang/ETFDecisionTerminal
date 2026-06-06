#include "AccountingEngine/AccountingEngine.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

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
};

struct ImportedReplayFacts {
    std::string sampleId;
    std::string accountId;
    std::string portfolioId;
    std::filesystem::path path;
    std::filesystem::file_time_type beforeWriteTime;
    bool rawUserDataExposed = true;
    std::vector<etfdt::accounting::TradeFactDto> tradeFacts;
    std::vector<etfdt::accounting::CashFactDto> cashFacts;
    std::vector<etfdt::accounting::MarketPriceFactDto> marketPriceFacts;
    std::vector<etfdt::accounting::FxRateFactDto> fxRateFacts;
    QJsonObject expectedProjection;
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

SheetRows sheetRows(const QJsonObject& exportSample, const char* sheetName)
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
        for (const auto& headerValue : arrayValue(sheet, "headers")) {
            require(headerValue.isString(), "Excel/VBA export header must be string");
            parsed.headers.push_back(headerValue.toString().toStdString());
        }
        parsed.rows = arrayValue(sheet, "rows");
        return parsed;
    }
    throw std::runtime_error("Excel/VBA export sheet missing");
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

ImportedReplayFacts importSample(const std::filesystem::path& samplePath)
{
    const auto root = parseJsonFile(samplePath);
    require(stringValue(root, "task") == "TASK-255", "sample task must be TASK-255");
    require(stringValue(root, "sampleId") == "TASK255_SANITIZED_EXCEL_VBA_EXPORT_BUY_PARTIAL_SELL",
            "unexpected TASK-255 sample id");
    require(stringValue(root, "schemaVersion") == "task-255-excel-vba-export-sample/v1",
            "sample schema version mismatch");
    require(stringValue(root, "sourceFormat") == "sanitized-excel-vba-style-json-export",
            "sample source format mismatch");
    require(!boolValue(root, "rawUserDataExposed"), "sample must not expose raw user data");
    require(arrayValue(root, "externalDependencies").isEmpty(), "sample must have no external dependencies");

    const auto scope = objectValue(root, "defaultScope");
    const auto exportSample = objectValue(root, "exportSample");

    ImportedReplayFacts imported;
    imported.sampleId = stringValue(root, "sampleId");
    imported.accountId = stringValue(scope, "accountId");
    imported.portfolioId = stringValue(scope, "portfolioId");
    imported.path = samplePath;
    imported.beforeWriteTime = std::filesystem::last_write_time(samplePath);
    imported.rawUserDataExposed = boolValue(root, "rawUserDataExposed");
    imported.cashFacts = importCashFacts(sheetRows(exportSample, "InitialCash"));
    imported.tradeFacts = importTradeFacts(sheetRows(exportSample, "Trades"));
    imported.marketPriceFacts = importMarketPriceFacts(sheetRows(exportSample, "MarketPrices"));
    imported.fxRateFacts = importFxRateFacts(sheetRows(exportSample, "FxRates"));
    imported.expectedProjection = objectValue(root, "expectedProjection");

    require(imported.cashFacts.size() == 1, "sample must import exactly one initial cash fact");
    require(imported.tradeFacts.size() == 2, "sample must import one BUY and one partial SELL");
    require(!imported.expectedProjection.isEmpty(), "expected projection must be present");
    return imported;
}

etfdt::accounting::ReplayRequestDto makeRequest(const ImportedReplayFacts& imported)
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
    request.calculationVersion = "TASK-255-test-only-excel-vba-export-sample-import";
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

QJsonObject evidenceJson(const ImportedReplayFacts& imported,
                         const etfdt::accounting::AccountingReplayResult& result,
                         bool sampleMutated)
{
    const auto expectedCash = objectValue(imported.expectedProjection, "cashSummary");
    const auto expectedPosition = objectValue(imported.expectedProjection, "positionSummary");
    const auto expectedPnl = objectValue(imported.expectedProjection, "pnlSummary");
    const auto expectedIssues = arrayValue(imported.expectedProjection, "issues");
    const auto expectedStatus = stringValue(imported.expectedProjection, "replayStatus");

    const auto actualCash = result.hasCashSummary ? cashSummaryJson(result.cashSummary) : QJsonObject{};
    const auto actualPosition = result.positionList.positions.size() == 1 ? positionSummaryJson(result.positionList.positions.front())
                                                                          : QJsonObject{};
    const auto actualPnl = result.hasPortfolioPnl ? pnlSummaryJson(result.portfolioPnl) : QJsonObject{};
    const bool cashMatched = actualCash == expectedCash;
    const bool positionMatched = actualPosition == expectedPosition;
    const bool pnlMatched = actualPnl == expectedPnl;
    const bool issuesMatched = result.issues.empty() && expectedIssues.isEmpty();
    const bool parityPassed = cashMatched && positionMatched && pnlMatched && issuesMatched &&
                              result.status == expectedStatus && !sampleMutated && !imported.rawUserDataExposed;

    return QJsonObject{
        {"task", "TASK-255"},
        {"mode", "test-only-excel-vba-export-sample-import-readonly-vertical-slice"},
        {"samplePath", QString::fromStdString(imported.path.generic_string())},
        {"sampleId", QString::fromStdString(imported.sampleId)},
        {"sampleLoaded", true},
        {"importerExecuted", true},
        {"replayFactsBuilt", !imported.cashFacts.empty() && !imported.tradeFacts.empty()},
        {"replayExecuted", result.replayExecuted},
        {"accountingEngineCalled", true},
        {"parityPassed", parityPassed},
        {"cashSummaryMatched", cashMatched},
        {"positionSummaryMatched", positionMatched},
        {"pnlSummaryMatched", pnlMatched},
        {"issuesMatched", issuesMatched},
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
        {"rawUserDataExposed", imported.rawUserDataExposed},
        {"actualProjection",
            QJsonObject{
                {"cashSummary", actualCash},
                {"positionSummary", actualPosition},
                {"pnlSummary", actualPnl},
            }},
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
    require(stringValue(evidence, "task") == "TASK-255", "evidence task mismatch");
    require(stringValue(evidence, "mode") == "test-only-excel-vba-export-sample-import-readonly-vertical-slice",
            "evidence mode mismatch");
    for (const char* key : {
             "sampleLoaded",
             "importerExecuted",
             "replayFactsBuilt",
             "replayExecuted",
             "accountingEngineCalled",
             "parityPassed",
         }) {
        require(boolField(evidence, key), std::string(key) + " must be true");
    }
    for (const char* key : {
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
             "rawUserDataExposed",
         }) {
        require(!boolField(evidence, key), std::string(key) + " must be false");
    }
}

} // namespace

int main(int argc, char** argv)
{
    try {
        require(argc == 2, "usage: T255ExcelVbaExportSampleImportSlice <sample-json>");
        const std::filesystem::path samplePath(argv[1]);
        const auto imported = importSample(samplePath);

        etfdt::accounting::AccountingReplayEngine engine;
        const auto result = engine.replayReadOnly(
            makeRequest(imported),
            imported.tradeFacts,
            imported.cashFacts,
            imported.marketPriceFacts,
            imported.fxRateFacts);

        const auto sampleMutated = imported.beforeWriteTime != std::filesystem::last_write_time(samplePath);
        const auto evidence = evidenceJson(imported, result, sampleMutated);
        assertEvidence(evidence);
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return EXIT_SUCCESS;
    } catch (const std::exception&) {
        QJsonObject failure{
            {"task", "TASK-255"},
            {"mode", "test-only-excel-vba-export-sample-import-readonly-vertical-slice"},
            {"sampleLoaded", false},
            {"importerExecuted", false},
            {"replayFactsBuilt", false},
            {"replayExecuted", false},
            {"accountingEngineCalled", false},
            {"parityPassed", false},
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
            {"rawUserDataExposed", false},
            {"sanitizedFailure", "TASK255_EXCEL_VBA_EXPORT_SAMPLE_IMPORT_FAILURE"},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        std::cerr << "TASK-255 failure: sanitized Excel/VBA export sample import parity assertion failed\n";
        return EXIT_FAILURE;
    }
}
