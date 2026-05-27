#include "ForbiddenSqlScanner.h"

#include <iostream>
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

void expectAllowed(std::string_view sql, std::string_view message)
{
    expectTrue(
        !etfdt::tests::accounting_no_write::containsForbiddenWriteSql(std::string(sql)),
        message);
}

void expectForbidden(std::string_view sql, std::string_view message)
{
    expectTrue(
        etfdt::tests::accounting_no_write::containsForbiddenWriteSql(std::string(sql)),
        message);
}

}  // namespace

int main()
{
    expectAllowed("SELECT * FROM trade_log;", "SELECT is allowed");
    expectAllowed("WITH x AS (SELECT 1) SELECT * FROM x;", "WITH SELECT is allowed");
    expectAllowed("PRAGMA query_only = ON;", "PRAGMA query_only is allowed");
    expectAllowed("EXPLAIN SELECT * FROM trade_log;", "EXPLAIN SELECT is allowed");

    expectForbidden("INSERT INTO trade_log(id) VALUES (1);", "INSERT is forbidden");
    expectForbidden("UPDATE trade_log SET note = 'x';", "UPDATE is forbidden");
    expectForbidden("DELETE FROM trade_log;", "DELETE is forbidden");
    expectForbidden("CREATE TABLE x(id INTEGER);", "CREATE is forbidden");
    expectForbidden("DROP TABLE x;", "DROP is forbidden");
    expectForbidden("ALTER TABLE trade_log ADD COLUMN x TEXT;", "ALTER is forbidden");
    expectForbidden("REPLACE INTO trade_log(id) VALUES (1);", "REPLACE is forbidden");
    expectForbidden("UPSERT INTO trade_log VALUES (1);", "UPSERT is forbidden");
    expectForbidden("VACUUM;", "VACUUM is forbidden");
    expectForbidden("PRAGMA writable_schema=ON;", "PRAGMA writable_schema is forbidden");
    expectForbidden("BEGIN IMMEDIATE;", "BEGIN IMMEDIATE is forbidden");
    expectForbidden("BEGIN EXCLUSIVE;", "BEGIN EXCLUSIVE is forbidden");
    expectForbidden("iNsErT INTO trade_log(id) VALUES (1);", "mixed case keyword is forbidden");

    return gFailures == 0 ? 0 : 1;
}
