#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_real_data_adapter_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::vector<std::filesystem::path> productionStartupFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> presenterCoreFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> realAdapterBoundaryFiles(const std::filesystem::path& root);
[[nodiscard]] bool containsTokenInFiles(const std::vector<std::filesystem::path>& files, const std::string& token);
[[nodiscard]] int countTokenInFiles(const std::vector<std::filesystem::path>& files, const std::string& token);
[[nodiscard]] std::filesystem::path docs78Path(const std::filesystem::path& root);
[[nodiscard]] std::filesystem::path docs79Path(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::string> writePathTokens();
[[nodiscard]] std::vector<std::string> sqliteDirectTokens();
[[nodiscard]] std::vector<std::string> accountingEngineDirectTokens();

}  // namespace etfdt::tests::shell_accounting_real_data_adapter_gate
