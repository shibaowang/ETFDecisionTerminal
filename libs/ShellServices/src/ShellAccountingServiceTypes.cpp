#include "ShellServices/ShellAccountingServiceTypes.h"

namespace etfdt::shell_services {

bool isShellAccountingServiceResultReadOnly(
    const ShellAccountingServiceResult& result) noexcept
{
    return result.readOnly && !result.writeEnabled &&
           !result.generatedTradeDraft &&
           !result.generatedTradeSuggestion &&
           !result.strategyExecuted &&
           !result.brokerOrderSubmitted &&
           !result.accountingEngineCalled &&
           !result.productionFileLoading &&
           !result.productionWrite &&
           !result.sqliteProductionWrite &&
           !result.auditWritten &&
           !result.ledgerWritten &&
           !result.snapshotWritten &&
           !result.tradeLogWritten &&
           !result.readModelPersistentWrite &&
           !result.networkAccess &&
           !result.credentialAccess &&
           !result.endpointAccess &&
           !result.automaticTrading;
}

}  // namespace etfdt::shell_services
