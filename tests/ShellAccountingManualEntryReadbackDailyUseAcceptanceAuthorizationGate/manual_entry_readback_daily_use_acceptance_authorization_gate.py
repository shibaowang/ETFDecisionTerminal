#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path


class Gate:
    def __init__(self) -> None:
        self.checks = 0

    def require(self, condition: bool, message: str) -> None:
        self.checks += 1
        if not condition:
            raise AssertionError(message)

    def require_contains(self, text: str, token: str, context: str) -> None:
        self.require(token in text, f"{context} missing `{token}`")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def changed_paths(root: Path) -> set[str]:
    completed = subprocess.run(
        ["git", "diff", "--name-only", "main"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    doc212_path = root / "docs" / "212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md"
    doc213_path = root / "docs" / "213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md"
    doc214_path = root / "docs" / "214_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance.md"
    doc215_path = root / "docs" / "215_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance_test_plan.md"
    test_dir = root / "tests" / "ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate"
    test_cmake = test_dir / "CMakeLists.txt"
    test_py = test_dir / "manual_entry_readback_daily_use_acceptance_authorization_gate.py"

    gate.require(doc212_path.exists(), "docs/212 exists")
    gate.require(doc213_path.exists(), "docs/213 exists")
    gate.require(doc214_path.exists(), "docs/214 exists")
    gate.require(doc215_path.exists(), "docs/215 exists")
    gate.require(test_cmake.exists(), "TASK-208 test CMake exists")
    gate.require(test_py.exists(), "TASK-208 test script exists")

    doc212 = read(doc212_path)
    doc213 = read(doc213_path)
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    doc210 = read(root / "docs" / "210_shell_accounting_manual_entry_readback_mapping_implementation.md")
    doc211 = read(root / "docs" / "211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md")

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        gate.require_contains(text, "TASK-208", context)
        gate.require_contains(
            text,
            "212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md",
            context,
        )
        gate.require_contains(
            text,
            "213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md",
            context,
        )

    gate.require_contains(tests_cmake, "ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate", "tests/CMakeLists")
    gate.require_contains(test_cmake.read_text(encoding="utf-8"), "shell_accounting_manual_entry_readback_daily_use_acceptance_authorization", "TASK-208 CTest")

    for token in [
        "TASK-208 is daily-use acceptance authorization gate-only",
        "does not implement new functionality",
        "does not modify production code",
        "does not modify production QML",
        "DataServiceActions",
        "repositories",
        "migrations",
        "runtime SQL / SQLite read/write behavior",
        "No new readback implementation",
        "No replay implementation",
        "TASK-192",
        "TASK-196",
        "TASK-198",
        "TASK-200",
        "TASK-202",
        "TASK-204",
        "TASK-205",
        "TASK-206",
        "TASK-207",
        "position.list",
        "BUY position visible",
        "SELL reduction visible",
        "safe not-ready issue",
        "instrument code visible",
        "Account / portfolio mapping visible",
        "Market value unavailable",
        "Unrealized PnL unavailable",
        "No fabricated market value",
        "No silent success",
        "No QML calculation of positions",
        "cash.summary",
        "Deposit cash inflow visible",
        "Withdrawal cash outflow visible",
        "BUY cash outflow visible",
        "SELL cash inflow visible",
        "Currency visible",
        "Principal base policy visible",
        "No fabricated principal or cash facts",
        "No QML calculation of cash",
        "portfolio.pnl.summary",
        "Cash balance may be visible",
        "Holding cost may be visible",
        "Realized PnL not fabricated",
        "Unrealized PnL not fabricated",
        "Market value not fabricated",
        "safe partial / unavailable issue",
        "No QML calculation of PnL",
        "manual entry write status",
        "readback refresh status",
        "invalid payload status",
        "duplicate idempotency status",
        "raw SQL",
        "raw payload",
        "credentials",
        "endpoint",
        "stack trace",
        "broker payload",
        "Manual entry write + readback is partially daily-use ready",
        "Position and cash readback can be accepted for the BUY + Deposit baseline",
        "SELL, Withdrawal, PnL, market price, and AccountingEngine replay remain separate future work",
        "recommended next task is TASK-209",
        "TASK-209 has now added tests-and-docs-only runtime daily-use acceptance coverage",
        "broker SDK",
        "network",
        "real order placement",
        "automatic trading",
        "Broker sandbox new capability development remains paused",
    ]:
        gate.require_contains(doc212, token, "docs/212")

    for token in [
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "docs/212 exists",
        "docs/213 exists",
        "Authorization-only scope",
        "position.list acceptance",
        "cash.summary acceptance",
        "portfolio.pnl.summary acceptance",
        "UI-visible status",
        "MVP conclusion",
        "Production code unchanged",
        "Existing gates retained",
        "Forbidden drift",
        "No production code scan",
        "No QML / startup / Presenter / Controller scan",
        "No ShellServices adapter / port scan",
        "No DataServiceActions scan",
        "No repository scan",
        "No migration / schema file scan",
        "No runtime SQL / SQLite read/write scan",
        "No readback implementation scan",
        "No AccountingEngine replay scan",
        "No audit / ledger write scan",
        "No broker / network / credentials / endpoint scan",
        "No real order / automatic trading scan",
        "TASK-209 runtime daily-use acceptance implementation",
        "TASK-209 runtime daily-use acceptance tests pass",
    ]:
        gate.require_contains(doc213, token, "docs/213")

    gate.require_contains(doc210, "TASK-208 has now added a daily-use acceptance authorization gate", "docs/210")
    gate.require_contains(doc211, "TASK-208 has now added a daily-use acceptance authorization gate", "docs/211")

    changes = changed_paths(root)
    allowed_changes = {
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/210_shell_accounting_manual_entry_readback_mapping_implementation.md",
        "docs/211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md",
        "docs/212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md",
        "docs/213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md",
        "docs/216_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.md",
        "docs/217_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_test_plan.md",
        "docs/218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md",
        "docs/219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md",
        "tests/CMakeLists.txt",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance/manual_entry_readback_daily_use_runtime_acceptance.cpp",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance/manual_entry_sell_withdrawal_daily_use_runtime_acceptance.cpp",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/manual_entry_readback_mapping_implementation.cpp",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md",
        "docs/223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md",
        "docs/225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md",
        "docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
        "docs/230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md",
        "docs/231_shell_accounting_manual_entry_replay_fixture_files_scaffold_test_plan.md",
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md",
        "docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
    }
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    gate.require(not unexpected, "TASK-208 changed unauthorized paths: " + ", ".join(unexpected))

    forbidden_prefixes = [
        "apps/",
        "libs/ShellServices/",
        "libs/ShellCore/",
        "libs/DataServiceApi/",
        "libs/DataAccess/",
        "libs/AccountingEngine/",
        "libs/StrategyEngine/",
        "libs/MarketEngine/",
        "migrations/",
    ]
    for prefix in forbidden_prefixes:
        gate.require(not any(path.startswith(prefix) for path in changes), f"TASK-208 must not change {prefix}")

    gate.require(not any(path.endswith(".sql") for path in changes), "TASK-208 must not add migration or schema files")

    retained_tokens = [
        "ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance",
        "ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance",
        "ShellAccountingManualEntryReadbackMappingImplementation",
        "ShellAccountingManualEntryReadbackMappingAuthorizationGate",
        "ShellAccountingManualEntryReadbackReplayAdequacyReviewGate",
        "ShellAccountingManualEntryMvpRuntimeE2eAcceptance",
        "ShellAccountingManualEntryPostWriteReadbackRefreshImplementation",
        "ShellAccountingManualEntryQmlPresenterImplementation",
        "ShellAccountingManualEntryDataServiceWriteWiringImplementation",
        "ShellAccountingManualCashMovementRepositoryDualWriteImplementation",
        "ShellAccountingManualTransactionRepositoryWriteImplementation",
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
    ]
    for token in retained_tokens:
        gate.require_contains(tests_cmake, token, "retained regression gate")

    gate.require(gate.checks >= 38, "TASK-208 gate must execute at least 38 checks")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
