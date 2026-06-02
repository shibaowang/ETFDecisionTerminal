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


def files_under(root: Path, suffixes: set[str] | None = None) -> list[Path]:
    if not root.exists():
        return []
    files = [path for path in root.rglob("*") if path.is_file()]
    if suffixes is not None:
        files = [path for path in files if path.suffix in suffixes]
    return files


def joined(paths: list[Path]) -> str:
    return "\n".join(read(path) for path in paths if path.exists())


def assert_not_changed(changes: set[str], relative_path: str) -> None:
    normalized = relative_path.replace("\\", "/")
    require(normalized not in changes, f"{relative_path} must not be changed by TASK-197")


def function_body(source: str, function_name: str) -> str:
    start = source.find(function_name + "(")
    require(start >= 0, f"{function_name} must exist")
    brace = source.find("{", start)
    require(brace >= 0, f"{function_name} body must start")
    depth = 0
    for index in range(brace, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[brace : index + 1]
    raise AssertionError(f"{function_name} body must end")


def added_lines(diff: str) -> str:
    return "\n".join(line for line in diff.splitlines() if line.startswith("+") and not line.startswith("+++"))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc188 = read(root / "docs" / "188_shell_accounting_manual_cash_movement_repository_dual_write_implementation.md")
    doc189 = read(root / "docs" / "189_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test_plan.md")
    doc190_path = root / "docs" / "190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md"
    doc191_path = root / "docs" / "191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md"
    doc190 = read(doc190_path)
    doc191 = read(doc191_path)
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    data_service_actions = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp")
    data_service_actions_h = read(root / "libs" / "DataServiceApi" / "include" / "DataServiceApi" / "DataServiceActions.h")
    registrar = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActionRegistrar.cpp")
    validation_wiring_cmake = read(
        root / "tests" / "ShellAccountingManualEntryDataServiceActionValidationWiring" / "CMakeLists.txt"
    )
    task196_cmake = read(root / "tests" / "ShellAccountingManualCashMovementRepositoryDualWriteImplementation" / "CMakeLists.txt")
    task192_cmake = read(root / "tests" / "ShellAccountingManualTransactionRepositoryWriteImplementation" / "CMakeLists.txt")

    require(doc190_path.exists(), "docs/190 exists")
    require(doc191_path.exists(), "docs/191 exists")
    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-197", context)
    require_contains(readme, "docs/190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md", "README")
    require_contains(readme, "docs/191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md", "README")
    require_contains(docs_index, "190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md", "docs/README")
    require_contains(docs_index, "191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate", "tests/CMakeLists")
    require_contains(tests_cmake, "ShellAccountingBrokerAdapterDisabledWiring", "tests/CMakeLists broker disabled retained")
    require_contains(tests_cmake, "ShellAccountingBrokerOrderImplementation", "tests/CMakeLists broker order retained")
    require_contains(tests_cmake, "ShellAccountingRealBrokerOrderAuthorizationGate", "tests/CMakeLists real broker authorization retained")
    require_contains(tests_cmake, "ShellAccountingRealBrokerOrderImplementationGate", "tests/CMakeLists real broker implementation retained")

    doc190_tokens = [
        "TASK-197",
        "DataService write wiring authorization gate-only",
        "does not implement DataService runtime write wiring",
        "does not modify `DataServiceActions.cpp`",
        "does not modify `DataServiceActions.h`",
        "does not modify `DataServiceActionRegistrar.cpp`",
        "does not wire repositories",
        "does not write database rows",
        "does not execute runtime SQL",
        "does not return persistent id",
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "writeImplemented=false",
        "ShellAccountingManualTransactionRepository",
        "ShellAccountingManualCashMovementRepository",
        "Future DataService write wiring must be separately authorized",
        "validation-first",
        "must not scatter SQL in DataService action handlers",
        "SQL location remains DataAccess only",
        "sanitized local write result",
        "raw SQL",
        "raw payloads",
        "credentials",
        "endpoint",
        "tokens",
        "internal stack traces",
        "Future audit write",
        "Future replay",
        "read model",
        "UI integration",
        "No broker semantics",
        "No replay trigger",
        "Repository transaction boundaries remain authoritative",
        "Duplicate idempotency",
        "stable protocol error",
        "Broker sandbox new capability development remains paused",
        "no real broker",
        "no automatic trading",
        "no real order placement",
    ]
    for token in doc190_tokens:
        require_contains(doc190, token, "docs/190")

    doc191_tokens = [
        "TASK-197",
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "docs/190 exists",
        "docs/191 exists",
        "DataServiceActions.cpp is not modified",
        "DataServiceActions.h is not modified",
        "DataServiceActionRegistrar.cpp is not modified",
        "TASK-178 validation production code is not modified",
        "TASK-192 repository header/source are not modified",
        "TASK-196 repository header/source are not modified",
        "TASK-182 validation wiring still returns `writeImplemented=false`",
        "Valid manual transaction payload still does not write database",
        "Valid manual cash movement payload still does not write database",
        "DataService action response still does not return persistent id",
        "DataServiceActions does not call `ShellAccountingManualTransactionRepository`",
        "DataServiceActions does not call `ShellAccountingManualCashMovementRepository`",
        "DataServiceApi does not add runtime SQL",
        "No TradeDraft implementation",
        "No broker SDK",
        "No network or endpoint",
        "No credentials",
        "No real broker order id",
        "No real order placement",
        "No automatic trading",
        "TASK-196 dual-write repository tests pass",
        "TASK-192 manual transaction repository tests pass",
        "TASK-182 validation-only wiring tests pass",
    ]
    for token in doc191_tokens:
        require_contains(doc191, token, "docs/191")

    for token in ["TASK-197", "DataService write wiring authorization gate"]:
        require_contains(doc188, token, "docs/188")
    for token in ["TASK-197", "DataService write wiring authorization gate tests pass"]:
        require_contains(doc189, token, "docs/189")

    changes = changed_paths(root)
    allowed_changes = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/188_shell_accounting_manual_cash_movement_repository_dual_write_implementation.md",
        "docs/189_shell_accounting_manual_cash_movement_repository_dual_write_implementation_test_plan.md",
        "docs/190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md",
        "docs/191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md",
        "tests/CMakeLists.txt",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    }
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    require(not unexpected, "TASK-197 changed unauthorized paths: " + ", ".join(unexpected))

    protected_paths = [
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
        "libs/DataServiceApi/src/DataServiceActions.cpp",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
        "libs/DataServiceApi/include/DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h",
        "libs/DataServiceApi/src/ShellAccountingManualTransactionCashMovementValidation.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
        "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
        "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
        "apps/ETFDecisionShell/src/main.cpp",
    ]
    for path in protected_paths:
        assert_not_changed(changes, path)
    require(not any(path.startswith("migrations/") for path in changes), "TASK-197 must not modify migrations")
    require(not any(path.endswith(".sql") for path in changes), "TASK-197 must not add schema or SQL files")
    require(not any(path.startswith("apps/ETFDecisionShell/qml/") for path in changes), "TASK-197 must not modify production QML")
    require(not any(path.startswith("libs/AccountingEngine/") for path in changes), "TASK-197 must not modify AccountingEngine")
    require(not any(path.startswith("libs/StrategyEngine/") for path in changes), "TASK-197 must not modify StrategyEngine")
    require(not any(path.startswith("libs/MarketEngine/") for path in changes), "TASK-197 must not modify MarketEngine")
    require(not any("Presenter" in path or "Controller" in path for path in changes), "TASK-197 must not modify Presenter/Controller")

    require_contains(data_service_actions, "manualEntryValidationOnlyResponse", "DataServiceActions")
    require_contains(data_service_actions, "validateManualTransactionEntry", "DataServiceActions")
    require_contains(data_service_actions, "validateManualCashMovement", "DataServiceActions")
    require_contains(data_service_actions, '\\"writeImplemented\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"writeEnabled\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"databaseWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"tradeLogWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"cashFactsWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"auditWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, '\\"ledgerWritten\\":false', "DataServiceActions")
    require_contains(data_service_actions, "VALIDATION_ACCEPTED_WRITE_NOT_IMPLEMENTED", "DataServiceActions")
    require_contains(data_service_actions_h, "handleAccountingManualEntryTransactionCreate", "DataServiceActions.h")
    require_contains(data_service_actions_h, "handleAccountingManualEntryCashMovementCreate", "DataServiceActions.h")
    require_contains(registrar, "kActionAccountingManualTransactionCreate", "DataServiceActionRegistrar")
    require_contains(registrar, "kActionAccountingManualCashMovementCreate", "DataServiceActionRegistrar")

    transaction_body = function_body(data_service_actions, "handleAccountingManualEntryTransactionCreate")
    cash_body = function_body(data_service_actions, "handleAccountingManualEntryCashMovementCreate")
    for body, context, validator in [
        (transaction_body, "manual transaction handler", "validateManualTransactionEntry"),
        (cash_body, "manual cash movement handler", "validateManualCashMovement"),
    ]:
        require_contains(body, validator, context)
        require_contains(body, "manualEntryValidationOnlyResponse", context)
        require("Repository" not in body, f"{context} must not call repository")
        require("executeStatement" not in body, f"{context} must not execute SQL")
        require("INSERT" not in body and "UPDATE" not in body and "DELETE" not in body and "REPLACE" not in body,
                f"{context} must not contain DML")

    data_service_diff = diff_text(root, "libs/DataServiceApi")
    require(data_service_diff == "", "TASK-197 must not change DataServiceApi files")
    runtime_diff = added_lines(diff_text(root, "libs", "apps"))
    for token in [
        "ShellAccountingManualTransactionRepository(",
        "ShellAccountingManualCashMovementRepository(",
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
        "tradeLogWritten\":true",
        "cashFactsWritten\":true",
        "auditWritten\":true",
        "ledgerWritten\":true",
        "persistentId",
        "tradeLogId",
        "cashAdjustmentId",
        "TradeDraft",
        "trade suggestion",
        "brokerOrder",
        "placeOrder",
        "real order",
        "automaticTrading",
        "http://",
        "https://",
        "api_key",
        "apiKey",
        "password=",
        "secret=",
        "endpoint=",
    ]:
        require(token not in runtime_diff, f"TASK-197 must not add runtime token {token}")

    require_contains(validation_wiring_cmake, "manual_transaction_success", "TASK-182 CMake")
    require_contains(validation_wiring_cmake, "manual_cash_movement_success", "TASK-182 CMake")
    require_contains(validation_wiring_cmake, "validation_accepted_write_not_implemented", "TASK-182 CMake")
    require_contains(validation_wiring_cmake, "no_database_write", "TASK-182 CMake")
    require_contains(validation_wiring_cmake, "no_persistent_ids", "TASK-182 CMake")
    require_contains(task192_cmake, "shell_accounting_manual_transaction_repository_write_implementation_manual_buy_success", "TASK-192 CMake")
    require_contains(task192_cmake, "shell_accounting_manual_transaction_repository_write_implementation_manual_sell_success", "TASK-192 CMake")
    require_contains(task192_cmake, "no_cash_adjustment_write", "TASK-192 CMake")
    require_contains(task192_cmake, "no_broker", "TASK-192 CMake")
    require_contains(task196_cmake, "deposit_success", "TASK-196 CMake")
    require_contains(task196_cmake, "withdrawal_success", "TASK-196 CMake")
    require_contains(task196_cmake, "duplicate_idempotency_no_extra_trade_log", "TASK-196 CMake")
    require_contains(task196_cmake, "no_audit_log_write", "TASK-196 CMake")
    require_contains(task196_cmake, "no_broker", "TASK-196 CMake")
    require_contains(task196_cmake, "no_network_endpoint", "TASK-196 CMake")
    require_contains(task196_cmake, "no_credentials", "TASK-196 CMake")
    require_contains(task196_cmake, "no_real_order", "TASK-196 CMake")
    require_contains(task196_cmake, "no_automatic_trading", "TASK-196 CMake")

    devdocs = read(root / "tests" / "DevDocs" / "test_readonly_demo_acceptance.py")
    for token in [
        "docs/190_shell_accounting_manual_entry_dataservice_write_wiring_authorization_gate.md",
        "docs/191_shell_accounting_manual_entry_dataservice_write_wiring_authorization_test_plan.md",
        "shell_accounting_manual_entry_dataservice_write_wiring_authorization",
    ]:
        require_contains(devdocs, token, "DevDocs acceptance")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
