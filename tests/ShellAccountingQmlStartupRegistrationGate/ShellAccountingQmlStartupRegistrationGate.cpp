#include "ShellAccountingQmlStartupRegistrationGate.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_qml_startup_registration_gate {
namespace {

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

bool isAllowedNegativeLine(const std::string& line)
{
    return line.find("not ") != std::string::npos || line.find("no ") != std::string::npos ||
           line.find("Do not") != std::string::npos || line.find("must not") != std::string::npos ||
           (line.find("QML") != std::string::npos &&
            line.find("DataServiceClient") != std::string::npos &&
            line.find("SQLite") != std::string::npos);
}

bool containsAll(const std::string& text, const std::vector<std::string>& tokens)
{
    for (const auto& token : tokens) {
        if (text.find(token) == std::string::npos) {
            return false;
        }
    }
    return true;
}

bool isAllowedEpic281DashboardToken(
    const std::filesystem::path& file,
    const std::string& text,
    const std::string& token)
{
    if (file.filename().string() != "ShellAccountingReadOnlyPage.qml" ||
        token != "createTradeDraft") {
        return false;
    }
    return containsAll(
        text,
        {
            "shellAccountingDashboardRoot",
            "shellAccountingTradeDraftPanel",
            "shellAccountingTradeDraftConfirmationCheckBox",
            "previewTradeDraftFromLastRecommendation()",
            "createTradeDraftFromLastRecommendation(true)",
            "Draft, not order",
            "not order",
        });
}

}  // namespace

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

std::vector<std::filesystem::path> productionStartupFiles(const std::filesystem::path& root)
{
    return filesUnder(root / "apps" / "ETFDecisionShell", {".cpp", ".h", ".hpp"});
}

std::vector<std::filesystem::path> productionQmlFiles(const std::filesystem::path& root)
{
    return filesUnder(root / "apps" / "ETFDecisionShell" / "qml", {".qml", ".js"});
}

std::vector<std::filesystem::path> productionAppFiles(const std::filesystem::path& root)
{
    auto files = productionStartupFiles(root);
    const auto qmlFiles = productionQmlFiles(root);
    files.insert(files.end(), qmlFiles.begin(), qmlFiles.end());
    return files;
}

std::vector<std::string> requiredPreGateTokens()
{
    return {"TASK-119", "TASK-121", "TASK-122", "TASK-125", "TASK-126", "TASK-127"};
}

std::vector<std::string> locationPolicyTokens()
{
    return {
        "centralized",
        "easy to static scan",
        "No scattered startup registration",
        "No hidden plugin registration",
        "No test-only registration used as production registration",
        "future location must be documented",
    };
}

std::vector<std::string> rollbackPolicyTokens()
{
    return {
        "Rollback",
        "disable",
        "must not call `DataServiceClient`",
        "must not fake data",
        "must not write",
        "Unavailable",
    };
}

std::vector<std::string> forbiddenExposureTokens()
{
    return {
        "AccountingEngine",
        "SQLite",
        "DataAccess",
        "createTradeDraft",
        "brokerOrder",
        "strategyExecute",
        "writeEnabled: true",
        "data.audit.append",
    };
}

bool containsToken(const std::vector<std::filesystem::path>& files, const std::string& token)
{
    for (const auto& file : files) {
        const auto text = readTextFile(file);
        if (text.find(token) != std::string::npos &&
            !isAllowedEpic281DashboardToken(file, text, token)) {
            std::cerr << file.generic_string() << ": unexpected token `" << token << "`\n";
            return true;
        }
    }
    return false;
}

bool containsForbiddenExposure(
    const std::vector<std::filesystem::path>& files,
    const std::vector<std::string>& tokens)
{
    for (const auto& file : files) {
        const auto fileText = readTextFile(file);
        std::ifstream input(file);
        std::string line;
        int lineNumber = 0;
        while (std::getline(input, line)) {
            ++lineNumber;
            for (const auto& token : tokens) {
                if (line.find(token) == std::string::npos || isAllowedNegativeLine(line)) {
                    continue;
                }
                if (isAllowedEpic281DashboardToken(file, fileText, token)) {
                    continue;
                }
                std::cerr << file.generic_string() << ':' << lineNumber
                          << ": forbidden exposure token `" << token << "`\n";
                return true;
            }
        }
    }
    return false;
}

}  // namespace etfdt::tests::shell_accounting_qml_startup_registration_gate
