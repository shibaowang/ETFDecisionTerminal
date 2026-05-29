#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::vector<std::filesystem::path> productionStartupFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> productionAppFiles(const std::filesystem::path& root);
[[nodiscard]] std::string shellAccountingQmlRegistrationHelperText(const std::filesystem::path& root);
[[nodiscard]] int countTokenInFiles(
    const std::vector<std::filesystem::path>& files,
    const std::string& token);
[[nodiscard]] bool containsToken(
    const std::vector<std::filesystem::path>& files,
    const std::string& token);
[[nodiscard]] bool containsForbiddenExposure(
    const std::vector<std::filesystem::path>& files,
    const std::vector<std::string>& tokens);
[[nodiscard]] std::vector<std::string> forbiddenRuntimeAccessTokens();

}  // namespace etfdt::tests::shell_accounting_qml_startup_registration_wiring
