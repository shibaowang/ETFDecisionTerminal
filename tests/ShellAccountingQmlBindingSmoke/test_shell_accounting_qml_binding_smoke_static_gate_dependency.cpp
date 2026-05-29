#include "ShellAccountingQmlSmokeExpectedBinding.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace etfdt::tests::shell_accounting_qml_binding_smoke;

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

std::string readText(const std::filesystem::path& path)
{
    std::ifstream input(path);
    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

bool contains(const std::string& text, const std::string& needle)
{
    return text.find(needle) != std::string::npos;
}

}  // namespace

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto staticGateCMakePath = root / "tests" / "ShellAccountingQmlStaticGate" / "CMakeLists.txt";
    const auto docs59Path = root / "docs" / "59_shell_accounting_qml_binding_smoke_plan.md";
    const auto docs60Path = root / "docs" / "60_shell_accounting_qml_static_gate.md";
    const auto docs61Path = root / "docs" / "61_shell_accounting_readonly_ui_readiness_milestone.md";
    const auto docs62Path = root / "docs" / "62_shell_accounting_next_phase_review.md";

    for (const auto& path : {staticGateCMakePath, docs59Path, docs60Path, docs61Path, docs62Path}) {
        if (!std::filesystem::exists(path)) {
            std::cerr << "Missing required static gate dependency file: " << path.generic_string() << '\n';
            return 1;
        }
    }

    const auto staticGateCMake = readText(staticGateCMakePath);
    for (const auto& testName : requiredStaticGateTests()) {
        if (!contains(staticGateCMake, testName)) {
            std::cerr << "Missing TASK-119 static gate CTest dependency: " << testName << '\n';
            return 1;
        }
    }

    const auto docs59 = readText(docs59Path);
    const auto docs60 = readText(docs60Path);
    const auto docs61 = readText(docs61Path);
    const auto docs62 = readText(docs62Path);
    if (!contains(docs59, "smoke plan") && !contains(docs59, "Smoke")) {
        std::cerr << "docs/59 must still describe the smoke plan.\n";
        return 1;
    }
    if (!contains(docs60, "static gate") && !contains(docs60, "Static Gate")) {
        std::cerr << "docs/60 must still describe the static gate.\n";
        return 1;
    }
    if (!contains(docs61, "QML static gate")) {
        std::cerr << "docs/61 must still reference QML static gate readiness.\n";
        return 1;
    }
    if (!contains(docs62, "用户明确授权 QML binding")) {
        std::cerr << "docs/62 must still require explicit QML binding authorization.\n";
        return 1;
    }
    return 0;
}
