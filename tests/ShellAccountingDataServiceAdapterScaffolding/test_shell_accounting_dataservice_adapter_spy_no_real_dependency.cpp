#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

namespace {

int gFailures = 0;

void expectTrue(bool condition, std::string_view message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

std::filesystem::path optionValue(int argc, char* argv[], const std::string& option)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (argv[i] == option) {
            return argv[i + 1];
        }
    }
    return {};
}

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string readFiles(const std::filesystem::path& root, const std::string& extension)
{
    std::ostringstream buffer;
    if (!std::filesystem::exists(root)) {
        return {};
    }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            buffer << readFile(entry.path()) << '\n';
        }
    }
    return buffer.str();
}

bool containsInclude(const std::string& text, const std::string& name)
{
    return text.find("#include \"" + name) != std::string::npos ||
           text.find("#include <" + name) != std::string::npos;
}

}  // namespace

int main(int argc, char* argv[])
{
    const auto root = optionValue(argc, argv, "--source-root");
    expectTrue(!root.empty(), "--source-root argument is required");

    const std::vector<std::filesystem::path> spyFiles{
        root / "tests" / "ShellAccountingDataServiceAdapterScaffolding" / "SpyAccountingDataServiceClient.h",
        root / "tests" / "ShellAccountingDataServiceAdapterScaffolding" / "SpyAccountingDataServiceClient.cpp",
        root / "tests" / "ShellAccountingDataServiceAdapterScaffolding" / "ShellAccountingDataServiceAdapterExpectedCall.h",
        root / "tests" / "ShellAccountingDataServiceAdapterScaffolding" / "ShellAccountingDataServiceAdapterExpectedCall.cpp",
    };

    std::ostringstream spyText;
    for (const auto& file : spyFiles) {
        expectTrue(std::filesystem::exists(file), "spy scaffolding file exists");
        spyText << readFile(file) << '\n';
    }

    const auto text = spyText.str();
    for (const auto& forbidden : {"DataServiceClient", "DataServiceApi", "DataAccess", "AccountingEngine", "SQLite", "QtQuick", "QML"}) {
        expectTrue(!containsInclude(text, forbidden), "spy scaffolding does not include forbidden production dependency");
    }

    expectTrue(
        !std::filesystem::exists(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceAdapter.h"),
        "ShellAccountingDataServiceAdapter production header is not added");
    expectTrue(
        !std::filesystem::exists(root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp"),
        "ShellAccountingDataServiceAdapter production source is not added");

    const auto qmlText = readFiles(root / "apps" / "ETFDecisionShell" / "qml", ".qml");
    expectTrue(qmlText.find("ShellAccountingDataServiceAdapter") == std::string::npos, "QML does not reference adapter");
    expectTrue(qmlText.find("ShellAccountingServiceAdapter") == std::string::npos, "QML does not reference service adapter");

    return gFailures == 0 ? 0 : 1;
}
