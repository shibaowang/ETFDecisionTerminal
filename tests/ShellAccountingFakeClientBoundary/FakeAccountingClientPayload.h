#pragma once

#include "ShellServices/ShellAccountingReadOnlyController.h"

#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_fake {

struct FakeAccountingClientPayload final {
    std::string actionName;
    bool implemented = false;
    bool readOnly = true;
    bool writeEnabled = false;
    std::string payloadStatus;
    std::string dataQualityStatus = "UNAVAILABLE";
    bool hasRows = false;
    bool stale = false;
    std::vector<etfdt::shell_services::ShellAccountingIssue> issues;
    std::vector<std::string> displayRows;
    std::string amountRawText;
    std::string amountMaskedText;
    std::string notes;
};

[[nodiscard]] etfdt::shell_services::ShellAccountingStateSnapshot toStateSnapshot(
    const FakeAccountingClientPayload& payload);
[[nodiscard]] etfdt::shell_services::ShellAccountingDisplayText toDisplayText(
    const FakeAccountingClientPayload& payload);

}  // namespace etfdt::tests::shell_accounting_fake
