#include "ShellServices/ShellAccountingIssue.h"

namespace etfdt::shell_services {

ShellAccountingViewState shellAccountingViewStateFromIssue(const ShellAccountingIssue& issue)
{
    if (issue.code == "MARKET_PRICE_MISSING" || issue.code == "MISSING_FEE") {
        return ShellAccountingViewState::Warning;
    }

    if (issue.code == "REPLAY_NOT_AVAILABLE") {
        return issue.blocking ? ShellAccountingViewState::Error
                              : ShellAccountingViewState::Unavailable;
    }

    if (issue.code == "NEGATIVE_CASH" || issue.code == "SELL_EXCEEDS_POSITION" ||
        issue.code == "MULTI_CURRENCY_UNSUPPORTED" || issue.code == "FX_RATE_MISSING") {
        return ShellAccountingViewState::Error;
    }

    return issue.blocking ? ShellAccountingViewState::Error
                          : ShellAccountingViewState::Warning;
}

}  // namespace etfdt::shell_services
