#include "AccountingEngine/AccountingReplayValidation.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <utility>

namespace etfdt::accounting {
namespace {

bool isBlank(const std::string& text)
{
    return text.empty();
}

void addRequiredIssue(AccountingValidationResult& result, const std::string& field, const std::string& sourceId = {})
{
    result.addIssue(makeAccountingIssue(
        AccountingIssueLevel::Error,
        AccountingIssueCode::MissingRequiredField,
        "required field is missing",
        true,
        field,
        sourceId));
}

void addInvalidEnumIssue(AccountingValidationResult& result, const std::string& field, const std::string& sourceId = {})
{
    result.addIssue(makeAccountingIssue(
        AccountingIssueLevel::Error,
        AccountingIssueCode::InvalidEnum,
        "field has an unsupported value",
        true,
        field,
        sourceId));
}

void addUnsupportedActionIssue(AccountingValidationResult& result, const std::string& field, const std::string& sourceId = {})
{
    result.addIssue(makeAccountingIssue(
        AccountingIssueLevel::Error,
        AccountingIssueCode::UnsupportedAction,
        "action is not supported by the parser boundary",
        true,
        field,
        sourceId));
}

void addMoneyIssue(AccountingValidationResult& result, const std::string& field, const std::string& sourceId = {})
{
    result.addIssue(makeAccountingIssue(
        AccountingIssueLevel::Error,
        AccountingIssueCode::InvalidMoneyText,
        "money text is not valid",
        true,
        field,
        sourceId));
}

void addCurrencyIssue(AccountingValidationResult& result, const std::string& field, const std::string& sourceId = {})
{
    result.addIssue(makeAccountingIssue(
        AccountingIssueLevel::Error,
        AccountingIssueCode::UnsupportedCurrency,
        "currency code must be three uppercase letters",
        true,
        field,
        sourceId));
}

bool isThreeUppercaseLetters(const std::string& text)
{
    return text.size() == 3 && std::all_of(text.begin(), text.end(), [](unsigned char ch) {
        return std::isupper(ch) != 0;
    });
}

} // namespace

AccountingValidationResult AccountingValidationResult::ok()
{
    return AccountingValidationResult{};
}

void AccountingValidationResult::addIssue(const AccountingIssueDto& issue)
{
    valid = false;
    issues.push_back(issue);
}

bool AccountingValidationResult::hasIssueCode(const std::string& code) const
{
    return std::any_of(issues.begin(), issues.end(), [&code](const AccountingIssueDto& issue) {
        return issue.code == code;
    });
}

bool isSupportedRequestedOutput(const std::string& text)
{
    return text == RequestedReplayOutput::Positions || text == RequestedReplayOutput::Cash ||
           text == RequestedReplayOutput::Pnl || text == RequestedReplayOutput::BasePosition ||
           text == RequestedReplayOutput::SniperPool || text == RequestedReplayOutput::Issues;
}

bool isSupportedTradeAction(const std::string& text)
{
    return text == TradeAction::Buy || text == TradeAction::Sell;
}

bool isSupportedCashAction(const std::string& text)
{
    return text == CashAction::InitialCash || text == CashAction::Deposit ||
           text == CashAction::Withdraw || text == CashAction::Adjustment;
}

bool isSupportedIssueLevel(const std::string& text)
{
    return text == AccountingIssueLevel::Info || text == AccountingIssueLevel::Warning ||
           text == AccountingIssueLevel::Error;
}

bool looksLikeIsoTime(const std::string& text)
{
    return text.size() >= 10 && std::isdigit(static_cast<unsigned char>(text[0])) != 0 &&
           std::isdigit(static_cast<unsigned char>(text[1])) != 0 &&
           std::isdigit(static_cast<unsigned char>(text[2])) != 0 &&
           std::isdigit(static_cast<unsigned char>(text[3])) != 0 && text[4] == '-' &&
           std::isdigit(static_cast<unsigned char>(text[5])) != 0 &&
           std::isdigit(static_cast<unsigned char>(text[6])) != 0 && text[7] == '-' &&
           std::isdigit(static_cast<unsigned char>(text[8])) != 0 &&
           std::isdigit(static_cast<unsigned char>(text[9])) != 0;
}

bool looksLikeMoneyText(const std::string& text)
{
    static const std::regex pattern(R"(^-?[0-9]+(\.[0-9]+)?$)");
    return !text.empty() && std::regex_match(text, pattern);
}

bool looksLikeQuantityText(const std::string& text)
{
    static const std::regex pattern(R"(^[0-9]+(\.[0-9]+)?$)");
    return !text.empty() && std::regex_match(text, pattern);
}

std::string normalizeCurrencyCode(const std::string& text)
{
    std::string normalized = text;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return normalized;
}

bool isSupportedCurrencyCode(const std::string& text)
{
    return isThreeUppercaseLetters(text);
}

AccountingValidationResult validateReplayRequest(const ReplayRequestDto& request)
{
    auto result = AccountingValidationResult::ok();

    if (!isBlank(request.sourceFromTime) && !isBlank(request.sourceToTime) &&
        request.sourceFromTime > request.sourceToTime) {
        result.addIssue(makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::InvalidTimeRange,
            "sourceFromTime must be earlier than or equal to sourceToTime",
            true,
            "sourceFromTime"));
    }

    for (const auto& output : request.requestedOutputs) {
        if (!isSupportedRequestedOutput(output)) {
            result.addIssue(makeAccountingIssue(
                AccountingIssueLevel::Error,
                AccountingIssueCode::InvalidEnum,
                "requested output is not supported",
                true,
                "requestedOutputs"));
        }
    }

    return result;
}

