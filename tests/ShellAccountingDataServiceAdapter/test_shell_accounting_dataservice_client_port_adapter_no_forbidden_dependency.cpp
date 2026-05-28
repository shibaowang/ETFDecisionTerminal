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

    const auto portHeader = readFile(
        root / "libs" / "ShellServices" / "include" / "ShellServices" /
            "ShellAccountingDataServiceClientPortAdapter.h");
    const auto portSource =
        readFile(root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceClientPortAdapter.cpp");
    const auto adapterSource = readFile(
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp");
    const auto adapterHeader = readFile(
        root / "libs" / "ShellServices" / "include" / "ShellServices" /
            "ShellAccountingDataServiceAdapter.h");
    const auto controllerText = readFile(
        root / "libs" / "ShellServices" / "include" / "ShellServices" /
            "ShellAccountingReadOnlyController.h")
        + readFile(root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp");

    expectTrue(portSource.find("DataServiceClient/DataServiceClient.h") != std::string::npos, "concrete port includes DataServiceClient in cpp");
    expectTrue(portHeader.find("DataServiceClient/DataServiceClient.h") == std::string::npos, "concrete port header forward declares DataServiceClient");
    expectTrue(adapterHeader.find("DataServiceClient/DataServiceClient.h") == std::string::npos, "adapter header does not include DataServiceClient");
    expectTrue(adapterSource.find("DataServiceClient/DataServiceClient.h") == std::string::npos, "adapter source does not include DataServiceClient");
    expectTrue(adapterHeader.find("DataServiceClient::") == std::string::npos, "adapter header does not call DataServiceClient");
    expectTrue(adapterSource.find("DataServiceClient::") == std::string::npos, "adapter source does not call DataServiceClient");
    expectTrue(controllerText.find("DataServiceClient/DataServiceClient.h") == std::string::npos, "controller does not include DataServiceClient");
    expectTrue(controllerText.find("DataServiceClient::") == std::string::npos, "controller does not call DataServiceClient");

    const auto portText = portHeader + portSource;
    for (const auto& forbidden : {"DataServiceApi", "DataAccess", "AccountingEngine", "SQLite", "QtQuick", "QML"}) {
        expectTrue(!containsInclude(portText, forbidden), "concrete port avoids forbidden include");
    }
    for (const auto& forbiddenCall : {
             "appendAuditDemo",
             "data.audit.append",
             "tradeDraft",
             "snapshotWrite",
             "strategyExecute",
             "brokerOrder",
         }) {
        expectTrue(portText.find(forbiddenCall) == std::string::npos, "concrete port avoids forbidden call");
    }

    const auto qmlText = readFiles(root / "apps" / "ETFDecisionShell" / "qml", ".qml");
    expectTrue(qmlText.find("ShellAccountingDataServiceClientPortAdapter") == std::string::npos, "QML does not reference concrete port");

    return gFailures == 0 ? 0 : 1;
}
