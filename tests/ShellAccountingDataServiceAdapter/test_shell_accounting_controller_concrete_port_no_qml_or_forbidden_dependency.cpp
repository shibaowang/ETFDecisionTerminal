#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

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

bool hasInclude(const std::string& text, const std::string& includeName)
{
    return text.find("#include \"" + includeName) != std::string::npos ||
        text.find("#include <" + includeName) != std::string::npos;
}

}  // namespace

int main(int argc, char* argv[])
{
    const auto root = optionValue(argc, argv, "--source-root");
    expectTrue(!root.empty(), "--source-root argument is required");

    const auto controllerText =
        readFile(root / "libs" / "ShellServices" / "include" / "ShellServices" /
            "ShellAccountingReadOnlyController.h") +
        readFile(root / "libs" / "ShellServices" / "src" /
            "ShellAccountingReadOnlyController.cpp");
    const auto adapterText =
        readFile(root / "libs" / "ShellServices" / "include" / "ShellServices" /
            "ShellAccountingDataServiceAdapter.h") +
        readFile(root / "libs" / "ShellServices" / "src" /
            "ShellAccountingDataServiceAdapter.cpp");
    const auto concretePortText =
        readFile(root / "libs" / "ShellServices" / "include" / "ShellServices" /
            "ShellAccountingDataServiceClientPortAdapter.h") +
        readFile(root / "libs" / "ShellServices" / "src" /
            "ShellAccountingDataServiceClientPortAdapter.cpp");

    for (const auto& forbidden : {
             "DataServiceClient/DataServiceClient.h",
             "DataServiceApi",
             "DataAccess",
             "AccountingEngine",
             "SQLite",
             "QtQuick",
             "QML",
         }) {
        expectTrue(!hasInclude(controllerText, forbidden), "controller avoids forbidden include");
    }
    for (const auto& forbidden : {
             "DataServiceClient/DataServiceClient.h",
             "DataServiceApi",
             "DataAccess",
             "AccountingEngine",
             "SQLite",
             "QtQuick",
             "QML",
         }) {
        expectTrue(!hasInclude(adapterText, forbidden), "adapter body avoids forbidden include");
    }

    expectTrue(
        concretePortText.find("DataServiceClient/DataServiceClient.h") != std::string::npos,
        "only concrete port includes DataServiceClient");
    for (const auto& allowedCall : {
             "client_->positionList(",
             "client_->cashSummary(",
             "client_->portfolioPnlSummary(",
             "client_->basePositionSummary(",
             "client_->sniperPoolSummary(",
         }) {
        expectTrue(concretePortText.find(allowedCall) != std::string::npos, "concrete port calls allowlisted wrapper");
    }
    for (const auto& forbiddenCall : {
             "appendAuditDemo",
             "data.audit.append",
             "tradeDraft",
             "snapshotWrite",
             "strategyExecute",
             "brokerOrder",
         }) {
        expectTrue(concretePortText.find(forbiddenCall) == std::string::npos, "concrete port avoids forbidden call");
    }

    const auto qmlText = readFiles(root / "apps" / "ETFDecisionShell" / "qml", ".qml");
    expectTrue(qmlText.find("ShellAccountingReadOnlyController") == std::string::npos, "QML does not reference controller");
    expectTrue(qmlText.find("ShellAccountingDataServiceAdapter") == std::string::npos, "QML does not reference adapter");
    expectTrue(qmlText.find("ShellAccountingDataServiceClientPortAdapter") == std::string::npos, "QML does not reference concrete port");
    expectTrue(qmlText.find("position.list") == std::string::npos, "QML does not call accounting action");
    expectTrue(qmlText.find("cash.summary") == std::string::npos, "QML does not call accounting action");
    expectTrue(qmlText.find("portfolio.pnl.summary") == std::string::npos, "QML does not call accounting action");
    expectTrue(qmlText.find("base_position.summary") == std::string::npos, "QML does not call accounting action");
    expectTrue(qmlText.find("sniper_pool.summary") == std::string::npos, "QML does not call accounting action");

    return gFailures == 0 ? 0 : 1;
}
