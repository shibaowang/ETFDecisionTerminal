#pragma once

#include <QJsonObject>

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace etfdt::tests::accounting {

struct AccountingFixtureIndexEntry {
    std::string fixtureId;
    std::string file;
    std::string expectedStatus;
    bool blockingIssue{false};
};

struct AccountingFixtureIndex {
    std::string fixtureSet;
    std::string schemaVersion;
    std::string description;
    std::vector<std::string> externalDependencies;
    std::vector<AccountingFixtureIndexEntry> entries;
};

struct AccountingTradeFact {
    std::string factId;
    std::string tradeTime;
    std::string accountId;
    std::string portfolioId;
    std::string instrumentCode;
    std::string action;
    std::string quantityText;
    std::string priceText;
    std::string amountText;
    std::string feeText;
    std::string cashFlowText;
    std::string currency;
    std::string source;
    std::string note;
};

struct AccountingCashFact {
    std::string factId;
    std::string time;
    std::string accountId;
    std::string portfolioId;
    std::string action;
    std::string amountText;
    std::string currency;
    std::string note;
};

struct AccountingMarketPriceFact {
    std::string instrumentCode;
    std::string priceTime;
    std::string priceText;
    std::string currency;
    std::string source;
};

struct AccountingExpectedIssue {
    std::string level;
    std::string code;
    std::string message;
    bool blocking{false};
};

struct AccountingFixture {
    std::string fixtureId;
    std::string name;
    std::string purpose;
    std::string schemaVersion;
    std::vector<std::string> externalDependencies;
    QJsonObject defaultScope;
    std::vector<AccountingTradeFact> tradeFacts;
    std::vector<AccountingCashFact> cashFacts;
    std::vector<AccountingMarketPriceFact> marketPriceFacts;
    QJsonObject expectedOutputsRawJson;
    std::vector<AccountingExpectedIssue> expectedIssues;
    bool blocking{false};
    std::vector<std::string> notes;
};

class AccountingFixtureLoader {
public:
    [[nodiscard]] bool loadIndex(const std::filesystem::path& fixturesDir);
    [[nodiscard]] bool loadFixture(
        const std::filesystem::path& fixturesDir,
        const std::string& fixtureFile,
        AccountingFixture* fixture);
    [[nodiscard]] bool loadAll(const std::filesystem::path& fixturesDir);

    [[nodiscard]] const AccountingFixtureIndex& index() const noexcept;
    [[nodiscard]] std::optional<AccountingFixture> fixtureById(const std::string& fixtureId) const;
    [[nodiscard]] std::vector<std::string> fixtureIds() const;
    [[nodiscard]] std::string lastError() const;

    [[nodiscard]] bool validateIndex(const AccountingFixtureIndex& index);
    [[nodiscard]] bool validateFixtureStructure(const AccountingFixture& fixture);

private:
    void setError(std::string message);

    AccountingFixtureIndex index_;
    std::vector<AccountingFixture> fixtures_;
    std::string lastError_;
};

} // namespace etfdt::tests::accounting
