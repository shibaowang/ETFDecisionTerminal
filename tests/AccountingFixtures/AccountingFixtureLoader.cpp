#include "AccountingFixtureLoader.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QString>

#include <algorithm>
#include <set>
#include <sstream>

namespace etfdt::tests::accounting {
namespace {

constexpr const char* kIndexFile = "fixtures_index.json";

const std::vector<std::string>& requiredFixtureIds()
{
    static const std::vector<std::string> ids{
        "FX001_EMPTY_LEDGER",
        "FX002_SINGLE_BUY",
        "FX003_BUY_SELL_PARTIAL",
        "FX004_SELL_EXCEEDS_POSITION",
        "FX005_MISSING_FEE",
        "FX006_NEGATIVE_CASH",
        "FX007_MULTI_INSTRUMENT",
        "FX008_MULTI_ACCOUNT",
        "FX009_BASE_POSITION_LOCKED",
        "FX010_SNIPER_TIER_COMPLETED",
        "FX011_STALE_SNAPSHOT",
        "FX012_MISSING_MARKET_PRICE",
        "FX013_MULTI_CURRENCY_UNSUPPORTED",
    };
    return ids;
}

const std::vector<std::pair<std::string, std::vector<std::string>>>& requiredIssueCodes()
{
    static const std::vector<std::pair<std::string, std::vector<std::string>>> codes{
        {"FX004_SELL_EXCEEDS_POSITION", {"SELL_EXCEEDS_POSITION"}},
        {"FX005_MISSING_FEE", {"MISSING_FEE"}},
        {"FX006_NEGATIVE_CASH", {"NEGATIVE_CASH"}},
        {"FX011_STALE_SNAPSHOT", {"SNAPSHOT_STALE"}},
        {"FX012_MISSING_MARKET_PRICE", {"MARKET_PRICE_MISSING"}},
        {"FX013_MULTI_CURRENCY_UNSUPPORTED", {"MULTI_CURRENCY_UNSUPPORTED", "FX_RATE_MISSING"}},
    };
    return codes;
}

std::string stringValue(const QJsonObject& object, const char* key)
{
    return object.value(QString::fromUtf8(key)).toString().toStdString();
}

std::vector<std::string> stringArray(const QJsonArray& array)
{
    std::vector<std::string> values;
    values.reserve(static_cast<std::size_t>(array.size()));
    for (const auto& value : array) {
        values.push_back(value.toString().toStdString());
    }
    return values;
}

bool readJsonObject(const std::filesystem::path& path, QJsonObject* object, std::string* error)
{
    QFile file(QString::fromStdWString(path.wstring()));
    if (!file.exists()) {
        *error = "Missing JSON file: " + path.string();
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        *error = "Cannot open JSON file: " + path.string();
        return false;
    }

    QJsonParseError parseError{};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        *error = "Invalid JSON in " + path.string() + ": " + parseError.errorString().toStdString();
        return false;
    }
    if (!document.isObject()) {
        *error = "JSON root must be an object: " + path.string();
        return false;
    }

    *object = document.object();
    return true;
}

bool requireObjectKey(const QJsonObject& object, const char* key, std::string* error)
{
    if (!object.contains(QString::fromUtf8(key))) {
        *error = std::string("Missing field: ") + key;
        return false;
    }
    return true;
}

AccountingTradeFact parseTradeFact(const QJsonObject& object)
{
    return AccountingTradeFact{
        stringValue(object, "factId"),
        stringValue(object, "tradeTime"),
        stringValue(object, "accountId"),
        stringValue(object, "portfolioId"),
        stringValue(object, "instrumentCode"),
        stringValue(object, "action"),
        stringValue(object, "quantityText"),
        stringValue(object, "priceText"),
        stringValue(object, "amountText"),
        stringValue(object, "feeText"),
        stringValue(object, "cashFlowText"),
        stringValue(object, "currency"),
        stringValue(object, "source"),
        stringValue(object, "note"),
    };
}

AccountingCashFact parseCashFact(const QJsonObject& object)
{
    return AccountingCashFact{
        stringValue(object, "factId"),
        stringValue(object, "time"),
        stringValue(object, "accountId"),
        stringValue(object, "portfolioId"),
        stringValue(object, "action"),
        stringValue(object, "amountText"),
        stringValue(object, "currency"),
        stringValue(object, "note"),
    };
}

AccountingMarketPriceFact parseMarketPriceFact(const QJsonObject& object)
{
    return AccountingMarketPriceFact{
        stringValue(object, "instrumentCode"),
        stringValue(object, "priceTime"),
        stringValue(object, "priceText"),
        stringValue(object, "currency"),
        stringValue(object, "source"),
    };
}

AccountingExpectedIssue parseIssue(const QJsonObject& object)
{
    return AccountingExpectedIssue{
        stringValue(object, "level"),
        stringValue(object, "code"),
        stringValue(object, "message"),
        object.value("blocking").toBool(false),
    };
}

template <typename T, typename ParseFn>
std::vector<T> parseObjectArray(const QJsonArray& array, ParseFn parse)
{
    std::vector<T> values;
    values.reserve(static_cast<std::size_t>(array.size()));
    for (const auto& value : array) {
        if (value.isObject()) {
            values.push_back(parse(value.toObject()));
        }
    }
    return values;
}

bool hasIssueCode(const AccountingFixture& fixture, const std::string& code)
{
    return std::any_of(
        fixture.expectedIssues.begin(),
        fixture.expectedIssues.end(),
        [&](const AccountingExpectedIssue& issue) { return issue.code == code; });
}

} // namespace

