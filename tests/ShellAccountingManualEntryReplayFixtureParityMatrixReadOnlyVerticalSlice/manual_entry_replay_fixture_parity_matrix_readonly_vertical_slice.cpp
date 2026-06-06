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

struct FixtureData {
    std::string fixtureId;
    std::string fixtureKind;
    std::string accountId;
    std::string portfolioId;
    std::filesystem::path path;
    std::filesystem::file_time_type beforeWriteTime;
    std::vector<etfdt::accounting::TradeFactDto> tradeFacts;
    std::vector<etfdt::accounting::CashFactDto> cashFacts;
    std::vector<etfdt::accounting::MarketPriceFactDto> marketPriceFacts;
    std::vector<etfdt::accounting::FxRateFactDto> fxRateFacts;
    QJsonObject expectedProjection;
};

struct FixtureParityResult {
    QJsonObject evidence;
    bool parityPassed = false;
    bool cashMatched = false;
    bool positionsMatched = false;
    bool pnlMatched = false;
    bool issuesMatched = false;
    bool issueSanitized = true;
    bool fixtureMutated = false;
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
    require(file.exists(), "fixture file missing");
    require(file.open(QIODevice::ReadOnly), "fixture file cannot be opened");

    QJsonParseError parseError{};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    require(parseError.error == QJsonParseError::NoError, "fixture JSON malformed");
    require(document.isObject(), "fixture JSON root must be object");
    return document.object();
}

etfdt::accounting::CashFactDto parseCashFact(const QJsonObject& object)
{
    return etfdt::accounting::CashFactDto{
        stringValue(object, "factId"),
        stringValue(object, "time"),
        stringValue(object, "accountId"),
        stringValue(object, "portfolioId"),
        stringValue(object, "action"),
        stringValue(object, "amountText"),
        stringValue(object, "currency"),
        stringValue(object, "note"),
    };
}

etfdt::accounting::TradeFactDto parseTradeFact(const QJsonObject& object)
{
    const auto action = stringValue(object, "action");
    require(action == etfdt::accounting::TradeAction::Buy || action == etfdt::accounting::TradeAction::Sell,
            "trade action must be BUY or SELL");
    return etfdt::accounting::TradeFactDto{
        stringValue(object, "factId"),
        stringValue(object, "tradeTime"),
        stringValue(object, "accountId"),
        stringValue(object, "portfolioId"),
        stringValue(object, "instrumentCode"),
        action,
        stringValue(object, "quantityText"),
        stringValue(object, "priceText"),
        stringValue(object, "amountText"),
        stringValue(object, "feeText"),
        stringValue(object, "cashFlowText"),
        stringValue(object, "currency"),
        stringValue(object, "source"),
        stringValue(object, "note"),
    };
}

etfdt::accounting::MarketPriceFactDto parseMarketPriceFact(const QJsonObject& object)
{
    return etfdt::accounting::MarketPriceFactDto{
        stringValue(object, "instrumentCode"),
        stringValue(object, "priceTime"),
        stringValue(object, "priceText"),
        stringValue(object, "currency"),
        stringValue(object, "source"),
    };
}

etfdt::accounting::FxRateFactDto parseFxRateFact(const QJsonObject& object)
{
    return etfdt::accounting::FxRateFactDto{
        stringValue(object, "baseCurrency"),
        stringValue(object, "quoteCurrency"),
        stringValue(object, "rateTime"),
        stringValue(object, "rateText"),
        stringValue(object, "source"),
    };
}

template <typename T, typename ParseFn>
std::vector<T> parseObjectArray(const QJsonArray& values, ParseFn parse)
{
    std::vector<T> parsed;
    parsed.reserve(static_cast<std::size_t>(values.size()));
    for (const auto& value : values) {
        require(value.isObject(), "fixture array entry must be object");
        parsed.push_back(parse(value.toObject()));
    }
    return parsed;
}

