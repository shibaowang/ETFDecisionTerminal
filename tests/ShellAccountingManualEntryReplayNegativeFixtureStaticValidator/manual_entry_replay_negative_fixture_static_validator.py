TASK_257_EXACT_PATHS = {
    "tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/manual_cash_movement_repository_dual_write_implementation.cpp",
    "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
    "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/310_shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary.md",
    "docs/311_shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary_test_plan.md",
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/include/DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportReadOnlyParser.cpp",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_buy_only_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_cash_adjustment_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_chinese_header_buy_partial_sell_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_invalid_action_amount_cash_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/fixtures/TASK257_missing_required_header_import_payload.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaImportReadOnlyProductionParserBoundary/t257_parser_boundary_slice.cpp",
    "docs/312_shell_accounting_excel_vba_import_readonly_dataservice_preview_action.md",
    "docs/313_shell_accounting_excel_vba_import_readonly_dataservice_preview_action_test_plan.md",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportReadOnlyPreviewAction.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/excel_vba_import_readonly_dataservice_preview_action.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_missing_required_header_preview_payload.json",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_valid_buy_preview_payload.json",
}
TASK_249_BRIDGE_CI_CLOSEOUT_SELF_CONSISTENCY_PATHS = {
    "docs/294_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_gate.md",
    "docs/295_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/manual_entry_replay_accountingengine_bridge_ci_closeout_gate.py",
}

#!/usr/bin/env python3

import argparse
import json
import subprocess
from pathlib import Path
from typing import Any


