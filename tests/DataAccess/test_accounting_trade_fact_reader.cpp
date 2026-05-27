#include "AccountingNoWriteTestUtils.h"
#include "ForbiddenSqlScanner.h"

#include "DataAccess/DataAccess.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

void expectEqual(std::string_view actual, std::string_view expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

void expectEqual(std::size_t actual, std::size_t expected, std::string_view message)
{
    if (actual != expected) {
        ++gFailures;
        std::cerr << "FAILED: " << message << " expected " << expected << " actual " << actual
                  << '\n';
    }
}

std::filesystem::path optionValue(int argc, char* argv[], const std::string& option)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == option) {
            return argv[i + 1];
        }
    }
    return {};
}

std::filesystem::path createTempDirectory()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    auto path = std::filesystem::temp_directory_path()
        / ("etfdt_trade_fact_reader_" + std::to_string(now));
    std::filesystem::create_directories(path);
    return path;
}

void expectSqlSuccess(etfdt::data_access::SQLiteConnection& connection, const std::string& sql)
{
    auto result = connection.executeSql(sql);
    expectTrue(result.hasValue(), "test setup SQL succeeds");
}

void openMigratedDatabase(
    const std::filesystem::path& migrationPath,
    const std::filesystem::path& dbPath,
    etfdt::data_access::SQLiteConnection& connection)
{
    etfdt::data_access::DatabaseConfig config;
    config.databasePath = dbPath;
    config.migrationFilePath = migrationPath;
    config.enableWal = true;
    config.foreignKeys = true;

    auto openResult = connection.open(config);
    expectTrue(openResult.hasValue(), "SQLiteConnection open succeeds");

    auto migrationResult =
        etfdt::data_access::MigrationRunner::runInitialMigration(connection, migrationPath);
    expectTrue(migrationResult.hasValue(), "Initial migration succeeds");
}

void seedTradeFacts(etfdt::data_access::SQLiteConnection& connection)
{
    expectSqlSuccess(
        connection,
        "INSERT OR IGNORE INTO instrument "
        "(uid, code, name, instrument_type, market_code, currency, enabled, created_at_utc) "
        "VALUES "
        "('10000000-0000-4000-8000-000000159509', '159509', 'ETF 159509', "
        "'ETF', 'CN_A', 'CNY', 1, '2026-01-01T00:00:00.000Z');");

    expectSqlSuccess(
        connection,
        "INSERT INTO trade_log "
        "(uid, account_id, portfolio_id, actual_instrument_id, actual_code, action_type, "
        "trade_source, price_1e6, quantity_1e6, amount_cents, fee_cents, "
        "net_cash_impact_cents, memo, local_time, trading_date, created_at_utc) "
        "VALUES "
        "('20000000-0000-4000-8000-000000000001', 1, 1, "
        "(SELECT id FROM instrument WHERE code = '159509'), '159509', 'BUY', "
        "'MANUAL', 1000000, 1000000000, 100000, 100, -100100, "
        "'buy one', '2026-01-02T09:30:00.000Z', '2026-01-02', "
        "'2026-01-02T09:30:00.000Z'), "
        "('20000000-0000-4000-8000-000000000002', 1, 1, "
        "(SELECT id FROM instrument WHERE code = '159509'), '159509', 'SELL', "
        "'MANUAL', 1200000, 400000000, 48000, 100, 47900, "
        "'sell one', '2026-01-03T09:30:00.000Z', '2026-01-03', "
        "'2026-01-03T09:30:00.000Z'), "
        "('20000000-0000-4000-8000-000000000003', 1, 1, "
        "(SELECT id FROM instrument WHERE code = '159509'), '159509', 'BUY', "
        "'MANUAL', 1100000, 200000000, 22000, 0, -22000, "
        "'zero fee schema row', '2026-01-04T09:30:00.000Z', '2026-01-04', "
        "'2026-01-04T09:30:00.000Z');");
}

void testSqlScanner()
{
    const auto sql = std::string(etfdt::data_access::accountingTradeFactsReadOnlySql());
    expectTrue(
        !etfdt::tests::accounting_no_write::containsForbiddenWriteSql(sql),
        "trade facts reader SQL is SELECT-only");
    for (const auto& forbidden : {"INSERT", "UPDATE", "DELETE", "CREATE", "DROP", "ALTER", "REPLACE", "VACUUM"}) {
        expectTrue(sql.find(forbidden) == std::string::npos, std::string("reader SQL excludes ") + forbidden);
    }
}

