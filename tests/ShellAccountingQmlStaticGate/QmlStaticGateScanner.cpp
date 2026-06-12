#include "QmlStaticGateScanner.h"

#include <algorithm>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>

namespace etfdt::tests::shell_accounting_qml_static_gate {
namespace {

bool hasExtension(const std::filesystem::path& path)
{
    const auto extension = path.extension().string();
    return extension == ".qml" || extension == ".js";
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

bool containsAny(const std::string& text, const std::vector<std::string>& tokens)
{
    return std::any_of(tokens.begin(), tokens.end(), [&](const std::string& token) {
        return text.find(token) != std::string::npos;
    });
}

std::string bytes(std::initializer_list<unsigned char> values)
{
    std::string result;
    result.reserve(values.size());
    for (const auto value : values) {
        result.push_back(static_cast<char>(value));
    }
    return result;
}

bool isAllowedNegativeDirectServiceStatement(
    const std::string& line,
    const std::string& token)
{
    if (token == "DataServiceClient") {
        const auto qmlDoesNotCallDataServiceClient =
            std::string("QML ") +
            bytes({0xE4, 0xB8, 0x8D, 0xE7, 0x9B, 0xB4, 0xE6, 0x8E, 0xA5,
                   0xE8, 0xB0, 0x83, 0xE7, 0x94, 0xA8}) +
            " DataServiceClient";
        return line.find(qmlDoesNotCallDataServiceClient) != std::string::npos;
    }
    if (token == "SQLite") {
        const auto doesNotAccessSqlite =
            bytes({0xE4, 0xB8, 0x8D, 0xE8, 0xAE, 0xBF, 0xE9, 0x97, 0xAE}) +
            std::string(" SQLite");
        return line.find(doesNotAccessSqlite) != std::string::npos;
    }
    return false;
}

bool containsAll(const std::string& text, const std::vector<std::string>& tokens)
{
    return std::all_of(tokens.begin(), tokens.end(), [&](const std::string& token) {
        return text.find(token) != std::string::npos;
    });
}

bool isAllowedEpic281DashboardDraftLine(
    const QmlFile& file,
    const std::string& line,
    const std::string& token)
{
    if (file.path.filename().string() != "ShellAccountingReadOnlyPage.qml") {
        return false;
    }
    if (token != "TradeDraft" && token != "createTradeDraft") {
        return false;
    }
    if (!containsAll(
            file.text,
            {
                "shellAccountingDashboardRoot",
                "shellAccountingTradeDraftPanel",
                "shellAccountingTradeDraftConfirmationCheckBox",
                "previewTradeDraftFromLastRecommendation()",
                "createTradeDraftFromLastRecommendation(true)",
                bytes({0xE8, 0xBF, 0x99, 0xE6, 0x98, 0xAF, 0xE5, 0x86,
                       0x85, 0xE9, 0x83, 0xA8, 0xE8, 0x8D, 0x89, 0xE6,
                       0xA1, 0x88}),
                bytes({0xE4, 0xB8, 0x8D, 0xE6, 0x98, 0xAF, 0xE8, 0xAE,
                       0xA2, 0xE5, 0x8D, 0x95}),
            })) {
        return false;
    }

    const std::vector<std::string> allowedFragments{
        "lastTradeDraftIssueCodes",
        "resetTradeDraftState()",
        "shellAccountingTradeDraft",
        "showTradeDraftRawJson",
        "TradeDraft from recommendation",
        "Preview TradeDraft",
        "previewTradeDraftFromLastRecommendation()",
        "lastTradeDraftStatus",
        "createTradeDraftFromLastRecommendation(true)",
        "lastTradeDraftDuplicate",
        "lastTradeDraftIdempotencyConflict",
        "lastTradeDraftSummary",
        "lastTradeDraftId",
        "createOtcMapMultiChannelTradeDraft(true)",
    };
    return containsAny(line, allowedFragments);
}

std::vector<QmlHit> scanTokensInternal(
    const std::vector<QmlFile>& files,
    const std::vector<std::string>& tokens,
    bool accountingScopeOnly)
{
    std::vector<QmlHit> hits;
    for (const auto& file : files) {
        if (accountingScopeOnly && !isAccountingScopedFile(file)) {
            continue;
        }

        for (std::size_t index = 0; index < file.lines.size(); ++index) {
            const auto& line = file.lines[index];
            for (const auto& token : tokens) {
                if (line.find(token) == std::string::npos) {
                    continue;
                }
                if (!accountingScopeOnly &&
                    isAllowedNegativeDirectServiceStatement(line, token)) {
                    continue;
                }
                if (isAllowedEpic281DashboardDraftLine(file, line, token)) {
                    continue;
                }
                hits.push_back({file.path, static_cast<int>(index + 1), token, line});
            }
        }
    }
    return hits;
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

std::vector<QmlFile> loadQmlFiles(const std::filesystem::path& sourceRoot)
{
    const auto qmlRoot = sourceRoot / "apps" / "ETFDecisionShell" / "qml";
    std::vector<QmlFile> files;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(qmlRoot)) {
        if (!entry.is_regular_file() || !hasExtension(entry.path())) {
            continue;
        }

        auto text = readTextFile(entry.path());
        files.push_back({entry.path(), text, splitLines(text)});
    }
    std::sort(files.begin(), files.end(), [](const QmlFile& lhs, const QmlFile& rhs) {
        return lhs.path.generic_string() < rhs.path.generic_string();
    });
    return files;
}

std::vector<QmlHit> scanTokens(
    const std::vector<QmlFile>& files,
    const std::vector<std::string>& tokens)
{
    return scanTokensInternal(files, tokens, false);
}

std::vector<QmlHit> scanTokensInAccountingScope(
    const std::vector<QmlFile>& files,
    const std::vector<std::string>& tokens)
{
    return scanTokensInternal(files, tokens, true);
}

bool isAccountingScopedFile(const QmlFile& file)
{
    const auto filename = file.path.filename().string();
    const std::vector<std::string> filenameTokens{
        "Accounting",
        "PositionReadOnly",
        "PositionList",
        "CashSummary",
        "PortfolioPnl",
        "BasePosition",
        "SniperPool",
    };
    const std::vector<std::string> contentTokens{
        "ShellAccounting",
        "accountingPresenter",
        "ShellPositionListModel",
    };
    return containsAny(filename, filenameTokens) || containsAny(file.text, contentTokens);
}

void printHits(const std::vector<QmlHit>& hits)
{
    for (const auto& hit : hits) {
        std::cerr << hit.path.generic_string()
                  << ':' << hit.line
                  << ": forbidden token `" << hit.token << "` in: "
                  << hit.text << '\n';
    }
}

std::string readTextFile(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

}  // namespace etfdt::tests::shell_accounting_qml_static_gate
