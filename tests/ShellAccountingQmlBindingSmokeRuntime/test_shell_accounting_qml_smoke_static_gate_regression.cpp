#include "ShellAccountingQmlSmokeTestObjects.h"

#include <QCoreApplication>

using namespace etfdt::tests::shell_accounting_qml_smoke;

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    const std::filesystem::path root = sourceRootFromArgs(argc, argv);

    const QString testsCmake = readTextFile(root / "tests" / "CMakeLists.txt");
    const QString staticGateCmake =
        readTextFile(root / "tests" / "ShellAccountingQmlStaticGate" / "CMakeLists.txt");
    const QString smokeCmake =
        readTextFile(root / "tests" / "ShellAccountingQmlBindingSmokeRuntime" / "CMakeLists.txt");
    const QString docs59 = readTextFile(root / "docs" / "59_shell_accounting_qml_binding_smoke_plan.md");
    const QString docs60 = readTextFile(root / "docs" / "60_shell_accounting_qml_static_gate.md");
    const QString docs61 = readTextFile(root / "docs" / "61_shell_accounting_readonly_ui_readiness_milestone.md");
    const QString docs62 = readTextFile(root / "docs" / "62_shell_accounting_next_phase_review.md");
    const QString readme = readTextFile(root / "README.md");

    expectTrue(testsCmake.contains(QStringLiteral("ShellAccountingQmlBindingSmokeRuntime")), "runtime smoke test directory is registered");
    expectTrue(smokeCmake.contains(QStringLiteral("shell_accounting_qml_smoke_object_availability")), "object availability CTest exists");
    expectTrue(smokeCmake.contains(QStringLiteral("shell_accounting_qml_smoke_state_display")), "state display CTest exists");
    expectTrue(smokeCmake.contains(QStringLiteral("shell_accounting_qml_smoke_guard_unavailable")), "guard unavailable CTest exists");
    expectTrue(smokeCmake.contains(QStringLiteral("shell_accounting_qml_smoke_issue_visibility")), "issue visibility CTest exists");
    expectTrue(smokeCmake.contains(QStringLiteral("shell_accounting_qml_smoke_privacy_display")), "privacy display CTest exists");
    expectTrue(smokeCmake.contains(QStringLiteral("shell_accounting_qml_smoke_no_trade_ui")), "no-trade UI CTest exists");
    expectTrue(smokeCmake.contains(QStringLiteral("shell_accounting_qml_smoke_static_gate_regression")), "static gate regression CTest exists");

    expectTrue(staticGateCmake.contains(QStringLiteral("shell_accounting_qml_static_gate_no_direct_service")), "TASK-119 no direct service CTest exists");
    expectTrue(staticGateCmake.contains(QStringLiteral("shell_accounting_qml_static_gate_no_accounting_binding_yet")), "TASK-119 no accounting binding CTest exists");
    expectTrue(staticGateCmake.contains(QStringLiteral("shell_accounting_qml_static_gate_no_write_tokens")), "TASK-119 no write token CTest exists");
    expectTrue(staticGateCmake.contains(QStringLiteral("shell_accounting_qml_static_gate_accounting_scope_no_trade")), "TASK-119 accounting scope no-trade CTest exists");
    expectTrue(staticGateCmake.contains(QStringLiteral("shell_accounting_qml_static_gate_docs_sync")), "TASK-119 docs sync CTest exists");

    expectTrue(docs59.contains(QStringLiteral("TASK-122")), "docs/59 mentions TASK-122");
    expectTrue(docs60.contains(QStringLiteral("TASK-122")), "docs/60 mentions TASK-122");
    expectTrue(docs61.contains(QStringLiteral("TASK-122")), "docs/61 mentions TASK-122");
    expectTrue(docs62.contains(QStringLiteral("TASK-122")), "docs/62 mentions TASK-122");
    expectTrue(readme.contains(QStringLiteral("ShellAccounting QML smoke CTest")), "README mentions smoke CTest");

    return failureCount() == 0 ? 0 : 1;
}