FixtureData loadFixture(const std::filesystem::path& fixturePath)
{
    const auto root = parseJsonFile(fixturePath);
    require(stringValue(root, "task") == "TASK-254", "fixture task must be TASK-254");
    require(stringValue(root, "schemaVersion") == "task-254-fixture-parity-matrix/v1",
            "fixture schema version mismatch");
    require(arrayValue(root, "externalDependencies").isEmpty(), "fixture must have no external dependencies");

    const auto fixtureKind = stringValue(root, "fixtureKind");
    require(fixtureKind == "buy_only" || fixtureKind == "buy_partial_sell" || fixtureKind == "cash_adjustment" ||
                fixtureKind == "unsupported_or_issue",
            "unexpected TASK-254 fixture kind");

    const auto scope = objectValue(root, "defaultScope");
    const auto inputFacts = objectValue(root, "inputFacts");

    FixtureData fixture;
    fixture.fixtureId = stringValue(root, "fixtureId");
    fixture.fixtureKind = fixtureKind;
    fixture.accountId = stringValue(scope, "accountId");
    fixture.portfolioId = stringValue(scope, "portfolioId");
    fixture.path = fixturePath;
    fixture.beforeWriteTime = std::filesystem::last_write_time(fixturePath);
    fixture.cashFacts =
        parseObjectArray<etfdt::accounting::CashFactDto>(arrayValue(inputFacts, "cashFacts"), parseCashFact);
    fixture.tradeFacts =
        parseObjectArray<etfdt::accounting::TradeFactDto>(arrayValue(inputFacts, "tradeFacts"), parseTradeFact);
    fixture.marketPriceFacts = parseObjectArray<etfdt::accounting::MarketPriceFactDto>(
        arrayValue(inputFacts, "marketPriceFacts"),
        parseMarketPriceFact);
    fixture.fxRateFacts =
        parseObjectArray<etfdt::accounting::FxRateFactDto>(arrayValue(inputFacts, "fxRateFacts"), parseFxRateFact);
    fixture.expectedProjection = objectValue(root, "expectedProjection");

    require(!fixture.fixtureId.empty(), "fixture id must be present");
    require(!fixture.expectedProjection.isEmpty(), "expected projection must be present");
    return fixture;
}

