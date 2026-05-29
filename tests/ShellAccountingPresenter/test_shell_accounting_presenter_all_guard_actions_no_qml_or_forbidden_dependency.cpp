#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

int gFailures = 0;

void expectTrue(bool condition, const char* message)
{
    if (!condition) {
        ++gFailures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

std::filesystem::path sourceRoot(int argc, char** argv)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--source-root") {
            return argv[i + 1];
        }
    }
    return std::filesystem::current_path();
}

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

}  // namespace

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const std::vector<std::filesystem::path> presenterFiles{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp",
    };
    const std::vector<std::filesystem::path> controllerAndAdapterFiles{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceAdapter.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp",
    };
    const auto portAdapterSource =
        root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceClientPortAdapter.cpp";

    for (const auto& file : presenterFiles) {
        const auto text = readFile(file);
        expectTrue(text.find("DataServiceClient") == std::string::npos, "presenter does not include DataServiceClient");
        expectTrue(text.find("DataServiceApi") == std::string::npos, "presenter does not include DataServiceApi");
        expectTrue(text.find("DataAccess") == std::string::npos, "presenter does not include DataAccess");
        expectTrue(text.find("AccountingEngine") == std::string::npos, "presenter does not include AccountingEngine");
        expectTrue(text.find("SQLite") == std::string::npos, "presenter does not include SQLite");
        expectTrue(text.find("QtQuick") == std::string::npos, "presenter does not include QtQuick or QML");
        expectTrue(text.find("qmlRegisterType") == std::string::npos, "presenter does not register QML type");
        expectTrue(text.find("createTradeDraft") == std::string::npos, "presenter does not expose createTradeDraft");
        expectTrue(text.find("brokerOrder") == std::string::npos, "presenter does not expose brokerOrder");
        expectTrue(text.find("strategyExecute") == std::string::npos, "presenter does not expose strategyExecute");
    }

    for (const auto& file : controllerAndAdapterFiles) {
        const auto text = readFile(file);
        expectTrue(text.find("DataServiceClient/DataServiceClient.h") == std::string::npos, "controller / adapter body does not include DataServiceClient");
    }

    const auto portText = readFile(portAdapterSource);
    expectTrue(portText.find("positionList(") != std::string::npos, "concrete port calls positionList");
    expectTrue(portText.find("cashSummary(") != std::string::npos, "concrete port calls cashSummary");
    expectTrue(portText.find("portfolioPnlSummary(") != std::string::npos, "concrete port calls portfolioPnlSummary");
    expectTrue(portText.find("basePositionSummary(") != std::string::npos, "concrete port calls basePositionSummary");
    expectTrue(portText.find("sniperPoolSummary(") != std::string::npos, "concrete port calls sniperPoolSummary");

    const auto qmlDir = root / "apps" / "ETFDecisionShell" / "qml";
    for (const auto& entry : std::filesystem::recursive_directory_iterator(qmlDir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".qml") {
            continue;
        }
        const auto text = readFile(entry.path());
        const bool authorizedShellAccountingShell = entry.path().filename() == "ShellAccountingReadOnlyPage.qml";
        expectTrue(
            authorizedShellAccountingShell || text.find("ShellAccountingPresenter") == std::string::npos,
            "QML only references accounting presenter in authorized read-only shell");
        expectTrue(text.find("ShellAccountingStatusObject") == std::string::npos, "QML does not bind accounting status");
        expectTrue(text.find("position.list") == std::string::npos, "QML does not call position.list");
        expectTrue(text.find("cash.summary") == std::string::npos, "QML does not call cash.summary");
        expectTrue(text.find("qmlRegisterType") == std::string::npos, "QML does not register accounting type");
    }

    return gFailures == 0 ? 0 : 1;
}
