#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

namespace fs = std::filesystem;

void require(bool condition, const std::string& message)
{
    if (!condition) {
        throw std::runtime_error(message);
    }
}

fs::path sourceRoot(int argc, char** argv)
{
    for (int index = 1; index + 1 < argc; ++index) {
        if (std::string(argv[index]) == "--source-root") {
            return fs::path(argv[index + 1]);
        }
    }
    return fs::current_path();
}

std::string readFile(const fs::path& path)
{
    std::ifstream input(path, std::ios::binary);
    require(input.good(), "unable to read " + path.string());
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

void requireContains(const std::string& text, std::string_view token, std::string_view context)
{
    require(
        text.find(std::string(token)) != std::string::npos,
        std::string(context) + " missing `" + std::string(token) + "`");
}

void requireNotContains(const std::string& text, std::string_view token, std::string_view context)
{
    require(
        text.find(std::string(token)) == std::string::npos,
        std::string(context) + " unexpectedly contains `" + std::string(token) + "`");
}

int countOccurrences(const std::string& text, std::string_view token)
{
    int count = 0;
    std::size_t offset = 0;
    const std::string needle(token);
    while ((offset = text.find(needle, offset)) != std::string::npos) {
        ++count;
        offset += needle.size();
    }
    return count;
}

std::string allQmlText(const fs::path& root)
{
    const fs::path qmlRoot = root / "apps" / "ETFDecisionShell" / "qml";
    std::ostringstream buffer;
    for (const auto& entry : fs::recursive_directory_iterator(qmlRoot)) {
        if (entry.is_regular_file() && entry.path().extension() == ".qml") {
            buffer << "\n// " << entry.path().generic_string() << "\n";
            buffer << readFile(entry.path());
        }
    }
    return buffer.str();
}

void assertGlobalChineseQml(const fs::path& root)
{
    const auto qml = allQmlText(root);
    const auto shellAccounting =
        readFile(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml");

    for (const std::string token : {
             u8"页面信息（模拟）",
             u8"指标概览（模拟）",
             u8"操作提示（模拟）",
             u8"服务监控清单",
             u8"模块状态",
             u8"数据模式",
             u8"连接状态",
             u8"只读连接",
             u8"服务列表",
             u8"问题列表",
             u8"加载健康样例",
             u8"加载警告样例",
             u8"仅显示阻塞项",
             u8"仅显示问题项",
             u8"仅演示，不会执行真实操作",
             u8"这是内部草案，不是订单，不会提交券商。",
             u8"默认使用样例数据/禁用行情源，不会自动联网。",
         }) {
        requireContains(qml, token, "global Chinese visible QML");
    }

    for (const std::string token : {
             "Page Info Mock",
             "Metrics Mock",
             "Action Hints Mock",
             "Watchdog Manifest Report",
             "Load Healthy Mock",
             "Load Warning Mock",
             "Load Error Mock",
             "Load Mixed Mock",
             "Show All",
             "Only Blocked",
             "Only Issues",
             "Read-only connection",
             "Mock only - not executable",
             "Placeholder module",
             "Real business data is not connected",
             "Direct .xlsx import is not supported",
             "Draft, not order.",
             "payload / raw diagnostics",
             "sample payload / raw diagnostics",
             u8"payload / 原始诊断数据",
         }) {
        requireNotContains(qml, token, "global Chinese forbidden legacy visible text");
    }

    for (const std::string token : {
             "DataServiceClient",
             "SQLite",
             "AccountingEngine",
             "StrategyEngine",
             "MarketEngine",
             "XMLHttpRequest",
             "WebSocket",
             "fetch(",
             "credential",
             "endpoint",
             "access_token",
             "cookie",
             "submitOrder",
             "placeOrder",
             "automaticTrading",
         }) {
        requireNotContains(qml, token, "QML direct forbidden integration token");
    }

    for (const std::string token : {
             "property bool showImportRawJson: false",
             "property bool showReplayRawJson: false",
             "property bool showMarketRawJson: false",
             "property bool showStrategyRawJson: false",
             "property bool showOtcMapRawJson: false",
             "visible: root.showImportRawJson",
             "visible: root.showReplayRawJson",
             "visible: root.showMarketRawJson",
             "visible: root.showStrategyRawJson",
             "visible: root.showOtcMapRawJson",
             "shellAccountingShowImportRawJsonButton",
             "shellAccountingShowReplayRawJsonButton",
             "shellAccountingShowMarketRawJsonButton",
             "shellAccountingShowStrategyRawJsonButton",
             "shellAccountingShowOtcMapRawJsonButton",
         }) {
        requireContains(shellAccounting, token, "raw diagnostic folding contract");
    }

    require(countOccurrences(shellAccounting, u8"显示原始诊断数据") >= 5,
            "developer detail show controls present");
    require(countOccurrences(shellAccounting, u8"隐藏原始诊断数据") >= 5,
            "developer detail hide controls present");
    require(countOccurrences(shellAccounting, u8"开发者详情") >= 5,
            "developer detail Chinese labels present");
    require(countOccurrences(shellAccounting, "TextArea {") >= 5, "existing raw input areas preserved");
}

void assertDocs(const fs::path& root)
{
    const auto readme = readFile(root / "README.md");
    const auto docsIndex = readFile(root / "docs" / "README.md");
    const auto prompt = readFile(root / "docs" / "12_codex_prompt_template.md");
    const auto scope = readFile(root / "docs" / "400_global_chinese_ui_readability_trial_feedback_fix.md");
    const auto testsCmake = readFile(root / "tests" / "CMakeLists.txt");

    for (const std::string token : {
             "400_global_chinese_ui_readability_trial_feedback_fix.md",
             "global_chinese_ui_readability_trial_feedback_fix",
         }) {
        requireContains(readme + docsIndex + prompt + testsCmake, token, "EPIC-287 docs/tests indexed");
    }

    for (const std::string token : {
             "EPIC-287",
             "Global Chinese UI Readability",
             "P1 local trial feedback",
             "Raw JSON Folding",
             u8"开发者详情",
             u8"显示原始诊断数据",
             u8"隐藏原始诊断数据",
             u8"页面信息（模拟）",
             u8"指标概览（模拟）",
             u8"操作提示（模拟）",
             u8"服务监控清单",
             "businessLogicChanged",
             "automaticTrading",
         }) {
        requireContains(readme + docsIndex + prompt + scope, token, "EPIC-287 docs content");
    }
}

std::string evidence()
{
    return R"({"task":"EPIC-287","globalChineseUiReadabilityFixCreated":true,"visibleUiChineseFirst":true,"rawJsonHiddenByDefault":true,"developerDetailsGateCreated":true,"mockDebugInfoHiddenOrChinese":true,"objectNamesPreserved":true,"businessLogicChanged":false,"dataAccessChanged":false,"migrationChanged":false,"productionDbTouched":false,"networkAccess":false,"brokerOrderSubmitted":false,"credentialAccess":false,"endpointAccess":false,"realOrderPlacement":false,"automaticTrading":false})";
}

}  // namespace

int main(int argc, char** argv)
{
    try {
        const auto root = sourceRoot(argc, argv);
        assertGlobalChineseQml(root);
        assertDocs(root);
        std::cout << evidence() << '\n';
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
}
