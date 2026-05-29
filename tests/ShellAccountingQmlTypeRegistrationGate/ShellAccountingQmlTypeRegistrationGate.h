#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_qml_type_registration_gate {

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);
[[nodiscard]] std::vector<std::string> preGateTokens();
[[nodiscard]] std::vector<std::string> firstStageAllowedTypes();
[[nodiscard]] std::vector<std::string> futureButNotFirstStageTypes();
[[nodiscard]] std::vector<std::string> forbiddenTypeTokens();
[[nodiscard]] std::vector<std::string> forbiddenMethodPropertyTokens();
[[nodiscard]] std::vector<std::string> registrationLocationRules();
[[nodiscard]] std::vector<std::string> rollbackRules();
[[nodiscard]] std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root);
[[nodiscard]] std::vector<std::filesystem::path> productionRegistrationFiles(const std::filesystem::path& root);
[[nodiscard]] bool containsAnyToken(
    const std::vector<std::filesystem::path>& files,
    const std::vector<std::string>& tokens);

}  // namespace etfdt::tests::shell_accounting_qml_type_registration_gate