bool AccountingFixtureLoader::loadIndex(const std::filesystem::path& fixturesDir)
{
    index_ = AccountingFixtureIndex{};
    fixtures_.clear();
    lastError_.clear();

    QJsonObject object;
    std::string error;
    if (!readJsonObject(fixturesDir / kIndexFile, &object, &error)) {
        setError(error);
        return false;
    }

    index_.fixtureSet = stringValue(object, "fixtureSet");
    index_.schemaVersion = stringValue(object, "schemaVersion");
    index_.description = stringValue(object, "description");
    index_.externalDependencies = stringArray(object.value("externalDependencies").toArray());

    const auto entries = object.value("fixtures").toArray();
    index_.entries.reserve(static_cast<std::size_t>(entries.size()));
    for (const auto& value : entries) {
        if (!value.isObject()) {
            setError("Index fixture entry must be an object");
            return false;
        }
        const auto entryObject = value.toObject();
        index_.entries.push_back(AccountingFixtureIndexEntry{
            stringValue(entryObject, "fixtureId"),
            stringValue(entryObject, "file"),
            stringValue(entryObject, "expectedStatus"),
            entryObject.value("blockingIssue").toBool(false),
        });
    }

    return validateIndex(index_);
}

bool AccountingFixtureLoader::loadFixture(
    const std::filesystem::path& fixturesDir,
    const std::string& fixtureFile,
    AccountingFixture* fixture)
{
    if (fixture == nullptr) {
        setError("Fixture output pointer is null");
        return false;
    }

    QJsonObject object;
    std::string error;
    if (!readJsonObject(fixturesDir / fixtureFile, &object, &error)) {
        setError(error);
        return false;
    }

    for (const char* key : {"fixtureId", "name", "purpose", "schemaVersion", "externalDependencies",
             "defaultScope", "inputFacts", "expectedOutputs", "expectedIssues", "blocking", "notes"}) {
        if (!requireObjectKey(object, key, &error)) {
            setError(error + " in " + fixtureFile);
            return false;
        }
    }

    const auto inputFacts = object.value("inputFacts").toObject();
    for (const char* key : {"tradeFacts", "cashFacts", "marketPriceFacts"}) {
        if (!requireObjectKey(inputFacts, key, &error) || !inputFacts.value(key).isArray()) {
            setError(std::string("Missing array inputFacts.") + key + " in " + fixtureFile);
            return false;
        }
    }

    const auto expectedOutputs = object.value("expectedOutputs").toObject();
    for (const char* key : {"positionSummaries", "cashSummary", "portfolioPnl"}) {
        if (!requireObjectKey(expectedOutputs, key, &error)) {
            setError(std::string("Missing expectedOutputs.") + key + " in " + fixtureFile);
            return false;
        }
    }

    if (!object.value("expectedIssues").isArray()) {
        setError("expectedIssues must be an array in " + fixtureFile);
        return false;
    }
    if (!object.value("blocking").isBool()) {
        setError("blocking must be a boolean in " + fixtureFile);
        return false;
    }
    if (!object.value("notes").isArray()) {
        setError("notes must be an array in " + fixtureFile);
        return false;
    }

    *fixture = AccountingFixture{
        stringValue(object, "fixtureId"),
        stringValue(object, "name"),
        stringValue(object, "purpose"),
        stringValue(object, "schemaVersion"),
        stringArray(object.value("externalDependencies").toArray()),
        object.value("defaultScope").toObject(),
        parseObjectArray<AccountingTradeFact>(inputFacts.value("tradeFacts").toArray(), parseTradeFact),
        parseObjectArray<AccountingCashFact>(inputFacts.value("cashFacts").toArray(), parseCashFact),
        parseObjectArray<AccountingMarketPriceFact>(
            inputFacts.value("marketPriceFacts").toArray(),
            parseMarketPriceFact),
        expectedOutputs,
        parseObjectArray<AccountingExpectedIssue>(object.value("expectedIssues").toArray(), parseIssue),
        object.value("blocking").toBool(false),
        stringArray(object.value("notes").toArray()),
    };

    return validateFixtureStructure(*fixture);
}

