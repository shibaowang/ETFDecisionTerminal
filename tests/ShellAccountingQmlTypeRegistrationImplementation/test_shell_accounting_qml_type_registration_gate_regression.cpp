#include "ShellAccountingQmlTypeRegistrationImplementationTestUtils.h"

using namespace etfdt::tests::shell_accounting_qml_type_registration_implementation;

int main(int argc, char** argv)
{
    const auto root = sourceRoot(argc, argv);
    const auto testsCmake = readTextFile(root / "tests" / "CMakeLists.txt");
    const auto docs12 = readTextFile(root / "docs" / "12_codex_prompt_template.md");
    const auto readme = readTextFile(root / "README.md");

    expectTrue(testsCmake.find("ShellAccountingQmlStaticGate") != std::string::npos, "TASK-119 static gate exists");
    expectTrue(testsCmake.find("ShellAccountingQmlBindingSmoke") != std::string::npos, "TASK-121 scaffold exists");
    expectTrue(testsCmake.find("ShellAccountingQmlBindingSmokeRuntime") != std::string::npos, "TASK-122 runtime smoke exists");
    expectTrue(testsCmake.find("ShellAccountingQmlTypeRegistrationScaffold") != std::string::npos, "TASK-125 scaffold exists");
    expectTrue(testsCmake.find("ShellAccountingQmlTypeRegistrationGate") != std::string::npos, "TASK-126 gate exists");
    expectTrue(
        std::filesystem::exists(root / "docs" / "65_shell_accounting_qml_type_registration_boundary.md"),
        "docs/65 exists");
    expectTrue(
        std::filesystem::exists(root / "docs" / "66_shell_accounting_qml_type_registration_test_plan.md"),
        "docs/66 exists");
    expectTrue(
        std::filesystem::exists(root / "docs" / "67_shell_accounting_qml_type_registration_implementation_gate.md"),
        "docs/67 exists");
    expectTrue(
        std::filesystem::exists(root / "docs" / "68_shell_accounting_qml_type_registration_rollback_strategy.md"),
        "docs/68 exists");
    expectTrue(readme.find("ShellAccounting QML type registration implementation") != std::string::npos, "README mentions implementation");
    expectTrue(docs12.find("TASK-127") != std::string::npos, "docs/12 mentions TASK-127");
    expectTrue(
        std::filesystem::exists(
            root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingQmlRegistration.h"),
        "production registration helper header exists");
    return 0;
}