void testTradeFactReader(const std::filesystem::path& migrationPath)
{
    const auto tempDir = createTempDirectory();
    const auto dbPath = tempDir / "trade_fact_reader.sqlite";

    etfdt::data_access::SQLiteConnection connection;
    openMigratedDatabase(migrationPath, dbPath, connection);
    seedTradeFacts(connection);

    etfdt::data_access::AccountingTradeFactReader reader(connection);

    etfdt::data_access::TradeFactQueryResult allFacts;
    const auto noWriteResult =
        etfdt::tests::accounting_no_write::assertNoWritesAround(connection, [&]() {
            auto result = reader.loadTradeFacts({});
            expectTrue(result.hasValue(), "load all trade facts succeeds");
            if (result.hasValue()) {
                allFacts = result.value();
            }
        });
    expectTrue(noWriteResult.unchanged, "reader keeps protected table row counts unchanged");
    expectTrue(allFacts.success, "trade facts result success=true");
    expectTrue(allFacts.readOnly, "trade facts result readOnly=true");
    expectEqual(allFacts.queryVersion, "trade-facts-readonly-v1", "query version");
    expectEqual(allFacts.rows.size(), 3, "load all trade facts row count");

    if (allFacts.rows.size() >= 3) {
        expectEqual(allFacts.rows[0].action, "BUY", "first action maps BUY");
        expectEqual(allFacts.rows[1].action, "SELL", "second action maps SELL");
        expectEqual(allFacts.rows[0].instrumentCode, "159509", "instrument code maps");
        expectEqual(allFacts.rows[0].quantityText, "1000", "quantity maps from 1e6 units");
        expectEqual(allFacts.rows[0].amountText, "1000.00 CNY", "amount maps from cents");
        expectEqual(allFacts.rows[0].feeText, "1.00 CNY", "fee maps from cents");
        expectEqual(allFacts.rows[0].cashFlowText, "-1001.00 CNY", "cash flow maps from cents");
        expectEqual(allFacts.rows[0].currency, "CNY", "currency maps from instrument");
        expectEqual(allFacts.rows[2].feeText, "0.00 CNY", "zero fee maps explicitly");
    }

    etfdt::data_access::TradeFactQueryRequest accountFilter;
    accountFilter.accountId = "1";
    auto byAccount = reader.loadTradeFacts(accountFilter);
    expectTrue(byAccount.hasValue(), "account filter succeeds");
    if (byAccount.hasValue()) {
        expectEqual(byAccount.value().rows.size(), 3, "account filter returns matching rows");
    }

    etfdt::data_access::TradeFactQueryRequest portfolioFilter;
    portfolioFilter.portfolioId = "99999";
    auto byMissingPortfolio = reader.loadTradeFacts(portfolioFilter);
    expectTrue(byMissingPortfolio.hasValue(), "portfolio filter succeeds");
    if (byMissingPortfolio.hasValue()) {
        expectTrue(byMissingPortfolio.value().rows.empty(), "portfolio filter excludes rows");
    }

    etfdt::data_access::TradeFactQueryRequest timeFilter;
    timeFilter.sourceFromTime = "2026-01-03T00:00:00.000Z";
    timeFilter.sourceToTime = "2026-01-03T23:59:59.999Z";
    auto byTime = reader.loadTradeFacts(timeFilter);
    expectTrue(byTime.hasValue(), "time filter succeeds");
    if (byTime.hasValue()) {
        expectEqual(byTime.value().rows.size(), 1, "time filter returns one row");
        if (!byTime.value().rows.empty()) {
            expectEqual(byTime.value().rows.front().action, "SELL", "time filter returns sell row");
        }
    }

    etfdt::data_access::TradeFactQueryRequest limited;
    limited.limit = 1;
    limited.offset = 1;
    auto byLimit = reader.loadTradeFacts(limited);
    expectTrue(byLimit.hasValue(), "limit offset succeeds");
    if (byLimit.hasValue()) {
        expectEqual(byLimit.value().rows.size(), 1, "limit offset returns one row");
        if (!byLimit.value().rows.empty()) {
            expectEqual(byLimit.value().rows.front().action, "SELL", "limit offset preserves stable order");
        }
    }

    etfdt::data_access::TradeFactQueryRequest invalid;
    invalid.limit = -1;
    const auto invalidNoWrite =
        etfdt::tests::accounting_no_write::assertNoWritesAround(connection, [&]() {
            auto result = reader.loadTradeFacts(invalid);
            expectTrue(!result.hasValue(), "invalid request fails");
        });
    expectTrue(invalidNoWrite.unchanged, "invalid request path keeps row counts unchanged");

    connection.close();
    std::filesystem::remove_all(tempDir);
}

}  // namespace

int main(int argc, char* argv[])
{
    const auto migrationPath = optionValue(argc, argv, "--migration");
    if (migrationPath.empty()) {
        std::cerr << "--migration <path> is required\n";
        return 1;
    }

    testSqlScanner();
    testTradeFactReader(migrationPath);

    if (gFailures != 0) {
        std::cerr << gFailures << " accounting trade fact reader test failure(s)" << '\n';
        return 1;
    }

    std::cout << "Accounting trade fact reader tests passed." << '\n';
    return 0;
}
