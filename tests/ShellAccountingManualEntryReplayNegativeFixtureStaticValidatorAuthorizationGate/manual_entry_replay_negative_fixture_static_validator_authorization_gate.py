#!/usr/bin/env python3

import argparse
import subprocess
from pathlib import Path


NEGATIVE_FILES = {
    "negative_fixtures_index.json",
    "NEG_MRF001_missing_required_field.json",
    "NEG_MRF002_wrong_schema_version.json",
    "NEG_MRF003_runtime_use_true.json",
    "NEG_MRF004_production_use_true.json",
    "NEG_MRF005_replay_executed_true.json",
    "NEG_MRF006_non_synthetic_privacy.json",
    "NEG_MRF007_extra_json_file.json",
    "NEG_MRF008_forbidden_token.sql.json",
    "NEG_MRF009_broker_payload_token.json",
    "NEG_MRF010_real_order_id_token.json",
}

ISSUE_CODES = {
    "NEG_FIXTURE_MISSING_REQUIRED_FIELD",
    "NEG_FIXTURE_SCHEMA_VERSION_INVALID",
    "NEG_FIXTURE_ID_MISMATCH",
    "NEG_FIXTURE_DUPLICATE_ID",
    "NEG_FIXTURE_UNAUTHORIZED_FILE",
    "NEG_FIXTURE_UNAUTHORIZED_PATH",
    "NEG_FIXTURE_RUNTIME_USE_TRUE",
    "NEG_FIXTURE_PRODUCTION_USE_TRUE",
    "NEG_FIXTURE_REPLAY_EXECUTED_TRUE",
    "NEG_FIXTURE_SYNTHETIC_FLAG_FALSE",
    "NEG_FIXTURE_FORBIDDEN_TOKEN",
    "NEG_FIXTURE_RAW_SQL_TOKEN",
    "NEG_FIXTURE_CREDENTIAL_TOKEN",
    "NEG_FIXTURE_ENDPOINT_TOKEN",
    "NEG_FIXTURE_BROKER_PAYLOAD_TOKEN",
    "NEG_FIXTURE_REAL_ORDER_TOKEN",
    "NEG_FIXTURE_DB_ARTIFACT_PRESENT",
    "NEG_FIXTURE_PRODUCTION_PATH_REFERENCE",
}

ALLOWED_CHANGED_PATHS = {


    "docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md",

    "docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/CMakeLists.txt",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md",
    "docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",

    "docs/272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md",
    "docs/273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md",
    "docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md",
    "docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "docs/268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md",
    "docs/269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md",
    "docs/270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md",
    "docs/271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "docs/266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md",
    "docs/267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md",
    "docs/243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md",
    "docs/244_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_gate.md",
    "docs/245_shell_accounting_manual_entry_replay_negative_fixture_static_validator_implementation_test_plan.md",
    "docs/246_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.md",
    "docs/247_shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix_test_plan.md",
    "docs/248_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.md",
    "docs/249_shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_test_plan.md",
    "docs/250_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_gate.md",
    "docs/251_shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout_test_plan.md",
    "docs/252_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_gate.md",
    "docs/253_shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout_handoff_test_plan.md",
    "docs/254_shell_accounting_manual_entry_replay_next_phase_authorization_planning_gate.md",
    "docs/255_shell_accounting_manual_entry_replay_next_phase_authorization_planning_test_plan.md",
    "tests/CMakeLists.txt",
    "docs/258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md",
    "docs/259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
    "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
    "docs/256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md",
    "docs/257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/CMakeLists.txt",
    "docs/260_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.md",
    "docs/261_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/CMakeLists.txt",
    "docs/262_shell_accounting_manual_entry_replay_implementation_authorization_gate.md",
    "docs/263_shell_accounting_manual_entry_replay_implementation_authorization_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
    "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",

    "docs/280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md",

    "docs/281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/CMakeLists.txt",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "docs/282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md",
    "docs/283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md",
    "docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
}

