#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def require_contains(text: str, token: str, context: str) -> None:
    require(token in text, f"{context} missing `{token}`")


def require_not_contains(text: str, token: str, context: str) -> None:
    require(token not in text, f"{context} unexpectedly contains `{token}`")


def changed_paths(root: Path) -> set[str]:
    diff = subprocess.run(
        ["git", "diff", "--name-only", "main"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    untracked = subprocess.run(
        ["git", "ls-files", "--others", "--exclude-standard"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    paths = set()
    for output in (diff.stdout, untracked.stdout):
        paths.update(line.strip().replace("\\", "/") for line in output.splitlines() if line.strip())
    return paths


def function_body(text: str, signature: str) -> str:
    start = text.find(signature)
    require(start >= 0, f"missing function `{signature}`")
    brace = text.find("{", start)
    require(brace >= 0, f"missing function body `{signature}`")
    depth = 0
    for index in range(brace, len(text)):
        if text[index] == "{":
            depth += 1
        elif text[index] == "}":
            depth -= 1
            if depth == 0:
                return text[brace:index + 1]
    raise AssertionError(f"unterminated function body `{signature}`")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc198 = read(root / "docs" / "198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md")
    doc199 = read(root / "docs" / "199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md")
    doc200_path = root / "docs" / "200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md"
    doc201_path = root / "docs" / "201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md"
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    devdocs = read(root / "tests" / "DevDocs" / "test_readonly_demo_acceptance.py")
    qml = read(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml")
    presenter_h = read(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h")
    presenter_cpp = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp")
    controller_h = read(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h")
    controller_cpp = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp")
    dataservice_actions = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp")

    require(doc200_path.exists(), "docs/200 exists")
    require(doc201_path.exists(), "docs/201 exists")
    doc200 = read(doc200_path)
    doc201 = read(doc201_path)

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-202", context)
    require_contains(
        readme,
        "docs/200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md",
        "README",
    )
    require_contains(
        readme,
        "docs/201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md",
        "README",
    )
    require_contains(docs_index, "200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md", "docs/README")
    require_contains(docs_index, "201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualEntryPostWriteReadbackRefreshImplementation", "tests/CMakeLists")
    require_contains(doc198, "TASK-202 Implementation Update", "docs/198")
    require_contains(doc199, "TASK-202 Implementation Update", "docs/199")

    for token in [
        "TASK-202",
        "post-write readback refresh",
        "postWriteRefreshEnabled",
        "postWriteRefreshBusy",
        "lastPostWriteRefreshStatus",
        "lastPostWriteRefreshIssue",
        "lastPostWriteRefreshSummary",
        "shellAccountingPostWriteRefreshStatus",
        "shellAccountingPostWriteRefreshIssue",
        "shellAccountingPostWriteRefreshSummary",
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "does not add DataService actions",
        "does not modify DataServiceActions",
        "does not modify DataServiceActionRegistrar",
        "does not trigger AccountingEngine replay",
        "snapshot rebuild after write",
        "derived table refresh after write",
        "write `audit_log` or ledger rows",
        "real order placement",
        "automatic trading",
    ]:
        require_contains(doc200, token, "docs/200")

    for token in [
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "QML Status Boundary",
        "Presenter And Controller Boundary",
        "No Production Drift Outside Scope",
        "Successful manual transaction writes trigger post-write refresh",
        "Successful manual cash movement writes trigger post-write refresh",
        "Validation failures do not trigger refresh",
        "Write failures do not trigger refresh",
        "DataServiceActions.cpp is not modified by TASK-202",
        "No audit_log or ledger write is introduced",
        "No snapshot rebuild after write is introduced",
        "No derived table refresh after write is introduced",
        "No real order placement is introduced",
        "No automatic trading is introduced",
    ]:
        require_contains(doc201, token, "docs/201")

    for token in [
        "shellAccountingPostWriteRefreshStatus",
        "shellAccountingPostWriteRefreshIssue",
        "shellAccountingPostWriteRefreshSummary",
        "accountingPresenter.lastPostWriteRefreshStatus",
        "accountingPresenter.lastPostWriteRefreshIssue",
        "accountingPresenter.lastPostWriteRefreshSummary",
    ]:
        require_contains(qml, token, "ShellAccountingReadOnlyPage.qml")
    for token in [
        "DataServiceClient",
        "SQLite",
        "DataAccess",
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
        "positionValue",
        "cashBalance",
        "pnlAmount",
        "brokerOrder",
        "placeOrder",
        "realOrderId",
        "automaticTrading",
    ]:
        require_not_contains(qml, token, "production QML")

    for token in [
        "Q_PROPERTY(bool postWriteRefreshEnabled",
        "Q_PROPERTY(bool postWriteRefreshBusy",
        "Q_PROPERTY(QString lastPostWriteRefreshStatus",
        "Q_PROPERTY(QString lastPostWriteRefreshIssue",
        "Q_PROPERTY(QString lastPostWriteRefreshSummary",
        "postWriteRefreshStateChanged",
        "Q_INVOKABLE void resetPostWriteRefreshState",
        "Q_INVOKABLE bool refreshManualEntryReadback",
        "refreshAfterManualEntryWrite",
    ]:
        require_contains(presenter_h, token, "ShellAccountingPresenter.h")
    for token in [
        "ShellAccountingPresenter::postWriteRefreshEnabled",
        "ShellAccountingPresenter::postWriteRefreshBusy",
        "ShellAccountingPresenter::lastPostWriteRefreshStatus",
        "ShellAccountingPresenter::lastPostWriteRefreshIssue",
        "ShellAccountingPresenter::lastPostWriteRefreshSummary",
        "ShellAccountingPresenter::resetPostWriteRefreshState",
        "ShellAccountingPresenter::refreshManualEntryReadback",
        "ShellAccountingPresenter::refreshAfterManualEntryWrite",
        "refreshAfterManualEntryWrite(true, \"manual_transaction\")",
        "refreshAfterManualEntryWrite(false, \"manual_cash_movement\")",
        "\"position.list\"",
        "\"cash.summary\"",
        "\"portfolio.pnl.summary\"",
        "controller_->submitManualTransaction",
        "controller_->submitManualCashMovement",
        "controller_->currentState()",
        "Post-write readback refresh failed.",
    ]:
        require_contains(presenter_cpp, token, "ShellAccountingPresenter.cpp")

    transaction_body = function_body(presenter_cpp, "bool ShellAccountingPresenter::submitManualTransaction")
    cash_body = function_body(presenter_cpp, "bool ShellAccountingPresenter::submitManualCashMovement")
    for body, context, refresh_call in [
        (transaction_body, "manual transaction submit", 'refreshAfterManualEntryWrite(true, "manual_transaction")'),
        (cash_body, "manual cash movement submit", 'refreshAfterManualEntryWrite(false, "manual_cash_movement")'),
    ]:
        require_contains(body, "if (!valid) {\n        return false;\n    }", context)
        require_contains(body, "const bool writeSucceeded", context)
        require_contains(body, "if (writeSucceeded)", context)
        require_contains(body, refresh_call, context)
        require(body.find("if (!valid)") < body.find("const auto result"), f"{context} validates before write")
        require(body.find("const bool writeSucceeded") > body.find("applyManualEntryResult"), f"{context} evaluates write result after mapping")

    refresh_body = function_body(presenter_cpp, "void ShellAccountingPresenter::refreshAfterManualEntryWrite")
    require_contains(refresh_body, "ShellAccountingReadOnlyController::refreshPositionList", "refresh helper")
    require_contains(refresh_body, "ShellAccountingReadOnlyController::refreshCashSummary", "refresh helper")
    require_contains(refresh_body, "ShellAccountingReadOnlyController::refreshPortfolioPnlSummary", "refresh helper")
    require_contains(refresh_body, "includePositionList", "refresh helper")
    require_contains(refresh_body, "cashMovementSteps", "refresh helper")
    for token in ["DataAccess", "SQLite", "INSERT INTO", "UPDATE ", "DELETE FROM", "REPLACE INTO", "AccountingReplay", "StrategyEngine", "BrokerOrderPort"]:
        require_not_contains(presenter_h, token, "ShellAccountingPresenter.h")
        require_not_contains(presenter_cpp, token, "ShellAccountingPresenter.cpp")
        require_not_contains(controller_h, token, "ShellAccountingReadOnlyController.h")
        require_not_contains(controller_cpp, token, "ShellAccountingReadOnlyController.cpp")

    for token in ["position.list", "cash.summary", "portfolio.pnl.summary"]:
        require_contains(controller_cpp, token, "ShellAccountingReadOnlyController.cpp")
    for token in ["DataAccess", "SQLite", "INSERT INTO", "UPDATE ", "DELETE FROM", "REPLACE INTO", "AccountingReplay", "StrategyEngine", "BrokerOrderPort"]:
        require_not_contains(controller_cpp, token, "ShellAccountingReadOnlyController.cpp")

    changes = changed_paths(root)
    allowed = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md",
        "docs/199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md",
        "docs/200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md",
        "docs/201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md",
        "docs/202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md",
        "docs/203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md",
        "docs/204_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance.md",
        "docs/205_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance_test_plan.md",
        "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml",
        "libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h",
        "libs/ShellServices/src/ShellAccountingPresenter.cpp",
        "libs/ShellServices/include/ShellServices/ShellAccountingReadOnlyController.h",
        "libs/ShellServices/src/ShellAccountingReadOnlyController.cpp",
        "tests/CMakeLists.txt",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/manual_entry_mvp_runtime_e2e_acceptance.cpp",
    }
    unexpected = sorted(path for path in changes if path not in allowed)
    require(not unexpected, "TASK-202 changed unauthorized paths: " + ", ".join(unexpected))
    for forbidden in [
        "libs/DataAccess/",
        "libs/DataServiceApi/",
        "libs/AccountingEngine/",
        "libs/StrategyEngine/",
        "libs/MarketEngine/",
        "migrations/",
    ]:
        require(not any(path.startswith(forbidden) for path in changes), f"TASK-202 must not change {forbidden}")
    for forbidden in [
        "apps/ETFDecisionShell/src/main.cpp",
        "libs/DataServiceApi/src/DataServiceActions.cpp",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
        "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
        "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
    ]:
        require(forbidden not in changes, f"TASK-202 must not change {forbidden}")

    for token in ["persistManualTransaction", "persistManualCashMovement"]:
        require_contains(dataservice_actions, token, "DataServiceActions")
    for token in ["INSERT INTO", "UPDATE ", "DELETE FROM", "REPLACE INTO"]:
        require_not_contains(dataservice_actions, token, "DataServiceActions")

    for token in [
        "docs/200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md",
        "docs/201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md",
        "shell_accounting_manual_entry_post_write_readback_refresh_implementation",
    ]:
        require_contains(devdocs, token, "DevDocs acceptance")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
