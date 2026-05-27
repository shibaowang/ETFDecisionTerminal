#include "DataAccess/AccountingTradeFactReader.h"

#include "Protocol/ErrorCode.h"

#include <iomanip>
#include <sstream>
#include <utility>

namespace etfdt::data_access {
namespace {

constexpr std::string_view kQueryVersion = "trade-facts-readonly-v1";

constexpr std::string_view kTradeFactsReadOnlySql =
    "SELECT "
    "tl.id, tl.uid, COALESCE(tl.local_time, tl.created_at_utc), "
    "tl.account_id, tl.portfolio_id, COALESCE(tl.actual_code, ''), "
    "tl.action_type, tl.quantity_1e6, tl.price_1e6, tl.amount_cents, "
    "tl.fee_cents, tl.net_cash_impact_cents, COALESCE(i.currency, ''), "
    "tl.trade_source, COALESCE(tl.memo, '') "
    "FROM trade_log tl "
    "LEFT JOIN instrument i ON i.id = tl.actual_instrument_id "
    "WHERE (? = '' OR CAST(tl.account_id AS TEXT) = ?) "
    "AND (? = '' OR CAST(tl.portfolio_id AS TEXT) = ?) "
    "AND (? = '' OR tl.created_at_utc >= ?) "
    "AND (? = '' OR tl.created_at_utc <= ?) "
    "ORDER BY tl.created_at_utc, tl.id "
    "LIMIT CASE WHEN CAST(? AS INTEGER) > 0 THEN CAST(? AS INTEGER) ELSE -1 END "
    "OFFSET CASE WHEN CAST(? AS INTEGER) > 0 THEN CAST(? AS INTEGER) ELSE 0 END;";

template <typename T>
RepositoryResult<T> failure(std::string message)
{
    return RepositoryResult<T>::failure(
        etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
        std::move(message));
}

std::string formatCents(std::int64_t cents, const std::string& currency)
{
    const bool negative = cents < 0;
    const auto absolute = negative ? -cents : cents;

    std::ostringstream output;
    if (negative) {
        output << '-';
    }
    output << (absolute / 100) << '.' << std::setw(2) << std::setfill('0') << (absolute % 100);
    if (!currency.empty()) {
        output << ' ' << currency;
    }
    return output.str();
}

std::string formatFixed1e6(std::int64_t value)
{
    const bool negative = value < 0;
    const auto absolute = negative ? -value : value;
    const auto whole = absolute / 1000000;
    auto fraction = absolute % 1000000;

    std::ostringstream output;
    if (negative) {
        output << '-';
    }
    output << whole;
    if (fraction == 0) {
        return output.str();
    }

    output << '.' << std::setw(6) << std::setfill('0') << fraction;
    auto text = output.str();
    while (!text.empty() && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }
    return text;
}

std::string formatPrice(std::int64_t price1e6, const std::string& currency)
{
    if (price1e6 == 0) {
        return {};
    }

    auto text = formatFixed1e6(price1e6);
    if (!currency.empty()) {
        text += " " + currency;
    }
    return text;
}

RepositoryResult<TradeFactRow> parseTradeFactRow(const SqlQueryRow& row)
{
    if (row.size() != 15) {
        return failure<TradeFactRow>("trade facts query returned unexpected column count");
    }

    const std::string currency = row[12].text;

    TradeFactRow fact;
    fact.rowId = row[0].int64Value;
    fact.factId = row[1].text;
    fact.tradeTime = row[2].text;
    fact.accountId = std::to_string(row[3].int64Value);
    fact.portfolioId = std::to_string(row[4].int64Value);
    fact.instrumentCode = row[5].text;
    fact.action = row[6].text;
    fact.quantityText = formatFixed1e6(row[7].int64Value);
    fact.priceText = formatPrice(row[8].int64Value, currency);
    fact.amountText = formatCents(row[9].int64Value, currency);
    fact.feeText = formatCents(row[10].int64Value, currency);
    fact.cashFlowText = formatCents(row[11].int64Value, currency);
    fact.currency = currency;
    fact.source = row[13].text;
    fact.note = row[14].text;
    return RepositoryResult<TradeFactRow>::success(std::move(fact));
}

}  // namespace

std::string_view accountingTradeFactsReadOnlySql() noexcept
{
    return kTradeFactsReadOnlySql;
}

RepositoryResult<TradeFactQueryResult> AccountingTradeFactReader::loadTradeFacts(
    const TradeFactQueryRequest& request) const
{
    if (request.limit < 0) {
        return failure<TradeFactQueryResult>("trade facts limit must be non-negative");
    }
    if (request.offset < 0) {
        return failure<TradeFactQueryResult>("trade facts offset must be non-negative");
    }

    const std::vector<std::string> parameters = {
        request.accountId,
        request.accountId,
        request.portfolioId,
        request.portfolioId,
        request.sourceFromTime,
        request.sourceFromTime,
        request.sourceToTime,
        request.sourceToTime,
        std::to_string(request.limit),
        std::to_string(request.limit),
        std::to_string(request.offset),
        std::to_string(request.offset),
    };

    auto rows = queryRows(std::string(kTradeFactsReadOnlySql), parameters);
    if (!rows) {
        return RepositoryResult<TradeFactQueryResult>::failure(
            rows.error().errorCode,
            rows.error().message);
    }

    TradeFactQueryResult result;
    result.success = true;
    result.readOnly = true;
    result.queryVersion = std::string(kQueryVersion);
    result.rows.reserve(rows.value().size());

    for (const auto& row : rows.value()) {
        auto fact = parseTradeFactRow(row);
        if (!fact) {
            return RepositoryResult<TradeFactQueryResult>::failure(
                fact.error().errorCode,
                fact.error().message);
        }
        result.rows.push_back(fact.value());
    }

    return RepositoryResult<TradeFactQueryResult>::success(std::move(result));
}

}  // namespace etfdt::data_access
