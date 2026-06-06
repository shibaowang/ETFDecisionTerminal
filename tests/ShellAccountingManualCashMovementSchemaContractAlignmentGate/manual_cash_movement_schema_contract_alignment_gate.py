#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


TASK_258_EXACT_PATHS = {
    "docs/312_shell_accounting_excel_vba_import_readonly_dataservice_preview_action.md",
    "docs/313_shell_accounting_excel_vba_import_readonly_dataservice_preview_action_test_plan.md",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportReadOnlyPreviewAction.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/excel_vba_import_readonly_dataservice_preview_action.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_missing_required_header_preview_payload.json",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_valid_buy_preview_payload.json",
}


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def require_contains(text: str, token: str, context: str) -> None:
    require(token in text, f"{context} missing `{token}`")


def changed_paths(root: Path) -> set[str]:
    completed = subprocess.run(
        ["git", "diff", "--name-only", "main"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def diff_text(root: Path, *paths: str) -> str:
    completed = subprocess.run(
        ["git", "diff", "main", "--", *paths],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return completed.stdout


def diff_text_excluding(root: Path, paths: list[str], excluded_paths: set[str]) -> str:
    completed = subprocess.run(
        ["git", "diff", "main", "--", *paths],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    kept: list[str] = []
    keep_block = True
    for line in completed.stdout.splitlines():
        if line.startswith("diff --git "):
            parts = line.split(" b/", 1)
            changed = parts[1].strip() if len(parts) == 2 else ""
            keep_block = changed not in excluded_paths
        if keep_block:
            kept.append(line)
    return "\n".join(kept)


def files_under(root: Path, suffixes: set[str] | None = None) -> list[Path]:
    if not root.exists():
        return []
    files = [path for path in root.rglob("*") if path.is_file()]
    if suffixes is not None:
        files = [path for path in files if path.suffix in suffixes]
    return files


def joined(files: list[Path]) -> str:
    return "\n".join(read(path) for path in files if path.exists())


def assert_not_changed(changes: set[str], relative_path: str) -> None:
    normalized = relative_path.replace("\\", "/")
    if normalized in TASK_258_EXACT_PATHS:
        return
    require(normalized not in changes, f"{relative_path} must not be changed by TASK-195")


def function_body(source: str, function_name: str) -> str:
    start = source.find(function_name + "(")
    require(start >= 0, f"{function_name} must exist")
    brace = source.find("{", start)
    require(brace >= 0, f"{function_name} body must start")
    depth = 0
    for index in range(brace, len(source)):
        char = source[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return source[brace : index + 1]
    raise AssertionError(f"{function_name} body must end")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc184 = read(root / "docs" / "184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md")
    doc185 = read(root / "docs" / "185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md")
    doc186_path = root / "docs" / "186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md"
    doc187_path = root / "docs" / "187_shell_accounting_manual_cash_movement_schema_contract_alignment_test_plan.md"
    doc186 = read(doc186_path)
    doc187 = read(doc187_path)
    migration001 = read(root / "migrations" / "001_initial_schema.sql")
    migration002 = read(root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    dataaccess_cmake = read(root / "libs" / "DataAccess" / "CMakeLists.txt")

    require(doc186_path.exists(), "docs/186 exists")
    require(doc187_path.exists(), "docs/187 exists")
    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-195", context)
    require_contains(readme, "docs/186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md", "README")
    require_contains(readme, "docs/187_shell_accounting_manual_cash_movement_schema_contract_alignment_test_plan.md", "README")
    require_contains(docs_index, "186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md", "docs/README")
    require_contains(docs_index, "187_shell_accounting_manual_cash_movement_schema_contract_alignment_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualCashMovementSchemaContractAlignmentGate", "tests/CMakeLists")

    require_contains(migration001, "trade_log_id INTEGER NOT NULL", "migrations/001")
    require_contains(migration001, "FOREIGN KEY (trade_log_id) REFERENCES trade_log(id)", "migrations/001")
    require_contains(migration002, "ALTER TABLE cash_adjustment ADD COLUMN trade_log_uid TEXT", "migrations/002")
    require_contains(migration002, "ALTER TABLE trade_log ADD COLUMN cash_adjustment_uid TEXT", "migrations/002")

    for token in [
        "TASK-195",
        "schema-contract alignment gate-only",
        "cash_adjustment.trade_log_id INTEGER NOT NULL",
        "FOREIGN KEY (trade_log_id) REFERENCES trade_log(id)",
        "current schema blocks cash_adjustment-only write",
        "must not use a cash_adjustment-only design",
        "write `trade_log` cash movement fact",
        "write `cash_adjustment`",
        "cash_adjustment.trade_log_id",
        "trade_log.id",
        "trade_log.cash_adjustment_uid",
        "atomic transaction",
        "rollback on any failure",
        "no partial facts",
        "Future DataService action write wiring must be a separate TASK",
        "Future replay, read model, UI integration",
        "does not authorize",
        "repository implementation",
        "runtime SQL / SQLite write",
        "broker SDK, network, credentials, endpoint",
        "automatic trading",
        "Broker sandbox new capability development remains paused",
    ]:
        require_contains(doc186, token, "docs/186")

    for token in [
        "TASK-195",
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "cash_adjustment-only implementation",
        "trade_log_id INTEGER NOT NULL",
        "FOREIGN KEY (trade_log_id) REFERENCES trade_log(id)",
        "current schema blocks cash_adjustment-only write",
        "future implementation must dual-write",
        "no partial facts",
        "No manual cash movement repository implementation is added",
        "Broker disabled / broker order / real broker gates pass",
    ]:
        require_contains(doc187, token, "docs/187")

    for token in [
        "current schema blocks cash_adjustment-only write",
        "must atomically dual-write",
        "cash_adjustment.trade_log_id",
        "trade_log.id",
        "trade_log.cash_adjustment_uid",
    ]:
        require_contains(doc184, token, "docs/184")
    require(
        "cash_adjustment-only implementation is viable" not in doc184,
        "docs/184 must not claim cash_adjustment-only implementation is viable",
    )
    require_contains(doc185, "current schema blocks cash_adjustment-only write", "docs/185")
    require_contains(doc185, "`cash_adjustment` + `trade_log` dual write", "docs/185")

    changes = changed_paths(root)
    for protected in [
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
        "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
    ]:
        assert_not_changed(changes, protected)
    require(not any(path.startswith("migrations/") for path in changes), "TASK-195 must not modify migrations")
    require(not any(path.endswith(".sql") for path in changes), "TASK-195 must not add schema or SQL files")
    allowed_task196_dataaccess = {
        "libs/DataAccess/CMakeLists.txt",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
        "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
    }
    require(
        not any(path.startswith("libs/DataAccess/") and path not in allowed_task196_dataaccess for path in changes),
        "TASK-195/TASK-196 alignment only allows the authorized DataAccess manual cash movement repository",
    )
    allowed_task200_qml = {"apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml"}
    unexpected_qml_changes = [
        path for path in changes
        if path.startswith("apps/ETFDecisionShell/qml/") and path not in allowed_task200_qml
    ]
    require(
        not unexpected_qml_changes,
        "TASK-195/TASK-200 must not modify unauthorized production QML: "
        + ", ".join(unexpected_qml_changes),
    )
    require(not any(path.startswith("libs/AccountingEngine/") for path in changes), "TASK-195 must not modify AccountingEngine")

    dataservice_actions = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp")
    require_contains(
        dataservice_actions,
        "ShellAccountingManualTransactionRepository repository(connection)",
        "TASK-198 DataServiceActions manual transaction wiring",
    )
    require_contains(
        dataservice_actions,
        "ShellAccountingManualCashMovementRepository repository(connection)",
        "TASK-198 DataServiceActions manual cash movement wiring",
    )
    for handler in [
        function_body(dataservice_actions, "handleAccountingManualEntryTransactionCreate"),
        function_body(dataservice_actions, "handleAccountingManualEntryCashMovementCreate"),
    ]:
        require("executeStatement" not in handler, "TASK-198 DataServiceActions handler must not execute SQL directly")
        require(re.search(r"\b(INSERT|UPDATE|DELETE|REPLACE)\b", handler, re.IGNORECASE) is None,
                "TASK-198 DataServiceActions handler must not contain direct DML")

    require("ShellAccountingManualCashMovementRepository.cpp" in dataaccess_cmake, "DataAccess CMake registers TASK-196 cash movement repository")
    dataaccess_text = joined(
        files_under(root / "libs" / "DataAccess" / "include", {".h", ".hpp"})
        + files_under(root / "libs" / "DataAccess" / "src", {".cpp", ".cc", ".cxx"})
    )
    require("ShellAccountingManualCashMovementRepository" in dataaccess_text, "TASK-196 manual cash movement repository implementation must exist")
    require("ManualCashMovementWriteRepository" not in dataaccess_text, "unauthorized manual cash movement write repository name must not exist")

    production_diff = diff_text_excluding(root, ["libs", "apps"], TASK_258_EXACT_PATHS)
    dataservice_shell_diff = diff_text_excluding(
        root,
        ["libs/DataServiceApi", "apps", "libs/ShellServices", "libs/ShellCore"],
        TASK_258_EXACT_PATHS,
    )
    require(re.search(r"\b(INSERT|UPDATE|DELETE|REPLACE)\b", dataservice_shell_diff, re.IGNORECASE) is None,
            "TASK-195 must not add DataService/Shell runtime DML")
    added_production_lines = "\n".join(
        line for line in production_diff.splitlines() if line.startswith("+") and not line.startswith("+++")
    )
    for token in [
        "INSERT INTO cash_adjustment",
        "INSERT INTO trade_log",
        "INSERT INTO audit_log",
        "ledgerWritten = true",
        "brokerOrder",
        "placeOrder",
        "http://",
        "https://",
        "api_key",
        "password=",
        "secret=",
        "endpoint=",
        "automaticTrading\\\":true",
    ]:
        require(token not in added_production_lines, f"TASK-195 must not add forbidden production token {token}")

    for required_subdir in [
        "ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate",
        "ShellAccountingManualTransactionRepositoryWriteImplementation",
        "ShellAccountingManualEntryDataServiceActionValidationWiring",
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
    ]:
        require_contains(tests_cmake, required_subdir, "tests/CMakeLists retained related gates")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
