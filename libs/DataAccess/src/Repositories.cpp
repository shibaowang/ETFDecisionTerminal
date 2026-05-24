#include "DataAccess/Repositories.h"

#include "Protocol/ErrorCode.h"

#include <sstream>
#include <utility>

namespace etfdt::data_access {
namespace {

constexpr const char* kAccountColumns =
    "id, uid, name, account_type, COALESCE(broker_name, ''), base_currency, "
    "is_active, initial_cash_cents";

constexpr const char* kPortfolioColumns =
    "id, uid, name, base_position_ratio_ppm, is_active";

constexpr const char* kInstrumentColumns =
    "id, uid, code, name, instrument_type, market_code, currency, enabled";

constexpr const char* kStrategyColumns =
    "id, uid, strategy_code, instrument_id, name, enabled, "
    "external_premium_threshold_ppm, take_profit_premium_threshold_ppm, "
    "return_take_profit_ratio_ppm, add_premium_limit_ppm, adjustment_factor_1e6";

constexpr const char* kOtcChannelColumns =
    "id, uid, strategy_code, actual_code, fund_class, enabled, daily_limit_cents, "
    "priority, min_buy_amount_cents";

constexpr const char* kSchemaVersionColumns =
    "id, uid, version, migration_name, COALESCE(checksum, ''), applied_at_utc";

template <typename T>
RepositoryResult<T> failure(std::string message)
{
    return RepositoryResult<T>::failure(
        etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
        std::move(message));
}

RepositoryResult<bool> requireColumnCount(
    const SqlQueryRow& row,
    std::size_t expected,
    const std::string& recordName)
{
    if (row.size() != expected) {
        std::ostringstream message;
        message << recordName << " query returned " << row.size() << " columns, expected "
                << expected;
        return failure<bool>(message.str());
    }
    return RepositoryResult<bool>::success(true);
}

RepositoryResult<etfdt::core::Uuid> parseUuid(
    const SqlValue& value,
    const std::string& fieldName)
{
    etfdt::core::Uuid uid(value.text);
    if (!uid.isValidBasicFormat()) {
        return failure<etfdt::core::Uuid>("Invalid " + fieldName + ": empty uid");
    }
    return RepositoryResult<etfdt::core::Uuid>::success(std::move(uid));
}

template <typename Enum>
RepositoryResult<Enum> parseEnum(const SqlValue& value, const std::string& fieldName)
{
    auto parsed = etfdt::core::fromString<Enum>(value.text);
    if (!parsed) {
        return failure<Enum>("Invalid " + fieldName + ": " + parsed.error());
    }
    return RepositoryResult<Enum>::success(parsed.value());
}

RepositoryResult<etfdt::core::InstrumentCode> parseInstrumentCode(
    const SqlValue& value,
    const std::string& fieldName)
{
    auto parsed = etfdt::core::InstrumentCode::fromString(value.text);
    if (!parsed) {
        return failure<etfdt::core::InstrumentCode>(
            "Invalid " + fieldName + ": " + parsed.error());
    }
    return RepositoryResult<etfdt::core::InstrumentCode>::success(parsed.value());
}

RepositoryResult<etfdt::core::StrategyCode> parseStrategyCode(
    const SqlValue& value,
    const std::string& fieldName)
{
    auto parsed = etfdt::core::StrategyCode::fromString(value.text);
    if (!parsed) {
        return failure<etfdt::core::StrategyCode>(
            "Invalid " + fieldName + ": " + parsed.error());
    }
    return RepositoryResult<etfdt::core::StrategyCode>::success(parsed.value());
}

template <typename T, typename Parser>
RepositoryResult<std::vector<T>> mapRows(
    const std::vector<SqlQueryRow>& rows,
    Parser parser)
{
    std::vector<T> records;
    records.reserve(rows.size());
    for (const auto& row : rows) {
        auto record = parser(row);
        if (!record) {
            return RepositoryResult<std::vector<T>>::failure(
                record.error().errorCode,
                record.error().message);
        }
        records.push_back(record.value());
    }
    return RepositoryResult<std::vector<T>>::success(std::move(records));
}

RepositoryResult<AccountRecord> parseAccountRecord(const SqlQueryRow& row)
{
    auto columnCount = requireColumnCount(row, 8, "account");
    if (!columnCount) {
        return RepositoryResult<AccountRecord>::failure(
            columnCount.error().errorCode,
            columnCount.error().message);
    }

    auto uid = parseUuid(row[1], "account.uid");
    if (!uid) {
        return RepositoryResult<AccountRecord>::failure(uid.error().errorCode, uid.error().message);
    }
    auto accountType = parseEnum<etfdt::core::AccountType>(row[3], "account.account_type");
    if (!accountType) {
        return RepositoryResult<AccountRecord>::failure(
            accountType.error().errorCode,
            accountType.error().message);
    }

    AccountRecord record;
    record.id = row[0].int64Value;
    record.uid = uid.value();
    record.name = row[2].text;
    record.accountType = accountType.value();
    record.brokerName = row[4].text;
    record.baseCurrency = row[5].text;
    record.isActive = row[6].int64Value != 0;
    record.initialCashCents = etfdt::core::MoneyCents::fromRaw(row[7].int64Value);
    return RepositoryResult<AccountRecord>::success(std::move(record));
}

RepositoryResult<PortfolioRecord> parsePortfolioRecord(const SqlQueryRow& row)
{
    auto columnCount = requireColumnCount(row, 5, "portfolio");
    if (!columnCount) {
        return RepositoryResult<PortfolioRecord>::failure(
            columnCount.error().errorCode,
            columnCount.error().message);
    }

    auto uid = parseUuid(row[1], "portfolio.uid");
    if (!uid) {
        return RepositoryResult<PortfolioRecord>::failure(
            uid.error().errorCode,
            uid.error().message);
    }

    PortfolioRecord record;
    record.id = row[0].int64Value;
    record.uid = uid.value();
    record.name = row[2].text;
    record.basePositionRatioPpm = etfdt::core::RatioPpm::fromRaw(row[3].int64Value);
    record.isActive = row[4].int64Value != 0;
    return RepositoryResult<PortfolioRecord>::success(std::move(record));
}

RepositoryResult<InstrumentRecord> parseInstrumentRecord(const SqlQueryRow& row)
{
    auto columnCount = requireColumnCount(row, 8, "instrument");
    if (!columnCount) {
        return RepositoryResult<InstrumentRecord>::failure(
            columnCount.error().errorCode,
            columnCount.error().message);
    }

    auto uid = parseUuid(row[1], "instrument.uid");
    if (!uid) {
        return RepositoryResult<InstrumentRecord>::failure(
            uid.error().errorCode,
            uid.error().message);
    }
    auto code = parseInstrumentCode(row[2], "instrument.code");
    if (!code) {
        return RepositoryResult<InstrumentRecord>::failure(
            code.error().errorCode,
            code.error().message);
    }
    auto type = parseEnum<etfdt::core::InstrumentType>(row[4], "instrument.instrument_type");
    if (!type) {
        return RepositoryResult<InstrumentRecord>::failure(
            type.error().errorCode,
            type.error().message);
    }
    auto marketCode = parseEnum<etfdt::core::MarketCode>(row[5], "instrument.market_code");
    if (!marketCode) {
        return RepositoryResult<InstrumentRecord>::failure(
            marketCode.error().errorCode,
            marketCode.error().message);
    }

    InstrumentRecord record;
    record.id = row[0].int64Value;
    record.uid = uid.value();
    record.code = code.value();
    record.name = row[3].text;
    record.instrumentType = type.value();
    record.marketCode = marketCode.value();
    record.currency = row[6].text;
    record.enabled = row[7].int64Value != 0;
    return RepositoryResult<InstrumentRecord>::success(std::move(record));
}

RepositoryResult<StrategyRecord> parseStrategyRecord(const SqlQueryRow& row)
{
    auto columnCount = requireColumnCount(row, 11, "strategy");
    if (!columnCount) {
        return RepositoryResult<StrategyRecord>::failure(
            columnCount.error().errorCode,
            columnCount.error().message);
    }

    auto uid = parseUuid(row[1], "strategy.uid");
    if (!uid) {
        return RepositoryResult<StrategyRecord>::failure(
            uid.error().errorCode,
            uid.error().message);
    }
    auto strategyCode = parseStrategyCode(row[2], "strategy.strategy_code");
    if (!strategyCode) {
        return RepositoryResult<StrategyRecord>::failure(
            strategyCode.error().errorCode,
            strategyCode.error().message);
    }

    StrategyRecord record;
    record.id = row[0].int64Value;
    record.uid = uid.value();
    record.strategyCode = strategyCode.value();
    record.instrumentId = row[3].int64Value;
    record.name = row[4].text;
    record.enabled = row[5].int64Value != 0;
    record.externalPremiumThresholdPpm = etfdt::core::RatioPpm::fromRaw(row[6].int64Value);
    record.takeProfitPremiumThresholdPpm = etfdt::core::RatioPpm::fromRaw(row[7].int64Value);
    record.returnTakeProfitRatioPpm = etfdt::core::RatioPpm::fromRaw(row[8].int64Value);
    record.addPremiumLimitPpm = etfdt::core::RatioPpm::fromRaw(row[9].int64Value);
    record.adjustmentFactor1e6 = row[10].int64Value;
    return RepositoryResult<StrategyRecord>::success(std::move(record));
}

RepositoryResult<OtcChannelRecord> parseOtcChannelRecord(const SqlQueryRow& row)
{
    auto columnCount = requireColumnCount(row, 9, "otc_channel");
    if (!columnCount) {
        return RepositoryResult<OtcChannelRecord>::failure(
            columnCount.error().errorCode,
            columnCount.error().message);
    }

    auto uid = parseUuid(row[1], "otc_channel.uid");
    if (!uid) {
        return RepositoryResult<OtcChannelRecord>::failure(
            uid.error().errorCode,
            uid.error().message);
    }
    auto strategyCode = parseStrategyCode(row[2], "otc_channel.strategy_code");
    if (!strategyCode) {
        return RepositoryResult<OtcChannelRecord>::failure(
            strategyCode.error().errorCode,
            strategyCode.error().message);
    }
    auto actualCode = parseInstrumentCode(row[3], "otc_channel.actual_code");
    if (!actualCode) {
        return RepositoryResult<OtcChannelRecord>::failure(
            actualCode.error().errorCode,
            actualCode.error().message);
    }

    OtcChannelRecord record;
    record.id = row[0].int64Value;
    record.uid = uid.value();
    record.strategyCode = strategyCode.value();
    record.actualCode = actualCode.value();
    record.fundClass = row[4].text;
    record.enabled = row[5].int64Value != 0;
    record.dailyLimitCents = etfdt::core::MoneyCents::fromRaw(row[6].int64Value);
    record.priority = static_cast<int>(row[7].int64Value);
    record.minBuyAmountCents = etfdt::core::MoneyCents::fromRaw(row[8].int64Value);
    return RepositoryResult<OtcChannelRecord>::success(std::move(record));
}

RepositoryResult<SchemaVersionRecord> parseSchemaVersionRecord(const SqlQueryRow& row)
{
    auto columnCount = requireColumnCount(row, 6, "schema_version");
    if (!columnCount) {
        return RepositoryResult<SchemaVersionRecord>::failure(
            columnCount.error().errorCode,
            columnCount.error().message);
    }

    auto uid = parseUuid(row[1], "schema_version.uid");
    if (!uid) {
        return RepositoryResult<SchemaVersionRecord>::failure(
            uid.error().errorCode,
            uid.error().message);
    }

    SchemaVersionRecord record;
    record.id = row[0].int64Value;
    record.uid = uid.value();
    record.version = static_cast<int>(row[2].int64Value);
    record.migrationName = row[3].text;
    record.checksum = row[4].text;
    record.appliedAtUtc = row[5].text;
    return RepositoryResult<SchemaVersionRecord>::success(std::move(record));
}

}  // namespace

RepositoryResult<std::vector<AccountRecord>> AccountRepository::listAccounts() const
{
    auto rows = queryRows(
        std::string("SELECT ") + kAccountColumns + " FROM account ORDER BY id;");
    if (!rows) {
        return RepositoryResult<std::vector<AccountRecord>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return mapRows<AccountRecord>(rows.value(), parseAccountRecord);
}

RepositoryResult<AccountRecord> AccountRepository::getActiveAccount() const
{
    auto row = querySingleRow(
        std::string("SELECT ") + kAccountColumns
            + " FROM account WHERE is_active = 1 ORDER BY id LIMIT 1;",
        {},
        "No active account found");
    if (!row) {
        return RepositoryResult<AccountRecord>::failure(row.error().errorCode, row.error().message);
    }
    return parseAccountRecord(row.value());
}

RepositoryResult<AccountRecord> AccountRepository::getById(std::int64_t id) const
{
    auto row = querySingleRow(
        std::string("SELECT ") + kAccountColumns + " FROM account WHERE id = ? LIMIT 1;",
        {std::to_string(id)},
        "Account not found");
    if (!row) {
        return RepositoryResult<AccountRecord>::failure(row.error().errorCode, row.error().message);
    }
    return parseAccountRecord(row.value());
}

RepositoryResult<std::vector<PortfolioRecord>> PortfolioRepository::listPortfolios() const
{
    auto rows = queryRows(
        std::string("SELECT ") + kPortfolioColumns + " FROM portfolio ORDER BY id;");
    if (!rows) {
        return RepositoryResult<std::vector<PortfolioRecord>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return mapRows<PortfolioRecord>(rows.value(), parsePortfolioRecord);
}

RepositoryResult<PortfolioRecord> PortfolioRepository::getActivePortfolio() const
{
    auto row = querySingleRow(
        std::string("SELECT ") + kPortfolioColumns
            + " FROM portfolio WHERE is_active = 1 ORDER BY id LIMIT 1;",
        {},
        "No active portfolio found");
    if (!row) {
        return RepositoryResult<PortfolioRecord>::failure(
            row.error().errorCode,
            row.error().message);
    }
    return parsePortfolioRecord(row.value());
}

RepositoryResult<PortfolioRecord> PortfolioRepository::getById(std::int64_t id) const
{
    auto row = querySingleRow(
        std::string("SELECT ") + kPortfolioColumns + " FROM portfolio WHERE id = ? LIMIT 1;",
        {std::to_string(id)},
        "Portfolio not found");
    if (!row) {
        return RepositoryResult<PortfolioRecord>::failure(
            row.error().errorCode,
            row.error().message);
    }
    return parsePortfolioRecord(row.value());
}

RepositoryResult<std::vector<InstrumentRecord>> InstrumentRepository::listEnabledInstruments()
    const
{
    auto rows = queryRows(
        std::string("SELECT ") + kInstrumentColumns
            + " FROM instrument WHERE enabled = 1 ORDER BY market_code, code;");
    if (!rows) {
        return RepositoryResult<std::vector<InstrumentRecord>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return mapRows<InstrumentRecord>(rows.value(), parseInstrumentRecord);
}

RepositoryResult<InstrumentRecord> InstrumentRepository::getByCode(const std::string& code) const
{
    auto parsedCode = etfdt::core::InstrumentCode::fromString(code);
    if (!parsedCode) {
        return failure<InstrumentRecord>("Invalid instrument code: " + parsedCode.error());
    }

    auto row = querySingleRow(
        std::string("SELECT ") + kInstrumentColumns
            + " FROM instrument WHERE code = ? ORDER BY id LIMIT 1;",
        {parsedCode.value().raw()},
        "Instrument not found");
    if (!row) {
        return RepositoryResult<InstrumentRecord>::failure(
            row.error().errorCode,
            row.error().message);
    }
    return parseInstrumentRecord(row.value());
}

RepositoryResult<InstrumentRecord> InstrumentRepository::getById(std::int64_t id) const
{
    auto row = querySingleRow(
        std::string("SELECT ") + kInstrumentColumns + " FROM instrument WHERE id = ? LIMIT 1;",
        {std::to_string(id)},
        "Instrument not found");
    if (!row) {
        return RepositoryResult<InstrumentRecord>::failure(
            row.error().errorCode,
            row.error().message);
    }
    return parseInstrumentRecord(row.value());
}

RepositoryResult<std::vector<StrategyRecord>> StrategyRepository::listEnabledStrategies() const
{
    auto rows = queryRows(
        std::string("SELECT ") + kStrategyColumns
            + " FROM strategy WHERE enabled = 1 ORDER BY strategy_code;");
    if (!rows) {
        return RepositoryResult<std::vector<StrategyRecord>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return mapRows<StrategyRecord>(rows.value(), parseStrategyRecord);
}

RepositoryResult<StrategyRecord> StrategyRepository::getByStrategyCode(
    const std::string& strategyCode) const
{
    auto parsedCode = etfdt::core::StrategyCode::fromString(strategyCode);
    if (!parsedCode) {
        return failure<StrategyRecord>("Invalid strategy code: " + parsedCode.error());
    }

    auto row = querySingleRow(
        std::string("SELECT ") + kStrategyColumns
            + " FROM strategy WHERE strategy_code = ? LIMIT 1;",
        {parsedCode.value().raw()},
        "Strategy not found");
    if (!row) {
        return RepositoryResult<StrategyRecord>::failure(
            row.error().errorCode,
            row.error().message);
    }
    return parseStrategyRecord(row.value());
}

RepositoryResult<StrategyRecord> StrategyRepository::getById(std::int64_t id) const
{
    auto row = querySingleRow(
        std::string("SELECT ") + kStrategyColumns + " FROM strategy WHERE id = ? LIMIT 1;",
        {std::to_string(id)},
        "Strategy not found");
    if (!row) {
        return RepositoryResult<StrategyRecord>::failure(
            row.error().errorCode,
            row.error().message);
    }
    return parseStrategyRecord(row.value());
}

RepositoryResult<std::vector<OtcChannelRecord>> OtcChannelRepository::listEnabledByStrategyCode(
    const std::string& strategyCode) const
{
    auto parsedCode = etfdt::core::StrategyCode::fromString(strategyCode);
    if (!parsedCode) {
        return RepositoryResult<std::vector<OtcChannelRecord>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "Invalid strategy code: " + parsedCode.error());
    }

    auto rows = queryRows(
        std::string("SELECT ") + kOtcChannelColumns
            + " FROM otc_channel WHERE strategy_code = ? AND enabled = 1 ORDER BY priority, id;",
        {parsedCode.value().raw()});
    if (!rows) {
        return RepositoryResult<std::vector<OtcChannelRecord>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return mapRows<OtcChannelRecord>(rows.value(), parseOtcChannelRecord);
}

RepositoryResult<std::vector<OtcChannelRecord>> OtcChannelRepository::listAllByStrategyCode(
    const std::string& strategyCode) const
{
    auto parsedCode = etfdt::core::StrategyCode::fromString(strategyCode);
    if (!parsedCode) {
        return RepositoryResult<std::vector<OtcChannelRecord>>::failure(
            etfdt::protocol::ErrorCode::E2000_DATABASE_ERROR,
            "Invalid strategy code: " + parsedCode.error());
    }

    auto rows = queryRows(
        std::string("SELECT ") + kOtcChannelColumns
            + " FROM otc_channel WHERE strategy_code = ? ORDER BY priority, id;",
        {parsedCode.value().raw()});
    if (!rows) {
        return RepositoryResult<std::vector<OtcChannelRecord>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return mapRows<OtcChannelRecord>(rows.value(), parseOtcChannelRecord);
}

RepositoryResult<int> OtcChannelRepository::countAll() const
{
    auto rows = queryRows("SELECT COUNT(*) FROM otc_channel;");
    if (!rows) {
        return RepositoryResult<int>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty() || rows.value().front().empty()) {
        return failure<int>("otc_channel count query returned no rows");
    }
    return RepositoryResult<int>::success(static_cast<int>(rows.value().front()[0].int64Value));
}

RepositoryResult<std::vector<SchemaVersionRecord>> SchemaVersionRepository::listVersions() const
{
    auto rows = queryRows(
        std::string("SELECT ") + kSchemaVersionColumns
            + " FROM schema_version ORDER BY version, id;");
    if (!rows) {
        return RepositoryResult<std::vector<SchemaVersionRecord>>::failure(
            rows.error().errorCode,
            rows.error().message);
    }
    return mapRows<SchemaVersionRecord>(rows.value(), parseSchemaVersionRecord);
}

RepositoryResult<bool> SchemaVersionRepository::hasMigration(
    const std::string& versionOrMigrationName) const
{
    if (versionOrMigrationName.empty()) {
        return failure<bool>("versionOrMigrationName is required");
    }

    auto rows = queryRows(
        "SELECT COUNT(*) FROM schema_version "
        "WHERE CAST(version AS TEXT) = ? OR migration_name = ? OR migration_name LIKE ?;",
        {versionOrMigrationName, versionOrMigrationName, "%" + versionOrMigrationName + "%"});
    if (!rows) {
        return RepositoryResult<bool>::failure(rows.error().errorCode, rows.error().message);
    }
    if (rows.value().empty() || rows.value().front().empty()) {
        return failure<bool>("schema_version migration check returned no rows");
    }
    return RepositoryResult<bool>::success(rows.value().front()[0].int64Value > 0);
}

}  // namespace etfdt::data_access
