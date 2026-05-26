#pragma once

#include "AccountingFixtureLoader.h"
#include "AccountingReplayResult.h"

#include <string>

namespace etfdt::tests::accounting {

class AccountingReplayResultMapper {
public:
    [[nodiscard]] static AccountingReplayResult makeNotImplementedResult(const AccountingFixture& fixture);
    [[nodiscard]] static AccountingReplayResult makeInvalidFixtureResult(const std::string& reason);
};

} // namespace etfdt::tests::accounting
