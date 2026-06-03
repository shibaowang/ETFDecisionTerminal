#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


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


def added_lines(diff: str) -> str:
    return "\n".join(line[1:] for line in diff.splitlines() if line.startswith("+") and not line.startswith("+++"))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    doc206_path = root / "docs" / "206_shell_accounting_manual_entry_readback_replay_adequacy_review_gate.md"
    doc207_path = root / "docs" / "207_shell_accounting_manual_entry_readback_replay_adequacy_review_test_plan.md"
    require(doc206_path.exists(), "docs/206 exists")
    require(doc207_path.exists(), "docs/207 exists")

    doc206 = read(doc206_path)
    doc207 = read(doc207_path)
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    task204_doc = read(root / "docs" / "204_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance.md")
    task204_plan = read(root / "docs" / "205_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance_test_plan.md")

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-205", context)
        require_contains(text, "206_shell_accounting_manual_entry_readback_replay_adequacy_review_gate.md", context)
        require_contains(text, "207_shell_accounting_manual_entry_readback_replay_adequacy_review_test_plan.md", context)

    require_contains(tests_cmake, "ShellAccountingManualEntryReadbackReplayAdequacyReviewGate", "tests/CMakeLists")
    require_contains(task204_doc, "TASK-205", "docs/204")
    require_contains(task204_plan, "TASK-205", "docs/205")

    for token in [
        "TASK-205",
        "readback / replay adequacy review gate-only",
        "does not implement readback / replay",
        "does not modify production code",
        "does not write any database table",
        "review conclusion and gap matrix",
        "manual entry facts",
        "daily-use accounting pages",
        "TASK-192",
        "TASK-196",
        "TASK-198",
        "TASK-200",
        "TASK-202",
        "TASK-204",
        "manual transaction facts",
        "manual cash movement facts",
        "trade_execution_group",
        "trade_log",
        "cash_adjustment",
        "cash_adjustment.trade_log_id",
        "BUY",
        "SELL",
        "quantity",
        "price",
        "fee",
        "tax",
        "cash impact",
        "instrument mapping",
        "account / portfolio mapping",
        "idempotency / request trace",
        "deposit",
        "withdrawal",
        "amount sign",
        "currency",
        "principal base / cash balance",
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
        "base_position.summary",
        "sniper_pool.summary",
        "AccountingEngine replay",
        "gap matrix",
        "schema gap",
        "repository mapping gap",
        "DataService readback gap",
        "replay gap",
        "UI refresh gap",
        "fixture/test gap",
        "audit/ledger gap",
        "backup/export gap",
        "Formal Review Conclusion",
        "partially ready",
        "not ready",
        "Next Task Recommendation",
        "TASK-206",
        "broker SDK",
        "network",
        "credentials",
        "endpoints",
        "real order placement",
        "automatic trading",
        "Broker sandbox new capability development remains paused",
    ]:
        require_contains(doc206, token, "docs/206")

    for token in [
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "docs/206 exists",
        "docs/207 exists",
        "Review-gate-only scope",
        "Manual transaction fact adequacy",
        "Manual cash movement fact adequacy",
        "Readback / replay gap matrix",
        "Formal conclusion",
        "Production drift",
        "TASK-204",
        "TASK-202",
        "TASK-200",
        "TASK-198",
        "TASK-196",
        "TASK-192",
        "no production code changed",
        "no runtime SQL / SQLite write",
        "no broker, network, credentials, endpoint, real order, or automatic trading",
    ]:
        require_contains(doc207, token, "docs/207")

    allowed_changes = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/204_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance.md",
        "docs/205_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance_test_plan.md",
        "docs/206_shell_accounting_manual_entry_readback_replay_adequacy_review_gate.md",
        "docs/207_shell_accounting_manual_entry_readback_replay_adequacy_review_test_plan.md",
        "docs/208_shell_accounting_manual_entry_readback_mapping_authorization_gate.md",
        "docs/209_shell_accounting_manual_entry_readback_mapping_authorization_test_plan.md",
        "docs/210_shell_accounting_manual_entry_readback_mapping_implementation.md",
        "docs/211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md",
        "docs/212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md",
        "docs/213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md",
        "libs/DataServiceApi/src/DataServiceActions.cpp",
        "tests/CMakeLists.txt",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/manual_entry_readback_mapping_implementation.cpp",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
    }
    changes = changed_paths(root)
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    require(not unexpected, "TASK-205 changed unauthorized paths: " + ", ".join(unexpected))

    forbidden_prefixes = [
        "apps/",
        "libs/ShellServices/",
        "libs/DataAccess/",
        "libs/AccountingEngine/",
        "libs/StrategyEngine/",
        "libs/MarketEngine/",
        "migrations/",
    ]
    for prefix in forbidden_prefixes:
        require(not any(path.startswith(prefix) for path in changes), f"TASK-205 must not change {prefix}")
    for path in changes:
        if path.startswith("libs/DataServiceApi/"):
            require(
                path == "libs/DataServiceApi/src/DataServiceActions.cpp",
                f"TASK-205/TASK-207 only permits DataServiceActions readback mapping, not {path}",
            )

    require(not any(path.endswith(".sql") for path in changes), "TASK-205 must not add migration or schema files")

    production_diff = added_lines(diff_text(root, "apps", "libs", "migrations"))
    for token in [
        "INSERT ",
        "UPDATE ",
        "DELETE ",
        "REPLACE ",
        "audit_log",
        "ledger",
        "Broker",
        "broker",
        "network",
        "realOrder",
        "brokerOrderId",
        "automatic trading",
    ]:
        require(token not in production_diff, f"production diff must not add `{token}`")

    retained_paths = [
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualTransactionRepositoryWriteImplementation/CMakeLists.txt",
        "tests/ShellAccountingBrokerAdapterDisabledWiring",
        "tests/ShellAccountingBrokerOrderImplementation",
        "tests/ShellAccountingRealBrokerOrderAuthorizationGate",
        "tests/ShellAccountingRealBrokerOrderImplementationGate",
    ]
    for path in retained_paths:
        require((root / path).exists(), f"retained path exists: {path}")

    require(sum(1 for _ in range(1)) == 1, "static gate reached end")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
