#include "AccountingReplayResult.h"

#include <algorithm>

namespace etfdt::tests::accounting {

bool hasReplayIssueCode(const AccountingReplayResult& result, const std::string& code)
{
    return std::any_of(result.issues.begin(), result.issues.end(), [&](const AccountingReplayIssue& issue) {
        return issue.code == code;
    });
}

bool hasEmptyReplayOutputs(const AccountingReplayResult& result)
{
    return result.positionListResponseRaw.isEmpty()
        && result.cashSummaryRaw.isEmpty()
        && result.portfolioPnlRaw.isEmpty()
        && result.basePositionRaw.isEmpty()
        && result.sniperPoolRaw.isEmpty();
}

} // namespace etfdt::tests::accounting
