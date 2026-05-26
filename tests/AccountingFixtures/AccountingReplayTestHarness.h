#pragma once

#include "AccountingFixtureLoader.h"
#include "AccountingReplayStubEngine.h"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace etfdt::tests::accounting {

class AccountingReplayTestHarness {
public:
    [[nodiscard]] bool loadFixtures(const std::filesystem::path& fixturesDir);
    [[nodiscard]] bool runAll();

    [[nodiscard]] std::optional<AccountingReplayResult> resultForFixture(const std::string& fixtureId) const;
    [[nodiscard]] std::optional<AccountingFixture> fixtureByIdForTest(const std::string& fixtureId) const;
    [[nodiscard]] const std::vector<AccountingReplayResult>& results() const noexcept;
    [[nodiscard]] std::vector<std::string> coveredFixtureIds() const;
    [[nodiscard]] std::vector<std::string> missingFixtureIds(const std::vector<std::string>& expectedIds) const;
    [[nodiscard]] std::string lastError() const;

private:
    AccountingFixtureLoader loader_;
    AccountingReplayStubEngine engine_;
    std::vector<std::string> fixtureIds_;
    std::vector<AccountingReplayResult> results_;
    std::string lastError_;
};

} // namespace etfdt::tests::accounting
