#include "AccountingReplayMinimalEngine.h"

#include "AccountingReplayResultMapper.h"

#include <QJsonArray>
#include <QString>

#include <charconv>
#include <cstdlib>
#include <optional>
#include <string_view>
#include <utility>

namespace etfdt::tests::accounting {
namespace {

constexpr const char* kFx001EmptyLedger = "FX001_EMPTY_LEDGER";
constexpr const char* kFx002SingleBuy = "FX002_SINGLE_BUY";
constexpr const char* kMinimalSource = "AccountingReplayMinimalEngine";

struct MoneyValue {
    long long cents{0};
    std::string currency;
};

std::string trim(std::string_view value)
{
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return std::string(value.substr(first, last - first + 1));
}

std::optional<long long> parseInteger(std::string_view value)
{
    const auto trimmed = trim(value);
    if (trimmed.empty()) {
        return std::nullopt;
    }
    long long parsed = 0;
    const auto* begin = trimmed.data();
    const auto* end = begin + trimmed.size();
    const auto result = std::from_chars(begin, end, parsed);
    if (result.ec != std::errc{} || result.ptr != end) {
        return std::nullopt;
    }
    return parsed;
}

std::optional<long long> parseDecimalToScale(std::string_view value, int scale)
{
    auto trimmed = trim(value);
    const auto space = trimmed.find(' ');
    if (space != std::string::npos) {
        trimmed = trimmed.substr(0, space);
    }
    if (trimmed.empty()) {
        return std::nullopt;
    }

    const auto dot = trimmed.find('.');
    const auto wholeText = dot == std::string::npos ? trimmed : trimmed.substr(0, dot);
    const auto fractionText = dot == std::string::npos ? std::string{} : trimmed.substr(dot + 1);
    const auto whole = parseInteger(wholeText.empty() ? "0" : wholeText);
    if (!whole.has_value()) {
        return std::nullopt;
    }

    long long multiplier = 1;
    for (int i = 0; i < scale; ++i) {
        multiplier *= 10;
    }

    long long fraction = 0;
    long long fractionMultiplier = multiplier;
    for (int i = 0; i < scale; ++i) {
        fractionMultiplier /= 10;
        if (i < static_cast<int>(fractionText.size())) {
            const char digit = fractionText[static_cast<std::size_t>(i)];
            if (digit < '0' || digit > '9') {
                return std::nullopt;
            }
            fraction += static_cast<long long>(digit - '0') * fractionMultiplier;
        }
    }
    for (std::size_t i = static_cast<std::size_t>(scale); i < fractionText.size(); ++i) {
        if (fractionText[i] < '0' || fractionText[i] > '9') {
            return std::nullopt;
        }
    }

    return (*whole * multiplier) + fraction;
}

std::optional<MoneyValue> parseMoneyText(const std::string& text, const std::string& fallbackCurrency)
{
    const auto trimmed = trim(text);
    if (trimmed.empty()) {
        return std::nullopt;
    }
    const auto space = trimmed.find(' ');
    const auto amountText = space == std::string::npos ? trimmed : trimmed.substr(0, space);
    const auto currency = space == std::string::npos ? fallbackCurrency : trim(std::string_view(trimmed).substr(space + 1));
    const auto cents = parseDecimalToScale(amountText, 2);
    if (!cents.has_value() || currency.empty()) {
        return std::nullopt;
    }
    return MoneyValue{*cents, currency};
}

std::string formatMoney(long long cents, const std::string& currency)
{
    const auto whole = cents / 100;
    const auto fraction = std::llabs(cents % 100);
    std::string text = std::to_string(whole);
    text += ".";
    if (fraction < 10) {
        text += "0";
    }
    text += std::to_string(fraction);
    text += " ";
    text += currency;
    return text;
}

std::string formatUnitPriceFromCost(long long costCents, long long quantity, const std::string& currency)
{
    if (quantity <= 0) {
        return {};
    }
    const long long mills = (costCents * 10) / quantity;
    const auto whole = mills / 1000;
    const auto fraction = std::llabs(mills % 1000);
    std::string text = std::to_string(whole);
    text += ".";
    if (fraction < 100) {
        text += "0";
    }
    if (fraction < 10) {
        text += "0";
    }
    text += std::to_string(fraction);
    text += " ";
    text += currency;
    return text;
}

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

QJsonObject makeSingleBuyPositionListResponse(
    const AccountingFixture& fixture,
    const AccountingTradeFact& trade,
    long long quantity,
    long long costCents)
{
    const auto currency = scopeString(fixture, "currency");
    QJsonObject position;
    position.insert(QStringLiteral("accountId"), QString::fromStdString(trade.accountId));
    position.insert(QStringLiteral("portfolioId"), QString::fromStdString(trade.portfolioId));
    position.insert(QStringLiteral("instrumentCode"), QString::fromStdString(trade.instrumentCode));
    position.insert(QStringLiteral("quantityText"), QString::fromStdString(std::to_string(quantity)));
    position.insert(QStringLiteral("costAmountText"), QString::fromStdString(formatMoney(costCents, currency)));
    position.insert(QStringLiteral("costPriceText"), QString::fromStdString(formatUnitPriceFromCost(costCents, quantity, currency)));
    position.insert(QStringLiteral("currency"), QString::fromStdString(currency));
    position.insert(QStringLiteral("dataQualityStatus"), QStringLiteral("OK"));

    QJsonArray positions;
    positions.append(position);

    QJsonObject response;
    response.insert(QStringLiteral("accountId"), QString::fromStdString(scopeString(fixture, "accountId")));
    response.insert(QStringLiteral("portfolioId"), QString::fromStdString(scopeString(fixture, "portfolioId")));
    response.insert(QStringLiteral("positions"), positions);
    response.insert(QStringLiteral("dataQualityStatus"), QStringLiteral("OK"));
    return response;
}

QJsonObject makeSingleBuyCashSummary(
    const AccountingFixture& fixture,
    const AccountingCashFact& initialCash,
    long long cashBalanceCents,
    long long feeCents)
{
    const auto currency = scopeString(fixture, "currency");
    QJsonObject response;
    response.insert(QStringLiteral("accountId"), QString::fromStdString(initialCash.accountId));
    response.insert(QStringLiteral("portfolioId"), QString::fromStdString(initialCash.portfolioId));
    response.insert(QStringLiteral("currency"), QString::fromStdString(currency));
    response.insert(QStringLiteral("cashBalanceText"), QString::fromStdString(formatMoney(cashBalanceCents, currency)));
    response.insert(QStringLiteral("availableCashText"), QString::fromStdString(formatMoney(cashBalanceCents, currency)));
    response.insert(QStringLiteral("feeTotalText"), QString::fromStdString(formatMoney(feeCents, currency)));
    response.insert(QStringLiteral("dataQualityStatus"), QStringLiteral("OK"));
    return response;
}

QJsonObject makeSingleBuyPortfolioPnl(
    const AccountingFixture& fixture,
    long long initialCashCents,
    long long cashBalanceCents)
{
    const auto currency = scopeString(fixture, "currency");
    QJsonObject response;
    response.insert(QStringLiteral("portfolioId"), QString::fromStdString(scopeString(fixture, "portfolioId")));
    response.insert(QStringLiteral("currency"), QString::fromStdString(currency));
    response.insert(QStringLiteral("principalText"), QString::fromStdString(formatMoney(initialCashCents, currency)));
    response.insert(QStringLiteral("cashText"), QString::fromStdString(formatMoney(cashBalanceCents, currency)));
    response.insert(QStringLiteral("marketValueText"), QStringLiteral("unavailable"));
    response.insert(QStringLiteral("unrealizedPnlText"), QStringLiteral("unavailable"));
    response.insert(QStringLiteral("dataQualityStatus"), QStringLiteral("OK"));
    return response;
}

} // namespace

bool AccountingReplayMinimalEngine::supportsFixture(const std::string& fixtureId) const
{
    return fixtureId == kFx001EmptyLedger || fixtureId == kFx002SingleBuy;
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

    if (fixture.fixtureId == kFx001EmptyLedger
        && (!fixture.tradeFacts.empty() || !fixture.cashFacts.empty() || !fixture.marketPriceFacts.empty())) {
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

    if (fixture.fixtureId == kFx002SingleBuy) {
        const auto invalidFx002 = [&](std::string message) {
            setError(std::move(message));
            AccountingReplayResult result = AccountingReplayResultMapper::makeInvalidFixtureResult(lastError_);
            result.fixtureId = fixture.fixtureId;
            result.metadata.fixtureId = fixture.fixtureId;
            result.metadata.sourceFixtureId = fixture.fixtureId;
            result.issues.push_back(makeIssue("ERROR", "FX002_INVALID_INPUT", lastError_, true, fixture.fixtureId));
            return result;
        };

        if (fixture.cashFacts.size() != 1 || fixture.cashFacts.front().action != "INITIAL_CASH") {
            return invalidFx002("FX002_SINGLE_BUY requires exactly one INITIAL_CASH fact.");
        }
        if (fixture.tradeFacts.size() != 1 || fixture.tradeFacts.front().action != "BUY") {
            return invalidFx002("FX002_SINGLE_BUY requires exactly one BUY trade fact.");
        }

        const auto& initialCash = fixture.cashFacts.front();
        const auto& trade = fixture.tradeFacts.front();
        const auto currency = scopeString(fixture, "currency");
        if (currency != "CNY" || initialCash.currency != "CNY" || trade.currency != "CNY") {
            return invalidFx002("FX002_SINGLE_BUY requires CNY currency.");
        }
        if (trade.instrumentCode.empty() || trade.quantityText.empty() || trade.priceText.empty()
            || trade.amountText.empty() || trade.feeText.empty()) {
            return invalidFx002("FX002_SINGLE_BUY requires instrument, quantity, price, amount, and fee.");
        }

        const auto quantity = parseInteger(trade.quantityText);
        const auto priceMills = parseDecimalToScale(trade.priceText, 3);
        const auto amount = parseMoneyText(trade.amountText, currency);
        const auto fee = parseMoneyText(trade.feeText, currency);
        const auto initial = parseMoneyText(initialCash.amountText, currency);
        if (!quantity.has_value() || *quantity <= 0 || !priceMills.has_value() || !amount.has_value()
            || !fee.has_value() || !initial.has_value()) {
            return invalidFx002("FX002_SINGLE_BUY contains an invalid numeric field.");
        }
        if (amount->currency != currency || fee->currency != currency || initial->currency != currency) {
            return invalidFx002("FX002_SINGLE_BUY money fields must use fixture currency.");
        }

        const long long costCents = amount->cents + fee->cents;
        const long long cashBalanceCents = initial->cents - costCents;

        AccountingReplayResult result;
        result.fixtureId = fixture.fixtureId;
        result.implemented = true;
        result.replayExecuted = true;
        result.status = kReplayStatusOk;
        result.message = "Single buy replay completed.";
        result.metadata = AccountingReplayMetadata{
            fixture.fixtureId,
            fixture.fixtureId,
            fixture.schemaVersion,
            "",
            "Implement the next fixture in a separate task.",
            static_cast<int>(fixture.expectedIssues.size()),
            fixture.blocking,
        };
        result.positionListResponseRaw = makeSingleBuyPositionListResponse(fixture, trade, *quantity, costCents);
        result.cashSummaryRaw = makeSingleBuyCashSummary(fixture, initialCash, cashBalanceCents, fee->cents);
        result.portfolioPnlRaw = makeSingleBuyPortfolioPnl(fixture, initial->cents, cashBalanceCents);
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
