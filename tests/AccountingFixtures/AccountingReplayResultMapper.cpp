#include "AccountingReplayResultMapper.h"

namespace etfdt::tests::accounting {
namespace {

constexpr const char* kRequiredFutureTask = "Implement fixture-backed accounting replay.";
constexpr const char* kStubSource = "AccountingReplayStubEngine";

AccountingReplayIssue makeIssue(
    std::string level,
    std::string code,
    std::string message,
    bool blocking,
    std::string fixtureId)
{
    return AccountingReplayIssue{
        std::move(level),
        std::move(code),
        std::move(message),
        blocking,
        std::move(fixtureId),
        kStubSource,
    };
}

} // namespace

AccountingReplayResult AccountingReplayResultMapper::makeNotImplementedResult(const AccountingFixture& fixture)
{
    AccountingReplayResult result;
    result.fixtureId = fixture.fixtureId;
    result.implemented = false;
    result.replayExecuted = false;
    result.status = kReplayStatusNotImplemented;
    result.message = "Accounting replay is not implemented yet.";
    result.metadata = AccountingReplayMetadata{
        fixture.fixtureId,
        fixture.fixtureId,
        fixture.schemaVersion,
        "",
        kRequiredFutureTask,
        static_cast<int>(fixture.expectedIssues.size()),
        fixture.blocking,
    };
    result.issues.push_back(makeIssue(
        "WARNING",
        "REPLAY_NOT_IMPLEMENTED",
        "Accounting replay is not implemented yet.",
        false,
        fixture.fixtureId));
    return result;
}

AccountingReplayResult AccountingReplayResultMapper::makeInvalidFixtureResult(const std::string& reason)
{
    AccountingReplayResult result;
    result.implemented = false;
    result.replayExecuted = false;
    result.status = kReplayStatusInvalidFixture;
    result.message = reason;
    result.metadata = AccountingReplayMetadata{
        "",
        "",
        "",
        "",
        "Provide a valid fixture before replay implementation.",
        0,
        false,
    };
    result.issues.push_back(makeIssue("ERROR", "INVALID_FIXTURE", reason, true, ""));
    return result;
}

} // namespace etfdt::tests::accounting
