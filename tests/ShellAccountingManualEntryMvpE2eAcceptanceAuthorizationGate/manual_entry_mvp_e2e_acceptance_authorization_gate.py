#!/usr/bin/env python3

import argparse
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
    paths: set[str] = set()
    for output in (diff.stdout, untracked.stdout):
        paths.update(line.strip().replace("\\", "/") for line in output.splitlines() if line.strip())
    return paths


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    devdocs = read(root / "tests" / "DevDocs" / "test_readonly_demo_acceptance.py")

    doc200 = read(root / "docs" / "200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md")
    doc201 = read(root / "docs" / "201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md")
    doc202_path = root / "docs" / "202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md"
    doc203_path = root / "docs" / "203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md"
    require(doc202_path.exists(), "docs/202 exists")
    require(doc203_path.exists(), "docs/203 exists")
    doc202 = read(doc202_path)
    doc203 = read(doc203_path)

    qml = read(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml")
    presenter_h = read(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h")
    presenter_cpp = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp")
    controller_cpp = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp")
    dataservice_actions = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp")
    manual_transaction_repo_h = read(root / "libs" / "DataAccess" / "include" / "DataAccess" / "ShellAccountingManualTransactionRepository.h")
    manual_cash_repo_h = read(root / "libs" / "DataAccess" / "include" / "DataAccess" / "ShellAccountingManualCashMovementRepository.h")

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-203", context)
    require_contains(readme, "docs/202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md", "README")
    require_contains(readme, "docs/203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md", "README")
    require_contains(docs_index, "202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md", "docs/README")
    require_contains(docs_index, "203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate", "tests/CMakeLists")
    require_contains(devdocs, "docs/202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md", "DevDocs")
    require_contains(devdocs, "docs/203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md", "DevDocs")
    require_contains(devdocs, "shell_accounting_manual_entry_mvp_e2e_acceptance_authorization", "DevDocs")

    for token in [
        "TASK-203",
        "manual entry MVP E2E acceptance",
        "gate-only",
        "does not implement new functionality",
        "does not modify production code",
        "does not write a database",
        "does not connect replay",
        "does not write audit or ledger rows",
        "does not connect broker, network, credentials, or endpoint",
        "does not place real orders",
        "does not enable automatic trading",
        "QML -> Presenter -> Controller -> ShellServices adapter -> DataService action -> DataAccess repository",
        "Acceptance must prove no layer bypasses its boundary",
        "manual transaction",
        "BUY / SELL",
        "manual cash movement",
        "Deposit / Withdrawal",
        "Valid BUY / SELL payload can write `trade_execution_group` and `trade_log`",
        "Valid Deposit / Withdrawal payload can dual-write `trade_log + cash_adjustment`",
        "`cash_adjustment.trade_log_id` points to `trade_log.id`",
        "Invalid payload does not write database rows",
        "Duplicate idempotencyKey does not duplicate writes",
        "Sensitive memo handling is safe and sanitized",
        "Post-write readback refresh state is visible",
        "Success and failure status are visible",
        "Validation issue messages are visible and safe",
        "Duplicate and idempotent status are visible",
        "No silent success is allowed",
        "Raw SQL, raw payload, credentials, endpoint values, and internal stack trace",
        "Broker, order placement, and automatic trading side effects are forbidden",
        "QML must not call DataServiceClient directly",
        "QML must not read or write SQLite",
        "QML must not import or call DataAccess",
        "Presenter / Controller must not call DataAccess repositories",
        "Presenter / Controller must not execute SQL",
        "Presenter / Controller must not trigger AccountingEngine replay",
        "new runtime E2E implementation",
        "new runtime SQL / SQLite write paths",
        "broker SDK, network, credentials, or endpoint integration",
        "real order placement or storage of real broker order ids",
        "Future full E2E implementation tests",
        "Broker sandbox new capability development remains paused",
    ]:
        require_contains(doc202, token, "docs/202")

    for token in [
        "Test Matrix",
        "Documentation And Registration",
        "Authorization-Only Boundary",
        "Manual Transaction E2E Acceptance",
        "Manual Cash Movement E2E Acceptance",
        "Daily-Use MVP Acceptance",
        "No Production Drift",
        "Required Probes",
        "Go / No-Go Checklist",
        "docs/202 exists",
        "docs/203 exists",
        "TASK-203 is authorization-only / gate-only",
        "does not implement new functionality",
        "QML -> Presenter -> Controller -> ShellServices adapter -> DataService action -> DataAccess repository",
        "manual transaction BUY / SELL acceptance",
        "manual cash movement Deposit / Withdrawal acceptance",
        "invalid payload does not write database rows",
        "duplicate idempotencyKey does not duplicate writes",
        "sensitive memo safe handling",
        "post-write readback refresh state",
        "safe visible status and issue messages",
        "forbids silent success",
        "forbids raw SQL, raw payload, credentials, endpoint values, and internal stack trace exposure",
        "forbids broker, real order, and automatic trading side effects",
        "production QML is not modified by TASK-203",
        "Presenter / Controller are not modified by TASK-203",
        "DataServiceActions and DataServiceActionRegistrar are not modified by",
        "DataAccess repositories are not modified by TASK-203",
        "migrations are not modified by TASK-203",
        "no new migration or schema file is added",
        "no new runtime SQL / SQLite write path is added",
        "no new AccountingEngine replay integration is added",
        "no new audit or ledger write is added",
        "no new broker SDK, network, credentials, endpoint, real order, or automatic",
        "TASK-202 post-write refresh implementation tests pass",
        "TASK-200 QML Presenter implementation tests pass",
        "TASK-198 DataService write wiring implementation tests pass",
        "TASK-196 manual cash movement repository tests pass",
        "TASK-192 manual transaction repository tests pass",
        "Broker disabled, broker order, real broker authorization, and real broker",
    ]:
        require_contains(doc203, token, "docs/203")

    require_contains(doc200, "TASK-203 Acceptance Gate Update", "docs/200")
    require_contains(doc200, "does not add new TASK-202 behavior", "docs/200")
    require_contains(doc201, "TASK-203 manual entry MVP E2E acceptance authorization gate passes", "docs/201")

    for token in [
        "submitManualTransaction",
        "submitManualCashMovement",
        "shellAccountingSubmitManualTransactionButton",
        "shellAccountingSubmitManualCashMovementButton",
        "shellAccountingPostWriteRefreshStatus",
        "shellAccountingPostWriteRefreshIssue",
        "shellAccountingPostWriteRefreshSummary",
    ]:
        require_contains(qml + presenter_h + presenter_cpp, token, "TASK-200/TASK-202 chain")
    for token in [
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "position.list",
        "cash.summary",
        "portfolio.pnl.summary",
    ]:
        require_contains(controller_cpp + dataservice_actions, token, "DataService action chain")
    for token in ["ShellAccountingManualTransactionRepository", "ShellAccountingManualCashMovementRepository"]:
        require_contains(dataservice_actions + manual_transaction_repo_h + manual_cash_repo_h, token, "DataAccess repository chain")

    changes = changed_paths(root)
    allowed = {
        "docs/258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md",
        "docs/259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
        "README.md",
        "docs/README.md",
        "docs/12_codex_prompt_template.md",
        "docs/200_shell_accounting_manual_entry_post_write_readback_refresh_implementation.md",
        "docs/201_shell_accounting_manual_entry_post_write_readback_refresh_implementation_test_plan.md",
        "docs/202_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_gate.md",
        "docs/203_shell_accounting_manual_entry_mvp_e2e_acceptance_authorization_test_plan.md",
        "docs/204_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance.md",
        "docs/205_shell_accounting_manual_entry_mvp_runtime_e2e_acceptance_test_plan.md",
        "docs/206_shell_accounting_manual_entry_readback_replay_adequacy_review_gate.md",
        "docs/207_shell_accounting_manual_entry_readback_replay_adequacy_review_test_plan.md",
        "docs/208_shell_accounting_manual_entry_readback_mapping_authorization_gate.md",
        "docs/209_shell_accounting_manual_entry_readback_mapping_authorization_test_plan.md",
        "docs/210_shell_accounting_manual_entry_readback_mapping_implementation.md",
        "docs/211_shell_accounting_manual_entry_readback_mapping_implementation_test_plan.md",
        "docs/214_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance.md",
        "docs/215_shell_accounting_manual_entry_readback_daily_use_runtime_acceptance_test_plan.md",
        "docs/216_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.md",
        "docs/217_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_test_plan.md",
        "docs/218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md",
        "docs/219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md",
        "docs/220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md",
        "docs/221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md",
        "libs/DataServiceApi/src/DataServiceActions.cpp",
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "docs/254_shell_accounting_manual_entry_replay_next_phase_authorization_planning_gate.md",
        "docs/255_shell_accounting_manual_entry_replay_next_phase_authorization_planning_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
        "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
        "tests/CMakeLists.txt",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/manual_entry_mvp_runtime_e2e_acceptance.cpp",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/manual_entry_readback_mapping_implementation.cpp",
        "docs/212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md",
        "docs/213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseRuntimeAcceptance/manual_entry_readback_daily_use_runtime_acceptance.cpp",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseRuntimeAcceptance/manual_entry_sell_withdrawal_daily_use_runtime_acceptance.cpp",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
        "docs/222_shell_accounting_manual_entry_replay_policy_authorization_gate.md",
        "docs/223_shell_accounting_manual_entry_replay_policy_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
        "docs/224_shell_accounting_manual_entry_replay_fixture_matrix_authorization_gate.md",
        "docs/225_shell_accounting_manual_entry_replay_fixture_matrix_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
        "docs/226_shell_accounting_manual_entry_replay_fixture_files_authorization_gate.md",
        "docs/227_shell_accounting_manual_entry_replay_fixture_files_authorization_test_plan.md",
        "docs/228_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_gate.md",
        "docs/229_shell_accounting_manual_entry_replay_fixture_files_scaffold_authorization_test_plan.md",
        "docs/232_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_gate.md",
        "docs/233_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_authorization_test_plan.md",
        "docs/236_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_gate.md",
        "docs/237_shell_accounting_manual_entry_replay_fixture_static_validator_negative_fixtures_authorization_test_plan.md",
        "docs/238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md",
        "docs/239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
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
        "docs/234_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator.md",
        "docs/235_shell_accounting_manual_entry_replay_fixture_scaffold_static_validator_test_plan.md",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/manual_entry_replay_fixture_static_validator.py",
        "docs/240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md",
        "docs/241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md",
        "docs/242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md",
        "docs/243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md",
        "docs/244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md",
        "docs/245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md",
        "docs/246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md",
        "docs/247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
        "docs/248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md",
        "docs/249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md",
        "docs/250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md",
        "docs/251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",
        "tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF001_missing_required_field.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF002_wrong_schema_version.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF003_runtime_use_true.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF004_production_use_true.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF005_replay_executed_true.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF006_non_synthetic_privacy.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF007_extra_json_file.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF008_forbidden_token.sql.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF009_broker_payload_token.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF010_real_order_id_token.json",
        "docs/256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md",
        "docs/257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    }
    unexpected = sorted(path for path in changes if path not in allowed)
    require(not unexpected, "TASK-203 changed unauthorized paths: " + ", ".join(unexpected))

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
        require(not any(path.startswith(prefix) for path in changes), f"TASK-203 must not change {prefix}")
    for path in changes:
        if path.startswith("libs/DataServiceApi/"):
            require(
                path == "libs/DataServiceApi/src/DataServiceActions.cpp",
                f"TASK-203/TASK-207 only permits DataServiceActions readback mapping, not {path}",
            )
    require(not any(path.endswith(".sql") for path in changes), "TASK-203 must not add SQL or schema files")

    for token in [
        "DataServiceClient",
        "SQLite",
        "DataAccess",
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
        "brokerOrder",
        "placeOrder",
        "realOrderId",
        "automaticTrading",
    ]:
        require_not_contains(qml, token, "production QML")
    for token in [
        "DataAccess",
        "SQLite",
        "INSERT INTO",
        "UPDATE ",
        "DELETE FROM",
        "REPLACE INTO",
        "AccountingReplay",
        "StrategyEngine",
        "BrokerOrderPort",
    ]:
        require_not_contains(presenter_h + presenter_cpp, token, "Presenter")

    for token in ["INSERT INTO", "UPDATE ", "DELETE FROM", "REPLACE INTO"]:
        require_not_contains(dataservice_actions, token, "DataServiceActions")
    for token in ["brokerOrder", "placeOrder", "realOrderId", "automaticTrading"]:
        require_not_contains(readme + doc202 + doc203, token, "TASK-203 docs")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
