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

bool contains(const std::string& text, const std::string& token)
{
    return text.find(token) != std::string::npos;
}

}  // namespace

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto docs65 = root / "docs" / "65_shell_accounting_qml_type_registration_boundary.md";
    const auto docs66 = root / "docs" / "66_shell_accounting_qml_type_registration_test_plan.md";
    const auto docs63 = root / "docs" / "63_shell_accounting_production_qml_binding_boundary.md";
    const auto docs64 = root / "docs" / "64_shell_accounting_first_stage_qml_binding_checklist.md";
    const auto docs60 = root / "docs" / "60_shell_accounting_qml_static_gate.md";
    const auto docs59 = root / "docs" / "59_shell_accounting_qml_binding_smoke_plan.md";
    const auto docs12 = root / "docs" / "12_codex_prompt_template.md";
    const auto readme = root / "README.md";

    for (const auto& path : {docs65, docs66, docs63, docs64, docs60, docs59, docs12, readme}) {
        if (!std::filesystem::exists(path)) {
            std::cerr << "Missing required registration gate document: "
                      << path.generic_string() << '\n';
            return 1;
        }
    }

    const auto boundary = readTextFile(docs65);
    const auto plan = readTextFile(docs66);
    const auto prompt = readTextFile(docs12);
    const auto readmeText = readTextFile(readme);

    for (const auto& gate : {"TASK-119", "TASK-121", "TASK-122"}) {
        if (!contains(boundary, gate) || !contains(plan, gate)) {
            std::cerr << "Registration docs must require gate: " << gate << '\n';
            return 1;
        }
    }
    if (!contains(prompt, "docs/65_shell_accounting_qml_type_registration_boundary.md") ||
        !contains(prompt, "docs/66_shell_accounting_qml_type_registration_test_plan.md")) {
        std::cerr << "Codex prompt template must reference docs/65 and docs/66.\n";
        return 1;
    }
    if (!contains(readmeText, "QML type registration boundary")) {
        std::cerr << "README must mention QML type registration boundary.\n";
        return 1;
    }
    if (!contains(plan, "用户明确授权 QML type registration")) {
        std::cerr << "Registration test plan must require explicit user authorization.\n";
        return 1;
    }
    return 0;
}
