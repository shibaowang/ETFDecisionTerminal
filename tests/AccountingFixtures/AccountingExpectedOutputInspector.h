#pragma once

#include "AccountingFixtureLoader.h"

#include <string>
#include <vector>

namespace etfdt::tests::accounting {

struct ExpectedOutputShape {
    std::string fixtureId;
    bool hasPositionSummaries{false};
    bool hasCashSummary{false};
    bool hasPortfolioPnl{false};
    bool hasBasePosition{false};
    bool hasSniperPool{false};
    int positionSummaryCount{0};
    int issueCount{0};
    bool blocking{false};
    std::string dataQualityStatusText;
    std::string lastError;
};

class AccountingExpectedOutputInspector {
public:
    [[nodiscard]] ExpectedOutputShape inspect(const AccountingFixture& fixture) const;
    [[nodiscard]] bool hasRequiredExpectedOutputs(const AccountingFixture& fixture) const;
    [[nodiscard]] std::vector<std::string> expectedIssueCodes(const AccountingFixture& fixture) const;
    [[nodiscard]] int positionSummaryCount(const AccountingFixture& fixture) const;
    [[nodiscard]] bool hasErrorCode(const AccountingFixture& fixture, const std::string& code) const;
};

} // namespace etfdt::tests::accounting
