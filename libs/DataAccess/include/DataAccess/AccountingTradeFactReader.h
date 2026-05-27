#pragma once

#include "DataAccess/ReadOnlyRepositoryBase.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace etfdt::data_access {

struct TradeFactQueryRequest final {
    std::string accountId;
    std::string portfolioId;
    std::string sourceFromTime;
    std::string sourceToTime;
    int limit = 0;
    int offset = 0;
};

struct TradeFactRow final {
    std::string factId;
    std::string tradeTime;
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string action;
    std::string quantityText;
    std::string priceText;
    std::string amountText;
    std::string feeText;
    std::string cashFlowText;
    std::string currency;
    std::string source;
    std::string note;
    std::int64_t rowId = 0;
};

struct TradeFactQueryResult final {
    bool success = false;
    bool readOnly = true;
    std::string queryVersion;
    std::vector<TradeFactRow> rows;
    std::vector<std::string> issues;
};

[[nodiscard]] std::string_view accountingTradeFactsReadOnlySql() noexcept;

class AccountingTradeFactReader final : public ReadOnlyRepositoryBase {
public:
    using ReadOnlyRepositoryBase::ReadOnlyRepositoryBase;

    [[nodiscard]] RepositoryResult<TradeFactQueryResult> loadTradeFacts(
        const TradeFactQueryRequest& request) const;
};

}  // namespace etfdt::data_access
