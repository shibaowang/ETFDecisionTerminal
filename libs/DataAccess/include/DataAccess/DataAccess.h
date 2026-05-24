#pragma once

#include <string_view>

namespace etfdt::data_access {

constexpr std::string_view kDatabaseWriteOwner = "ETFDataService";
constexpr std::string_view kInitialMigrationFile = "001_initial_schema.sql";

std::string_view dataAccessModuleName() noexcept;

}  // namespace etfdt::data_access
