#include "AccountingReplayStubEngine.h"

namespace etfdt::tests::accounting {

AccountingReplayStubResult AccountingReplayStubEngine::previewFixture(const AccountingFixture& fixture) const
{
    if (fixture.fixtureId.empty()) {
        return AccountingReplayStubResult{
            "",
            false,
            false,
            "INVALID_FIXTURE",
            "Fixture id is empty.",
            "Provide a valid fixture id before replay implementation.",
            "",
            0,
            false,
        };
    }

    return AccountingReplayStubResult{
        fixture.fixtureId,
        false,
        false,
        "NOT_IMPLEMENTED",
        "Fixture-backed accounting replay is not implemented yet.",
        "Implement fixture-backed accounting replay.",
        fixture.fixtureId,
        static_cast<int>(fixture.expectedIssues.size()),
        fixture.blocking,
    };
}

} // namespace etfdt::tests::accounting
