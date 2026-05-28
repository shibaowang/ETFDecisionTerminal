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

std::string readQmlFiles(const std::filesystem::path& qmlRoot)
{
    std::ostringstream buffer;
    if (!std::filesystem::exists(qmlRoot)) {
        return {};
    }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(qmlRoot)) {
        if (entry.is_regular_file() && entry.path().extension() == ".qml") {
            buffer << readFile(entry.path()) << '\n';
        }
    }
    return buffer.str();
}

}  // namespace

int main(int argc, char* argv[])
{
    const auto root = optionValue(argc, argv, "--source-root");
    expectTrue(!root.empty(), "--source-root argument is required");

    const std::vector<std::filesystem::path> controllerFiles{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp",
    };

    for (const auto& file : controllerFiles) {
        expectTrue(std::filesystem::exists(file), "controller production file exists");
        const auto text = readFile(file);
        expectTrue(text.find("DataServiceClient") == std::string::npos, "controller does not include DataServiceClient");
        expectTrue(text.find("DataServiceApi") == std::string::npos, "controller does not include DataServiceApi");
        expectTrue(text.find("DataAccess") == std::string::npos, "controller does not include DataAccess");
        expectTrue(text.find("AccountingEngine") == std::string::npos, "controller does not include AccountingEngine");
        expectTrue(text.find("SQLite") == std::string::npos, "controller does not include SQLite");
    }

    const std::vector<std::filesystem::path> fakeFiles{
        root / "tests" / "ShellAccountingServiceAdapter" / "FakeShellAccountingServiceAdapter.h",
        root / "tests" / "ShellAccountingServiceAdapter" / "FakeShellAccountingServiceAdapter.cpp",
    };

    for (const auto& file : fakeFiles) {
        expectTrue(std::filesystem::exists(file), "test-only fake adapter file exists");
        const auto text = readFile(file);
        expectTrue(text.find("DataServiceClient") == std::string::npos, "fake adapter does not include DataServiceClient");
        expectTrue(text.find("SQLite") == std::string::npos, "fake adapter does not include SQLite");
    }

    const auto qmlText = readQmlFiles(root / "apps" / "ETFDecisionShell" / "qml");
    expectTrue(qmlText.find("ShellAccountingServiceAdapter") == std::string::npos, "QML does not reference accounting adapter");
    expectTrue(qmlText.find("ShellAccountingReadOnlyController") == std::string::npos, "QML does not reference accounting controller");

    return gFailures == 0 ? 0 : 1;
}
