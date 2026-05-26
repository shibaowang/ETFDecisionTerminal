#include "AccountingReplayTestHarness.h"

#include "AccountingReplayMinimalEngine.h"

#include <algorithm>

namespace etfdt::tests::accounting {

bool AccountingReplayTestHarness::loadFixtures(const std::filesystem::path& fixturesDir)
{
    fixtureIds_.clear();
    results_.clear();
    lastError_.clear();

    if (!loader_.loadAll(fixturesDir)) {
        lastError_ = loader_.lastError();
        return false;
    }

    fixtureIds_ = loader_.fixtureIds();
    return true;
}

bool AccountingReplayTestHarness::runAll()
{
    results_.clear();
    lastError_.clear();

    if (fixtureIds_.empty()) {
        lastError_ = "No fixtures are loaded.";
        return false;
    }

    for (const auto& fixtureId : fixtureIds_) {
        const auto fixture = loader_.fixtureById(fixtureId);
        if (!fixture.has_value()) {
            lastError_ = "Loaded fixture id cannot be resolved: " + fixtureId;
            return false;
        }
        results_.push_back(engine_.previewFixture(*fixture));
    }

    return true;
}

bool AccountingReplayTestHarness::runAllWithMinimalEngine()
{
    results_.clear();
    lastError_.clear();

    if (fixtureIds_.empty()) {
        lastError_ = "No fixtures are loaded.";
        return false;
    }

    AccountingReplayMinimalEngine minimalEngine;
    for (const auto& fixtureId : fixtureIds_) {
        const auto fixture = loader_.fixtureById(fixtureId);
        if (!fixture.has_value()) {
            lastError_ = "Loaded fixture id cannot be resolved: " + fixtureId;
            return false;
        }
        results_.push_back(minimalEngine.replayFixture(*fixture));
    }

    return true;
}

std::optional<AccountingReplayResult> AccountingReplayTestHarness::resultForFixture(
    const std::string& fixtureId) const
{
    const auto it = std::find_if(results_.begin(), results_.end(), [&](const AccountingReplayResult& result) {
        return result.fixtureId == fixtureId;
    });
    if (it == results_.end()) {
        return std::nullopt;
    }
    return *it;
}

std::optional<AccountingFixture> AccountingReplayTestHarness::fixtureByIdForTest(
    const std::string& fixtureId) const
{
    return loader_.fixtureById(fixtureId);
}

const std::vector<AccountingReplayResult>& AccountingReplayTestHarness::results() const noexcept
{
    return results_;
}

std::vector<std::string> AccountingReplayTestHarness::coveredFixtureIds() const
{
    std::vector<std::string> covered;
    covered.reserve(results_.size());
    for (const auto& result : results_) {
        covered.push_back(result.fixtureId);
    }
    return covered;
}

std::vector<std::string> AccountingReplayTestHarness::missingFixtureIds(
    const std::vector<std::string>& expectedIds) const
{
    const auto covered = coveredFixtureIds();
    std::vector<std::string> missing;
    for (const auto& expectedId : expectedIds) {
        const auto found = std::find(covered.begin(), covered.end(), expectedId);
        if (found == covered.end()) {
            missing.push_back(expectedId);
        }
    }
    return missing;
}

std::string AccountingReplayTestHarness::lastError() const
{
    return lastError_;
}

} // namespace etfdt::tests::accounting
