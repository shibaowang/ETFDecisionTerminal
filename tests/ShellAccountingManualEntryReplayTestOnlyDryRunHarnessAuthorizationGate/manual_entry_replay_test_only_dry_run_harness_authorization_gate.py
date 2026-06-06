TASK_249_BRIDGE_CI_CLOSEOUT_SELF_CONSISTENCY_PATHS = {
    "docs/294_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_gate.md",
    "docs/295_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/manual_entry_replay_accountingengine_bridge_ci_closeout_gate.py",
}

#!/usr/bin/env python3

import argparse
import json
import re
import subprocess
from pathlib import Path
from typing import Any


TASK_230_DOC = Path(
    "docs/256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md"
)
TASK_230_PLAN = Path(
    "docs/257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md"
)
TASK_230_GATE_DIR = Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate")
TASK_230_GATE = TASK_230_GATE_DIR / "manual_entry_replay_test_only_dry_run_harness_authorization_gate.py"
TASK_230_CMAKE = TASK_230_GATE_DIR / "CMakeLists.txt"
TASK_231_DOC = Path(
    "docs/258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md"
)
TASK_231_PLAN = Path(
    "docs/259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md"
)

TASK_231_HARNESS_DIR = Path("tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness")
TASK_231_HARNESS = TASK_231_HARNESS_DIR / "manual_entry_replay_test_only_dry_run_harness.py"
TASK_231_CMAKE = TASK_231_HARNESS_DIR / "CMakeLists.txt"
TASK_231_HARNESS_CTEST = "shell_accounting_manual_entry_replay_test_only_dry_run_harness"
AUTHORIZATION_CTEST = "shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization"

NEGATIVE_DIR = Path("tests/fixtures/manual_entry_replay_negative")
POSITIVE_DIR = Path("tests/fixtures/manual_entry_replay")
TASK_219_VALIDATOR = Path(
    "tests/ShellAccountingManualEntryReplayFixtureStaticValidator/"
    "manual_entry_replay_fixture_static_validator.py"
)
TASK_224_VALIDATOR = Path(
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/"
    "manual_entry_replay_negative_fixture_static_validator.py"
)

TASK_DOCS = {
    "TASK-229": [
        Path("docs/254_shell_accounting_manual_entry_replay_next_phase_authorization_planning_gate.md"),
        Path("docs/255_shell_accounting_manual_entry_replay_next_phase_authorization_planning_test_plan.md"),
    ],
    "TASK-230": [TASK_230_DOC, TASK_230_PLAN],
    "TASK-231": [TASK_231_DOC, TASK_231_PLAN],
}

CRITICAL_CTEST_NAMES = [
    AUTHORIZATION_CTEST,
    "shell_accounting_manual_entry_replay_next_phase_authorization_planning",
    "shell_accounting_manual_entry_replay_negative_fixture_validator_phase_closeout",
    "shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout",
    "shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening",
    "shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix",
    "shell_accounting_manual_entry_replay_negative_fixture_static_validator",
    "shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization",
    "shell_accounting_manual_entry_replay_negative_fixture_scaffold_files",
    "shell_diagnostics_facade",
]

EXPECTED_NEGATIVE_FILES = {
    "NEG_MRF001": ("NEG_MRF001_missing_required_field.json", "NEG_FIXTURE_MISSING_REQUIRED_FIELD"),
    "NEG_MRF002": ("NEG_MRF002_wrong_schema_version.json", "NEG_FIXTURE_SCHEMA_VERSION_INVALID"),
    "NEG_MRF003": ("NEG_MRF003_runtime_use_true.json", "NEG_FIXTURE_RUNTIME_USE_TRUE"),
    "NEG_MRF004": ("NEG_MRF004_production_use_true.json", "NEG_FIXTURE_PRODUCTION_USE_TRUE"),
    "NEG_MRF005": ("NEG_MRF005_replay_executed_true.json", "NEG_FIXTURE_REPLAY_EXECUTED_TRUE"),
    "NEG_MRF006": ("NEG_MRF006_non_synthetic_privacy.json", "NEG_FIXTURE_SYNTHETIC_FLAG_FALSE"),
    "NEG_MRF007": ("NEG_MRF007_extra_json_file.json", "NEG_FIXTURE_UNAUTHORIZED_FILE"),
    "NEG_MRF008": ("NEG_MRF008_forbidden_token.sql.json", "NEG_FIXTURE_RAW_SQL_TOKEN"),
    "NEG_MRF009": ("NEG_MRF009_broker_payload_token.json", "NEG_FIXTURE_BROKER_PAYLOAD_TOKEN"),
    "NEG_MRF010": ("NEG_MRF010_real_order_id_token.json", "NEG_FIXTURE_REAL_ORDER_TOKEN"),
}

