#include "AccountingReplayMinimalEngine.h"

#include "AccountingReplayResultMapper.h"

#include <QJsonArray>
#include <QString>

#include <utility>

namespace etfdt::tests::accounting {
namespace {

constexpr const char* kFx001EmptyLedger = "FX001_EMPTY_LEDGER";
constexpr const char* kMinimalSource = "AccountingReplayMinimalEngine";

std::string scopeString(const AccountingFixture& fixture, const char* key)
{
    const auto value = fixture.defaultScope.value(QString::fromUtf8(key));
    return value.isString() ? value.toString().toStdString() : std::string{};
}

AccountingReplayIssue makeIssue(
    std::string level,
    std::string code,
    std::string message,
    bool blocking,
    std::string fixtureId)
{
    return AccountingReplayIssue{
        std::move(level),
        std::move(code),
        std::move(message),
        blocking,
        std::move(fixtureId),
        kMinimalSource,
    };
}

QJsonObject makePositionListResponse(const AccountingFixture& fixture)
{
    QJsonObject response;
    response.insert(QStringLiteral("accountId"), QString::fromStdString(scopeString(fixture, "accountId")));
    response.insert(QStringLiteral("portfolioId"), QString::fromStdString(scopeString(fixture, "portfolioId")));
    response.insert(QStringLiteral("positions"), QJsonArray{});
    response.insert(QStringLiteral("dataQualityStatus"), QStringLiteral("OK"));
    return response;
}

QJsonObject makeCashSummary(const AccountingFixture& fixture)
{
    const auto currency = scopeString(fixture, "currency");
    const auto zeroText = QString::fromStdString("0.00 " + currency);
    QJsonObject response;
    response.insert(QStringLiteral("accountId"), QString::fromStdString(scopeString(fixture, "accountId")));
    response.insert(QStringLiteral("portfolioId"), QString::fromStdString(scopeString(fixture, "portfolioId")));
    response.insert(QStringLiteral("currency"), QString::fromStdString(currency));
    response.insert(QStringLiteral("cashBalanceText"), zeroText);
    response.insert(QStringLiteral("availableCashText"), zeroText);
    response.insert(QStringLiteral("frozenCashText"), zeroText);
    response.insert(QStringLiteral("netContributionText"), zeroText);
    response.insert(QStringLiteral("dataQualityStatus"), QStringLiteral("OK"));
    return response;
}

QJsonObject makePortfolioPnl(const AccountingFixture& fixture)
{
    const auto currency = scopeString(fixture, "currency");
    const auto zeroText = QString::fromStdString("0.00 " + currency);
    QJsonObject response;
    response.insert(QStringLiteral("portfolioId"), QString::fromStdString(scopeString(fixture, "portfolioId")));
    response.insert(QStringLiteral("currency"), QString::fromStdString(currency));
    response.insert(QStringLiteral("totalAssetsText"), zeroText);
    response.insert(QStringLiteral("cashText"), zeroText);
    response.insert(QStringLiteral("marketValueText"), zeroText);
    response.insert(QStringLiteral("realizedPnlText"), zeroText);
    response.insert(QStringLiteral("unrealizedPnlText"), zeroText);
    response.insert(QStringLiteral("totalPnlText"), zeroText);
    response.insert(QStringLiteral("dataQualityStatus"), QStringLiteral("OK"));
    return response;
}

} // namespace

bool AccountingReplayMinimalEngine::supportsFixture(const std::string& fixtureId) const
{
    return fixtureId == kFx001EmptyLedger;
}

AccountingReplayResult AccountingReplayMinimalEngine::replayFixture(const AccountingFixture& fixture)
{
    lastError_.clear();

    if (fixture.fixtureId.empty()) {
        setError("Fixture id is empty.");
        return AccountingReplayResultMapper::makeInvalidFixtureResult(lastError_);
    }

    if (!supportsFixture(fixture.fixtureId)) {
        return AccountingReplayResultMapper::makeNotImplementedResult(fixture);
    }

    if (!fixture.tradeFacts.empty() || !fixture.cashFacts.empty() || !fixture.marketPriceFacts.empty()) {
        setError("FX001_EMPTY_LEDGER must not contain trade, cash, or market price facts.");
        AccountingReplayResult result = AccountingReplayResultMapper::makeInvalidFixtureResult(lastError_);
        result.fixtureId = fixture.fixtureId;
        result.metadata.fixtureId = fixture.fixtureId;
        result.metadata.sourceFixtureId = fixture.fixtureId;
        result.issues.push_back(makeIssue(
            "ERROR",
            "FX001_INPUT_NOT_EMPTY",
            lastError_,
            true,
            fixture.fixtureId));
        return result;
    }

    AccountingReplayResult result;
    result.fixtureId = fixture.fixtureId;
    result.implemented = true;
    result.replayExecuted = true;
    result.status = kReplayStatusOk;
    result.message = "Empty ledger replay completed.";
    result.metadata = AccountingReplayMetadata{
        fixture.fixtureId,
        fixture.fixtureId,
        fixture.schemaVersion,
        "",
        "Implement the next fixture in a separate task.",
        static_cast<int>(fixture.expectedIssues.size()),
        fixture.blocking,
    };
    result.positionListResponseRaw = makePositionListResponse(fixture);
    result.cashSummaryRaw = makeCashSummary(fixture);
    result.portfolioPnlRaw = makePortfolioPnl(fixture);
    return result;
}

std::string AccountingReplayMinimalEngine::lastError() const
{
    return lastError_;
}

void AccountingReplayMinimalEngine::setError(std::string message)
{
    lastError_ = std::move(message);
}

} // namespace etfdt::tests::accounting
