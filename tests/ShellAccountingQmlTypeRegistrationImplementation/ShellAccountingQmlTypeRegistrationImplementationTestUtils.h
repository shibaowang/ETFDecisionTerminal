#pragma once

#include <QString>
#include <QStringList>

#include <filesystem>
#include <string>
#include <vector>

namespace etfdt::tests::shell_accounting_qml_type_registration_implementation {

constexpr const char* kShellAccountingQmlModule = "ETFDecisionTerminal.ShellAccounting";
constexpr int kShellAccountingQmlMajorVersion = 1;
constexpr int kShellAccountingQmlMinorVersion = 0;
constexpr const char* kReadOnlyReason =
    "ShellAccounting objects are provided by ShellServices and are read-only.";

[[nodiscard]] std::filesystem::path sourceRoot(int argc, char** argv);
[[nodiscard]] std::string readTextFile(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions);

[[nodiscard]] QString importOnlyQml();
[[nodiscard]] QString instantiateTypeQml(const QString& typeName);
[[nodiscard]] QString componentErrors(const QString& qmlSource);

[[nodiscard]] QStringList allowlistTypes();
[[nodiscard]] QStringList forbiddenTypes();
[[nodiscard]] QStringList forbiddenMethodTokens();
[[nodiscard]] QStringList productionQmlBindingTokens();

[[nodiscard]] bool textContainsAny(const std::string& text, const QStringList& tokens);
[[nodiscard]] bool productionQmlContainsAny(
    const std::filesystem::path& root,
    const QStringList& tokens);

void expectTrue(bool condition, const char* message);

}  // namespace etfdt::tests::shell_accounting_qml_type_registration_implementation