bool AccountingFixtureLoader::loadAll(const std::filesystem::path& fixturesDir)
{
    if (!loadIndex(fixturesDir)) {
        return false;
    }

    fixtures_.clear();
    for (const auto& entry : index_.entries) {
        AccountingFixture fixture;
        if (!loadFixture(fixturesDir, entry.file, &fixture)) {
            return false;
        }
        if (fixture.fixtureId != entry.fixtureId) {
            setError("fixtureId mismatch between index and file: " + entry.fixtureId);
            return false;
        }
        fixtures_.push_back(std::move(fixture));
    }

    return true;
}

const AccountingFixtureIndex& AccountingFixtureLoader::index() const noexcept
{
    return index_;
}

std::optional<AccountingFixture> AccountingFixtureLoader::fixtureById(const std::string& fixtureId) const
{
    const auto it = std::find_if(fixtures_.begin(), fixtures_.end(), [&](const AccountingFixture& fixture) {
        return fixture.fixtureId == fixtureId;
    });
    if (it == fixtures_.end()) {
        return std::nullopt;
    }
    return *it;
}

std::vector<std::string> AccountingFixtureLoader::fixtureIds() const
{
    std::vector<std::string> ids;
    ids.reserve(fixtures_.size());
    for (const auto& fixture : fixtures_) {
        ids.push_back(fixture.fixtureId);
    }
    return ids;
}

std::string AccountingFixtureLoader::lastError() const
{
    return lastError_;
}

bool AccountingFixtureLoader::validateIndex(const AccountingFixtureIndex& index)
{
    if (!index.externalDependencies.empty()) {
        setError("Index externalDependencies must be empty");
        return false;
    }
    if (index.entries.size() != requiredFixtureIds().size()) {
        setError("Index must contain exactly 13 fixtures");
        return false;
    }

    std::set<std::string> seen;
    for (const auto& entry : index.entries) {
        if (entry.fixtureId.empty() || entry.file.empty()) {
            setError("Index entries must include fixtureId and file");
            return false;
        }
        if (!seen.insert(entry.fixtureId).second) {
            setError("Duplicate fixtureId in index: " + entry.fixtureId);
            return false;
        }
    }

    for (const auto& fixtureId : requiredFixtureIds()) {
        if (seen.find(fixtureId) == seen.end()) {
            setError("Missing required fixture in index: " + fixtureId);
            return false;
        }
    }

    return true;
}

bool AccountingFixtureLoader::validateFixtureStructure(const AccountingFixture& fixture)
{
    if (fixture.fixtureId.empty()) {
        setError("Fixture has empty fixtureId");
        return false;
    }
    if (!fixture.externalDependencies.empty()) {
        setError("Fixture externalDependencies must be empty: " + fixture.fixtureId);
        return false;
    }
    if (fixture.expectedOutputsRawJson.isEmpty()) {
        setError("Fixture expectedOutputs must be present: " + fixture.fixtureId);
        return false;
    }

    for (const auto& [fixtureId, acceptedCodes] : requiredIssueCodes()) {
        if (fixture.fixtureId != fixtureId) {
            continue;
        }

        const bool matched = std::any_of(acceptedCodes.begin(), acceptedCodes.end(), [&](const std::string& code) {
            return hasIssueCode(fixture, code);
        });
        if (!matched) {
            std::ostringstream message;
            message << "Fixture " << fixture.fixtureId << " must contain one of:";
            for (const auto& code : acceptedCodes) {
                message << ' ' << code;
            }
            setError(message.str());
            return false;
        }
    }

    return true;
}

void AccountingFixtureLoader::setError(std::string message)
{
    lastError_ = std::move(message);
}

} // namespace etfdt::tests::accounting
