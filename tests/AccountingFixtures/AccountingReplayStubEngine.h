#pragma once

#include "AccountingFixtureLoader.h"

#include <string>

namespace etfdt::tests::accounting {

struct AccountingReplayStubResult {
    std::string fixtureId;
    bool implemented{false};
    bool replayExecuted{false};
    std::string status;
    std::string message;
    std::string requiredFutureTask;
    std::string sourceFixtureId;
    int issueCount{0};
    bool blockingExpected{false};
};

class AccountingReplayStubEngine {
public:
    [[nodiscard]] AccountingReplayStubResult previewFixture(const AccountingFixture& fixture) const;
};

} // namespace etfdt::tests::accounting