ALLOWED_CHANGED_PATHS = {
    "docs/300_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.md",
    "docs/301_shell_accounting_manual_entry_replay_readonly_runtime_integration_vertical_slice_test_plan.md",
    "docs/302_shell_accounting_manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice.md",
    "docs/303_shell_accounting_manual_entry_replay_fixture_backed_vba_parity_test_plan.md",
    "tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/fixtures/TASK253_vba_parity_buy_partial_sell.json",
    "tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/manual_entry_replay_fixture_backed_vba_parity_readonly_vertical_slice.cpp",
    "docs/304_shell_accounting_manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice.md",
    "docs/305_shell_accounting_manual_entry_replay_fixture_parity_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_only.json",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_partial_sell.json",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_cash_adjustment.json",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_unsupported_or_issue.json",
    "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/manual_entry_replay_fixture_parity_matrix_readonly_vertical_slice.cpp",
    "docs/306_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice.md",
    "docs/307_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_test_plan.md",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/fixtures/TASK255_sanitized_excel_vba_export_buy_partial_sell.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice.cpp",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice.cpp",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.py",
    "docs/298_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_gate.md",
    "docs/299_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/manual_entry_replay_accountingengine_runtime_integration_authorization_gate.py",
"docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md",

    "docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/CMakeLists.txt",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md",
    "docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",

    "docs/268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md",
    "docs/269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md",
    "docs/270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md",
    "docs/271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md",
    "docs/272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md",
    "docs/273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md",
    "docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md",
    "docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "docs/266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md",
    "docs/267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    TASK_230_DOC.as_posix(),
    TASK_230_PLAN.as_posix(),
    "tests/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/CMakeLists.txt",
    TASK_231_DOC.as_posix(),
    TASK_231_PLAN.as_posix(),
    "docs/260_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.md",
    "docs/261_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_test_plan.md",
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    TASK_231_CMAKE.as_posix(),
    TASK_231_HARNESS.as_posix(),
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    TASK_224_VALIDATOR.as_posix(),
    TASK_230_CMAKE.as_posix(),
    TASK_230_GATE.as_posix(),
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

FORBIDDEN_IMPLEMENTATION_PATH_PARTS = (
    "Parser",
    "Loader",
    "Reader",
    "ReplayHarness",
    "ReplayImplementation",
)

ALLOWED_IMPLEMENTATION_AUTHORIZATION_PATHS = {

    "docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md",
    "docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",

    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
}

FORBIDDEN_RUNTIME_TEXT_TOKENS = (
    ("import " + "sqlite3", "does not import sqlite"),
    ("INSERT " + "INTO", "does not contain insert statement"),
    ("UPDATE " + "manual", "does not update manual data"),
    ("DELETE " + "FROM", "does not contain delete statement"),
    ("request" + "s", "does not import network client"),
    ("place" + "Order", "does not place orders"),
    ("execute" + "Order", "does not execute orders"),
    ("broker" + "Order", "does not call broker order"),
)

ACCOUNTING_ENGINE_REPLAY_CALL = "AccountingEngine()" + ".replay"
RECURSIVE_FULL_CTEST_TOKENS = (
    "ctest --test-dir build " + "--output-on-failure",
    "ctest --test-dir build " + "-j",
    "subprocess.run([\"ctest\", \"--test-dir\", \"build\"]",
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
        self.require(token in text or normalized_token in normalized_text, f"{context} missing `{token}`")


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_json(path: Path) -> Any:
    return json.loads(read(path))


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
    paths = set()
    paths.update(git_lines(root, "diff", "--name-only", "main"))
    paths.update(git_lines(root, "diff", "--cached", "--name-only"))
    paths.update(status_paths(root))
    return {path for path in paths if path}


def ctest_names(root: Path, gate: Gate) -> set[str]:
    build = root / "build"
    gate.require(build.exists(), "build directory exists for CTest registration probe")
    completed = subprocess.run(
        ["ctest", "--test-dir", str(build), "-N"],
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


def validate_docs(gate: Gate, root: Path) -> None:
    doc256 = read(root / TASK_230_DOC)
    doc257 = read(root / TASK_230_PLAN)

    for task, docs in TASK_DOCS.items():
        for doc in docs:
            gate.require((root / doc).exists(), f"{doc.as_posix()} exists")
            gate.contains(read(root / doc), task, doc.as_posix())

    for section in [
        "## Purpose",
        "## Relationship To TASK-229",
        "## Future Harness Scope",
        "## Future Directory Boundary",
        "## Future CTest Name",
        "## Future Input Boundary",
        "## Future Output Boundary",
        "## No-Write Boundary",
        "## No-Replay Boundary",
        "## AccountingEngine Boundary",
        "## Runtime SQL And Ledger Boundary",
        "## Broker And Network Boundary",
        "## Explicit Non-Authorization List",
        "## Go / No-Go Handoff Rules",
        "## Formal Conclusion And Next Task",
    ]:
        gate.contains(doc256, section, "docs/256")

    for token in [
        TASK_231_HARNESS_DIR.as_posix() + "/",
        TASK_231_HARNESS_CTEST,
        "tests/fixtures/manual_entry_replay/fixtures_index.json",
        "tests/fixtures/manual_entry_replay/*.json",
        "tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json",
        "tests/fixtures/manual_entry_replay_negative/NEG_MRF*.json",
        "fixtureId",
        "dryRunStatus",
        "sanitizedSummary",
        "plannedStepCount",
        "warnings",
        "TASK-230 does not authorize dry-run harness implementation.",
        "TASK-230 does not authorize replay implementation.",
        "TASK-230 does not authorize AccountingEngine replay calls.",
        "TASK-230 does not authorize parser / loader / reader implementation.",
        "TASK-230 does not authorize runtime SQL / SQLite read/write.",
        "TASK-230 does not authorize audit / ledger / snapshot writes.",
        "TASK-230 does not authorize production integration.",
        "TASK-230 does not authorize QML / DataServiceActions / repositories changes.",
        "TASK-230 does not authorize broker / network / credentials / endpoint.",
        "TASK-230 does not authorize real order placement or automatic trading.",
        "TASK-230 authorizes the future test-only dry-run harness boundary only.",
        "Recommended next task: TASK-231 manual entry replay test-only dry-run harness implementation gate.",
    ]:
        gate.contains(doc256, token, "docs/256")

    for section in [
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
    ]:
        gate.contains(doc257, section, "docs/257")

    for token in [
        "TASK-230 authorization gate passes",
        "Future harness is not implemented",
        "Fixture JSON unchanged",
        "Validator validation behavior unchanged",
        "Production code unchanged",
        "Full CTest is run externally and passes",
    ]:
        gate.contains(doc257, token, "docs/257")

    doc258 = read(root / TASK_231_DOC)
    doc259 = read(root / TASK_231_PLAN)
    for section in [
        "## Purpose",
        "## Relationship To TASK-230",
        "## Test-Only Harness Scope",
        "## Input Boundary",
        "## Output Contract",
        "## No-Write Boundary",
        "## No-Replay Boundary",
        "## AccountingEngine Boundary",
        "## Runtime SQL And Ledger Boundary",
        "## Broker And Network Boundary",
        "## Fixture Immutability Boundary",
        "## Formal Conclusion And Next Task",
    ]:
        gate.contains(doc258, section, "docs/258")
    for token in [
        "TASK-231 implements the manual entry replay test-only dry-run harness",
        TASK_231_HARNESS_CTEST,
        "TASK-231 does not implement production parser / loader / reader behavior.",
        "TASK-231 does not implement replay",
        "TASK-231 does not call AccountingEngine replay",
        "TASK-231 does not modify fixture JSON",
        "TASK-231 does not write runtime SQL",
        "TASK-231 does not compute real position",
        "Recommended next task: TASK-232",
    ]:
        gate.contains(doc258, token, "docs/258")
    for section in [
        "## Document Purpose",
        "## Test Matrix",
        "## Required Probes",
        "## Go / No-Go Checklist",
    ]:
        gate.contains(doc259, section, "docs/259")
    for token in [
        "TASK-231 dry-run harness CTest passes",
        "TASK-230 authorization gate passes",
        "Fixture JSON unchanged",
        "Validator validation behavior unchanged",
        "Production code unchanged",
    ]:
        gate.contains(doc259, token, "docs/259")

    for index_path in [Path("README.md"), Path("docs/README.md"), Path("docs/12_codex_prompt_template.md")]:
        text = read(root / index_path)
        gate.contains(text, "TASK-230", index_path.as_posix())
        gate.contains(text, TASK_230_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_230_PLAN.name, index_path.as_posix())
        gate.contains(text, "TASK-231", index_path.as_posix())
        gate.contains(text, TASK_231_DOC.name, index_path.as_posix())
        gate.contains(text, TASK_231_PLAN.name, index_path.as_posix())


def validate_registration(gate: Gate, root: Path) -> None:
    tests_cmake = read(root / "tests/CMakeLists.txt")
    gate.contains(
        tests_cmake,
        "add_subdirectory(ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate)",
        "tests/CMakeLists",
    )
    gate.require((root / TASK_230_CMAKE).exists(), "TASK-230 CMakeLists exists")
    gate.require((root / TASK_230_GATE).exists(), "TASK-230 gate script exists")
    cmake = read(root / TASK_230_CMAKE)
    gate.contains(cmake, AUTHORIZATION_CTEST, "TASK-230 CMakeLists")
    gate.contains(cmake, TASK_230_GATE.name, "TASK-230 CMakeLists")

    names = ctest_names(root, gate)
    for name in CRITICAL_CTEST_NAMES:
        gate.require(name in names, f"CTest registered: {name}")
    gate.require(TASK_231_HARNESS_CTEST in names, "TASK-231 harness implementation CTest is registered")


def validate_assets(gate: Gate, root: Path) -> None:
    gate.require((root / TASK_231_HARNESS_DIR).exists(), "TASK-231 harness directory exists")
    gate.require((root / TASK_231_CMAKE).exists(), "TASK-231 harness CMakeLists exists")
    gate.require((root / TASK_231_HARNESS).exists(), "TASK-231 harness script exists")
    harness_files = {
        path.relative_to(root).as_posix()
        for path in (root / TASK_231_HARNESS_DIR).iterdir()
        if path.is_file()
    }
    gate.require(
        harness_files == {TASK_231_CMAKE.as_posix(), TASK_231_HARNESS.as_posix()},
        "TASK-231 harness directory contains only authorized test-only files",
    )

    index_path = root / NEGATIVE_DIR / "negative_fixtures_index.json"
    gate.require(index_path.exists(), "negative fixture index exists")
    index = read_json(index_path)
    gate.require(index.get("schemaVersion") == "manual-entry-replay-negative-fixtures-index/v1", "negative fixture index schema")
    gate.require(index.get("generatedByTask") == "TASK-222", "negative fixture index generated by TASK-222")
    gate.require(index.get("runtimeUse") is False, "negative fixture index runtimeUse false")
    gate.require(index.get("productionUse") is False, "negative fixture index productionUse false")
    gate.require(index.get("replayExecuted") is False, "negative fixture index replayExecuted false")
    fixtures = index.get("fixtures")
    gate.require(isinstance(fixtures, list), "negative fixture index fixtures list")
    gate.require(len(fixtures) == 10, "negative fixture index has ten fixtures")
    by_id = {fixture.get("negativeFixtureId"): fixture for fixture in fixtures}
    gate.require(set(by_id) == set(EXPECTED_NEGATIVE_FILES), "negative fixture ids match expected set")

    for fixture_id, (file_name, issue_code) in EXPECTED_NEGATIVE_FILES.items():
        fixture = by_id[fixture_id]
        path = root / NEGATIVE_DIR / file_name
        gate.require(path.exists(), f"negative fixture exists: {file_name}")
        gate.require(fixture.get("file") == file_name, f"index file mapping: {fixture_id}")
        gate.require(fixture.get("expectedIssueCode") == issue_code, f"index issue mapping: {fixture_id}")
        gate.require(fixture.get("runtimeUse") is False, f"fixture index runtime false: {fixture_id}")
        gate.require(fixture.get("productionUse") is False, f"fixture index production false: {fixture_id}")
        gate.require(fixture.get("replayExecuted") is False, f"fixture index replay false: {fixture_id}")

    gate.require((root / POSITIVE_DIR / "fixtures_index.json").exists(), "positive fixture index exists")
    gate.require(TASK_219_VALIDATOR.exists(), "TASK-219 positive validator exists")
    gate.require(TASK_224_VALIDATOR.exists(), "TASK-224 negative validator exists")


def validate_changed_paths(gate: Gate, root: Path) -> set[str]:
    changes = changed_paths(root)
    ALLOWED_CHANGED_PATHS.update({
        "docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md",
        "docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    })
    for path in sorted(changes):
        gate.require(path in ALLOWED_CHANGED_PATHS, f"changed path exact allowlisted: {path}")
    for prefix in FORBIDDEN_CHANGED_PREFIXES:
        gate.require(not any(path.startswith(prefix) for path in changes), f"no changed path under {prefix}")
    gate.require(TASK_219_VALIDATOR.as_posix() not in changes, "TASK-219 positive validator unchanged")
    if TASK_224_VALIDATOR.as_posix() in changes:
        gate.require(
            TASK_224_VALIDATOR.as_posix() in ALLOWED_CHANGED_PATHS,
            "TASK-224 negative validator exact-path self-consistency allowlisted",
        )
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "apps") == set(), "apps diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "libs") == set(), "libs diff empty")
    gate.require(git_lines(root, "diff", "--name-only", "main", "--", "migrations") == set(), "migrations diff empty")
    gate.require(
        git_lines(root, "diff", "--name-only", "main", "--", "tests/fixtures/manual_entry_replay") == set(),
        "positive fixture JSON diff empty",
    )
    gate.require(
        git_lines(root, "diff", "--name-only", "main", "--", "tests/fixtures/manual_entry_replay_negative") == set(),
        "negative fixture JSON diff empty",
    )
    return changes


def validate_no_runtime_boundary(gate: Gate, root: Path, changes: set[str]) -> None:
    gate.require(not any(path.startswith(("apps/", "libs/", "migrations/")) for path in changes), "no production changed paths")
    gate.require(all(path in {"tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_only.json", "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_buy_partial_sell.json", "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_cash_adjustment.json", "tests/ShellAccountingManualEntryReplayFixtureParityMatrixReadOnlyVerticalSlice/fixtures/TASK254_unsupported_or_issue.json", "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportReadOnlyVerticalSlice/fixtures/TASK255_sanitized_excel_vba_export_buy_partial_sell.json"} or path == "tests/ShellAccountingManualEntryReplayFixtureBackedVbaParityReadOnlyVerticalSlice/fixtures/TASK253_vba_parity_buy_partial_sell.json" or not path.endswith(".json") for path in changes), "no fixture JSON changed paths")
    implementation_like_paths = [
        path
        for path in changes
        if path not in ALLOWED_IMPLEMENTATION_AUTHORIZATION_PATHS
        and any(part in path for part in FORBIDDEN_IMPLEMENTATION_PATH_PARTS)
    ]
    gate.require(not implementation_like_paths, "no parser / loader / reader / replay implementation changed paths")

    scanned_paths = [
        TASK_230_DOC,
        TASK_230_PLAN,
        TASK_231_DOC,
        TASK_231_PLAN,
        TASK_230_GATE,
        TASK_231_HARNESS,
    ]
    for path in scanned_paths:
        text = read(root / path)
        for token in RECURSIVE_FULL_CTEST_TOKENS:
            gate.require(token not in text, f"{path.as_posix()} does not recursively run full CTest")
        gate.require(ACCOUNTING_ENGINE_REPLAY_CALL not in text, f"{path.as_posix()} does not call AccountingEngine replay")
        lowered = text.lower()
        for token, message in FORBIDDEN_RUNTIME_TEXT_TOKENS:
            haystack = lowered if token.islower() else text
            gate.require(token not in haystack, f"{path.as_posix()} {message}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)
    gate = Gate()

    changes = validate_changed_paths(gate, root)
    validate_docs(gate, root)
    validate_registration(gate, root)
    validate_assets(gate, root)
    validate_no_runtime_boundary(gate, root, changes)
    gate.require(gate.checks >= 160, f"expected at least 160 gate checks, got {gate.checks}")
    print(json.dumps({"status": "passed", "checks": gate.checks}, sort_keys=True))
    return 0



TASK_247_SELF_CONSISTENCY_PATHS = {
    "docs/290_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_gate.md",
    "docs/291_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
}
ALLOWED_CHANGED_PATHS.update(TASK_247_SELF_CONSISTENCY_PATHS)


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

ALLOWED_CHANGED_PATHS.update(TASK_249_BRIDGE_CI_CLOSEOUT_SELF_CONSISTENCY_PATHS)

ALLOWED_CHANGED_PATHS.update({

    "docs/296_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_gate.md",

    "docs/297_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_test_plan.md",

    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/CMakeLists.txt",

    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/manual_entry_replay_accountingengine_bridge_phase_closeout_gate.py",

})
if __name__ == "__main__":
    raise SystemExit(main())
