#include "AccountingReplayStubEngine.h"

#include "AccountingReplayResultMapper.h"

namespace etfdt::tests::accounting {

AccountingReplayResult AccountingReplayStubEngine::previewFixture(const AccountingFixture& fixture) const
{
    return previewFixtureResult(fixture);
}

AccountingReplayResult AccountingReplayStubEngine::previewFixtureResult(const AccountingFixture& fixture) const
{
    if (fixture.fixtureId.empty()) {
        return AccountingReplayResultMapper::makeInvalidFixtureResult("Fixture id is empty.");
    }

    return AccountingReplayResultMapper::makeNotImplementedResult(fixture);
}

} // namespace etfdt::tests::accounting
