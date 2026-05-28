#include "QmlStaticGateScanner.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace etfdt::tests::shell_accounting_qml_static_gate;

namespace {

bool contains(const std::string& text, const std::string& needle)
{
    return text.find(needle) != std::string::npos;
}

void requireText(
    bool condition,
    const std::string& message,
    std::vector<std::string>& failures)
{
    if (!condition) {
        failures.push_back(message);
    }
}

}  // namespace

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs59Path = root / "docs" / "59_shell_accounting_qml_binding_smoke_plan.md";
    const auto docs60Path = root / "docs" / "60_shell_accounting_qml_static_gate.md";
    const auto docs12Path = root / "docs" / "12_codex_prompt_template.md";
    const auto readmePath = root / "README.md";

    std::vector<std::string> failures;
    requireText(std::filesystem::exists(docs59Path), "docs/59 exists", failures);
    requireText(std::filesystem::exists(docs60Path), "docs/60 exists", failures);

    const auto docs60 = readTextFile(docs60Path);
    const auto docs12 = readTextFile(docs12Path);
    const auto readme = readTextFile(readmePath);

    requireText(contains(docs60, "DataServiceClient"), "docs/60 mentions DataServiceClient", failures);
    requireText(contains(docs60, "TradeDraft"), "docs/60 mentions TradeDraft", failures);
    requireText(contains(docs60, "brokerOrder"), "docs/60 mentions brokerOrder", failures);
    requireText(contains(docs60, "strategyExecute"), "docs/60 mentions strategyExecute", failures);
    requireText(contains(docs60, "writeEnabled: true"), "docs/60 mentions writeEnabled: true", failures);
    requireText(contains(docs60, "Empty is not Unavailable"), "docs/60 states Empty is not Unavailable", failures);
    requireText(
        contains(docs60, "User explicitly authorized QML integration") ||
            contains(docs60, "用户明确授权 QML 接入"),
        "docs/60 requires explicit QML authorization",
        failures);
    requireText(contains(docs12, "TASK-119"), "docs/12 mentions TASK-119", failures);
    requireText(contains(readme, "QML static gate CTest"), "README mentions QML static gate CTest", failures);

    if (!failures.empty()) {
        for (const auto& failure : failures) {
            std::cerr << failure << '\n';
        }
        return 1;
    }
    return 0;
}
