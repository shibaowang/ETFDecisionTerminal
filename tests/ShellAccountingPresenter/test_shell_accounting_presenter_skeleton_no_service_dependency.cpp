#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

void expectTrue(bool condition, const char* message)
{
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
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

    for (const auto& file : presenterFiles) {
        const auto text = readFile(file);
        expectTrue(text.find("DataServiceClient") == std::string::npos, "presenter does not include DataServiceClient");
        expectTrue(text.find("DataServiceApi") == std::string::npos, "presenter does not include DataServiceApi");
        expectTrue(text.find("DataAccess") == std::string::npos, "presenter does not include DataAccess");
        expectTrue(text.find("AccountingEngine") == std::string::npos, "presenter does not include AccountingEngine");
        expectTrue(text.find("SQLite") == std::string::npos, "presenter does not include SQLite");
        expectTrue(text.find("QtQuick") == std::string::npos, "presenter does not include QtQuick");
        expectTrue(text.find("ShellAccountingDataServiceClientPortAdapter") == std::string::npos, "presenter does not include concrete client port");
        expectTrue(text.find("ShellAccountingDataServiceAdapter") == std::string::npos, "presenter does not include data service adapter");
        expectTrue(text.find("confirmTradeDraft") == std::string::npos, "presenter does not expose confirmTradeDraft");
        expectTrue(text.find("brokerOrder") == std::string::npos, "presenter does not expose brokerOrder");
        expectTrue(text.find("strategyExecute") == std::string::npos, "presenter does not expose strategyExecute");
        expectTrue(text.find("qmlRegisterType") == std::string::npos, "presenter does not register QML type");
    }

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
        expectTrue(text.find("ShellAccountingFacade") == std::string::npos, "QML does not bind accounting facade");
    }

    return 0;
}
