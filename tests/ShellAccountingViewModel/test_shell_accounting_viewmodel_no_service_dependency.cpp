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
    const std::vector<std::filesystem::path> files{
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingStatusObject.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingStatusObject.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingIssueListModel.h",
        root / "libs" / "ShellServices" / "src" / "ShellAccountingIssueListModel.cpp",
        root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellPositionListModel.h",
        root / "libs" / "ShellServices" / "src" / "ShellPositionListModel.cpp",
    };

    for (const auto& file : files) {
        const auto text = readFile(file);
        expectTrue(text.find("DataServiceClient") == std::string::npos, "ViewModel / Model does not include DataServiceClient");
        expectTrue(text.find("DataServiceApi") == std::string::npos, "ViewModel / Model does not include DataServiceApi");
        expectTrue(text.find("DataAccess") == std::string::npos, "ViewModel / Model does not include DataAccess");
        expectTrue(text.find("AccountingEngine") == std::string::npos, "ViewModel / Model does not include AccountingEngine");
        expectTrue(text.find("SQLite") == std::string::npos, "ViewModel / Model does not include SQLite");
        expectTrue(text.find("QtQuick") == std::string::npos, "ViewModel / Model does not include QtQuick");
        expectTrue(text.find("data.audit.append") == std::string::npos, "ViewModel / Model does not call audit append");
        expectTrue(text.find("position.list") == std::string::npos, "ViewModel / Model does not call position.list");
        expectTrue(text.find("cash.summary") == std::string::npos, "ViewModel / Model does not call cash.summary");
    }

    const auto qmlDir = root / "apps" / "ETFDecisionShell" / "qml";
    for (const auto& entry : std::filesystem::recursive_directory_iterator(qmlDir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".qml") {
            continue;
        }
        const auto text = readFile(entry.path());
        expectTrue(text.find("ShellAccountingStatusObject") == std::string::npos, "QML does not bind accounting status object");
        expectTrue(text.find("ShellAccountingIssueListModel") == std::string::npos, "QML does not bind accounting issue model");
        expectTrue(text.find("ShellPositionListModel") == std::string::npos, "QML does not bind accounting position model");
    }

    return 0;
}
