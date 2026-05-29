#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::filesystem::path sourceRoot(int argc, char** argv)
{
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::string(argv[i]) == "--source-root") {
            return argv[i + 1];
        }
    }
    return std::filesystem::current_path();
}

std::string readTextFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

bool hasExtension(const std::filesystem::path& path, const std::vector<std::string>& extensions)
{
    const auto extension = path.extension().string();
    for (const auto& allowed : extensions) {
        if (extension == allowed) {
            return true;
        }
    }
    return false;
}

std::vector<std::filesystem::path> filesUnder(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions)
{
    std::vector<std::filesystem::path> result;
    if (!std::filesystem::exists(root)) {
        return result;
    }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (entry.is_regular_file() && hasExtension(entry.path(), extensions)) {
            result.push_back(entry.path());
        }
    }
    return result;
}

std::vector<std::string> splitLines(const std::string& text)
{
    std::vector<std::string> lines;
    std::istringstream input(text);
    std::string line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }
    return lines;
}

bool isAllowedNegativeQmlStatement(const std::string& line, const std::string& token)
{
    return (token == "DataServiceClient" || token == "SQLite") &&
           line.find("QML ") != std::string::npos &&
           line.find(token) != std::string::npos;
}

bool containsForbiddenToken(
    const std::vector<std::filesystem::path>& files,
    const std::vector<std::string>& tokens)
{
    for (const auto& file : files) {
        const auto lines = splitLines(readTextFile(file));
        for (std::size_t index = 0; index < lines.size(); ++index) {
            const auto& line = lines[index];
            for (const auto& token : tokens) {
                if (line.find(token) == std::string::npos ||
                    isAllowedNegativeQmlStatement(line, token)) {
                    continue;
                }
                std::cerr << file.generic_string() << ':' << (index + 1)
                          << ": forbidden production token `" << token << "`\n";
                return true;
            }
        }
    }
    return false;
}

}  // namespace

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto qmlRoot = root / "apps" / "ETFDecisionShell" / "qml";
    const auto appRoot = root / "apps" / "ETFDecisionShell";

    const auto qmlFiles = filesUnder(qmlRoot, {".qml", ".js"});
    const std::vector<std::string> qmlForbidden{
        "ShellAccountingPresenter",
        "ShellAccountingStatusObject",
        "ShellAccountingIssueListModel",
        "ShellPositionListModel",
        "accountingPresenter",
        "DataServiceClient",
        "AccountingEngine",
        "SQLite",
    };
    if (containsForbiddenToken(qmlFiles, qmlForbidden)) {
        return 1;
    }

    const auto productionSources = filesUnder(appRoot, {".cpp", ".h", ".hpp", ".qml", ".js"});
    const std::vector<std::string> registrationForbidden{
        "qmlRegisterType<ShellAccountingPresenter>",
        "qmlRegisterType<ShellAccountingStatusObject>",
        "qmlRegisterType<ShellAccountingIssueListModel>",
        "qmlRegisterType<ShellPositionListModel>",
        "qmlRegisterSingletonType<ShellAccountingPresenter>",
        "qmlRegisterSingletonType<ShellAccountingStatusObject>",
        "qmlRegisterSingletonType<ShellAccountingIssueListModel>",
        "qmlRegisterSingletonType<ShellPositionListModel>",
    };
    if (containsForbiddenToken(productionSources, registrationForbidden)) {
        return 1;
    }
    return 0;
}
