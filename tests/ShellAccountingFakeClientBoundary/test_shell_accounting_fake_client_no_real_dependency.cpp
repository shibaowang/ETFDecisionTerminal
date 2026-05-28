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

    const auto fakeDir = root / "tests" / "ShellAccountingFakeClientBoundary";
    expectTrue(std::filesystem::exists(fakeDir), "fake client helper is test-only");

    const std::vector<std::filesystem::path> files{
        fakeDir / "FakeAccountingClientPayload.h",
        fakeDir / "FakeAccountingClientPayload.cpp",
        fakeDir / "FakeAccountingClientScenario.h",
        fakeDir / "FakeAccountingClientScenario.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp",
    };

    for (const auto& file : files) {
        expectTrue(std::filesystem::exists(file), "dependency scan file exists");
        const auto text = readFile(file);
        expectTrue(text.find("DataServiceClient") == std::string::npos, "no real DataServiceClient dependency");
        expectTrue(text.find("DataServiceApi") == std::string::npos, "no DataServiceApi dependency");
        expectTrue(text.find("DataAccess") == std::string::npos, "no DataAccess dependency");
        expectTrue(text.find("AccountingEngine") == std::string::npos, "no AccountingEngine dependency");
        expectTrue(text.find("SQLite") == std::string::npos, "no SQLite dependency");
        expectTrue(text.find("socket") == std::string::npos, "no socket dependency");
        expectTrue(text.find("IPC") == std::string::npos, "no IPC dependency");
    }

    const auto qmlDir = root / "apps" / "ETFDecisionShell" / "qml";
    for (const auto& entry : std::filesystem::recursive_directory_iterator(qmlDir)) {
        if (entry.path().extension() != ".qml") {
            continue;
        }
        const auto text = readFile(entry.path());
        expectTrue(text.find("ShellAccountingReadOnlyController") == std::string::npos, "QML does not reference ShellAccountingReadOnlyController");
        expectTrue(text.find("position.list") == std::string::npos, "QML does not call accounting action names");
    }

    return gFailures == 0 ? 0 : 1;
}
