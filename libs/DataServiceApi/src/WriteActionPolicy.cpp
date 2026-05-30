#include "DataServiceApi/WriteActionPolicy.h"

#include "DataServiceApi/DataServiceActions.h"

#include <algorithm>
#include <array>

namespace etfdt::data_service_api {
namespace {

constexpr std::array<const char*, 6> kAllowedWriteActions = {
    kActionDataAuditAppend,
    kActionAccountingSnapshotWrite,
    kActionAccountingAuditWrite,
    kActionAccountingTradeDraftCreate,
    kActionAccountingTradeDraftConfirm,
    kActionAccountingBrokerOrderDryRun,
};

}  // namespace

bool isWriteActionAllowed(const std::string& actionName)
{
    return std::find(kAllowedWriteActions.begin(), kAllowedWriteActions.end(), actionName)
        != kAllowedWriteActions.end();
}

std::vector<std::string> allowedWriteActions()
{
    return {kAllowedWriteActions.begin(), kAllowedWriteActions.end()};
}

}  // namespace etfdt::data_service_api
