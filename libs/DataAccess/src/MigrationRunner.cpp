#include "DataAccess/MigrationRunner.h"

#include <algorithm>

namespace etfdt::data_access {
namespace {

bool migrationNameMatchesInitial(const std::string& migrationName)
{
    return migrationName.find("001_initial_schema") != std::string::npos;
}

}  // namespace

DatabaseResult<bool> MigrationRunner::runInitialMigration(
    SQLiteConnection& connection,
    const std::filesystem::path& migrationPath)
{
    if (migrationPath.empty() || !std::filesystem::exists(migrationPath)) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2002_MIGRATION_FAILED,
            "Initial migration file does not exist: " + migrationPath.string());
    }

    auto applied = isInitialMigrationApplied(connection);
    if (!applied) {
        return applied;
    }
    if (applied.value()) {
        return DatabaseResult<bool>::success(true);
    }

    auto migrationResult = connection.applyMigrationFile(migrationPath);
    if (!migrationResult) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2002_MIGRATION_FAILED,
            migrationResult.error().message);
    }

    auto appliedAfterMigration = isInitialMigrationApplied(connection);
    if (!appliedAfterMigration) {
        return appliedAfterMigration;
    }
    if (!appliedAfterMigration.value()) {
        return DatabaseResult<bool>::failure(
            etfdt::protocol::ErrorCode::E2002_MIGRATION_FAILED,
            "Initial migration completed but schema_version was not recorded");
    }

    return DatabaseResult<bool>::success(true);
}

DatabaseResult<bool> MigrationRunner::isInitialMigrationApplied(SQLiteConnection& connection)
{
    auto schemaVersionExists = connection.queryTableExists("schema_version");
    if (!schemaVersionExists) {
        return DatabaseResult<bool>::failure(
            schemaVersionExists.error().errorCode,
            schemaVersionExists.error().message);
    }
    if (!schemaVersionExists.value()) {
        return DatabaseResult<bool>::success(false);
    }

    auto versions = connection.getSchemaVersions();
    if (!versions) {
        return DatabaseResult<bool>::failure(
            versions.error().errorCode,
            versions.error().message);
    }

    const bool found = std::any_of(
        versions.value().begin(),
        versions.value().end(),
        migrationNameMatchesInitial);
    if (found) {
        return DatabaseResult<bool>::success(true);
    }

    return DatabaseResult<bool>::success(false);
}

}  // namespace etfdt::data_access
