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

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc194 = read(root / "docs" / "194_shell_accounting_manual_entry_qml_presenter_authorization_gate.md")
    doc195 = read(root / "docs" / "195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md")
    doc196_path = root / "docs" / "196_shell_accounting_manual_entry_qml_presenter_implementation.md"
    doc197_path = root / "docs" / "197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md"
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    devdocs = read(root / "tests" / "DevDocs" / "test_readonly_demo_acceptance.py")
    qml = read(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml")
    presenter_h = read(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingPresenter.h")
    presenter_cpp = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingPresenter.cpp")
    controller_h = read(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingReadOnlyController.h")
    controller_cpp = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingReadOnlyController.cpp")
    adapter_h = read(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingServiceAdapter.h")
    adapter_cpp = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceAdapter.cpp")
    port_h = read(root / "libs" / "ShellServices" / "include" / "ShellServices" / "ShellAccountingDataServiceClientPort.h")
    port_adapter = read(root / "libs" / "ShellServices" / "src" / "ShellAccountingDataServiceClientPortAdapter.cpp")

    require(doc196_path.exists(), "docs/196 exists")
    require(doc197_path.exists(), "docs/197 exists")
    doc196 = read(doc196_path)
    doc197 = read(doc197_path)

    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-200", context)
    require_contains(readme, "docs/196_shell_accounting_manual_entry_qml_presenter_implementation.md", "README")
    require_contains(readme, "docs/197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md", "README")
    require_contains(docs_index, "196_shell_accounting_manual_entry_qml_presenter_implementation.md", "docs/README")
    require_contains(docs_index, "197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualEntryQmlPresenterImplementation", "tests/CMakeLists")

    for token in [
        "TASK-200",
        "submitManualTransaction",
        "submitManualCashMovement",
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "does not modify migrations",
        "does not modify DataServiceActions",
        "does not modify DataAccess repositories",
        "does not trigger AccountingEngine replay",
        "does not add broker SDKs",
        "real order placement",
        "automatic trading",
    ]:
        require_contains(doc196, token, "docs/196")
    for token in ["Test Matrix", "Required Probes", "Go / No-Go Checklist", "TASK-199 authorization gate passes"]:
        require_contains(doc197, token, "docs/197")
    require_contains(doc194, "TASK-200 Implementation Update", "docs/194")
    require_contains(doc195, "TASK-200 Implementation Update", "docs/195")

    for token in [
        "shellAccountingManualEntrySection",
        "shellAccountingSubmitManualTransactionButton",
        "shellAccountingSubmitManualCashMovementButton",
        "shellAccountingManualEntryStatusText",
        "shellAccountingManualEntryIssueText",
        "submitManualTransaction(",
        "submitManualCashMovement(",
    ]:
        require_contains(qml, token, "ShellAccountingReadOnlyPage.qml")
    for token in ["DataServiceClient", "SQLite", "DataAccess", "INSERT INTO", "UPDATE ", "DELETE FROM", "REPLACE INTO"]:
        require_not_contains(qml, token, "production QML")
    for token in ["brokerOrder", "placeOrder", "StrategyEngine", "automaticTrading", "realOrderId"]:
        require_not_contains(qml, token, "production QML")

    for token in [
        "Q_PROPERTY(bool manualEntryBusy",
        "Q_PROPERTY(QString lastManualEntryStatus",
        "Q_PROPERTY(QString lastManualEntryIssue",
        "Q_PROPERTY(QString lastManualEntryResult",
        "Q_INVOKABLE bool submitManualTransaction",
        "Q_INVOKABLE bool submitManualCashMovement",
        "manualEntryStateChanged",
    ]:
        require_contains(presenter_h, token, "ShellAccountingPresenter.h")
    for token in [
        "makeManualTransactionRequest",
        "makeManualCashMovementRequest",
        "controller_->submitManualTransaction",
        "controller_->submitManualCashMovement",
        "accounting.manual_transaction.create",
        "accounting.manual_cash_movement.create",
        "databaseWritten",
        "tradeLogWritten",
        "cashAdjustmentWritten",
    ]:
        require_contains(presenter_cpp, token, "ShellAccountingPresenter.cpp")
    for token in ["submitManualTransaction", "submitManualCashMovement"]:
        require_contains(controller_h, token, "ShellAccountingReadOnlyController.h")
        require_contains(controller_cpp, token, "ShellAccountingReadOnlyController.cpp")
        require_contains(adapter_h, token, "ShellAccountingServiceAdapter.h")
    for token in ["callManualTransactionCreate", "callManualCashMovementCreate"]:
        require_contains(adapter_cpp, token, "ShellAccountingDataServiceAdapter.cpp")
        require_contains(port_h, token, "ShellAccountingDataServiceClientPort.h")
        require_contains(port_adapter, token, "ShellAccountingDataServiceClientPortAdapter.cpp")

    changes = changed_paths(root)
    allowed = {
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
        "docs/194_shell_accounting_manual_entry_qml_presenter_authorization_gate.md",
        "docs/195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md",
        "docs/196_shell_accounting_manual_entry_qml_presenter_implementation.md",
        "docs/197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md",
        "docs/198_shell_accounting_manual_entry_post_write_readback_refresh_authorization_gate.md",
        "docs/199_shell_accounting_manual_entry_post_write_readback_refresh_authorization_test_plan.md",
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
        "docs/216_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.md",
        "docs/217_shell_accounting_manual_entry_sell_withdrawal_daily_use_acceptance_authorization_test_plan.md",
        "docs/218_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance.md",
        "docs/219_shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance_test_plan.md",
        "docs/220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md",
        "docs/221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md",
        "libs/DataServiceApi/src/DataServiceActions.cpp",
        "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml",
        "libs/ShellServices/include/ShellServices/ShellAccountingDataServiceAdapter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingDataServiceClientPort.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingDataServiceClientPortAdapter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingReadOnlyController.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingServiceAdapter.h",
        "libs/ShellServices/include/ShellServices/ShellAccountingServiceTypes.h",
        "libs/ShellServices/src/ShellAccountingDataServiceAdapter.cpp",
        "libs/ShellServices/src/ShellAccountingDataServiceClientPort.cpp",
        "libs/ShellServices/src/ShellAccountingDataServiceClientPortAdapter.cpp",
        "libs/ShellServices/src/ShellAccountingPresenter.cpp",
        "libs/ShellServices/src/ShellAccountingReadOnlyController.cpp",
        "libs/ShellServices/src/ShellAccountingServiceAdapter.cpp",
        "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
        "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",
        "tests/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
        "tests/DevDocs/test_readonly_demo_acceptance.py",
        "tests/ShellAccountingProductionQmlBindingGate/ShellAccountingProductionQmlBindingGate.cpp",
        "tests/ShellAccountingProductionQmlBindingImplementation/ShellAccountingProductionQmlBindingImplementation.cpp",
        "tests/ShellAccountingPresenterLifecycleGate/ShellAccountingPresenterLifecycleGate.cpp",
        "tests/ShellAccountingPresenterLifecycleImplementation/ShellAccountingPresenterLifecycleImplementation.cpp",
        "tests/ShellAccountingRealDataAdapterGate/ShellAccountingRealDataAdapterGate.cpp",
        "tests/ShellAccountingRealDataAdapterImplementation/ShellAccountingRealDataAdapterImplementation.cpp",
        "tests/ShellAccountingManualTransactionCashMovementMvpAuthorizationGate/manual_transaction_cash_movement_mvp_authorization_gate.cpp",
        "tests/ShellAccountingManualTransactionCashMovementValidationScaffold/manual_transaction_cash_movement_validation_scaffold.cpp",
        "tests/ShellAccountingManualEntryDataServiceActionAuthorizationGate/manual_entry_dataservice_action_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryDataServiceActionScaffold/manual_entry_dataservice_action_scaffold.cpp",
        "tests/ShellAccountingManualEntryDataServiceActionImplementationAuthorizationGate/manual_entry_dataservice_action_implementation_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryPersistenceAuthorizationGate/manual_entry_persistence_authorization_gate.cpp",
        "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
        "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
        "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
        "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/CMakeLists.txt",
        "tests/ShellAccountingManualEntryMvpRuntimeE2eAcceptance/manual_entry_mvp_runtime_e2e_acceptance.cpp",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
        "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackMappingImplementation/manual_entry_readback_mapping_implementation.cpp",
        "docs/212_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_gate.md",
        "docs/213_shell_accounting_manual_entry_readback_daily_use_acceptance_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
        "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
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
    unexpected = sorted(path for path in changes if path not in allowed)
    require(not unexpected, "TASK-200 changed unauthorized paths: " + ", ".join(unexpected))
    for forbidden in [
        "migrations/",
        "libs/DataAccess/",
        "libs/AccountingEngine/",
        "libs/StrategyEngine/",
        "libs/MarketEngine/",
    ]:
        require(not any(path.startswith(forbidden) for path in changes), f"TASK-200 must not change {forbidden}")
    for forbidden in [
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
        "apps/ETFDecisionShell/src/main.cpp",
    ]:
        require(forbidden not in changes, f"TASK-200 must not change {forbidden}")

    for token in [
        "docs/196_shell_accounting_manual_entry_qml_presenter_implementation.md",
        "docs/197_shell_accounting_manual_entry_qml_presenter_implementation_test_plan.md",
        "shell_accounting_manual_entry_qml_presenter_implementation",
        "submitManualTransaction",
        "submitManualCashMovement",
    ]:
        require_contains(devdocs, token, "DevDocs acceptance")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
