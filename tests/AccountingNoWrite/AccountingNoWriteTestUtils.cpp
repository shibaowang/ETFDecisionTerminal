#include "AccountingNoWriteTestUtils.h"

#include <algorithm>
#include <stdexcept>

namespace etfdt::tests::accounting_no_write {

std::vector<std::string> protectedAccountingTables()
{
    return {
        "trade_log",
        "trade_execution_group",
        "trade_draft",
        "cash_snapshot",
        "position_snapshot",
        "portfolio_summary",
        "audit_log",
    };
}

std::vector<ProtectedTableRowCount> captureProtectedTableRowCounts(
    etfdt::data_access::SQLiteConnection& connection)
{
    std::vector<ProtectedTableRowCount> counts;
    for (const auto& tableName : protectedAccountingTables()) {
        ProtectedTableRowCount count;
        count.tableName = tableName;

        auto exists = connection.queryTableExists(tableName);
        if (!exists.hasValue()) {
            count.exists = false;
            counts.push_back(count);
            continue;
        }

        count.exists = exists.value();
        if (count.exists) {
            auto rowCount = connection.querySingleInt("SELECT COUNT(*) FROM " + tableName + ";");
            if (!rowCount.hasValue()) {
                throw std::runtime_error("row count failed for " + tableName);
            }
            count.rowCount = rowCount.value();
        }
        counts.push_back(count);
    }
    return counts;
}

NoWriteCheckResult compareProtectedTableRowCounts(
    const std::vector<ProtectedTableRowCount>& before,
    const std::vector<ProtectedTableRowCount>& after)
{
    NoWriteCheckResult result;
    for (const auto& beforeCount : before) {
        const auto afterIt = std::find_if(
            after.begin(),
            after.end(),
            [&](const ProtectedTableRowCount& count) {
                return count.tableName == beforeCount.tableName;
            });

        if (afterIt == after.end()) {
            result.unchanged = false;
            result.messages.push_back(beforeCount.tableName + " missing from after counts");
            continue;
        }

        if (beforeCount.exists != afterIt->exists) {
            result.unchanged = false;
            result.messages.push_back(beforeCount.tableName + " existence changed");
            continue;
        }

        if (beforeCount.exists && beforeCount.rowCount != afterIt->rowCount) {
            result.unchanged = false;
            result.messages.push_back(
                beforeCount.tableName + " row count changed from "
                + std::to_string(beforeCount.rowCount) + " to "
                + std::to_string(afterIt->rowCount));
        }
    }
    return result;
}

NoWriteCheckResult assertNoWritesAround(
    etfdt::data_access::SQLiteConnection& connection,
    const std::function<void()>& callback)
{
    const auto before = captureProtectedTableRowCounts(connection);

    try {
        callback();
    } catch (const std::exception& ex) {
        const auto after = captureProtectedTableRowCounts(connection);
        auto result = compareProtectedTableRowCounts(before, after);
        result.messages.push_back(std::string("callback threw: ") + ex.what());
        return result;
    } catch (...) {
        const auto after = captureProtectedTableRowCounts(connection);
        auto result = compareProtectedTableRowCounts(before, after);
        result.messages.push_back("callback threw unknown exception");
        return result;
    }

    const auto after = captureProtectedTableRowCounts(connection);
    return compareProtectedTableRowCounts(before, after);
}

}  // namespace etfdt::tests::accounting_no_write
