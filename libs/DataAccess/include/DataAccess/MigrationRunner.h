#pragma once

#include "DataAccess/SQLiteConnection.h"

#include <filesystem>

namespace etfdt::data_access {

class MigrationRunner final {
public:
    [[nodiscard]] static DatabaseResult<bool> runInitialMigration(
        SQLiteConnection& connection,
        const std::filesystem::path& migrationPath);

    [[nodiscard]] static DatabaseResult<bool> isInitialMigrationApplied(
        SQLiteConnection& connection);
};

}  // namespace etfdt::data_access
