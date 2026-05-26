#pragma once

#include "AccountingFixtureLoader.h"
#include "AccountingReplayResult.h"

namespace etfdt::tests::accounting {

class AccountingReplayStubEngine {
public:
    [[nodiscard]] AccountingReplayResult previewFixture(const AccountingFixture& fixture) const;
    [[nodiscard]] AccountingReplayResult previewFixtureResult(const AccountingFixture& fixture) const;
};

} // namespace etfdt::tests::accounting
