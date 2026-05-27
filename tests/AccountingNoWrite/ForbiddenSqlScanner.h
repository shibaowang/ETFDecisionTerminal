#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::accounting_no_write {

struct ForbiddenSqlMatch final {
    std::string keyword;
};

[[nodiscard]] std::vector<ForbiddenSqlMatch> findForbiddenWriteKeywords(
    const std::string& sqlText);

[[nodiscard]] bool containsForbiddenWriteSql(const std::string& sqlText);

[[nodiscard]] std::vector<ForbiddenSqlMatch> scanFile(const std::filesystem::path& path);

[[nodiscard]] std::vector<ForbiddenSqlMatch> scanDirectory(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);

}  // namespace etfdt::tests::accounting_no_write
