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


def joined(root: Path, paths: list[Path]) -> str:
    return "\n".join(read(path) for path in paths if path.exists())


def assert_not_changed(root: Path, relative_path: str) -> None:
    normalized = relative_path.replace("\\", "/")
    require(normalized not in changed_paths(root), f"{relative_path} must not be changed by TASK-194")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    docs184 = root / "docs" / "184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md"
    docs185 = root / "docs" / "185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md"
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc184 = read(docs184)
    doc185 = read(docs185)
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    dataaccess_cmake = read(root / "libs" / "DataAccess" / "CMakeLists.txt")

    require(docs184.exists(), "docs/184 exists")
    require(docs185.exists(), "docs/185 exists")
    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-194", context)
    require_contains(readme, "docs/184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md", "README")
    require_contains(readme, "docs/185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md", "README")
    require_contains(docs_index, "184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md", "docs/README")
    require_contains(docs_index, "185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate", "tests/CMakeLists")

    doc184_tokens = [
        "TASK-194",
        "authorization gate-only",
        "does not implement manual cash movement repository write",
        "does not modify the TASK-192 manual transaction repository",
        "does not modify DataAccess production implementation",
        "does not modify DataServiceActions.cpp / DataServiceActions.h / DataServiceActionRegistrar.cpp",
        "does not modify TASK-178 validation production code",
        "`migrations/001_initial_schema.sql`",
        "`migrations/002_shell_accounting_manual_entry_schema.sql`",
        "does not write SQLite",
        "does not execute runtime SQL",
        "does not write `cash_adjustment`, `trade_log`, `audit_log`",
        "TASK-190 completed",
        "TASK-192 implemented DataAccess-only manual transaction repository write",
        "TASK-193 aligned",
        "Future manual cash movement repository write implementation must be a separate",
        "DataAccess repository boundary",
        "must not scatter SQL in DataService action handlers",
        "DataService action write wiring must be a separate TASK",
        "TASK-190 002 migration schema fields",
        "without modifying TASK-192 repository behavior",
        "no broker",
        "no network",
        "no credentials",
        "no endpoint",
        "no real order placement",
        "no automatic trading",
        "cash_adjustment.request_id",
        "cash_adjustment.idempotency_key",
        "cash_adjustment.occurred_at_utc",
        "cash_adjustment.source_memo_sanitized",
        "cash_adjustment.trade_log_uid",
        "cash_adjustment.amount_cents",
        "cash_adjustment.adjustment_type",
        "trade_log.request_id",
        "trade_log.idempotency_key",
        "trade_log.occurred_at_utc",
        "trade_log.cash_adjustment_uid",
        "trade_log.trade_source",
        "trade_log.manual_entry",
        "Deposit mapping",
        "Withdrawal mapping",
        "fail closed",
        "amount sign policy",
        "net cash impact policy",
        "request_id trace behavior",
        "idempotency_key duplicate behavior",
        "transaction boundary",
        "rollback / failure atomicity",
        "unique index conflict handling",
        "no partial facts on failure",
        "sourceMemo sanitization",
        "raw credential",
        "raw SQL",
        "internal exception stack",
        "Broker sandbox new capability development remains paused",
    ]
    for token in doc184_tokens:
        require_contains(doc184, token, "docs/184")

    doc185_tokens = [
        "TASK-194",
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "docs/184 exists",
        "docs/185 exists",
        "Cash adjustment mapping includes request_id",
        "Trade log cash fact mapping includes request_id",
        "Deposit and Withdrawal policy",
        "Rollack" if False else "Rollback",
        "Duplicate idempotency_key behavior",
        "sourceMemo sanitization",
        "Runtime SQL INSERT / UPDATE / DELETE / REPLACE is not added",
        "Runtime cash_adjustment write is not added",
        "Broker disabled / broker order / real broker gates pass",
    ]
    for token in doc185_tokens:
        require_contains(doc185, token, "docs/185")

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
        "apps/ETFDecisionShell/src/main.cpp",
    ]
    for path in protected_paths:
        assert_not_changed(root, path)

    changes = changed_paths(root)
    allowed_task196_dataaccess = {
        "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
        "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
        "libs/DataAccess/CMakeLists.txt",
    }
    require(not any(path.startswith("migrations/") for path in changes), "TASK-194 must not modify migrations")
    require(not any(path.endswith(".sql") for path in changes), "TASK-194 must not add schema or SQL files")
    require(not any(path.startswith("apps/ETFDecisionShell/qml/") for path in changes), "production QML must not change")
    require(not any("Presenter" in path or "Controller" in path for path in changes), "Presenter / Controller must not change")
    require(not any(path.startswith("libs/AccountingEngine/") for path in changes), "AccountingEngine must not change")
    require(not any(path.startswith("libs/StrategyEngine/") or path.startswith("libs/MarketEngine/") for path in changes),
            "StrategyEngine / MarketEngine must not change")

    require("ShellAccountingManualCashMovementRepository.cpp" in dataaccess_cmake, "TASK-196 DataAccess CMake registers cash movement repository")
    dataaccess_prod_text = joined(
        root,
        files_under(root / "libs" / "DataAccess" / "include", {".h", ".hpp"})
        + files_under(root / "libs" / "DataAccess" / "src", {".cpp", ".cc", ".cxx"}),
    )
    require("ShellAccountingManualCashMovementRepository" in dataaccess_prod_text,
            "TASK-196 manual cash movement repository implementation must exist")
    require("ManualCashMovementWriteRepository" not in dataaccess_prod_text,
            "manual cash movement write repository implementation must not exist")

    production_diff = "\n".join(
        diff_text(root, path)
        for path in changes
        if (path.startswith("libs/") or path.startswith("apps/"))
        and path not in allowed_task196_dataaccess
    )
    dataservice_shell_diff = diff_text(root, "libs/DataServiceApi", "apps", "libs/ShellServices", "libs/ShellCore")
    require(re.search(r"\b(INSERT|UPDATE|DELETE|REPLACE)\b", dataservice_shell_diff, re.IGNORECASE) is None,
            "TASK-194 must not add DataService/Shell runtime DML")
    for token in ["INSERT INTO audit_log", "ledgerId", "auditLogId"]:
        require(token not in production_diff, f"TASK-194 must not add runtime token {token}")
    for token in ["sqlite3_exec", "SQLite::Statement", "prepareStatement", "executeWrite"]:
        require(token not in dataservice_shell_diff, f"TASK-194 must not add SQLite runtime write token {token}")
    require("cash_adjustment" not in dataservice_shell_diff, "TASK-194 must not add DataService/Shell cash_adjustment references")

    added_lines = "\n".join(
        line for line in production_diff.splitlines() if line.startswith("+") and not line.startswith("+++")
    )
    forbidden_added = [
        "TradeDraft",
        "trade suggestion",
        "strategy recommendation",
        "brokerOrder",
        "placeOrder",
        "realBrokerOrderId",
        "automatic trading",
        "auto trading",
        "http://",
        "https://",
        "api_key",
        "password=",
        "secret=",
        "endpoint=",
    ]
    for token in forbidden_added:
        require(token not in added_lines, f"TASK-194 must not add forbidden production token {token}")

    for required_subdir in [
        "ShellAccountingManualTransactionRepositoryWriteImplementation",
        "ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate",
        "ShellAccountingManualEntrySchemaMigrationImplementation",
        "ShellAccountingManualEntryDataServiceActionValidationWiring",
        "ShellAccountingManualTransactionCashMovementValidationScaffold",
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderAuthorizationGate",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
    ]:
        require_contains(tests_cmake, required_subdir, "tests/CMakeLists retained related gates")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
