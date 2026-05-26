#include "AccountingExpectedOutputInspector.h"

#include <QJsonArray>
#include <QJsonValue>
#include <QString>

#include <algorithm>

namespace etfdt::tests::accounting {
namespace {

std::string optionalString(const QJsonObject& object, const char* key)
{
    const auto value = object.value(QString::fromUtf8(key));
    return value.isString() ? value.toString().toStdString() : std::string{};
}

std::string firstDataQualityStatus(const QJsonObject& expectedOutputs)
{
    const auto cashSummary = expectedOutputs.value("cashSummary");
    if (cashSummary.isObject()) {
        const auto status = optionalString(cashSummary.toObject(), "dataQualityStatus");
        if (!status.empty()) {
            return status;
        }
    }

    const auto portfolioPnl = expectedOutputs.value("portfolioPnl");
    if (portfolioPnl.isObject()) {
        const auto status = optionalString(portfolioPnl.toObject(), "dataQualityStatus");
        if (!status.empty()) {
            return status;
        }
    }

    const auto positionSummaries = expectedOutputs.value("positionSummaries");
    if (positionSummaries.isArray() && !positionSummaries.toArray().isEmpty()) {
        const auto firstPosition = positionSummaries.toArray().first();
        if (firstPosition.isObject()) {
            return optionalString(firstPosition.toObject(), "dataQualityStatus");
        }
    }

    return {};
}

} // namespace

ExpectedOutputShape AccountingExpectedOutputInspector::inspect(const AccountingFixture& fixture) const
{
    ExpectedOutputShape shape;
    shape.fixtureId = fixture.fixtureId;
    shape.issueCount = static_cast<int>(fixture.expectedIssues.size());
    shape.blocking = fixture.blocking;

    const auto& expectedOutputs = fixture.expectedOutputsRawJson;
    const auto positionSummaries = expectedOutputs.value("positionSummaries");
    shape.hasPositionSummaries = positionSummaries.isArray();
    shape.positionSummaryCount = shape.hasPositionSummaries ? positionSummaries.toArray().size() : 0;
    shape.hasCashSummary = expectedOutputs.value("cashSummary").isObject();
    shape.hasPortfolioPnl = expectedOutputs.value("portfolioPnl").isObject();
    shape.hasBasePosition = expectedOutputs.contains("basePosition");
    shape.hasSniperPool = expectedOutputs.contains("sniperPool");
    shape.dataQualityStatusText = firstDataQualityStatus(expectedOutputs);

    if (!shape.hasPositionSummaries) {
        shape.lastError = "expectedOutputs.positionSummaries is missing or is not an array";
    } else if (!shape.hasCashSummary) {
        shape.lastError = "expectedOutputs.cashSummary is missing or is not an object";
    } else if (!shape.hasPortfolioPnl) {
        shape.lastError = "expectedOutputs.portfolioPnl is missing or is not an object";
    }

    return shape;
}

bool AccountingExpectedOutputInspector::hasRequiredExpectedOutputs(const AccountingFixture& fixture) const
{
    const auto shape = inspect(fixture);
    return shape.hasPositionSummaries && shape.hasCashSummary && shape.hasPortfolioPnl;
}

std::vector<std::string> AccountingExpectedOutputInspector::expectedIssueCodes(const AccountingFixture& fixture) const
{
    std::vector<std::string> codes;
    codes.reserve(fixture.expectedIssues.size());
    for (const auto& issue : fixture.expectedIssues) {
        codes.push_back(issue.code);
    }
    return codes;
}

int AccountingExpectedOutputInspector::positionSummaryCount(const AccountingFixture& fixture) const
{
    return inspect(fixture).positionSummaryCount;
}

bool AccountingExpectedOutputInspector::hasErrorCode(const AccountingFixture& fixture, const std::string& code) const
{
    const auto codes = expectedIssueCodes(fixture);
    return std::any_of(codes.begin(), codes.end(), [&](const std::string& current) {
        return current == code;
    });
}

} // namespace etfdt::tests::accounting