TASK_246_SELF_CONSISTENCY_PATHS = {
    "README.md",
    "docs/12_codex_prompt_template.md",
    "docs/288_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_gate.md",
    "docs/289_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_test_plan.md",
    "docs/README.md",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
    "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
    "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridge/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridge/manual_entry_replay_accountingengine_bridge.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureNegativeFixturesScaffoldAuthorizationGate/manual_entry_replay_fixture_negative_fixtures_scaffold_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureStaticValidatorAuthorizationGate/manual_entry_replay_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureScaffoldFilesGate/manual_entry_replay_negative_fixture_scaffold_files_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/manual_entry_replay_negative_fixture_static_validator_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/manual_entry_replay_negative_fixture_validator_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
}

ALLOWED_CHANGED_PATHS.update(TASK_246_SELF_CONSISTENCY_PATHS)

FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    "tests/fixtures/manual_entry_replay/",
    "tests/fixtures/manual_entry_replay_negative/",
)

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

    def not_contains(self, text: str, token: str, context: str) -> None:
        self.require(token not in text, f"{context} must not contain `{token}`")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def git_lines(root: Path, *args: str) -> set[str]:
    completed = subprocess.run(
        ["git", *args],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def changed_paths(root: Path) -> set[str]:
    changed = git_lines(root, "diff", "--name-only", "main")
    staged = git_lines(root, "diff", "--cached", "--name-only")
    untracked = git_lines(root, "ls-files", "--others", "--exclude-standard")
    return changed | staged | untracked


def assert_indexed(gate: Gate, root: Path, doc_name: str, task: str) -> None:
    for relative in ["README.md", "docs/README.md", "docs/12_codex_prompt_template.md"]:
        text = read(root / relative)
        gate.contains(text, task, relative)
        gate.contains(text, doc_name, relative)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    doc242_path = root / "docs" / "242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md"
    doc243_path = root / "docs" / "243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md"
    tests_cmake_path = root / "tests" / "CMakeLists.txt"
    test_dir = root / "tests" / "ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate"
    test_cmake_path = test_dir / "CMakeLists.txt"
    test_py_path = test_dir / "manual_entry_replay_negative_fixture_static_validator_authorization_gate.py"
    negative_dir = root / "tests" / "fixtures" / "manual_entry_replay_negative"
    positive_dir = root / "tests" / "fixtures" / "manual_entry_replay"
    task219_validator = (
        root
        / "tests"
        / "ShellAccountingManualEntryReplayFixtureStaticValidator"
        / "manual_entry_replay_fixture_static_validator.py"
    )
    future_validator_dir = root / "tests" / "ShellAccountingManualEntryReplayNegativeFixtureStaticValidator"
    future_validator_py = future_validator_dir / "manual_entry_replay_negative_fixture_static_validator.py"

    gate.require(
        test_dir.relative_to(root).as_posix()
        == "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate",
        "TASK-223 authorization gate directory stays exact",
    )
    gate.require(
        test_cmake_path.relative_to(root).as_posix()
        == "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/CMakeLists.txt",
        "TASK-223 authorization gate CMakeLists path stays exact",
    )
    gate.require(
        test_py_path.relative_to(root).as_posix()
        == "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py",
        "TASK-223 authorization gate script path stays exact",
    )
    gate.require(
        future_validator_dir.relative_to(root).as_posix()
        == "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator",
        "future negative validator implementation directory is explicitly separate",
    )
    gate.require(
        future_validator_py.relative_to(root).as_posix()
        == "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
        "future negative validator implementation script path is explicit",
    )
    gate.require(
        all(not path.endswith("/") for path in ALLOWED_CHANGED_PATHS),
        "TASK-223 changed-path allowlist uses exact files only",
    )
    gate.require(
        all("*" not in path for path in ALLOWED_CHANGED_PATHS),
        "TASK-223 changed-path allowlist uses no wildcards",
    )
    gate.require(
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate/manual_entry_replay_negative_fixture_static_validator_authorization_gate.py"
        in ALLOWED_CHANGED_PATHS,
        "TASK-223 allowlist includes its own authorization gate script",
    )
    gate.require(
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py"
        in ALLOWED_CHANGED_PATHS,
        "TASK-223 allowlist permits only the TASK-224 test-only validator implementation script",
    )
    gate.require(
        set(FORBIDDEN_CHANGED_PREFIXES)
        == {
            "apps/",
            "libs/",
            "migrations/",
            "tests/fixtures/manual_entry_replay/",
            "tests/fixtures/manual_entry_replay_negative/",
        },
        "TASK-223 forbidden changed prefixes remain complete",
    )

    for path in [
        doc242_path,
        doc243_path,
        tests_cmake_path,
        test_cmake_path,
        test_py_path,
        negative_dir,
        positive_dir,
        task219_validator,
        root / "docs" / "238_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_gate.md",
        root / "docs" / "239_shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization_test_plan.md",
        root / "docs" / "240_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_gate.md",
        root / "docs" / "241_shell_accounting_manual_entry_replay_negative_fixture_scaffold_files_test_plan.md",
    ]:
        gate.require(path.exists(), f"required path exists: {path.relative_to(root).as_posix()}")

    assert_indexed(gate, root, "242_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_gate.md", "TASK-223")
    assert_indexed(gate, root, "243_shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization_test_plan.md", "TASK-223")

    tests_cmake = read(tests_cmake_path)
    test_cmake = read(test_cmake_path)
    test_py = read(test_py_path)
    doc242 = read(doc242_path)
    doc243 = read(doc243_path)

    gate.contains(tests_cmake, "ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorAuthorizationGate", "tests/CMakeLists")
    gate.contains(test_cmake, "shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization", "TASK-223 CTest")
    gate.contains(test_py, 'git_lines(root, "ls-files", "--others", "--exclude-standard")', "untracked detection")

    for section in [
        "# ShellAccounting Manual Entry Replay Negative Fixture Static Validator Authorization Gate",
        "## Purpose",
        "## Relationship To TASK-220 Through TASK-222",
        "## Future Static Validator Scope",
        "## Future Input Boundary",
        "## Future Output Contract",
        "## Future Expected Issue Code Mapping",
        "## Future Failure Semantics",
        "## Privacy And Sanitization Boundary",
        "## No-Replay Boundary",
        "## No-Runtime-Dependency Boundary",
        "## Out-of-Scope Boundaries",
        "## Formal Conclusion And Next Task",
    ]:
        gate.contains(doc242, section, "docs/242")

    for token in [
        "TASK-223 authorizes future negative fixture static validator policy only.",
        "TASK-223 does not implement negative validator.",
        "TASK-223 does not implement parser, loader, reader.",
        "TASK-223 does not implement replay.",
        "TASK-223 does not call AccountingEngine replay.",
        "TASK-223 does not modify negative fixture JSON.",
        "TASK-223 does not modify positive fixture JSON.",
        "TASK-223 does not modify TASK-219 static validator.",
        "TASK-223 does not modify production code.",
        "TASK-223 does not modify QML / DataServiceActions / repositories / migrations.",
        "TASK-223 does not write runtime SQL / SQLite / audit / ledger / snapshot.",
        "TASK-223 does not connect broker / network / credentials / endpoint.",
        "TASK-223 does not place real orders or enable automatic trading.",
        "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/",
        "tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF*.json",
        "negativeFixtureId",
        "issueCode",
        "severity",
        "blocking",
        "message",
        "The validator must not echo raw fixture payload.",
        "Unknown issue code must fail.",
        "Duplicate fixture id must fail.",
        "Unauthorized JSON must fail.",
        "Unsafe scaffold flag must fail.",
        "Recommended next task: TASK-224 manual entry replay negative fixture static validator implementation gate.",
    ]:
        gate.contains(doc242, token, "docs/242")

    for issue_code in sorted(ISSUE_CODES):
        gate.contains(doc242, issue_code, "docs/242 issue code mapping")

    for section in ["## Document Purpose", "## Test Matrix", "## Required Probes", "## Go / No-Go Checklist"]:
        gate.contains(doc243, section, "docs/243")

    for token in [
        "docs/242 exists",
        "docs/243 exists",
        "TASK-223 CTest is registered",
        "future validator scope",
        "future input boundary",
        "future output contract",
        "future issue code mapping",
        "future failure semantics",
        "privacy and sanitization",
        "no replay",
        "no runtime dependency",
        "TASK-222 negative fixture scaffold files remain unchanged",
        "Negative fixture JSON is not modified",
        "Positive fixture JSON is not modified",
        "TASK-219 static validator is not modified",
        "No negative validator / parser / loader / reader / replay implementation",
        "No AccountingEngine replay call",
        "No production code / QML / DataServiceActions / repositories / migrations / broker",
    ]:
        gate.contains(doc243, token, "docs/243")

    actual_negative_files = {path.name for path in negative_dir.iterdir() if path.is_file()}
    gate.require(actual_negative_files == NEGATIVE_FILES, f"TASK-222 negative fixture exact file set changed: {actual_negative_files}")
    gate.require(len(actual_negative_files) == 11, "TASK-222 negative fixture scaffold keeps 11 JSON files")
    gate.require(
        len([name for name in actual_negative_files if name.startswith("NEG_MRF")]) == 10,
        "TASK-222 negative fixture scaffold keeps 10 NEG_MRF files",
    )
    gate.require(
        "negative_fixtures_index.json" in actual_negative_files,
        "TASK-222 negative fixture scaffold keeps index file",
    )
    for filename in NEGATIVE_FILES:
        gate.require((negative_dir / filename).exists(), f"negative fixture file exists: {filename}")
        gate.require((negative_dir / filename).suffix.lower() == ".json", f"negative fixture is JSON: {filename}")

    gate.require((positive_dir / "fixtures_index.json").exists(), "positive fixture index exists")
    gate.require(future_validator_dir.exists(), "TASK-224 test-only negative validator directory exists")
    gate.require(future_validator_py.exists(), "TASK-224 test-only negative validator implementation exists")
    gate.require(
        future_validator_dir.relative_to(root).as_posix()
        == "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator",
        "TASK-224 test-only negative validator directory stays exact",
    )
    gate.require(
        future_validator_py.relative_to(root).as_posix()
        == "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",
        "TASK-224 test-only negative validator implementation path stays exact",
    )

    changed = changed_paths(root)
    task245_allowed_paths = {
        "docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md",
        "docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    }
    ALLOWED_CHANGED_PATHS.update(task245_allowed_paths)
    unexpected = sorted(changed - ALLOWED_CHANGED_PATHS)
    gate.require(not unexpected, "TASK-223 changed unauthorized paths: " + ", ".join(unexpected))
    gate.require(
        not any(path.startswith("tests/fixtures/manual_entry_replay_negative/") and path.endswith(".json") for path in changed),
        "TASK-223 changed set does not include negative fixture JSON",
    )
    gate.require(
        not any(path.startswith("tests/fixtures/manual_entry_replay/") and path.endswith(".json") for path in changed),
        "TASK-223 changed set does not include positive fixture JSON",
    )
    gate.require(not any(path.startswith("apps/") for path in changed), "TASK-223 changed set does not include apps")
    gate.require(not any(path.startswith("libs/") for path in changed), "TASK-223 changed set does not include libs")
    gate.require(not any(path.startswith("migrations/") for path in changed), "TASK-223 changed set does not include migrations")

    task219_relative = task219_validator.relative_to(root).as_posix()
    for path in changed:
        gate.require(not path.startswith(FORBIDDEN_CHANGED_PREFIXES), f"forbidden changed path: {path}")
        gate.require(path != task219_relative, "TASK-219 static validator must not change")
        gate.require(not path.endswith(".json"), f"TASK-223 must not modify JSON fixtures: {path}")
        gate.require(not path.endswith(".sql"), f"TASK-223 must not add SQL files: {path}")

    for forbidden_path in [
        "tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF001_missing_required_field.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF010_real_order_id_token.json",
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        task219_relative,
    ]:
        gate.require(forbidden_path not in changed, f"forbidden path changed: {forbidden_path}")

    for token in ["broker", "endpoint", "credential", "real order", "automatic trading"]:
        gate.contains(doc242, token, "docs/242 forbidden boundary")

    gate.require(gate.checks >= 150, f"expected at least 150 gate checks, got {gate.checks}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
