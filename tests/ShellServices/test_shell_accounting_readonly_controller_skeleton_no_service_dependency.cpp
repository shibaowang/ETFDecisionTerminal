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

}  // namespace

int main(int argc, char* argv[])
{
    const auto root = optionValue(argc, argv, "--source-root");
    expectTrue(!root.empty(), "--source-root argument is required");

    const std::vector<std::filesystem::path> files{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingState.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingState.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingIssue.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingIssue.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDisplayText.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDisplayText.cpp",
    };

    for (const auto& file : files) {
        expectTrue(std::filesystem::exists(file), "Shell accounting skeleton file exists");
        const auto text = readFile(file);
        expectTrue(text.find("DataServiceClient") == std::string::npos, "skeleton does not include DataServiceClient");
        expectTrue(text.find("DataServiceApi") == std::string::npos, "skeleton does not include DataServiceApi");
        expectTrue(text.find("DataAccess") == std::string::npos, "skeleton does not include DataAccess");
        expectTrue(text.find("AccountingEngine") == std::string::npos, "skeleton does not include AccountingEngine");
        expectTrue(text.find("SQLite") == std::string::npos, "skeleton does not include SQLite");
        expectTrue(text.find("QtQuick") == std::string::npos, "skeleton does not include QtQuick");
        expectTrue(text.find("QML") == std::string::npos, "skeleton does not include QML");
    }

    return gFailures == 0 ? 0 : 1;
}
