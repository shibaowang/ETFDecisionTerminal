#include "ForbiddenSqlScanner.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <set>

namespace etfdt::tests::accounting_no_write {
namespace {

std::string toUpper(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::toupper(ch));
    });
    return text;
}

bool isWordChar(char ch)
{
    return std::isalnum(static_cast<unsigned char>(ch)) || ch == '_';
}

bool containsKeyword(const std::string& upperText, const std::string& keyword)
{
    std::size_t pos = upperText.find(keyword);
    while (pos != std::string::npos) {
        const bool leftBoundary = pos == 0 || !isWordChar(upperText[pos - 1]);
        const auto right = pos + keyword.size();
        const bool rightBoundary = right >= upperText.size() || !isWordChar(upperText[right]);
        if (leftBoundary && rightBoundary) {
            return true;
        }
        pos = upperText.find(keyword, pos + 1);
    }
    return false;
}

bool containsPhrase(const std::string& upperText, const std::string& phrase)
{
    return upperText.find(phrase) != std::string::npos;
}

bool hasExtension(const std::filesystem::path& path, const std::vector<std::string>& extensions)
{
    const auto extension = path.extension().string();
    return std::find(extensions.begin(), extensions.end(), extension) != extensions.end();
}

}  // namespace

std::vector<ForbiddenSqlMatch> findForbiddenWriteKeywords(const std::string& sqlText)
{
    const auto upperText = toUpper(sqlText);
    std::vector<ForbiddenSqlMatch> matches;
    std::set<std::string> seen;

    const std::vector<std::string> keywords = {
        "INSERT",
        "UPDATE",
        "DELETE",
        "CREATE",
        "DROP",
        "ALTER",
        "REPLACE",
        "UPSERT",
        "VACUUM",
    };

    for (const auto& keyword : keywords) {
        if (containsKeyword(upperText, keyword) && seen.insert(keyword).second) {
            matches.push_back({keyword});
        }
    }

    const std::vector<std::pair<std::string, std::string>> phrases = {
        {"PRAGMA WRITABLE_SCHEMA", "PRAGMA writable_schema"},
        {"BEGIN IMMEDIATE", "BEGIN IMMEDIATE"},
        {"BEGIN EXCLUSIVE", "BEGIN EXCLUSIVE"},
    };

    for (const auto& [needle, label] : phrases) {
        if (containsPhrase(upperText, needle) && seen.insert(label).second) {
            matches.push_back({label});
        }
    }

    return matches;
}

bool containsForbiddenWriteSql(const std::string& sqlText)
{
    return !findForbiddenWriteKeywords(sqlText).empty();
}

std::vector<ForbiddenSqlMatch> scanFile(const std::filesystem::path& path)
{
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        return {{"SCAN_FILE_OPEN_FAILED"}};
    }

    const std::string text{
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()};
    return findForbiddenWriteKeywords(text);
}

std::vector<ForbiddenSqlMatch> scanDirectory(
    const std::filesystem::path& root,
    const std::vector<std::string>& extensions)
{
    std::vector<ForbiddenSqlMatch> matches;
    if (!std::filesystem::exists(root)) {
        return matches;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file() || !hasExtension(entry.path(), extensions)) {
            continue;
        }

        auto fileMatches = scanFile(entry.path());
        matches.insert(matches.end(), fileMatches.begin(), fileMatches.end());
    }
    return matches;
}

}  // namespace etfdt::tests::accounting_no_write
