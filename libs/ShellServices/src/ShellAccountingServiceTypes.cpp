#include "ShellServices/ShellAccountingServiceTypes.h"

namespace etfdt::shell_services {

bool isShellAccountingServiceResultReadOnly(
    const ShellAccountingServiceResult& result) noexcept
{
    return result.readOnly && !result.writeEnabled &&
           !result.generatedTradeDraft &&
           !result.generatedTradeSuggestion &&
           !result.strategyExecuted &&
           !result.brokerOrderSubmitted;
}

}  // namespace etfdt::shell_services