FAILURE_SCHEMA = "manual-entry-replay-negative-fixture-static-validator-failure/v1"
EXPECTED_NEGATIVE_FILES = {
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

EXPECTED_ISSUE_CODES = {
    "NEG_MRF001": "NEG_FIXTURE_MISSING_REQUIRED_FIELD",
    "NEG_MRF002": "NEG_FIXTURE_SCHEMA_VERSION_INVALID",
    "NEG_MRF003": "NEG_FIXTURE_RUNTIME_USE_TRUE",
    "NEG_MRF004": "NEG_FIXTURE_PRODUCTION_USE_TRUE",
    "NEG_MRF005": "NEG_FIXTURE_REPLAY_EXECUTED_TRUE",
    "NEG_MRF006": "NEG_FIXTURE_SYNTHETIC_FLAG_FALSE",
    "NEG_MRF007": "NEG_FIXTURE_UNAUTHORIZED_FILE",
    "NEG_MRF008": "NEG_FIXTURE_RAW_SQL_TOKEN",
    "NEG_MRF009": "NEG_FIXTURE_BROKER_PAYLOAD_TOKEN",
    "NEG_MRF010": "NEG_FIXTURE_REAL_ORDER_TOKEN",
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
    "docs/308_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_readonly_vertical_slice.md",
    "docs/309_shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_test_plan.md",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_buy_only_export_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_buy_partial_sell_export_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_cash_adjustment_export_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_missing_required_header_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/fixtures/TASK256_unsupported_or_issue_sample.json",
    "tests/ShellAccountingManualEntryReplayExcelVbaExportSampleImportMatrixDiagnosticsReadOnlyVerticalSlice/t256_matrix_slice.cpp",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice.cpp",
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.py",
    "docs/298_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_gate.md",
    "docs/299_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/manual_entry_replay_accountingengine_runtime_integration_authorization_gate.py",
"docs/268_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_gate.md",
    "docs/269_shell_accounting_manual_entry_replay_test_only_implementation_failure_mode_hardening_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationFailureModeHardeningGate/manual_entry_replay_implementation_failure_mode_hardening_gate.py",
    "docs/270_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_gate.md",
    "docs/271_shell_accounting_manual_entry_replay_test_only_implementation_ci_closeout_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationCiCloseoutGate/manual_entry_replay_implementation_ci_closeout_gate.py",
    "docs/272_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_gate.md",
    "docs/273_shell_accounting_manual_entry_replay_test_only_implementation_phase_closeout_handoff_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationPhaseCloseoutGate/manual_entry_replay_implementation_phase_closeout_gate.py",
    "docs/274_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_gate.md",
    "docs/275_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "docs/276_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_gate.md",
    "docs/277_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "docs/266_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_gate.md",
    "docs/267_shell_accounting_manual_entry_replay_test_only_implementation_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationRegressionMatrixGate/manual_entry_replay_implementation_regression_matrix_gate.py",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
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
    "docs/256_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_gate.md",
    "docs/257_shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization_test_plan.md",
    "tests/CMakeLists.txt",
    "docs/258_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_gate.md",
    "docs/259_shell_accounting_manual_entry_replay_test_only_dry_run_harness_implementation_test_plan.md",
    "docs/260_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.md",
    "docs/261_shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorFailureModeHardeningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidatorRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterAuthorizationGate/manual_entry_qml_presenter_authorization_gate.py",
    "tests/ShellAccountingManualEntryQmlPresenterImplementation/manual_entry_qml_presenter_implementation.py",
    "tests/ShellAccountingManualEntryReadbackDailyUseAcceptanceAuthorizationGate/manual_entry_readback_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackMappingAuthorizationGate/manual_entry_readback_mapping_authorization_gate.py",
    "tests/ShellAccountingManualEntryReadbackReplayAdequacyReviewGate/manual_entry_readback_replay_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayAuditLedgerAdequacyReviewGate/manual_entry_replay_audit_ledger_adequacy_review_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureMatrixAuthorizationGate/manual_entry_replay_fixture_matrix_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureStaticValidator/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorCiCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNegativeFixtureValidatorPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayNextPhaseAuthorizationPlanningGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayPolicyAuthorizationGate/manual_entry_replay_policy_authorization_gate.py",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "docs/262_shell_accounting_manual_entry_replay_implementation_authorization_gate.md",
    "docs/263_shell_accounting_manual_entry_replay_implementation_authorization_test_plan.md",
    "docs/264_shell_accounting_manual_entry_replay_test_only_implementation_gate.md",
    "docs/265_shell_accounting_manual_entry_replay_test_only_implementation_test_plan.md",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementationAuthorizationGate/manual_entry_replay_implementation_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayImplementation/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py",
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


    "docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md",

    "docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/CMakeLists.txt",

    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",    "docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md",
    "docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
}
ALLOWED_CHANGED_PATHS.update(TASK_257_EXACT_PATHS)

ALLOWED_CHANGED_PATHS.update(
    {
        "docs/280_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.md",
        "docs/281_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
        "docs/282_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.md",
        "docs/283_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    }
)

FORBIDDEN_STORAGE_SUFFIXES = {".db", ".sqlite", ".sqlite3", ".wal", ".shm", ".sql"}
FORBIDDEN_PATH_TOKENS = {"apps/", "libs/", "migrations/", "DataServiceActions", "ETFDecisionShell"}
FORBIDDEN_UNSANITIZED_TOKENS = {
    "select *",
    "insert into",
    "update ",
    "delete from",
    "drop table",
    "access token",
    "secret key",
    "private key",
    "password",
    "credential=",
    "endpoint=",
    "broker payload:",
    "real order id:",
}
ALLOWED_PRIVACY = {"SYNTHETIC_ONLY", "NO_RAW_PAYLOAD"}


class ValidationError(Exception):
    def __init__(self, issue_code: str, message: str) -> None:
        super().__init__(message)
        self.issue_code = issue_code
        self.message = message


class Validator:
    def __init__(self) -> None:
        self.checks = 0
        self.summaries: list[dict[str, Any]] = []

    def require(self, condition: bool, issue_code: str, message: str) -> None:
        self.checks += 1
        if not condition:
            raise ValidationError(issue_code, message)

    def add_summary(self, fixture: dict[str, Any]) -> None:
        fixture_id = fixture["negativeFixtureId"]
        self.summaries.append(
            {
                "negativeFixtureId": fixture_id,
                "file": expected_file_for(fixture_id),
                "issueCode": fixture["expectedIssueCode"],
                "severity": fixture["expectedSeverity"],
                "blocking": fixture["expectedBlocking"],
                "rule": fixture["targetValidatorRule"],
                "message": f"sanitized expected issue for {fixture_id}",
            }
        )


def expected_file_for(fixture_id: str) -> str:
    for name in EXPECTED_NEGATIVE_FILES:
        if name.startswith(fixture_id):
            return name
    raise ValidationError("NEG_FIXTURE_ID_MISMATCH", f"unknown fixture id {fixture_id}")


def read_json(path: Path) -> Any:
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        raise ValidationError("NEG_FIXTURE_JSON_INVALID", path.name) from exc


def git_lines(root: Path, *args: str) -> set[str]:
    completed = subprocess.run(["git", *args], cwd=root, check=True, capture_output=True, text=True)
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def changed_paths(root: Path) -> set[str]:
    return (
        git_lines(root, "diff", "--name-only", "main")
        | git_lines(root, "diff", "--cached", "--name-only")
        | git_lines(root, "ls-files", "--others", "--exclude-standard")
    )


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

def validate_changed_paths(validator: Validator, root: Path) -> None:
    changes = changed_paths(root)
    task245_allowed_paths = {
        "docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md",
        "docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/CMakeLists.txt",
        "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    }
    ALLOWED_CHANGED_PATHS.update(task245_allowed_paths)
    unexpected = sorted(changes - ALLOWED_CHANGED_PATHS)
    validator.require(not unexpected, "NEG_FIXTURE_UNAUTHORIZED_PATH", "unauthorized changed path")
    validator.require(
        not any(path.startswith("tests/fixtures/manual_entry_replay_negative/") for path in changes),
        "NEG_FIXTURE_UNAUTHORIZED_FILE",
        "negative fixture JSON changed",
    )
    validator.require(
        not any(path.startswith("tests/fixtures/manual_entry_replay/") for path in changes),
        "NEG_FIXTURE_UNAUTHORIZED_FILE",
        "positive fixture JSON changed",
    )
    validator.require(
        not any(
            path.startswith(("apps/", "libs/", "migrations/"))
            and path not in TASK_257_PARSER_BOUNDARY_PATHS
            and path not in TASK_258_READONLY_PREVIEW_ACTION_PATHS
            for path in changes
        ),
        "NEG_FIXTURE_PRODUCTION_PATH_REFERENCE",
        "production path changed",
    )


def validate_no_forbidden_text(validator: Validator, text: str, context: str) -> None:
    lowered = text.lower()
    for path in TASK_257_PARSER_BOUNDARY_PATHS:
        lowered = lowered.replace(path.lower(), "")
        lowered = lowered.replace(path.replace("/", "\\").lower(), "")
    for token in FORBIDDEN_UNSANITIZED_TOKENS:
        validator.require(token not in lowered, "NEG_FIXTURE_FORBIDDEN_TOKEN", f"{context} forbidden token")
    for token in FORBIDDEN_PATH_TOKENS:
        validator.require(token.lower() not in lowered, "NEG_FIXTURE_PRODUCTION_PATH_REFERENCE", context)


def validate_negative_directory(validator: Validator, negative_dir: Path) -> None:
    validator.require(negative_dir.exists(), "NEG_FIXTURE_DIR_MISSING", "negative fixture directory exists")
    validator.require(negative_dir.is_dir(), "NEG_FIXTURE_DIR_INVALID", "negative fixture directory is dir")
    actual_files = {child.name for child in negative_dir.iterdir() if child.is_file()}
    validator.require(actual_files == EXPECTED_NEGATIVE_FILES, "NEG_FIXTURE_UNAUTHORIZED_FILE", "exact file set")
    validator.require(len([name for name in actual_files if name.startswith("NEG_MRF")]) == 10,
                      "NEG_FIXTURE_UNAUTHORIZED_FILE",
                      "ten NEG_MRF files")
    for child in negative_dir.iterdir():
        validator.require(child.suffix.lower() not in FORBIDDEN_STORAGE_SUFFIXES,
                          "NEG_FIXTURE_DB_ARTIFACT_PRESENT",
                          child.name)


def positive_fixture_ids(validator: Validator, positive_index_path: Path) -> set[str]:
    index = read_json(positive_index_path)
    validator.require(isinstance(index, dict), "NEG_FIXTURE_SOURCE_INDEX_INVALID", "positive index object")
    rows = index.get("fixtures")
    validator.require(isinstance(rows, list), "NEG_FIXTURE_SOURCE_INDEX_INVALID", "positive index fixtures")
    ids = {row.get("fixtureId") for row in rows if isinstance(row, dict)}
    validator.require(ids >= {"MRF001", "MRF002", "MRF003", "MRF004", "MRF005", "MRF006"},
                      "NEG_FIXTURE_SOURCE_INDEX_INVALID",
                      "positive fixture ids")
    return {fixture_id for fixture_id in ids if isinstance(fixture_id, str)}


def validate_index(validator: Validator, negative_dir: Path, positive_ids: set[str]) -> dict[str, dict[str, Any]]:
    index_path = negative_dir / "negative_fixtures_index.json"
    index_text = index_path.read_text(encoding="utf-8")
    validate_no_forbidden_text(validator, index_text, index_path.name)
    index = read_json(index_path)
    validator.require(isinstance(index, dict), "NEG_FIXTURE_INDEX_INVALID", "negative index object")
    validator.require(index.get("schemaVersion") == "manual-entry-replay-negative-fixtures-index/v1",
                      "NEG_FIXTURE_SCHEMA_VERSION_INVALID",
                      "index schema")
    for field, expected in [
        ("scaffoldOnly", True),
        ("runtimeUse", False),
        ("productionUse", False),
        ("replayExecuted", False),
        ("containsSyntheticDataOnly", True),
    ]:
        validator.require(index.get(field) is expected, "NEG_FIXTURE_FLAG_INVALID", f"index {field}")
    rows = index.get("fixtures")
    validator.require(isinstance(rows, list), "NEG_FIXTURE_INDEX_INVALID", "index fixtures")
    validator.require(len(rows) == 10, "NEG_FIXTURE_INDEX_INVALID", "index fixture count")
    seen: set[str] = set()
    by_id: dict[str, dict[str, Any]] = {}
    for row in rows:
        validator.require(isinstance(row, dict), "NEG_FIXTURE_INDEX_INVALID", "index row")
        fixture_id = row.get("negativeFixtureId")
        validator.require(isinstance(fixture_id, str), "NEG_FIXTURE_ID_MISMATCH", "index fixture id")
        validator.require(fixture_id not in seen, "NEG_FIXTURE_DUPLICATE_ID", fixture_id)
        seen.add(fixture_id)
        validator.require(fixture_id in EXPECTED_ISSUE_CODES, "NEG_FIXTURE_ID_MISMATCH", fixture_id)
        validator.require(row.get("file") == expected_file_for(fixture_id), "NEG_FIXTURE_ID_MISMATCH", fixture_id)
        validator.require(row.get("expectedIssueCode") == EXPECTED_ISSUE_CODES[fixture_id],
                          "NEG_FIXTURE_EXPECTED_ISSUE_MISMATCH",
                          fixture_id)
        validator.require(row.get("sourcePositiveFixtureId") in positive_ids,
                          "NEG_FIXTURE_SOURCE_POSITIVE_MISSING",
                          fixture_id)
        for field, expected in [
            ("scaffoldOnly", True),
            ("runtimeUse", False),
            ("productionUse", False),
            ("replayExecuted", False),
            ("containsSyntheticDataOnly", True),
        ]:
            validator.require(row.get(field) is expected, "NEG_FIXTURE_FLAG_INVALID", f"{fixture_id}.{field}")
        by_id[fixture_id] = row
    validator.require(seen == set(EXPECTED_ISSUE_CODES), "NEG_FIXTURE_INDEX_INVALID", "all expected ids")
    return by_id


def validate_fixture(validator: Validator, path: Path, index_row: dict[str, Any], positive_ids: set[str]) -> None:
    text = path.read_text(encoding="utf-8")
    validate_no_forbidden_text(validator, text, path.name)
    fixture = read_json(path)
    validator.require(isinstance(fixture, dict), "NEG_FIXTURE_INVALID", path.name)
    fixture_id = fixture.get("negativeFixtureId")
    validator.require(fixture_id == index_row["negativeFixtureId"], "NEG_FIXTURE_ID_MISMATCH", path.name)
    validator.require(path.name == index_row["file"], "NEG_FIXTURE_ID_MISMATCH", path.name)
    validator.require(path.name.startswith(fixture_id), "NEG_FIXTURE_ID_MISMATCH", path.name)
    validator.require(fixture.get("schemaVersion") == "manual-entry-replay-negative-fixture-scaffold/v1",
                      "NEG_FIXTURE_SCHEMA_VERSION_INVALID",
                      fixture_id)
    for field in ["title", "purpose", "mutationCategory", "targetValidatorRule", "mutationDescription"]:
        validator.require(isinstance(fixture.get(field), str) and fixture[field].strip(),
                          "NEG_FIXTURE_MISSING_REQUIRED_FIELD",
                          f"{fixture_id}.{field}")
    validator.require(fixture.get("sourcePositiveFixtureId") in positive_ids,
                      "NEG_FIXTURE_SOURCE_POSITIVE_MISSING",
                      fixture_id)
    validator.require(fixture.get("sourcePositiveFixtureId") == index_row["sourcePositiveFixtureId"],
                      "NEG_FIXTURE_SOURCE_POSITIVE_MISMATCH",
                      fixture_id)
    validator.require(fixture.get("expectedIssueCode") == EXPECTED_ISSUE_CODES[fixture_id],
                      "NEG_FIXTURE_EXPECTED_ISSUE_MISMATCH",
                      fixture_id)
    validator.require(fixture.get("expectedIssueCode") == index_row["expectedIssueCode"],
                      "NEG_FIXTURE_EXPECTED_ISSUE_MISMATCH",
                      fixture_id)
    validator.require(fixture.get("expectedSeverity") == "error", "NEG_FIXTURE_SEVERITY_INVALID", fixture_id)
    validator.require(fixture.get("expectedBlocking") is True, "NEG_FIXTURE_BLOCKING_INVALID", fixture_id)
    for field, expected in [
        ("scaffoldOnly", True),
        ("runtimeUse", False),
        ("productionUse", False),
        ("replayExecuted", False),
        ("containsSyntheticDataOnly", True),
    ]:
        validator.require(fixture.get(field) is expected, "NEG_FIXTURE_FLAG_INVALID", f"{fixture_id}.{field}")
    privacy = fixture.get("privacyExpectations")
    validator.require(isinstance(privacy, list), "NEG_FIXTURE_PRIVACY_INVALID", fixture_id)
    validator.require(set(privacy) == ALLOWED_PRIVACY, "NEG_FIXTURE_PRIVACY_INVALID", fixture_id)
    sanitized = fixture.get("sanitizationExpectations")
    validator.require(isinstance(sanitized, list) and sanitized, "NEG_FIXTURE_SANITIZATION_INVALID", fixture_id)
    for marker in sanitized:
        validator.require(isinstance(marker, str), "NEG_FIXTURE_SANITIZATION_INVALID", fixture_id)
        validator.require(marker.startswith("SANITIZED_") and marker.endswith("_PLACEHOLDER"),
                          "NEG_FIXTURE_FORBIDDEN_TOKEN",
                          fixture_id)
    metadata = fixture.get("metadata")
    validator.require(isinstance(metadata, dict), "NEG_FIXTURE_METADATA_INVALID", fixture_id)
    validator.require(metadata.get("task") == "TASK-222", "NEG_FIXTURE_METADATA_INVALID", fixture_id)
    validator.require(metadata.get("notReplayInput") is True, "NEG_FIXTURE_REPLAY_EXECUTED_TRUE", fixture_id)
    validator.require(metadata.get("notProductionInput") is True, "NEG_FIXTURE_PRODUCTION_USE_TRUE", fixture_id)
    validator.require(metadata.get("notRuntimeDataSource") is True, "NEG_FIXTURE_RUNTIME_USE_TRUE", fixture_id)
    validator.add_summary(fixture)


def summary_payload(validator: Validator) -> dict[str, Any]:
    return {
        "schemaVersion": "manual-entry-replay-negative-fixture-static-validator-summary/v1",
        "issueCount": len(validator.summaries),
        "issues": sorted(validator.summaries, key=lambda issue: issue["negativeFixtureId"]),
    }


def safe_failure_file(message: str) -> str:
    candidate = Path(str(message).replace("\\", "/")).name
    if candidate.endswith(".json"):
        return candidate
    return "validator-input"


def failure_payload(issue_code: str, message: str) -> dict[str, Any]:
    return {
        "schemaVersion": FAILURE_SCHEMA,
        "ok": False,
        "failureCount": 1,
        "failures": [
            {
                "issueCode": issue_code,
                "rule": "validator-fail-closed",
                "file": safe_failure_file(message),
                "message": "sanitized failure",
            }
        ],
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    parser.add_argument("--negative-fixtures-dir")
    parser.add_argument("--positive-fixtures-index")
    parser.add_argument("--summary-json", action="store_true")
    args = parser.parse_args()
    root = Path(args.source_root)
    validator = Validator()
    negative_dir = (
        Path(args.negative_fixtures_dir)
        if args.negative_fixtures_dir
        else root / "tests" / "fixtures" / "manual_entry_replay_negative"
    )
    positive_index = (
        Path(args.positive_fixtures_index)
        if args.positive_fixtures_index
        else root / "tests" / "fixtures" / "manual_entry_replay" / "fixtures_index.json"
    )

    validate_changed_paths(validator, root)
    validate_negative_directory(validator, negative_dir)
    positive_ids = positive_fixture_ids(validator, positive_index)
    index_rows = validate_index(validator, negative_dir, positive_ids)
    for fixture_id in sorted(EXPECTED_ISSUE_CODES):
        validate_fixture(validator, negative_dir / expected_file_for(fixture_id), index_rows[fixture_id], positive_ids)

    validator.require(len(validator.summaries) == 10, "NEG_FIXTURE_SUMMARY_INVALID", "summary count")
    validator.require(validator.checks >= 260, "NEG_FIXTURE_CHECK_COUNT_LOW", str(validator.checks))
    if args.summary_json:
        print(json.dumps(summary_payload(validator), sort_keys=True))
    else:
        print(json.dumps({"status": "passed", "checks": validator.checks, "issues": validator.summaries}, sort_keys=True))
    return 0



TASK_247_SELF_CONSISTENCY_PATHS = {
    "docs/290_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_gate.md",
    "docs/291_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_test_plan.md",
    "tests/ShellAccountingManualEntryReplayAccounting"
    "EngineBridgeRegressionMatrixGate/CMakeLists.txt",
    "tests/ShellAccountingManualEntryReplayAccounting"
    "EngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
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

TASK_257_PARSER_BOUNDARY_PATHS = {
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/include/DataServiceApi/ShellAccountingExcelVbaImportReadOnlyParser.h",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportReadOnlyParser.cpp",
}

TASK_258_READONLY_PREVIEW_ACTION_PATHS = {
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportReadOnlyPreviewAction.cpp",
}

ALLOWED_CHANGED_PATHS.update({

    "docs/296_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_gate.md",

    "docs/297_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_test_plan.md",

    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/CMakeLists.txt",

    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/manual_entry_replay_accountingengine_bridge_phase_closeout_gate.py",

})
if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except ValidationError as exc:
        print(json.dumps(failure_payload(exc.issue_code, exc.message), sort_keys=True))
        raise SystemExit(1)
    except Exception:
        print(json.dumps(failure_payload("VALIDATOR_CONTRACT_VIOLATION", "validator-input"), sort_keys=True))
        raise SystemExit(1)
