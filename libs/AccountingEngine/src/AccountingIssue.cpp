#include "AccountingEngine/AccountingIssue.h"

#include <utility>

namespace etfdt::accounting {

AccountingIssueDto makeAccountingIssue(
    std::string level,
    std::string code,
    std::string message,
    bool blocking,
    std::string field,
    std::string sourceId)
{
    return AccountingIssueDto{
        std::move(level),
        std::move(code),
        std::move(message),
        blocking,
        std::move(field),
        std::move(sourceId),
    };
}

} // namespace etfdt::accounting
