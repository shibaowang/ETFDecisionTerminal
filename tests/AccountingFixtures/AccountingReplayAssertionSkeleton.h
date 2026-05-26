#pragma once

#include "AccountingFixtureLoader.h"
#include "AccountingReplayResult.h"

#include <string>
#include <vector>

namespace etfdt::tests::accounting {

inline constexpr const char* kAssertionPassNotImplementedGuard = "PASS_NOT_IMPLEMENTED_GUARD";
inline constexpr const char* kAssertionFailUnexpectedImplemented = "FAIL_UNEXPECTED_IMPLEMENTED";
inline constexpr const char* kAssertionFailResultOutputNotEmpty = "FAIL_RESULT_OUTPUT_NOT_EMPTY";
inline constexpr const char* kAssertionFailMissingExpectedOutput = "FAIL_MISSING_EXPECTED_OUTPUT";
inline constexpr const char* kAssertionPassFx001EmptyLedger = "PASS_FX001_EMPTY_LEDGER";
inline constexpr const char* kAssertionPassFx002SingleBuy = "PASS_FX002_SINGLE_BUY";
inline constexpr const char* kAssertionFailInvalidFx001EmptyLedger = "FAIL_INVALID_FX001_EMPTY_LEDGER";
inline constexpr const char* kAssertionFailInvalidFx002SingleBuy = "FAIL_INVALID_FX002_SINGLE_BUY";
inline constexpr const char* kAssertionSkippedByDesign = "SKIPPED_BY_DESIGN";

struct AccountingAssertionResult {
    std::string fixtureId;
    bool checked{false};
    bool passed{false};
    std::string status;
    std::string message;
    bool skippedRealAssertions{false};
    std::string requiredFutureTask;
    std::vector<std::string> checkedFields;
};

class AccountingReplayAssertionSkeleton {
public:
    [[nodiscard]] AccountingAssertionResult assertCurrentStubResult(
        const AccountingFixture& fixture,
        const AccountingReplayResult& result) const;
    [[nodiscard]] AccountingAssertionResult assertFx001EmptyLedgerResult(
        const AccountingFixture& fixture,
        const AccountingReplayResult& result) const;
    [[nodiscard]] AccountingAssertionResult assertFx002SingleBuyResult(
        const AccountingFixture& fixture,
        const AccountingReplayResult& result) const;
    [[nodiscard]] AccountingAssertionResult assertExpectedOutputShape(const AccountingFixture& fixture) const;
    [[nodiscard]] AccountingAssertionResult assertPositionList(
        const AccountingFixture& fixture,
        const AccountingReplayResult& result) const;
    [[nodiscard]] AccountingAssertionResult assertCashSummary(
        const AccountingFixture& fixture,
        const AccountingReplayResult& result) const;
    [[nodiscard]] AccountingAssertionResult assertPortfolioPnl(
        const AccountingFixture& fixture,
        const AccountingReplayResult& result) const;
    [[nodiscard]] AccountingAssertionResult assertIssuesShape(const AccountingFixture& fixture) const;
};

} // namespace etfdt::tests::accounting
