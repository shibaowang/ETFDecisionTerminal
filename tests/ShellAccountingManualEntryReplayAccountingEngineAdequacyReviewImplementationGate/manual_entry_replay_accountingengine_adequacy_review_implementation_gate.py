#!/usr/bin/env python3

import argparse
import hashlib
import json
import os
import re
import subprocess
from pathlib import Path


TASK_250_PHASE_CLOSEOUT_SELF_CONSISTENCY_PATHS = {
    "docs/296_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_gate.md",
    "docs/297_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/manual_entry_replay_accountingengine_bridge_phase_closeout_gate.py",
}

TASK_240_DOC = Path("docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md")
TASK_240_PLAN = Path("docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md")
TASK_240_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate")
TASK_240_CMAKE = TASK_240_DIR / "CMakeLists.txt"
TASK_240_GATE = TASK_240_DIR / "manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py"
TASK_240_CTEST = "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation"

TASK_239_DOC = Path("docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md")
TASK_239_PLAN = Path("docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md")
TASK_239_CTEST = "shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization"

FUTURE_REVIEW_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReview")
FUTURE_BRIDGE_DIR = Path("tests/ShellAccountingManualEntryReplayAccountingEngineBridge")

TASK_231_HARNESS = Path(
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/"
    "manual_entry_replay_test_only_dry_run_harness.py"
)
TASK_224_VALIDATOR = Path(
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/"
    "manual_entry_replay_negative_fixture_static_validator.py"
)
TASK_234_IMPLEMENTATION = Path(
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py"
)

TASK_DOCS = [
    Path("docs/262_shell_accounting_manual_entry_replay_implementation_authorization_gate.md"),
    Path("docs/263_shell_accounting_manual_entry_replay_implementation_authorization_test_plan.md"),
    Path("docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md"),
    Path("docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md"),
    Path("docs/266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md"),
    Path("docs/267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md"),
    Path("docs/268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md"),
    Path("docs/269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md"),
    Path("docs/270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md"),
    Path("docs/271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md"),
    Path("docs/272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md"),
    Path("docs/273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md"),
    TASK_239_DOC,
    TASK_239_PLAN,
    TASK_240_DOC,
    TASK_240_PLAN,
]

CRITICAL_SCRIPTS = [
    TASK_231_HARNESS,
    TASK_224_VALIDATOR,
    TASK_234_IMPLEMENTATION,
    Path(
        "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/"
        "manual_entry_replay_implementation_authorization_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/"
        "manual_entry_replay_implementation_regression_matrix_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/"
        "manual_entry_replay_implementation_failure_mode_hardening_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/"
        "manual_entry_replay_implementation_ci_closeout_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/"
        "manual_entry_replay_implementation_phase_closeout_gate.py"
    ),
    Path(
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/"
        "manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py"
    ),
    TASK_240_GATE,
]

CRITICAL_CTEST_NAMES = [
    TASK_240_CTEST,
    TASK_239_CTEST,
    "shell_accounting_manual_entry_replay_implementation_phase_closeout",
    "shell_accounting_manual_entry_replay_implementation_ci_closeout",
    "shell_accounting_manual_entry_replay_implementation_failure_mode_hardening",
    "shell_accounting_manual_entry_replay_implementation_regression_matrix",
    "shell_accounting_manual_entry_replay_implementation",
    "shell_accounting_manual_entry_replay_implementation_authorization",
    "shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix",
    "shell_accounting_manual_entry_replay_test_only_dry_run_harness",
    "shell_diagnostics_facade",
    "transport_local_socket_echo",
]

