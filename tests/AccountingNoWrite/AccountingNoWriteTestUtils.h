#pragma once

#include "DataAccess/SQLiteConnection.h"

#include <functional>
#include <string>
#include <vector>

namespace etfdt::tests::accounting_no_write {

struct ProtectedTableRowCount final {
    std::string tableName;
    bool exists = false;
    int rowCount = 0;
};

struct NoWriteCheckResult final {
    bool unchanged = true;
    std::vector<std::string> messages;
};

[[nodiscard]] std::vector<std::string> protectedAccountingTables();

[[nodiscard]] std::vector<ProtectedTableRowCount> captureProtectedTableRowCounts(
    etfdt::data_access::SQLiteConnection& connection);

[[nodiscard]] NoWriteCheckResult compareProtectedTableRowCounts(
    const std::vector<ProtectedTableRowCount>& before,
    const std::vector<ProtectedTableRowCount>& after);

[[nodiscard]] NoWriteCheckResult assertNoWritesAround(
    etfdt::data_access::SQLiteConnection& connection,
    const std::function<void()>& callback);

}  // namespace etfdt::tests::accounting_no_write
