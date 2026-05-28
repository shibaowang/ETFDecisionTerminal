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

    const std::vector<std::filesystem::path> adapterFiles{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceAdapter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp",
    };

    std::ostringstream adapterText;
    for (const auto& file : adapterFiles) {
        expectTrue(std::filesystem::exists(file), "ShellAccountingDataServiceAdapter skeleton file exists");
        adapterText << readFile(file) << '\n';
    }

    const auto text = adapterText.str();
    for (const auto& forbidden : {
             "DataServiceClient",
             "DataServiceApi",
             "DataAccess",
             "AccountingEngine",
             "SQLite",
             "QtQuick",
             "QML",
         }) {
        expectTrue(!containsInclude(text, forbidden), "adapter skeleton does not include forbidden dependency");
    }

    for (const auto& forbiddenCall : {
             "DataServiceClient::positionList",
             "DataServiceClient::cashSummary",
             "DataServiceClient::portfolioPnlSummary",
             "DataServiceClient::basePositionSummary",
             "DataServiceClient::sniperPoolSummary",
             ".positionList(",
             ".cashSummary(",
             ".portfolioPnlSummary(",
             ".basePositionSummary(",
             ".sniperPoolSummary(",
             "->positionList(",
             "->cashSummary(",
             "->portfolioPnlSummary(",
             "->basePositionSummary(",
             "->sniperPoolSummary(",
             "QLocalSocket",
             "QTcpSocket",
             "connectToServer",
             "connectToHost",
         }) {
        expectTrue(text.find(forbiddenCall) == std::string::npos, "adapter skeleton does not call live wrapper or transport");
    }

    const auto qmlText = readFiles(root / "apps" / "ETFDecisionShell" / "qml", ".qml");
    expectTrue(qmlText.find("ShellAccountingDataServiceAdapter") == std::string::npos, "QML does not reference adapter");

    return gFailures == 0 ? 0 : 1;
}
