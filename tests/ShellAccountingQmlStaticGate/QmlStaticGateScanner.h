#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_qml_static_gate {

struct QmlHit final {
    std::filesystem::path path;
    int line = 0;
    std::string token;
    std::string text;
};

struct QmlFile final {
    std::filesystem::path path;
    std::string text;
    std::vector<std::string> lines;
};

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::vector<QmlFile> loadQmlFiles(const std::filesystem::path& sourceRoot);
[[nodiscard]] std::vector<QmlHit> scanTokens(
    const std::vector<QmlFile>& files,
    const std::vector<std::string>& tokens);
[[nodiscard]] std::vector<QmlHit> scanTokensInAccountingScope(
    const std::vector<QmlFile>& files,
    const std::vector<std::string>& tokens);
[[nodiscard]] bool isAccountingScopedFile(const QmlFile& file);
void printHits(const std::vector<QmlHit>& hits);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);

}  // namespace etfdt::tests::shell_accounting_qml_static_gate
