#include "DataAccess/DataAccess.h"

namespace etfdt::data_access {

std::string_view dataAccessModuleName() noexcept
{
    return "DataAccess";
}

const std::vector<std::string>& requiredCoreTables()
{
    static const std::vector<std::string> tables = {
        "account",
        "portfolio",
        "instrument",
        "strategy",
        "otc_channel",
        "trade_log",
        "trade_draft",
        "trade_execution_group",
        "trade_tier_allocation",
        "base_position_allocation",
        "grid_cycle",
        "cash_allocation_plan_current",
        "audit_log",
        "system_log",
        "error_log",
        "schema_version",
    };
    return tables;
}

}  // namespace etfdt::data_access
