#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_production_qml_binding_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::vector<std::filesystem::path> productionStartupFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> productionAppFiles(const std::filesystem::path& root);
[[nodiscard]] bool containsToken(
    const std::vector<std::filesystem::path>& files,
    const std::string& token);
[[nodiscard]] bool containsForbiddenExposure(
    const std::vector<std::filesystem::path>& files,
    const std::vector<std::string>& tokens);
[[nodiscard]] std::vector<std::string> contextExposureTokens();
[[nodiscard]] std::vector<std::string> forbiddenRuntimeAccessTokens();

}  // namespace etfdt::tests::shell_accounting_production_qml_binding_gate
