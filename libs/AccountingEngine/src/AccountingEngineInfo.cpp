#include "AccountingEngine/AccountingEngineInfo.h"

namespace etfdt::accounting {

AccountingEngineInfo accountingEngineInfo()
{
    return AccountingEngineInfo{
        "AccountingEngine",
        "0.1-skeleton",
        "0.3-draft",
        false,
        false,
        false,
        false,
        "Production module skeleton only; no replay logic is implemented."
    };
}

} // namespace etfdt::accounting