AccountingValidationResult validateTradeFact(const TradeFactDto& fact)
{
    auto result = AccountingValidationResult::ok();

    if (isBlank(fact.factId)) {
        addRequiredIssue(result, "factId");
    }
    if (isBlank(fact.tradeTime)) {
        addRequiredIssue(result, "tradeTime", fact.factId);
    }
    if (isBlank(fact.accountId)) {
        addRequiredIssue(result, "accountId", fact.factId);
    }
    if (isBlank(fact.portfolioId)) {
        addRequiredIssue(result, "portfolioId", fact.factId);
    }
    if (isBlank(fact.instrumentCode)) {
        addRequiredIssue(result, "instrumentCode", fact.factId);
    }
    if (isBlank(fact.action)) {
        addRequiredIssue(result, "action", fact.factId);
    } else if (!isSupportedTradeAction(fact.action)) {
        addUnsupportedActionIssue(result, "action", fact.factId);
    }
    if (isBlank(fact.quantityText)) {
        addRequiredIssue(result, "quantityText", fact.factId);
    } else if (!looksLikeQuantityText(fact.quantityText)) {
        result.addIssue(makeAccountingIssue(
            AccountingIssueLevel::Error,
            AccountingIssueCode::InvalidQuantityText,
            "quantity text is not valid",
            true,
            "quantityText",
            fact.factId));
    }
    if (!isBlank(fact.priceText) && !looksLikeMoneyText(fact.priceText)) {
        addMoneyIssue(result, "priceText", fact.factId);
    }
    if (isBlank(fact.amountText)) {
        addRequiredIssue(result, "amountText", fact.factId);
    } else if (!looksLikeMoneyText(fact.amountText)) {
        addMoneyIssue(result, "amountText", fact.factId);
    }
    if (!isBlank(fact.feeText) && !looksLikeMoneyText(fact.feeText)) {
        addMoneyIssue(result, "feeText", fact.factId);
    }
    if (isBlank(fact.currency)) {
        addRequiredIssue(result, "currency", fact.factId);
    } else if (!isSupportedCurrencyCode(fact.currency)) {
        addCurrencyIssue(result, "currency", fact.factId);
    }

    return result;
}

AccountingValidationResult validateCashFact(const CashFactDto& fact)
{
    auto result = AccountingValidationResult::ok();

    if (isBlank(fact.factId)) {
        addRequiredIssue(result, "factId");
    }
    if (isBlank(fact.time)) {
        addRequiredIssue(result, "time", fact.factId);
    }
    if (isBlank(fact.accountId)) {
        addRequiredIssue(result, "accountId", fact.factId);
    }
    if (isBlank(fact.portfolioId)) {
        addRequiredIssue(result, "portfolioId", fact.factId);
    }
    if (isBlank(fact.action)) {
        addRequiredIssue(result, "action", fact.factId);
    } else if (!isSupportedCashAction(fact.action)) {
        addUnsupportedActionIssue(result, "action", fact.factId);
    }
    if (isBlank(fact.amountText)) {
        addRequiredIssue(result, "amountText", fact.factId);
    } else if (!looksLikeMoneyText(fact.amountText)) {
        addMoneyIssue(result, "amountText", fact.factId);
    }
    if (isBlank(fact.currency)) {
        addRequiredIssue(result, "currency", fact.factId);
    } else if (!isSupportedCurrencyCode(fact.currency)) {
        addCurrencyIssue(result, "currency", fact.factId);
    }

    return result;
}

AccountingValidationResult validateMarketPriceFact(const MarketPriceFactDto& fact)
{
    auto result = AccountingValidationResult::ok();

    if (isBlank(fact.instrumentCode)) {
        addRequiredIssue(result, "instrumentCode");
    }
    if (isBlank(fact.priceTime)) {
        addRequiredIssue(result, "priceTime", fact.instrumentCode);
    }
    if (isBlank(fact.priceText)) {
        addRequiredIssue(result, "priceText", fact.instrumentCode);
    } else if (!looksLikeMoneyText(fact.priceText)) {
        addMoneyIssue(result, "priceText", fact.instrumentCode);
    }
    if (isBlank(fact.currency)) {
        addRequiredIssue(result, "currency", fact.instrumentCode);
    } else if (!isSupportedCurrencyCode(fact.currency)) {
        addCurrencyIssue(result, "currency", fact.instrumentCode);
    }

    return result;
}

AccountingValidationResult validateFxRateFact(const FxRateFactDto& fact)
{
    auto result = AccountingValidationResult::ok();

    if (isBlank(fact.baseCurrency)) {
        addRequiredIssue(result, "baseCurrency");
    } else if (!isSupportedCurrencyCode(fact.baseCurrency)) {
        addCurrencyIssue(result, "baseCurrency");
    }
    if (isBlank(fact.quoteCurrency)) {
        addRequiredIssue(result, "quoteCurrency");
    } else if (!isSupportedCurrencyCode(fact.quoteCurrency)) {
        addCurrencyIssue(result, "quoteCurrency");
    }
    if (!isBlank(fact.baseCurrency) && !isBlank(fact.quoteCurrency) &&
        normalizeCurrencyCode(fact.baseCurrency) == normalizeCurrencyCode(fact.quoteCurrency)) {
        addInvalidEnumIssue(result, "quoteCurrency");
    }
    if (isBlank(fact.rateTime)) {
        addRequiredIssue(result, "rateTime");
    }
    if (isBlank(fact.rateText)) {
        addRequiredIssue(result, "rateText");
    } else if (!looksLikeMoneyText(fact.rateText)) {
        addMoneyIssue(result, "rateText");
    }

    return result;
}

} // namespace etfdt::accounting
