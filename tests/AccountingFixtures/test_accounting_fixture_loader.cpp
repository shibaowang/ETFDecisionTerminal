#include "AccountingFixtureLoader.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

using etfdt::tests::accounting::AccountingFixture;
using etfdt::tests::accounting::AccountingFixtureLoader;

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAILED: " << message << '\n';
        std::exit(1);
    }
}

std::filesystem::path fixtureDirFromArgs(int argc, char** argv)
{
    if (argc >= 2) {
        return std::filesystem::path(argv[1]);
    }
    return std::filesystem::path("tests") / "fixtures" / "accounting_replay";
}

bool hasIssueCode(const AccountingFixture& fixture, const std::string& code)
{
    for (const auto& issue : fixture.expectedIssues) {
        if (issue.code == code) {
            return true;
        }
    }
    return false;
}

bool hasTradeAction(const AccountingFixture& fixture, const std::string& action)
{
    for (const auto& tradeFact : fixture.tradeFacts) {
        if (tradeFact.action == action) {
            return true;
        }
    }
    return false;
}

AccountingFixture requireFixture(const AccountingFixtureLoader& loader, const std::string& fixtureId)
{
    auto fixture = loader.fixtureById(fixtureId);
    require(fixture.has_value(), "fixture exists: " + fixtureId);
    return *fixture;
}

} // namespace

int main(int argc, char** argv)
{
    const auto fixturesDir = fixtureDirFromArgs(argc, argv);

    AccountingFixtureLoader indexLoader;
    require(indexLoader.loadIndex(fixturesDir), "loadIndex succeeds: " + indexLoader.lastError());
    require(indexLoader.index().entries.size() == 13, "index contains 13 entries");
    require(indexLoader.index().externalDependencies.empty(), "index externalDependencies is empty");

    AccountingFixtureLoader loader;
    require(loader.loadAll(fixturesDir), "loadAll succeeds: " + loader.lastError());
    require(loader.fixtureIds().size() == 13, "fixtureIds size is 13");
    require(loader.fixtureById("FX001_EMPTY_LEDGER").has_value(), "FX001 exists");
    require(loader.fixtureById("FX013_MULTI_CURRENCY_UNSUPPORTED").has_value(), "FX013 exists");

    const auto fx001 = requireFixture(loader, "FX001_EMPTY_LEDGER");
    require(fx001.tradeFacts.empty(), "FX001 tradeFacts is empty");
    require(fx001.cashFacts.empty(), "FX001 cashFacts is empty");
    require(fx001.marketPriceFacts.empty(), "FX001 marketPriceFacts is empty");
    require(!fx001.blocking, "FX001 is non-blocking");

    const auto fx002 = requireFixture(loader, "FX002_SINGLE_BUY");
    require(!fx002.tradeFacts.empty(), "FX002 tradeFacts is non-empty");
    require(!fx002.cashFacts.empty(), "FX002 cashFacts is non-empty");
    require(fx002.tradeFacts.front().instrumentCode == "159509", "FX002 parses trade instrument code");
    require(fx002.cashFacts.front().action == "INITIAL_CASH", "FX002 parses cash action");

    const auto fx003 = requireFixture(loader, "FX003_BUY_SELL_PARTIAL");
    require(hasTradeAction(fx003, "BUY"), "FX003 contains BUY");
    require(hasTradeAction(fx003, "SELL"), "FX003 contains SELL");

    const auto fx004 = requireFixture(loader, "FX004_SELL_EXCEEDS_POSITION");
    require(hasIssueCode(fx004, "SELL_EXCEEDS_POSITION"), "FX004 has SELL_EXCEEDS_POSITION");
    require(fx004.blocking, "FX004 is blocking");

    const auto fx005 = requireFixture(loader, "FX005_MISSING_FEE");
    require(hasIssueCode(fx005, "MISSING_FEE"), "FX005 has MISSING_FEE");

    const auto fx006 = requireFixture(loader, "FX006_NEGATIVE_CASH");
    require(hasIssueCode(fx006, "NEGATIVE_CASH"), "FX006 has NEGATIVE_CASH");
    require(fx006.blocking, "FX006 is blocking");

    const auto fx011 = requireFixture(loader, "FX011_STALE_SNAPSHOT");
    require(hasIssueCode(fx011, "SNAPSHOT_STALE"), "FX011 has SNAPSHOT_STALE");

    const auto fx012 = requireFixture(loader, "FX012_MISSING_MARKET_PRICE");
    require(hasIssueCode(fx012, "MARKET_PRICE_MISSING"), "FX012 has MARKET_PRICE_MISSING");

    const auto fx013 = requireFixture(loader, "FX013_MULTI_CURRENCY_UNSUPPORTED");
    require(
        hasIssueCode(fx013, "MULTI_CURRENCY_UNSUPPORTED") || hasIssueCode(fx013, "FX_RATE_MISSING"),
        "FX013 has multi-currency or FX issue");
    require(fx013.blocking, "FX013 is blocking");

    for (const auto& fixtureId : loader.fixtureIds()) {
        const auto fixture = requireFixture(loader, fixtureId);
        require(fixture.externalDependencies.empty(), fixtureId + " externalDependencies is empty");
        require(!fixture.expectedOutputsRawJson.isEmpty(), fixtureId + " expectedOutputs exists");
    }

    require(!loader.fixtureById("FX999_MISSING").has_value(), "missing fixture id returns empty optional");

    AccountingFixtureLoader missingLoader;
    require(!missingLoader.loadAll(fixturesDir / "missing"), "invalid fixture dir fails");
    require(!missingLoader.lastError().empty(), "invalid fixture dir reports an error");

    std::cout << "Accounting fixture loader tests passed\n";
    return 0;
}