ALLOWED_CHANGED_PATHS = {
    "docs/294_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_gate.md",
    "docs/295_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/manual_entry_replay_accountingengine_bridge_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
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
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
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
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/manual_entry_replay_negative_fixture_validator_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/manual_entry_replay_next_phase_authorization_planning_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",

    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    TASK_240_DOC.as_posix(),
    TASK_240_PLAN.as_posix(),
    "tests/CMakeLists.txt",
    TASK_240_CMAKE.as_posix(),
    TASK_240_GATE.as_posix(),
    "docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md",
    "docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/manual_entry_replay_negative_fixture_static_validator.py",


    "docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md",

    "docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md",

    "docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md",

    "docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/CMakeLists.txt",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",}

ALLOWED_CHANGED_PATHS.update(
    {
        "docs/280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md",
        "docs/281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
        "docs/282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md",
        "docs/283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md",
        "docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md",
        "docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md",
        "docs/288_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_gate.md",
        "docs/289_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_test_plan.md",
        "docs/290_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_gate.md",
        "docs/291_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridge/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridge/manual_entry_replay_accountingengine_bridge.py",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
    }
)

SANITIZED_FAILURE_ENV = "ETFDT_SANITIZED_FAILURE_OUTPUT"

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

TASK_248_SELF_CONSISTENCY_PATHS = {
    "docs/292_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.md",
    "docs/293_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.py",
}

ALLOWED_CHANGED_PATHS.update(TASK_248_SELF_CONSISTENCY_PATHS)

TASK_248_OLD_GATE_SELF_CONSISTENCY_PATHS = {
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
}

ALLOWED_CHANGED_PATHS.update(TASK_248_OLD_GATE_SELF_CONSISTENCY_PATHS)


FORBIDDEN_CHANGED_PREFIXES = (
    "apps/",
    "libs/",
    "migrations/",
    "tests/fixtures/manual_entry_replay/",
    "tests/fixtures/manual_entry_replay_negative/",
)

BAD_CLEAN_MAIN_ASSERTIONS = (
    "changed path detection " + "must include current TASK",
    "must include current TASK-",
    "implementation extension " + "changed",
    "test-only parameter extension " + "changed",
)

RECURSIVE_FULL_CTEST_TOKENS = (
    "ctest --test-dir build " + "--output-on-failure",
    'subprocess.run(["ctest", "--test-dir", "build"]',
)

FORBIDDEN_GATE_TOKENS = (
    "import " + "sqlite3",
    "INSERT " + "INTO",
    "UP" + "DATE ",
    "DELETE " + "FROM",
    "REPLACE " + "INTO",
    "AccountingReplayEngine(",
    "AccountingReplayEngine::",
    "AccountingEngine()" + ".replay",
    ".replayLedger(",
    "refreshReadModel(",
    "writeAuditLog(",
    "writeLedger(",
    "writeSnapshot(",
    "broker" + "Order(",
    "place" + "Order(",
    "execute" + "Order(",
    "request" + "s.",
    "url" + "lib.",
)

POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")
DB_SUFFIXES = (".db", ".sqlite", ".sqlite3", ".db-wal", ".db-shm", ".sqlite-wal", ".sqlite-shm")


class Gate:
    def __init__(self) -> None:
        self.checks = 0
        self.results: list[dict[str, object]] = []
        self.sanitized_output = os.environ.get(SANITIZED_FAILURE_ENV) == "1"

    def require(self, condition: bool, message: str) -> None:
        self.checks += 1
        passed = bool(condition)
        self.results.append({"check": self.checks, "passed": passed, "message": message})
        visible_message = "check passed" if self.sanitized_output and passed else message
        if self.sanitized_output and not passed:
            visible_message = "sanitized failure"
        print(f"CHECK {self.checks:03d} {'PASS' if passed else 'FAIL'} {visible_message}")
        if not passed:
            raise AssertionError(message)

    def contains(self, text: str, token: str, context: str) -> None:
        normalized_text = " ".join(text.split())
        normalized_token = " ".join(token.split())
        self.require(token in text or normalized_token in normalized_text, f"{context} contains `{token}`")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def git_lines(root: Path, *args: str) -> set[str]:
    completed = subprocess.run(["git", *args], cwd=root, check=True, capture_output=True, text=True)
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def status_paths(root: Path) -> set[str]:
    completed = subprocess.run(
        ["git", "status", "--porcelain=v1", "-uall"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    paths: set[str] = set()
    for line in completed.stdout.splitlines():
        if not line:
            continue
        path = line[3:].strip().replace("\\", "/")
        if " -> " in path:
            path = path.split(" -> ", 1)[1]
        if path:
            paths.add(path)
    return paths


def changed_paths(root: Path) -> set[str]:
    return (
        git_lines(root, "diff", "--name-only", "main")
        | git_lines(root, "diff", "--cached", "--name-only")
        | git_lines(root, "ls-files", "--others", "--exclude-standard")
        | status_paths(root)
    )


def fixture_paths(root: Path) -> list[Path]:
    return sorted((root / POSITIVE_DIR).glob("*.json")) + sorted((root / NEGATIVE_DIR).glob("*.json"))


def fixture_hashes(root: Path) -> dict[str, str]:
    return {path.relative_to(root).as_posix(): sha256(path) for path in fixture_paths(root)}


def db_like_untracked_paths(root: Path) -> set[str]:
    return {path for path in status_paths(root) if path.lower().endswith(DB_SUFFIXES)}


def ctest_names(root: Path, gate: Gate) -> set[str]:
    completed = subprocess.run(
        ["ctest", "--test-dir", str(root / "build"), "-N"],
        cwd=root,
        check=False,
        capture_output=True,
        text=True,
    )
    gate.require(completed.returncode == 0, "ctest -N exits zero")
    names: set[str] = set()
    for line in completed.stdout.splitlines():
        match = re.search(r"Test\s+#\d+:\s+(\S+)", line)
        if match:
            names.add(match.group(1))
    gate.require(bool(names), "ctest -N returns registered tests")
    return names


def has_recursive_full_ctest_invocation(text: str) -> bool:
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if stripped.startswith(("RECURSIVE_FULL_CTEST_TOKENS", '"', "'")):
            continue
        if stripped.startswith(
            (
                'if "ctest --test-dir build"',
                "if 'ctest --test-dir build'",
                'if "subprocess.run([',
                "if 'subprocess.run([",
                "if token.startswith",
            )
        ):
            continue
        if "not in text" in stripped or "not in haystack" in stripped:
            continue
        if "ctest --test-dir build" in line and "--output-on-failure" in line:
            if "not in" not in line:
                return True
        if 'subprocess.run(["ctest", "--test-dir", "build"]' in line:
            return True
    return False


def has_clean_main_brittle_assertion(text: str, token: str) -> bool:
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if stripped.startswith(("BAD_CLEAN_MAIN_ASSERTIONS", '"', "'")):
            continue
        if token in line:
            return True
    return False


def token_in_executable_lines(text: str, token: str) -> bool:
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped or stripped.startswith("#"):
            continue
        if stripped.startswith(("FORBIDDEN_GATE_TOKENS", '"', "'")):
            continue
        if "not in text" in stripped or "not in haystack" in stripped:
            continue
        if token in line:
            return True
    return False


def validate_changed_paths(gate: Gate, root: Path) -> set[str]:
    changes = changed_paths(root)
    gate.require(all(not path.endswith("/") for path in ALLOWED_CHANGED_PATHS), "allowlist uses exact files only")
    gate.require(all("*" not in path for path in ALLOWED_CHANGED_PATHS), "allowlist uses no wildcards")
    for forbidden_dir in ["docs/", "tests/", "apps/", "libs/", "migrations/"]:
        gate.require(forbidden_dir not in ALLOWED_CHANGED_PATHS, f"allowlist does not include {forbidden_dir} directory")
    unexpected = sorted(changes - ALLOWED_CHANGED_PATHS)
    gate.require(not unexpected, "TASK-240 changed unauthorized paths: " + ", ".join(unexpected))
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
        gate.require(not path.startswith(FORBIDDEN_CHANGED_PREFIXES), f"forbidden changed prefix not used: {path}")
        gate.require(not path.endswith(".json"), f"fixture JSON unchanged: {path}")
        gate.require(not path.endswith(".sql"), f"SQL file unchanged: {path}")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "apps") == set(), "apps diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "libs") == set(), "libs diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "migrations") == set(), "migrations diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "libs/AccountingEngine") == set(), "AccountingEngine diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", POSITIVE_DIR.as_posix()) == set(), "positive fixture JSON diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", NEGATIVE_DIR.as_posix()) == set(), "negative fixture JSON diff empty")
    return changes


def validate_docs(gate: Gate, root: Path) -> None:
    for path in TASK_DOCS:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")

    doc276 = read(root / TASK_240_DOC)
    doc277 = read(root / TASK_240_PLAN)
    for section in [
        "## Scope",
        "## Preconditions",
        "## Review Inputs",
        "## Static Adequacy Matrix",
        "## Explicit Non-Goals",
        "## Forbidden Side Effects",
        "## Formal Conclusion",
        "## Go/No-Go Decision",
        "## Next Phase Handoff",
    ]:
        gate.contains(doc276, section, "docs/276")
    for section in [
        "## Test Objective",
        "## Static Checks",
        "## Changed-Path Allowlist",
        "## Regression Gates",
        "## Failure Modes",
        "## Acceptance Criteria",
        "## Formal Conclusion",
    ]:
        gate.contains(doc277, section, "docs/277")
    for token in [
        "Implementation in this task means review gate implementation only.",
        "TASK-240 does not implement an AccountingEngine bridge.",
        "TASK-240 does not call AccountingEngine replay.",
        "TASK-240 does not modify AccountingEngine source code.",
        "TASK-240 does not implement production replay.",
        "TASK-240 does not refresh read models.",
        "TASK-240 implements a review gate only.",
        "Future AccountingEngine bridge, runtime integration, replay implementation, read model refresh, production parser / loader / reader work, and any AccountingEngine replay call must be separately authorized in a later TASK.",
        TASK_239_DOC.as_posix(),
        TASK_239_PLAN.as_posix(),
        TASK_240_DOC.as_posix(),
        TASK_240_PLAN.as_posix(),
        TASK_234_IMPLEMENTATION.as_posix(),
        TASK_231_HARNESS.as_posix(),
        TASK_224_VALIDATOR.as_posix(),
    ]:
        gate.contains(doc276, token, "docs/276")
    for token in [
        "TASK-240 verifies the ShellAccounting manual entry replay AccountingEngine adequacy review implementation gate.",
        "Each targeted probe must first run `ctest -N -R` and confirm at least one matching test.",
        "Gate check count below 40 fails.",
        "TASK-240 implementation gate passes.",
        "Future AccountingEngine bridge, runtime integration, replay implementation, read model refresh, production parser / loader / reader work, or AccountingEngine replay calls must be separately authorized by a later TASK.",
        TASK_240_CTEST,
        TASK_239_CTEST,
        "transport_local_socket_echo",
    ]:
        gate.contains(doc277, token, "docs/277")
    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-240", index_path.as_posix())
        gate.contains(text, TASK_240_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_240_PLAN.name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    gate.contains(
        tests_cmake,
        "add_subdirectory(ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate)",
        "tests/CMakeLists",
    )
    gate.require((root / TASK_240_CMAKE).exists(), "TASK-240 CMakeLists exists")
    gate.require((root / TASK_240_GATE).exists(), "TASK-240 gate script exists")
    cmake_text = read(root / TASK_240_CMAKE)
    gate.contains(cmake_text, TASK_240_CTEST, "TASK-240 CMakeLists")
    names = ctest_names(root, gate)
    for name in CRITICAL_CTEST_NAMES:
        gate.require(name in names, f"CTest registered: {name}")


def validate_accountingengine_bridge_test_directory(gate: Gate, root: Path, bridge_path: Path) -> None:
    bridge_dir = root / bridge_path
    if not bridge_dir.exists():
        gate.require(True, "AccountingEngine bridge test directory may be absent before TASK-246")
        return

    allowed_files = {
        (bridge_dir / "CMakeLists.txt").relative_to(root).as_posix(),
        (bridge_dir / ("manual_entry_replay_accountingengine_" + "bridge.py")).relative_to(root).as_posix(),
    }
    observed_files = {
        path.relative_to(root).as_posix()
        for path in bridge_dir.rglob("*")
        if path.is_file()
    }
    observed_directories = [
        path.relative_to(root).as_posix()
        for path in bridge_dir.rglob("*")
        if path.is_dir()
    ]
    gate.require(bridge_dir.is_dir(), "AccountingEngine bridge path is a TASK-246 test-only directory")
    gate.require(observed_files == allowed_files, "AccountingEngine bridge directory contains only TASK-246 exact files")
    gate.require(not observed_directories, "AccountingEngine bridge directory has no runtime integration subdirectory")
    for relative_path in sorted(allowed_files):
        gate.require((root / relative_path).exists(), f"{relative_path} exists")


def validate_future_runtime_absent(gate: Gate, root: Path) -> None:
    for path in [
        FUTURE_REVIEW_DIR,
        FUTURE_BRIDGE_DIR,
        Path("libs/AccountingEngineManualEntryReplayBridge"),
        Path("libs/ManualEntryReplayAccountingEngineBridge"),
        Path("apps/ETFDecisionShell/qml/pages/ShellAccountingReplayPage.qml"),
    ]:
        if path == FUTURE_BRIDGE_DIR:
            validate_accountingengine_bridge_test_directory(gate, root, path)
            continue
        gate.require(not (root / path).exists(), f"future runtime/bridge path absent: {path.as_posix()}")


def validate_artifacts(gate: Gate, root: Path) -> None:
    for path in CRITICAL_SCRIPTS:
        gate.require((root / path).exists(), f"{path.as_posix()} exists")
        gate.require((root / path).is_file(), f"{path.as_posix()} is file")


def validate_clean_main_invariants(gate: Gate, root: Path) -> None:
    for path in CRITICAL_SCRIPTS:
        text = read(root / path)
        for token in BAD_CLEAN_MAIN_ASSERTIONS:
            gate.require(
                not has_clean_main_brittle_assertion(text, token),
                f"{path.as_posix()} has no clean-main brittle assertion `{token}`",
            )
        gate.require(not has_recursive_full_ctest_invocation(text), f"{path.as_posix()} does not recursively run full CTest")


def validate_static_boundaries(gate: Gate, root: Path) -> None:
    gate_text = read(root / TASK_240_GATE)
    for token in FORBIDDEN_GATE_TOKENS:
        gate.require(not token_in_executable_lines(gate_text, token), f"TASK-240 gate does not execute token `{token}`")
    for path in [TASK_234_IMPLEMENTATION, TASK_231_HARNESS, TASK_224_VALIDATOR]:
        text = read(root / path)
        gate.require(not has_recursive_full_ctest_invocation(text), f"{path.as_posix()} does not run full CTest")
    for path in [
        "apps",
        "libs",
        "migrations",
        "libs/AccountingEngine",
        POSITIVE_DIR.as_posix(),
        NEGATIVE_DIR.as_posix(),
    ]:
        gate.require(git_lines(root, "diff", "--name-only", "main", "--", path) == set(), f"{path} diff empty")


def validate_fixture_immutability(gate: Gate, before: dict[str, str], after: dict[str, str]) -> None:
    gate.require(before == after, "fixture JSON hashes unchanged")
    gate.require(bool(before), "fixture hash set is non-empty")
    gate.require(any(path.startswith(POSITIVE_DIR.as_posix()) for path in before), "positive fixtures included")
    gate.require(any(path.startswith(NEGATIVE_DIR.as_posix()) for path in before), "negative fixtures included")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    before_hashes = fixture_hashes(root)
    before_db_like = db_like_untracked_paths(root)
    changes_before = validate_changed_paths(gate, root)
    validate_docs(gate, root)
    validate_registration(gate, root)
    validate_future_runtime_absent(gate, root)
    validate_artifacts(gate, root)
    validate_clean_main_invariants(gate, root)
    validate_static_boundaries(gate, root)
    after_hashes = fixture_hashes(root)
    after_db_like = db_like_untracked_paths(root)
    changes_after = changed_paths(root)

    validate_fixture_immutability(gate, before_hashes, after_hashes)
    gate.require(before_db_like == after_db_like, "no DB/WAL/SHM/SQLite files created")
    gate.require(changes_before == changes_after, "implementation gate does not mutate changed-path set")
    gate.require(gate.checks >= 40, f"expected at least 40 gate checks, got {gate.checks}")
    print(json.dumps({"status": "passed", "checks": gate.checks}, sort_keys=True))
    return 0


def run_entrypoint() -> int:
    try:
        return main()
    except Exception as exc:
        if os.environ.get(SANITIZED_FAILURE_ENV) != "1":
            raise
        print(
            json.dumps(
                {
                    "status": "failed",
                    "error": "sanitized failure",
                    "reason": type(exc).__name__,
                },
                sort_keys=True,
            )
        )
        return 1



TASK_248_FULL_CTEST_SELF_CONSISTENCY_PATHS = {
    "docs/12_codex_prompt_template.md",
    "docs/292_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.md",
    "docs/293_shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening_test_plan.md",
    "docs/README.md",
    "README.md",
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
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridge/manual_entry_replay_accountingengine_bridge.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
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

ALLOWED_CHANGED_PATHS.update(TASK_248_FULL_CTEST_SELF_CONSISTENCY_PATHS)

ALLOWED_CHANGED_PATHS.update(TASK_250_PHASE_CLOSEOUT_SELF_CONSISTENCY_PATHS)
if __name__ == "__main__":
    raise SystemExit(run_entrypoint())
