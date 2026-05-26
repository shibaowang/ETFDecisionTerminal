#pragma once

#include "AccountingFixtureLoader.h"
#include "AccountingReplayResult.h"

#include <string>

namespace etfdt::tests::accounting {

class AccountingReplayMinimalEngine {
public:
    [[nodiscard]] bool supportsFixture(const std::string& fixtureId) const;
    [[nodiscard]] AccountingReplayResult replayFixture(const AccountingFixture& fixture);
    [[nodiscard]] std::string lastError() const;

private:
    void setError(std::string message);

    std::string lastError_;
};

} // namespace etfdt::tests::accounting
