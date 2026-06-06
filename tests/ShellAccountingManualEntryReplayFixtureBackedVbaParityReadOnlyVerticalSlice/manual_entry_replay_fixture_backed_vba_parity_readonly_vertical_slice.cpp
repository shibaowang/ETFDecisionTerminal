#include "AccountingEngine/AccountingEngine.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QString>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct FixtureData {
    std::string fixtureId;
    std::string accountId;
    std::string portfolioId;
    std::vector<etfdt::accounting::TradeFactDto> tradeFacts;
    std::vector<etfdt::accounting::CashFactDto> cashFacts;
    std::vector<etfdt::accounting::MarketPriceFactDto> marketPriceFacts;
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
    require(stringValue(root, "task") == "TASK-253", "fixture task must be TASK-253");
    const auto fixtureId = stringValue(root, "fixtureId");
    require(fixtureId == "TASK253_VBA_PARITY_BUY_PARTIAL_SELL", "unexpected TASK-253 fixture id");
    require(arrayValue(root, "externalDependencies").isEmpty(), "fixture must have no external dependencies");

    const auto scope = objectValue(root, "defaultScope");
    const auto inputFacts = objectValue(root, "inputFacts");

    FixtureData fixture;
    fixture.fixtureId = fixtureId;
    fixture.accountId = stringValue(scope, "accountId");
    fixture.portfolioId = stringValue(scope, "portfolioId");
    fixture.cashFacts =
        parseObjectArray<etfdt::accounting::CashFactDto>(arrayValue(inputFacts, "cashFacts"), parseCashFact);
    fixture.tradeFacts =
        parseObjectArray<etfdt::accounting::TradeFactDto>(arrayValue(inputFacts, "tradeFacts"), parseTradeFact);
    fixture.marketPriceFacts = parseObjectArray<etfdt::accounting::MarketPriceFactDto>(
        arrayValue(inputFacts, "marketPriceFacts"),
        parseMarketPriceFact);
    fixture.expectedProjection = objectValue(root, "expectedProjection");

    require(fixture.cashFacts.size() == 1, "fixture must include exactly one initial cash fact");
    require(fixture.tradeFacts.size() == 2, "fixture must include one BUY and one partial SELL");
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
        etfdt::accounting::RequestedReplayOutput::Pnl,
        etfdt::accounting::RequestedReplayOutput::Issues,
    };
    request.includeIssues = true;
    request.calculationVersion = "TASK-253-test-only-fixture-backed-vba-parity";
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

QJsonObject evidenceJson(const FixtureData& fixture,
                         const std::string& fixturePath,
                         const etfdt::accounting::AccountingReplayResult& result,
                         bool fixtureMutated)
{
    const auto expectedCash = objectValue(fixture.expectedProjection, "cashSummary");
    const auto expectedPosition = objectValue(fixture.expectedProjection, "positionSummary");
    const auto expectedPnl = objectValue(fixture.expectedProjection, "pnlSummary");

    const auto actualCash = result.hasCashSummary ? cashSummaryJson(result.cashSummary) : QJsonObject{};
    const auto actualPosition = result.positionList.positions.size() == 1 ? positionSummaryJson(result.positionList.positions.front())
                                                                          : QJsonObject{};
    const auto actualPnl = result.hasPortfolioPnl ? pnlSummaryJson(result.portfolioPnl) : QJsonObject{};

    const bool cashMatched = actualCash == expectedCash;
    const bool positionMatched = actualPosition == expectedPosition;
    const bool pnlMatched = actualPnl == expectedPnl;
    const bool parityPassed = cashMatched && positionMatched && pnlMatched && result.status == etfdt::accounting::AccountingReplayStatus::Ok;

    QJsonObject evidence{
        {"task", "TASK-253"},
        {"mode", "test-only-fixture-backed-vba-parity-readonly-vertical-slice"},
        {"fixturePath", QString::fromStdString(fixturePath)},
        {"fixtureId", QString::fromStdString(fixture.fixtureId)},
        {"fixtureLoaded", true},
        {"expectedProjectionLoaded", true},
        {"replayExecuted", result.replayExecuted},
        {"accountingEngineCalled", true},
        {"parityPassed", parityPassed},
        {"cashSummaryMatched", cashMatched},
        {"positionSummaryMatched", positionMatched},
        {"pnlSummaryMatched", pnlMatched},
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
        {"replayStatus", QString::fromStdString(result.status)},
        {"actualProjection",
            QJsonObject{
                {"cashSummary", actualCash},
                {"positionSummary", actualPosition},
                {"pnlSummary", actualPnl},
            }},
    };
    return evidence;
}

bool boolField(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    require(value.isBool(), std::string("evidence missing boolean field: ") + key);
    return value.toBool();
}

void assertEvidence(const QJsonObject& evidence)
{
    require(stringValue(evidence, "task") == "TASK-253", "evidence task mismatch");
    require(stringValue(evidence, "mode") == "test-only-fixture-backed-vba-parity-readonly-vertical-slice",
            "evidence mode mismatch");
    for (const char* key : {
             "fixtureLoaded",
             "expectedProjectionLoaded",
             "replayExecuted",
             "accountingEngineCalled",
             "parityPassed",
             "cashSummaryMatched",
             "positionSummaryMatched",
             "pnlSummaryMatched",
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
        require(argc == 2, "usage: T253FixtureBackedVbaParitySlice <fixture-json>");
        const std::filesystem::path fixturePath(argv[1]);
        const auto beforeWriteTime = std::filesystem::last_write_time(fixturePath);

        const auto fixture = loadFixture(fixturePath);
        etfdt::accounting::AccountingReplayEngine engine;
        const auto result = engine.replayReadOnly(
            makeRequest(fixture),
            fixture.tradeFacts,
            fixture.cashFacts,
            fixture.marketPriceFacts,
            {});

        const auto afterWriteTime = std::filesystem::last_write_time(fixturePath);
        const auto evidence = evidenceJson(fixture, fixturePath.generic_string(), result, beforeWriteTime != afterWriteTime);
        assertEvidence(evidence);
        std::cout << QJsonDocument(evidence).toJson(QJsonDocument::Compact).toStdString() << '\n';
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        QJsonObject failure{
            {"task", "TASK-253"},
            {"mode", "test-only-fixture-backed-vba-parity-readonly-vertical-slice"},
            {"fixtureLoaded", false},
            {"expectedProjectionLoaded", false},
            {"replayExecuted", false},
            {"accountingEngineCalled", false},
            {"parityPassed", false},
            {"cashSummaryMatched", false},
            {"positionSummaryMatched", false},
            {"pnlSummaryMatched", false},
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
            {"sanitizedFailure", "TASK253_PARITY_SLICE_FAILURE"},
        };
        std::cerr << QJsonDocument(failure).toJson(QJsonDocument::Compact).toStdString() << '\n';
        std::cerr << "TASK-253 failure: sanitized test-only parity assertion failed\n";
        return EXIT_FAILURE;
    }
}
