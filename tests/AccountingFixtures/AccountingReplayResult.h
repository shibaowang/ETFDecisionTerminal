#pragma once

#include <QJsonObject>

#include <string>
#include <vector>

namespace etfdt::tests::accounting {

inline constexpr const char* kReplayStatusNotImplemented = "NOT_IMPLEMENTED";
inline constexpr const char* kReplayStatusOk = "OK";
inline constexpr const char* kReplayStatusInvalidFixture = "INVALID_FIXTURE";
inline constexpr const char* kReplayStatusUnsupported = "UNSUPPORTED";
inline constexpr const char* kReplayStatusError = "ERROR";

struct AccountingReplayIssue {
    std::string level;
    std::string code;
    std::string message;
    bool blocking{false};
    std::string fixtureId;
    std::string source;
};

struct AccountingReplayMetadata {
    std::string fixtureId;
    std::string sourceFixtureId;
    std::string schemaVersion;
    std::string generatedAtText;
    std::string requiredFutureTask;
    int fixtureIssueCount{0};
    bool fixtureBlockingExpected{false};
};

struct AccountingReplayResult {
    std::string fixtureId;
    bool implemented{false};
    bool replayExecuted{false};
    std::string status;
    std::string message;
    AccountingReplayMetadata metadata;
    std::vector<AccountingReplayIssue> issues;
    QJsonObject positionListResponseRaw;
    QJsonObject cashSummaryRaw;
    QJsonObject portfolioPnlRaw;
    QJsonObject basePositionRaw;
    QJsonObject sniperPoolRaw;
};

[[nodiscard]] bool hasReplayIssueCode(const AccountingReplayResult& result, const std::string& code);
[[nodiscard]] bool hasEmptyReplayOutputs(const AccountingReplayResult& result);
[[nodiscard]] bool hasPositionListResponse(const AccountingReplayResult& result);
[[nodiscard]] bool hasCashSummary(const AccountingReplayResult& result);
[[nodiscard]] bool hasPortfolioPnl(const AccountingReplayResult& result);
[[nodiscard]] bool hasBasePosition(const AccountingReplayResult& result);
[[nodiscard]] bool hasSniperPool(const AccountingReplayResult& result);

} // namespace etfdt::tests::accounting