etfdt::accounting::ReplayRequestDto makeRequest(const FixtureData& fixture)
{
    etfdt::accounting::ReplayRequestDto request;
    request.accountId = fixture.accountId;
    request.portfolioId = fixture.portfolioId;
    request.requestedOutputs = {
        etfdt::accounting::RequestedReplayOutput::Positions,
        etfdt::accounting::RequestedReplayOutput::Cash,
        etfdt::accounting::RequestedReplayOutput::Issues,
    };
    request.includeIssues = true;
    request.calculationVersion = "TASK-254-test-only-fixture-parity-matrix";
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

QJsonArray positionSummariesJson(const std::vector<etfdt::accounting::PositionSummaryDto>& positions)
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

QJsonValue actualCashSummaryValue(const etfdt::accounting::AccountingReplayResult& result)
{
    return result.hasCashSummary ? QJsonValue(cashSummaryJson(result.cashSummary)) : QJsonValue(QJsonValue::Null);
}

QJsonValue actualPnlSummaryValue(const etfdt::accounting::AccountingReplayResult& result)
{
    return result.hasPortfolioPnl ? QJsonValue(pnlSummaryJson(result.portfolioPnl)) : QJsonValue(QJsonValue::Null);
}

bool issueScenarioIsSanitized(const FixtureData& fixture,
                              const etfdt::accounting::AccountingReplayResult& result,
                              const QJsonArray& actualIssues)
{
    if (fixture.fixtureKind != "cash_adjustment" && fixture.fixtureKind != "unsupported_or_issue") {
        return true;
    }
    if (actualIssues.isEmpty() || result.issues.empty()) {
        return false;
    }
    const auto compact = QString::fromUtf8(QJsonDocument(actualIssues).toJson(QJsonDocument::Compact));
    return !compact.contains("message", Qt::CaseInsensitive) && !compact.contains("payload", Qt::CaseInsensitive) &&
           !compact.contains("sql", Qt::CaseInsensitive) && !compact.contains("credential", Qt::CaseInsensitive) &&
           !compact.contains("endpoint", Qt::CaseInsensitive) && !compact.contains("broker", Qt::CaseInsensitive);
}

FixtureParityResult evaluateFixture(const FixtureData& fixture,
                                    const etfdt::accounting::AccountingReplayResult& result)
{
    const auto actualCash = actualCashSummaryValue(result);
    const auto actualPositions = positionSummariesJson(result.positionList.positions);
    const auto actualPnl = actualPnlSummaryValue(result);
    const auto actualIssues = issuesJson(result.issues);

    const auto expectedCash = fixture.expectedProjection.value("cashSummary");
    const auto expectedPositions = fixture.expectedProjection.value("positionSummaries").toArray();
    const auto expectedPnl = fixture.expectedProjection.value("pnlSummary");
    const auto expectedIssues = fixture.expectedProjection.value("issues").toArray();
    const auto expectedStatus = stringValue(fixture.expectedProjection, "replayStatus");

    FixtureParityResult parity;
    parity.cashMatched = actualCash == expectedCash;
    parity.positionsMatched = actualPositions == expectedPositions;
    parity.pnlMatched = actualPnl == expectedPnl;
    parity.issuesMatched = actualIssues == expectedIssues;
    parity.issueSanitized = issueScenarioIsSanitized(fixture, result, actualIssues);
    parity.fixtureMutated = fixture.beforeWriteTime != std::filesystem::last_write_time(fixture.path);
    parity.parityPassed = parity.cashMatched && parity.positionsMatched && parity.pnlMatched && parity.issuesMatched &&
                          parity.issueSanitized && !parity.fixtureMutated && result.status == expectedStatus;

    parity.evidence = QJsonObject{
        {"fixtureId", QString::fromStdString(fixture.fixtureId)},
        {"fixtureKind", QString::fromStdString(fixture.fixtureKind)},
        {"fixturePath", QString::fromStdString(fixture.path.generic_string())},
        {"replayStatus", QString::fromStdString(result.status)},
        {"resultReplayExecuted", result.replayExecuted},
        {"parityPassed", parity.parityPassed},
        {"cashSummaryMatched", parity.cashMatched},
        {"positionSummaryMatched", parity.positionsMatched},
        {"pnlSummaryMatched", parity.pnlMatched},
        {"issuesMatched", parity.issuesMatched},
        {"issueScenarioSanitized", parity.issueSanitized},
        {"fixtureMutated", parity.fixtureMutated},
        {"actualProjection",
            QJsonObject{
                {"cashSummary", actualCash},
                {"positionSummaries", actualPositions},
                {"pnlSummary", actualPnl},
                {"issues", actualIssues},
            }},
    };
    return parity;
}

QJsonObject matrixEvidence(const std::vector<FixtureData>& fixtures,
                           const std::vector<FixtureParityResult>& parityResults)
{
    require(fixtures.size() == parityResults.size(), "fixture and parity result sizes must match");
    require(fixtures.size() >= 4, "fixture matrix must include at least four fixtures");

    bool allParityPassed = true;
    bool cashMatched = true;
    bool positionMatched = true;
    bool pnlMatched = true;
    bool issuesSanitized = true;
    bool fixtureMutated = false;
    QJsonArray fixtureEvidence;
    for (const auto& parity : parityResults) {
        allParityPassed = allParityPassed && parity.parityPassed;
        cashMatched = cashMatched && parity.cashMatched;
        positionMatched = positionMatched && parity.positionsMatched;
        pnlMatched = pnlMatched && parity.pnlMatched;
        issuesSanitized = issuesSanitized && parity.issueSanitized;
        fixtureMutated = fixtureMutated || parity.fixtureMutated;
        fixtureEvidence.append(parity.evidence);
    }

    return QJsonObject{
        {"task", "TASK-254"},
        {"mode", "test-only-fixture-parity-matrix-readonly-vertical-slice"},
        {"fixtureMatrixLoaded", true},
        {"fixtureCount", static_cast<int>(fixtures.size())},
        {"replayExecuted", true},
        {"accountingEngineCalled", true},
        {"allParityPassed", allParityPassed},
        {"cashSummaryMatched", cashMatched},
        {"positionSummaryMatched", positionMatched},
        {"pnlSummaryMatched", pnlMatched},
        {"issueScenarioSanitized", issuesSanitized},
        {"fixtureMutated", fixtureMutated},
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
        {"fixtures", fixtureEvidence},
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
    require(stringValue(evidence, "task") == "TASK-254", "evidence task mismatch");
    require(stringValue(evidence, "mode") == "test-only-fixture-parity-matrix-readonly-vertical-slice",
            "evidence mode mismatch");
    require(evidence.value("fixtureCount").toInt() >= 4, "fixture count must be at least four");
    for (const char* key : {
             "fixtureMatrixLoaded",
             "replayExecuted",
             "accountingEngineCalled",
             "allParityPassed",
             "cashSummaryMatched",
             "positionSummaryMatched",
             "pnlSummaryMatched",
             "issueScenarioSanitized",
         }) {
        require(boolField(evidence, key), std::string(key) + " must be true");
    }
    for (const char* key : {
             "fixtureMutated",
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
        require(argc >= 5, "usage: T254FixtureParityMatrixSlice <fixture-json>...");

        std::vector<FixtureData> fixtures;
        fixtures.reserve(static_cast<std::size_t>(argc - 1));
        for (int index = 1; index < argc; ++index) {
            fixtures.push_back(loadFixture(std::filesystem::path(argv[index])));
        }

        std::vector<std::string> fixtureKinds;
        fixtureKinds.reserve(fixtures.size());
        for (const auto& fixture : fixtures) {
            fixtureKinds.push_back(fixture.fixtureKind);
        }
        for (const char* requiredKind : {"buy_only", "buy_partial_sell", "cash_adjustment", "unsupported_or_issue"}) {
            require(std::find(fixtureKinds.begin(), fixtureKinds.end(), requiredKind) != fixtureKinds.end(),
                    std::string("fixture matrix missing kind: ") + requiredKind);
        }

        etfdt::accounting::AccountingReplayEngine engine;
        std::vector<FixtureParityResult> parityResults;
        parityResults.reserve(fixtures.size());
        for (const auto& fixture : fixtures) {
            const auto result = engine.replayReadOnly(
                makeRequest(fixture),
                fixture.tradeFacts,
                fixture.cashFacts,
                fixture.marketPriceFacts,
                fixture.fxRateFacts);
            parityResults.push_back(evaluateFixture(fixture, result));
        }

        const auto evidence = matrixEvidence(fixtures, parityResults);
        assertEvidence(evidence);
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return EXIT_SUCCESS;
    } catch (const std::exception&) {
        QJsonObject failure{
            {"task", "TASK-254"},
            {"mode", "test-only-fixture-parity-matrix-readonly-vertical-slice"},
            {"fixtureMatrixLoaded", false},
            {"fixtureCount", 0},
            {"replayExecuted", false},
            {"accountingEngineCalled", false},
            {"allParityPassed", false},
            {"cashSummaryMatched", false},
            {"positionSummaryMatched", false},
            {"pnlSummaryMatched", false},
            {"issueScenarioSanitized", false},
            {"fixtureMutated", false},
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
            {"sanitizedFailure", "TASK254_FIXTURE_PARITY_MATRIX_FAILURE"},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        std::cerr << "TASK-254 failure: sanitized fixture matrix parity assertion failed\n";
        return EXIT_FAILURE;
    }
}
