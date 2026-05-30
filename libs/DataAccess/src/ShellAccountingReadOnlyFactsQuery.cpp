#include "DataAccess/ShellAccountingReadOnlyFactsQuery.h"

#include "Protocol/ErrorCode.h"

#include <iomanip>
#include <sstream>
#include <utility>

namespace etfdt::data_access {
namespace {

constexpr std::string_view kPositionListReadOnlySql =
    "SELECT "
    "ps.account_id, ps.portfolio_id, COALESCE(ps.strategy_code, ''), "
    "COALESCE(ps.actual_code, ''), ps.trade_source, ps.quantity_1e6, "
    "ps.cost_amount_cents, ps.market_value_cents, ps.unrealized_pnl_cents, "
    "ps.updated_at_utc "
    "FROM position_snapshot ps "
    "WHERE (? = '' OR CAST(ps.account_id AS TEXT) = ?) "
    "AND (? = '' OR CAST(ps.portfolio_id AS TEXT) = ?) "
    "ORDER BY ps.account_id, ps.portfolio_id, ps.strategy_code, ps.actual_code, ps.id;";

constexpr std::string_view kCashSummaryReadOnlySql =
    "SELECT "
    "cs.account_id, cs.portfolio_id, cs.cash_balance_cents, "
    "cs.principal_base_cents, cs.updated_at_utc "
    "FROM cash_snapshot cs "
    "WHERE (? = '' OR CAST(cs.account_id AS TEXT) = ?) "
    "AND (? = '' OR CAST(cs.portfolio_id AS TEXT) = ?) "
    "ORDER BY cs.account_id, cs.portfolio_id, cs.id;";

constexpr std::string_view kPortfolioSummaryReadOnlySql =
    "SELECT "
    "ps.account_id, ps.portfolio_id, ps.total_assets_cents, "
    "ps.total_market_value_cents, ps.cash_balance_cents, ps.principal_base_cents, "
    "ps.holding_cost_cents, ps.total_pnl_cents, ps.total_return_ppm, "
    "ps.base_completion_ratio_ppm, ps.sniper_pool_cents, "
    "ps.active_grid_cycle_count, ps.active_draft_count, ps.base_violation_count, "
    "ps.updated_at_utc "
    "FROM portfolio_summary ps "
    "WHERE (? = '' OR CAST(ps.account_id AS TEXT) = ?) "
    "AND (? = '' OR CAST(ps.portfolio_id AS TEXT) = ?) "
    "ORDER BY ps.account_id, ps.portfolio_id, ps.id;";

constexpr std::string_view kBasePositionSummaryReadOnlySql =
    "SELECT "
    "bpa.account_id, bpa.portfolio_id, bpa.strategy_code, "
    "COALESCE(bpa.actual_code, ''), SUM(bpa.target_base_amount_cents), "
    "SUM(bpa.allocated_amount_cents), MAX(bpa.created_at_utc) "
    "FROM base_position_allocation bpa "
    "WHERE (? = '' OR CAST(bpa.account_id AS TEXT) = ?) "
    "AND (? = '' OR CAST(bpa.portfolio_id AS TEXT) = ?) "
    "GROUP BY bpa.account_id, bpa.portfolio_id, bpa.strategy_code, bpa.actual_code "
    "ORDER BY bpa.account_id, bpa.portfolio_id, bpa.strategy_code, bpa.actual_code;";

constexpr std::string_view kSniperPoolSummaryReadOnlySql =
    "SELECT "
    "tta.account_id, tta.portfolio_id, tta.strategy_code, tta.tier_no, "
    "tta.tier_name, SUM(tta.allocated_amount_cents), MAX(tta.created_at_utc) "
    "FROM trade_tier_allocation tta "
    "WHERE (? = '' OR CAST(tta.account_id AS TEXT) = ?) "
    "AND (? = '' OR CAST(tta.portfolio_id AS TEXT) = ?) "
    "GROUP BY tta.account_id, tta.portfolio_id, tta.strategy_code, tta.tier_no, tta.tier_name "
    "ORDER BY tta.account_id, tta.portfolio_id, tta.strategy_code, tta.tier_no;";

constexpr std::string_view kInitialCashFactsReadOnlySql =
    "SELECT "
    "a.id, a.initial_cash_cents, a.base_currency, COALESCE(a.updated_at_utc, a.created_at_utc) "
    "FROM account a "
    "WHERE (? = '' OR CAST(a.id AS TEXT) = ?) "
    "ORDER BY a.id;";

template <typename T>
RepositoryResult<T> failure(std::string message)
{
    return RepositoryResult<T>::failure(
        etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
        std::move(message));
}

std::vector<std::string> filterParameters(const ShellAccountingFactsQueryRequest& request)
{
    return {
        request.accountId,
        request.accountId,
        request.portfolioId,
        request.portfolioId,
    };
}

std::string formatCents(std::int64_t cents)
{
    const bool negative = cents < 0;
    const auto absolute = negative ? -cents : cents;

    std::ostringstream output;
    if (negative) {
        output << '-';
    }
    output << (absolute / 100) << '.' << std::setw(2) << std::setfill('0') << (absolute % 100);
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

std::string formatRatioPpm(std::int64_t ppm)
{
    const bool negative = ppm < 0;
    const auto absolute = negative ? -ppm : ppm;

    std::ostringstream output;
    if (negative) {
        output << '-';
    }
    output << (absolute / 10000) << '.' << std::setw(4) << std::setfill('0') << (absolute % 10000)
           << '%';
    return output.str();
}

RepositoryResult<ShellAccountingPositionRow> parsePositionRow(const SqlQueryRow& row)
{
    if (row.size() != 10) {
        return failure<ShellAccountingPositionRow>("position list query returned unexpected column count");
    }

    ShellAccountingPositionRow value;
    value.accountId = std::to_string(row[0].int64Value);
    value.portfolioId = std::to_string(row[1].int64Value);
    value.strategyCode = row[2].text;
    value.instrumentCode = row[3].text;
    value.tradeSource = row[4].text;
    value.quantityText = formatFixed1e6(row[5].int64Value);
    value.costAmountText = formatCents(row[6].int64Value);
    value.marketValueText = formatCents(row[7].int64Value);
    value.unrealizedPnlText = formatCents(row[8].int64Value);
    value.updatedAtUtc = row[9].text;
    return RepositoryResult<ShellAccountingPositionRow>::success(std::move(value));
}

RepositoryResult<ShellAccountingCashSummaryRow> parseCashSummaryRow(const SqlQueryRow& row)
{
    if (row.size() != 5) {
        return failure<ShellAccountingCashSummaryRow>("cash summary query returned unexpected column count");
    }

    ShellAccountingCashSummaryRow value;
    value.accountId = std::to_string(row[0].int64Value);
    value.portfolioId = std::to_string(row[1].int64Value);
    value.cashBalanceText = formatCents(row[2].int64Value);
    value.principalBaseText = formatCents(row[3].int64Value);
    value.updatedAtUtc = row[4].text;
    return RepositoryResult<ShellAccountingCashSummaryRow>::success(std::move(value));
}

RepositoryResult<ShellAccountingPortfolioSummaryRow> parsePortfolioSummaryRow(const SqlQueryRow& row)
{
    if (row.size() != 15) {
        return failure<ShellAccountingPortfolioSummaryRow>(
            "portfolio summary query returned unexpected column count");
    }

    ShellAccountingPortfolioSummaryRow value;
    value.accountId = std::to_string(row[0].int64Value);
    value.portfolioId = std::to_string(row[1].int64Value);
    value.totalAssetsText = formatCents(row[2].int64Value);
    value.totalMarketValueText = formatCents(row[3].int64Value);
    value.cashBalanceText = formatCents(row[4].int64Value);
    value.principalBaseText = formatCents(row[5].int64Value);
    value.holdingCostText = formatCents(row[6].int64Value);
    value.totalPnlText = formatCents(row[7].int64Value);
    value.totalReturnText = formatRatioPpm(row[8].int64Value);
    value.baseCompletionRatioText = formatRatioPpm(row[9].int64Value);
    value.sniperPoolText = formatCents(row[10].int64Value);
    value.activeGridCycleCount = static_cast<int>(row[11].int64Value);
    value.activeDraftCount = static_cast<int>(row[12].int64Value);
    value.baseViolationCount = static_cast<int>(row[13].int64Value);
    value.updatedAtUtc = row[14].text;
    return RepositoryResult<ShellAccountingPortfolioSummaryRow>::success(std::move(value));
}

RepositoryResult<ShellAccountingBasePositionSummaryRow> parseBasePositionSummaryRow(
    const SqlQueryRow& row)
{
    if (row.size() != 7) {
        return failure<ShellAccountingBasePositionSummaryRow>(
            "base position summary query returned unexpected column count");
    }

    ShellAccountingBasePositionSummaryRow value;
    value.accountId = std::to_string(row[0].int64Value);
    value.portfolioId = std::to_string(row[1].int64Value);
    value.strategyCode = row[2].text;
    value.instrumentCode = row[3].text;
    value.targetBaseAmountText = formatCents(row[4].int64Value);
    value.allocatedAmountText = formatCents(row[5].int64Value);
    value.updatedAtUtc = row[6].text;
    return RepositoryResult<ShellAccountingBasePositionSummaryRow>::success(std::move(value));
}

RepositoryResult<ShellAccountingSniperPoolTierRow> parseSniperPoolTierRow(const SqlQueryRow& row)
{
    if (row.size() != 7) {
        return failure<ShellAccountingSniperPoolTierRow>(
            "sniper pool summary query returned unexpected column count");
    }

    ShellAccountingSniperPoolTierRow value;
    value.accountId = std::to_string(row[0].int64Value);
    value.portfolioId = std::to_string(row[1].int64Value);
    value.strategyCode = row[2].text;
    value.tierNo = static_cast<int>(row[3].int64Value);
    value.tierName = row[4].text;
    value.allocatedAmountText = formatCents(row[5].int64Value);
    value.updatedAtUtc = row[6].text;
    return RepositoryResult<ShellAccountingSniperPoolTierRow>::success(std::move(value));
}

RepositoryResult<ShellAccountingInitialCashFactRow> parseInitialCashFactRow(const SqlQueryRow& row)
{
    if (row.size() != 4) {
        return failure<ShellAccountingInitialCashFactRow>(
            "initial cash facts query returned unexpected column count");
    }

    ShellAccountingInitialCashFactRow value;
    value.accountId = std::to_string(row[0].int64Value);
    value.amountText = formatCents(row[1].int64Value);
    value.currency = row[2].text.empty() ? "CNY" : row[2].text;
    value.updatedAtUtc = row[3].text;
    return RepositoryResult<ShellAccountingInitialCashFactRow>::success(std::move(value));
}

template <typename T, typename Parser>
RepositoryResult<std::vector<T>> loadRows(
    const std::vector<SqlQueryRow>& rows,
    Parser parser)
{
    std::vector<T> values;
    values.reserve(rows.size());
    for (const auto& row : rows) {
        auto parsed = parser(row);
        if (!parsed) {
            return RepositoryResult<std::vector<T>>::failure(
                parsed.error().errorCode,
                parsed.error().message);
        }
        values.push_back(parsed.value());
    }
    return RepositoryResult<std::vector<T>>::success(std::move(values));
}

}  // namespace

std::string_view shellAccountingPositionListReadOnlySql() noexcept
{
    return kPositionListReadOnlySql;
}

std::string_view shellAccountingCashSummaryReadOnlySql() noexcept
{
    return kCashSummaryReadOnlySql;
}

std::string_view shellAccountingPortfolioSummaryReadOnlySql() noexcept
{
    return kPortfolioSummaryReadOnlySql;
}

std::string_view shellAccountingBasePositionSummaryReadOnlySql() noexcept
{
    return kBasePositionSummaryReadOnlySql;
}

std::string_view shellAccountingSniperPoolSummaryReadOnlySql() noexcept
{
    return kSniperPoolSummaryReadOnlySql;
}

std::string_view shellAccountingInitialCashFactsReadOnlySql() noexcept
{
    return kInitialCashFactsReadOnlySql;
}

RepositoryResult<std::vector<ShellAccountingPositionRow>>
ShellAccountingReadOnlyFactsQuery::loadPositions(
    const ShellAccountingFactsQueryRequest& request) const
{
    auto rows = queryRows(std::string(kPositionListReadOnlySql), filterParameters(request));
    if (!rows) {
        return RepositoryResult<std::vector<ShellAccountingPositionRow>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return loadRows<ShellAccountingPositionRow>(rows.value(), parsePositionRow);
}

RepositoryResult<std::vector<ShellAccountingCashSummaryRow>>
ShellAccountingReadOnlyFactsQuery::loadCashSummaries(
    const ShellAccountingFactsQueryRequest& request) const
{
    auto rows = queryRows(std::string(kCashSummaryReadOnlySql), filterParameters(request));
    if (!rows) {
        return RepositoryResult<std::vector<ShellAccountingCashSummaryRow>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return loadRows<ShellAccountingCashSummaryRow>(rows.value(), parseCashSummaryRow);
}

RepositoryResult<std::vector<ShellAccountingPortfolioSummaryRow>>
ShellAccountingReadOnlyFactsQuery::loadPortfolioSummaries(
    const ShellAccountingFactsQueryRequest& request) const
{
    auto rows = queryRows(std::string(kPortfolioSummaryReadOnlySql), filterParameters(request));
    if (!rows) {
        return RepositoryResult<std::vector<ShellAccountingPortfolioSummaryRow>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return loadRows<ShellAccountingPortfolioSummaryRow>(rows.value(), parsePortfolioSummaryRow);
}

RepositoryResult<std::vector<ShellAccountingBasePositionSummaryRow>>
ShellAccountingReadOnlyFactsQuery::loadBasePositionSummaries(
    const ShellAccountingFactsQueryRequest& request) const
{
    auto rows = queryRows(std::string(kBasePositionSummaryReadOnlySql), filterParameters(request));
    if (!rows) {
        return RepositoryResult<std::vector<ShellAccountingBasePositionSummaryRow>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return loadRows<ShellAccountingBasePositionSummaryRow>(rows.value(), parseBasePositionSummaryRow);
}

RepositoryResult<std::vector<ShellAccountingSniperPoolTierRow>>
ShellAccountingReadOnlyFactsQuery::loadSniperPoolTiers(
    const ShellAccountingFactsQueryRequest& request) const
{
    auto rows = queryRows(std::string(kSniperPoolSummaryReadOnlySql), filterParameters(request));
    if (!rows) {
        return RepositoryResult<std::vector<ShellAccountingSniperPoolTierRow>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return loadRows<ShellAccountingSniperPoolTierRow>(rows.value(), parseSniperPoolTierRow);
}

RepositoryResult<std::vector<ShellAccountingInitialCashFactRow>>
ShellAccountingReadOnlyFactsQuery::loadInitialCashFacts(
    const ShellAccountingFactsQueryRequest& request) const
{
    auto rows = queryRows(
        std::string(kInitialCashFactsReadOnlySql),
        {
            request.accountId,
            request.accountId,
        });
    if (!rows) {
        return RepositoryResult<std::vector<ShellAccountingInitialCashFactRow>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return loadRows<ShellAccountingInitialCashFactRow>(rows.value(), parseInitialCashFactRow);
}

}  // namespace etfdt::data_access
