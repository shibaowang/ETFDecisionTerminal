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

    def contains(self, text: str, token: str, context: str) -> None:
        normalized_text = " ".join(text.split())
        normalized_token = " ".join(token.split())
        self.require(
            token in text or normalized_token in normalized_text,
            f"{context} missing `{token}`",
        )


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
    gate = Gate()

    doc222_path = root / "docs" / "222_shell_accounting_manual_entry_replay_policy_authorization_gate.md"
    doc223_path = root / "docs" / "223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md"
    test_dir = root / "tests" / "ShellAccountingManualEntryReplayPolicyAuthorizationGate"
    test_cmake_path = test_dir / "CMakeLists.txt"
    test_py_path = test_dir / "manual_entry_replay_policy_authorization_gate.py"

    gate.require(doc222_path.exists(), "docs/222 exists")
    gate.require(doc223_path.exists(), "docs/223 exists")
    gate.require(test_cmake_path.exists(), "TASK-213 CMake exists")
    gate.require(test_py_path.exists(), "TASK-213 gate script exists")

    doc222 = read(doc222_path)
    doc223 = read(doc223_path)
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    task212_doc = read(root / "docs" / "220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md")
    task212_plan = read(root / "docs" / "221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md")

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        gate.contains(text, "TASK-213", context)
        gate.contains(text, "222_shell_accounting_manual_entry_replay_policy_authorization_gate.md", context)
        gate.contains(text, "223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md", context)

    gate.contains(tests_cmake, "ShellAccountingManualEntryReplayPolicyAuthorizationGate", "tests/CMakeLists")
    gate.contains(read(test_cmake_path), "shell_accounting_manual_entry_replay_policy_authorization", "TASK-213 CTest")

    for token in [
        "# ShellAccounting Manual Entry Replay Policy Authorization Gate",
        "TASK-213 authorizes replay policy only.",
        "TASK-213 does not authorize replay implementation.",
        "TASK-213 does not authorize audit / ledger writes.",
        "TASK-213 does not authorize backup/export/restore.",
        "TASK-213 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.",
        "policy-authorization-only",
        "docs/tests only",
        "does not modify production code",
        "does not modify production QML",
        "DataServiceActions",
        "DataServiceActionRegistrar",
        "DataAccess",
        "repositories",
        "migrations",
        "AccountingEngine",
        "StrategyEngine",
        "MarketEngine",
        "## Replay Input Policy",
        "authorized, persisted manual entry facts",
        "Manual BUY / SELL facts",
        "Manual Deposit / Withdrawal facts",
        "requestId",
        "idempotencyKey",
        "must not consume QML raw payloads",
        "strategy output",
        "broker payloads",
        "Replay input DTOs",
        "account id",
        "portfolio id",
        "instrument id / symbol",
        "side",
        "quantity",
        "price",
        "fee",
        "tax",
        "currency",
        "movement type",
        "## Replay Ordering Policy",
        "occurred-at time",
        "stable source row id",
        "Duplicate idempotencyKey",
        "same-timestamp",
        "multi-account, multi-portfolio, and multi-instrument",
        "## Position Replay Policy",
        "BUY may increase quantity",
        "SELL may reduce quantity",
        "cost basis",
        "SELL exceeds position",
        "Negative positions are not authorized",
        "Fee and tax treatment",
        "## Cash Replay Policy",
        "Deposit increases cash",
        "Withdrawal decreases cash",
        "BUY cash outflow and SELL cash inflow",
        "Insufficient cash and negative cash",
        "Multi-currency behavior",
        "## PnL Replay Policy",
        "Realized PnL",
        "Unrealized PnL",
        "market price dependency policy",
        "PnL must not be fabricated",
        "## Replay Output Policy",
        "Position outputs",
        "Cash outputs",
        "PnL outputs",
        "safe unavailable markers",
        "sanitized issue codes",
        "must not write audit_log",
        "trade_log",
        "trade_execution_group",
        "cash_adjustment",
        "## Unsupported And Safety Policy",
        "No silent success is allowed",
        "Missing source facts",
        "Duplicate source facts",
        "stale or out-of-order facts",
        "Unsupported states must return explicit sanitized issue codes",
        "## Privacy And Sanitization Policy",
        "No raw SQL",
        "No raw manual entry payload",
        "No full trade_log payload",
        "No full cash_adjustment payload",
        "No credentials",
        "No endpoint values",
        "No broker payloads",
        "No internal exception stacks",
        "No real broker order ids",
        "## Integration Boundary",
        "separately authorized",
        "DataService / AccountingEngine boundary",
        "must not be triggered directly by QML",
        "## Out-of-Scope Boundaries",
        "AccountingEngine replay implementation",
        "Replay fixture matrix implementation",
        "Runtime SQL / SQLite read/write",
        "audit_log writes",
        "Ledger writes",
        "Backup/export/restore",
        "Production QML changes",
        "DataAccess repository changes",
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
        "## Formal Authorization Conclusion And Next Task",
        "Manual entry replay is policy-authorized only",
        "Recommended next task: TASK-214 manual entry replay fixture matrix authorization gate.",
    ]:
        gate.contains(doc222, token, "docs/222")

    for token in [
        "# ShellAccounting Manual Entry Replay Policy Authorization Test Plan",
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
        "docs/222",
        "docs/223",
        "Documentation registration",
        "Gate-only scope",
        "Replay input policy",
        "Replay ordering policy",
        "Position replay policy",
        "Cash replay policy",
        "PnL replay policy",
        "Replay output policy",
        "Unsupported/safety",
        "Privacy/sanitization",
        "Integration boundary",
        "Out-of-scope",
        "Formal conclusion",
        "Production drift",
        "TASK-212 regression",
        "TASK-211 regression",
        "TASK-209 regression",
        "TASK-207 regression",
        "TASK-204 regression",
        "TASK-198 regression",
        "TASK-196 regression",
        "TASK-192 regression",
        "Broker/real broker gates",
        "transport 50-repeat",
        "Static source scan",
        "Changed-path allowlist scan",
        "No production code changed scan",
        "No production QML / startup / Presenter / Controller drift scan",
        "No ShellServices adapter / port drift scan",
        "No DataServiceActions drift scan",
        "No DataServiceActionRegistrar drift scan",
        "No DataAccess repository drift scan",
        "No migrations drift scan",
        "No runtime SQL / SQLite read/write scan",
        "No AccountingEngine replay implementation scan",
        "No replay fixture implementation scan",
        "No audit / ledger write scan",
        "No backup/export/restore implementation scan",
        "No broker / network / credentials / endpoint scan",
        "No real order / automatic trading scan",
        "Retained TASK-212 / TASK-211 / TASK-209 / TASK-207 / TASK-204 / TASK-198 / TASK-196 / TASK-192 regression probes",
        "Retained broker and real broker gate probes",
        "Retained transport 50-repeat regression probe",
    ]:
        gate.contains(doc223, token, "docs/223")

    for token in [
        "Recommended next task: TASK-213 manual entry replay policy authorization gate",
        "Replay is not ready for implementation until replay input/output policy is separately authorized",
    ]:
        gate.contains(task212_doc, token, "docs/220")
    gate.contains(task212_plan, "TASK-213", "docs/221")

    allowed_changes = {
        "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
        "docs/272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md",
        "docs/273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "docs/268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md",
    "docs/269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md",
        "docs/223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md",
        "docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md",
        "docs/225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md",
        "docs/230_shell_accounting_manual_entry_replay_fixture_files_scaffold.md",
        "docs/231_shell_accounting_manual_entry_replay_fixture_files_scaffold_test_plan.md",
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/MRF001_empty_manual_facts.json",
        "tests/fixtures/manual_entry_replay/MRF002_single_buy.json",
        "tests/fixtures/manual_entry_replay/MRF003_buy_deposit_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF004_buy_sell_partial_reduction.json",
        "tests/fixtures/manual_entry_replay/MRF005_deposit_withdrawal_baseline.json",
        "tests/fixtures/manual_entry_replay/MRF006_daily_use_combined_baseline.json",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md",
        "docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md",
        "docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md",
        "docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
        "docs/256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md",
        "docs/257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
    "docs/260_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.md",
    "docs/261_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    }
    changes = changed_paths(root)
    unexpected = sorted(path for path in changes if path not in allowed_changes)
    gate.require(not unexpected, "TASK-213 changed unauthorized paths: " + ", ".join(unexpected))

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
        gate.require(not any(path.startswith(prefix) for path in changes), f"TASK-213 must not change {prefix}")
    gate.require(not any(path.endswith(".sql") for path in changes), "TASK-213 must not add migration or schema files")

    production_diff = added_lines(diff_text(root, "apps", "libs", "migrations"))
    for token in [
        "INSERT ",
        "UPDATE ",
        "DELETE ",
        "REPLACE ",
        "audit_log",
        "ledger",
        "backup",
        "export",
        "restore",
        "AccountingReplay",
        "replay",
        "Broker",
        "broker",
        "network",
        "credentials",
        "endpoint",
        "realOrder",
        "brokerOrderId",
        "automaticTrading",
    ]:
        gate.require(token not in production_diff, f"production diff must not add `{token}`")

    retained_paths = [
        "docs/220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md",
        "docs/221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualTransactionRepositoryWriteImplementation/CMakeLists.txt",
        "tests/ShellAccountingBrokerAdapterDisabledWiring/CMakeLists.txt",
        "tests/ShellAccountingBrokerOrderImplementation/CMakeLists.txt",
        "tests/ShellAccountingRealBrokerOrderAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingRealBrokerOrderImplementationGate/CMakeLists.txt",
        "tests/Transport/CMakeLists.txt",
    ]
    for path in retained_paths:
        gate.require((root / path).exists(), f"retained path exists: {path}")
    gate.contains(read(root / "tests" / "Transport" / "CMakeLists.txt"), "transport_local_socket_echo", "transport CTest")

    gate.require(gate.checks >= 80, "TASK-213 gate must execute at least 80 checks")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
