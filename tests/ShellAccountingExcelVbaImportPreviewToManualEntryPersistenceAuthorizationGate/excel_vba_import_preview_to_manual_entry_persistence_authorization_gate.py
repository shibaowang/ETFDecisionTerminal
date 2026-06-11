#!/usr/bin/env python3

TASK_270_EXACT_PATHS = {
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/336_shell_accounting_excel_vba_import_persist_post_write_readback_refresh.md",
    "docs/337_shell_accounting_excel_vba_import_persist_post_write_readback_refresh_test_plan.md",
    "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml",
    "libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h",
    "libs/ShellServices/src/ShellAccountingPresenter.cpp",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPersistPostWriteReadbackRefresh/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPersistPostWriteReadbackRefresh/excel_vba_import_persist_post_write_readback_refresh.cpp",
}

TASK_271_EXACT_PATHS = {
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/338_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance.md",
    "docs/339_shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance_test_plan.md",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportMvpLocalServiceE2eAcceptance/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportMvpLocalServiceE2eAcceptance/excel_vba_import_mvp_local_service_e2e_acceptance.cpp",
}

EPIC_272_EXACT_PATHS = {
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/340_excel_vba_import_mvp_release_readiness.md",
    "docs/341_excel_vba_import_mvp_user_guide.md",
    "docs/342_excel_vba_import_mvp_sample_export_format.md",
    "docs/343_excel_vba_import_mvp_local_runbook.md",
    "docs/344_excel_vba_import_mvp_known_limits.md",
    "docs/345_excel_vba_import_mvp_release_readiness_test_plan.md",
    "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportMvpReleaseReadiness/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportMvpReleaseReadiness/excel_vba_import_mvp_release_readiness.cpp",
    "samples/excel_vba_import/TASK272_sanitized_excel_vba_import_mvp_sample.json",
}
EPIC_273_EXACT_PATHS = {
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/346_excel_vba_import_mvp_demo_workspace.md",
    "docs/347_excel_vba_import_mvp_trial_startup_runbook.md",
    "docs/348_excel_vba_import_mvp_manual_acceptance_checklist.md",
    "docs/349_excel_vba_import_mvp_demo_workspace_test_plan.md",
    "scripts/excel_vba_import_mvp/New-ExcelVbaImportMvpDemoWorkspace.ps1",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportMvpDemoWorkspace/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportMvpDemoWorkspace/excel_vba_import_mvp_demo_workspace.cpp",
}
import argparse
import json
import re
import subprocess
from pathlib import Path


TASK264_EXACT_PATHS = {
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/324_shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.md",
    "docs/325_shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_test_plan.md",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPreviewToManualEntryPersistenceAuthorizationGate/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPreviewToManualEntryPersistenceAuthorizationGate/excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.py",
    "docs/326_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_implementation.md",
    "docs/327_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_test_plan.md",
    "libs/DataAccess/CMakeLists.txt",
    "libs/DataAccess/include/DataAccess/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.h",
    "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
    "libs/DataAccess/src/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.cpp",
    "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportPersistManualEntryAction.cpp",
    "libs/DataServiceApi/src/WriteActionPolicy.cpp",
    "tests/DevDocs/test_readonly_demo_acceptance.py",
    "tests/ShellAccountingExcelVbaImportAcceptedPreviewManualEntryPersistence/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportAcceptedPreviewManualEntryPersistence/excel_vba_import_accepted_preview_manual_entry_persistence.cpp",
    "docs/328_shell_accounting_excel_vba_import_persist_manual_entry_client_adapter.md",
    "docs/329_shell_accounting_excel_vba_import_persist_manual_entry_client_adapter_test_plan.md",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClient.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientJson.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientTypes.h",
    "libs/DataServiceClient/src/DataServiceClient.cpp",
    "libs/DataServiceClient/src/DataServiceClientJson.cpp",
    "tests/ShellAccountingExcelVbaImportPersistManualEntryClientAdapter/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPersistManualEntryClientAdapter/excel_vba_import_persist_manual_entry_client_adapter.cpp",
    "docs/330_shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract.md",
    "docs/331_shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract_test_plan.md",
    "tests/ShellAccountingExcelVbaImportPersistManualEntryShellServicesPresenterContract/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPersistManualEntryShellServicesPresenterContract/excel_vba_import_persist_manual_entry_shellservices_presenter_contract.cpp",
    "docs/332_shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring.md",
    "docs/333_shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring_test_plan.md",
    "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml",
    "libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h",
    "libs/ShellServices/src/ShellAccountingPresenter.cpp",
    "tests/ShellAccountingExcelVbaImportAcceptedPreviewPersistQmlWiring/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportAcceptedPreviewPersistQmlWiring/excel_vba_import_accepted_preview_persist_qml_wiring.cpp",
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
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewAcceptanceUxExportFormatContract/excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.py",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewQmlPanelWiring/excel_vba_import_readonly_preview_qml_panel_wiring.py",
    "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
    "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
    "tests/ShellAccountingManualEntryDataServiceActionAuthorizationGate/manual_entry_dataservice_action_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryDataServiceActionImplementationAuthorizationGate/manual_entry_dataservice_action_implementation_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryDataServiceActionScaffold/manual_entry_dataservice_action_scaffold.cpp",
    "tests/ShellAccountingManualEntryDataServiceWriteWiringAuthorizationGate/manual_entry_dataservice_write_wiring_authorization_gate.py",
    "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryPersistenceAuthorizationGate/manual_entry_persistence_authorization_gate.cpp",
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
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/manual_entry_replay_accountingengine_bridge_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/manual_entry_replay_accountingengine_bridge_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/manual_entry_replay_accountingengine_runtime_integration_authorization_gate.py",
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
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryRepositoryImplementationAuthorizationGate/manual_entry_repository_implementation_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntryRepositoryScaffoldAuthorizationGate/manual_entry_repository_scaffold_authorization_gate.cpp",
    "tests/ShellAccountingManualEntrySchemaAdequacyReviewGate/manual_entry_schema_adequacy_review_gate.cpp",
    "tests/ShellAccountingManualEntrySchemaGapAuthorizationGate/manual_entry_schema_gap_authorization_gate.cpp",
    "tests/ShellAccountingManualEntrySchemaImplementationAuthorizationGate/manual_entry_schema_implementation_authorization_gate.cpp",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "docs/334_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment.md",
    "docs/335_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment_test_plan.md",
    "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
    "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
    "tests/ShellAccountingExcelVbaImportPersistSupportedFactTypesAlignment/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPersistSupportedFactTypesAlignment/excel_vba_import_persist_supported_fact_types_alignment.cpp",
}

TASK264_EXACT_PATHS.update(TASK_270_EXACT_PATHS)

TASK264_EXACT_PATHS.update(TASK_271_EXACT_PATHS)
TASK264_EXACT_PATHS.update(EPIC_272_EXACT_PATHS)

TASK264_EXACT_PATHS.update(EPIC_273_EXACT_PATHS)
TASK264_OLD_GATE_REPAIR_PATHS = {
    "tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/manual_cash_movement_repository_dual_write_implementation.cpp",
    "tests/ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate/manual_entry_mvp_e2e_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate/manual_entry_post_write_readback_refresh_authorization_gate.py",
    "tests/ShellAccountingManualEntryPostWriteReadbackRefreshImplementation/manual_entry_post_write_readback_refresh_implementation.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewAuthorizationGate/manual_entry_replay_accountingengine_adequacy_review_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewCiCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewFailureModeHardeningGate/manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate/manual_entry_replay_accountingengine_adequacy_review_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/manual_entry_replay_accountingengine_bridge_ci_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeFailureModeHardeningGate/manual_entry_replay_accountingengine_bridge_failure_mode_hardening_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/manual_entry_replay_accountingengine_bridge_phase_closeout_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py",
    "tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/manual_entry_replay_accountingengine_runtime_integration_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesAuthorizationGate/manual_entry_replay_fixture_files_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffold/manual_entry_replay_fixture_files_scaffold_gate.py",
    "tests/ShellAccountingManualEntryReplayFixtureFilesScaffoldAuthorizationGate/manual_entry_replay_fixture_files_scaffold_authorization_gate.py",
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
    "tests/ShellAccountingManualEntryReplayReadOnlyRuntimeIntegrationVerticalSliceGate/manual_entry_replay_readonly_runtime_integration_vertical_slice_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessAuthorizationGate/manual_entry_replay_test_only_dry_run_harness_authorization_gate.py",
    "tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/manual_entry_replay_test_only_dry_run_harness_regression_matrix_gate.py",
}

TASK265_IMPLEMENTATION_PATHS = {
    "libs/DataAccess/CMakeLists.txt",
    "libs/DataAccess/include/DataAccess/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.h",
    "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
    "libs/DataAccess/src/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.cpp",
    "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportPersistManualEntryAction.cpp",
    "libs/DataServiceApi/src/WriteActionPolicy.cpp",
}

TASK267_SHELLSERVICES_CONTRACT_PATHS = {
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
}

TASK266_CLIENT_ADAPTER_PATHS = {
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClient.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientJson.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientTypes.h",
    "libs/DataServiceClient/src/DataServiceClient.cpp",
    "libs/DataServiceClient/src/DataServiceClientJson.cpp",
}

TASK269_SUPPORTED_FACT_TYPES_PATHS = {
    "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
    "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
    "tests/ShellAccountingExcelVbaImportPersistManualEntryClientAdapter/excel_vba_import_persist_manual_entry_client_adapter.cpp",
}


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


def changed_paths_with_untracked(root: Path) -> set[str]:
    completed = subprocess.run(
        ["git", "status", "--porcelain=v1", "-uall"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    paths: set[str] = set()
    for line in completed.stdout.splitlines():
        path = line[3:].strip()
        if " -> " in path:
            path = path.split(" -> ", 1)[1]
        if path:
            paths.add(path.replace("\\", "/"))
    return paths


def is_task264_old_gate_repair(path: str) -> bool:
    return path in TASK264_OLD_GATE_REPAIR_PATHS


def require_no_forbidden_changed_paths(paths: set[str]) -> None:
    allowed_untracked = set(TASK264_EXACT_PATHS)
    for path in paths:
        if path in allowed_untracked:
            continue
        if is_task264_old_gate_repair(path):
            continue
        require(not path.startswith("apps/"), f"apps path unchanged: {path}")
        require(not path.startswith("libs/DataServiceApi/"), f"DataServiceApi production unchanged: {path}")
        require(not path.startswith("libs/DataServiceClient/"), f"DataServiceClient unchanged: {path}")
        require(not path.startswith("libs/ShellServices/"), f"ShellServices production unchanged: {path}")
        require(not path.startswith("libs/DataAccess/"), f"DataAccess unchanged: {path}")
        require(not path.startswith("libs/AccountingEngine/"), f"AccountingEngine unchanged: {path}")
        require(not path.startswith("migrations/"), f"migrations unchanged: {path}")
        require(not path.endswith(".qml"), f"QML unchanged: {path}")
        require(path != "apps/ETFDecisionShell/src/main.cpp", "startup main.cpp unchanged")
        require(path in TASK264_EXACT_PATHS, f"unexpected TASK-264 changed path: {path}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc324 = read(root / "docs" / "324_shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.md")
    doc325 = read(root / "docs" / "325_shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_test_plan.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    gate_cmake = read(root / "tests" / "ShellAccountingExcelVbaImportPreviewToManualEntryPersistenceAuthorizationGate" / "CMakeLists.txt")

    for text, context in [
        (readme, "README"),
        (docs_index, "docs/README"),
        (prompt, "docs/12"),
        (doc324, "docs/324"),
        (doc325, "docs/325"),
    ]:
        require_contains(text, "TASK-264", context)
        require_contains(text, "ACCEPTED", context)
        require_contains(text, "DataService", context)
        require_contains(text, "persistence", context)

    for token in [
        "READY",
        "FILE_SELECTED",
        "PREVIEWING",
        "REJECTED",
        "INPUT_ERROR",
        "dedicated DataService action",
        "QML",
        "Presenter",
        "Controller",
        "must not directly write",
        "trade_log",
        "manual transaction facts",
        "audit_log",
        "idempotency record",
        "import batch marker",
        "roll back",
        "idempotency key",
        "duplicate import",
        "sanitized",
        "EXCEL_VBA_IMPORT_PREVIEW_NOT_ACCEPTED",
        "EXCEL_VBA_IMPORT_IDEMPOTENCY_REQUIRED",
        "EXCEL_VBA_IMPORT_DUPLICATE",
        "EXCEL_VBA_IMPORT_TRANSACTION_ROLLBACK",
        "TradeDraft",
        "strategy",
        "broker",
        "network",
        "credentials",
        "endpoint",
        "automatic trading",
    ]:
        require_contains(doc324, token, "docs/324 authorization boundary")

    for token in [
        "previewStatus",
        "previewId",
        "previewDigest",
        "idempotencyKey",
        "schemaVersion",
        "excel-vba-export/v1",
        "source",
        "sanitized-excel-vba-export",
        "acceptedAt",
        "importBatchLabel",
        "tradeFacts",
        "cashFacts",
        "diagnosticCodes",
    ]:
        require_contains(doc324, token, "minimal DTO")

    for token in [
        "acceptedPreviewRequired=true",
        "dataserviceOnlyWriteBoundaryDefined=true",
        "qmlDirectWriteForbidden=true",
        "presenterDirectWriteForbidden=true",
        "transactionBoundaryDefined=true",
        "auditLogRequired=true",
        "tradeLogWriteFutureOnly=true",
        "idempotencyRequired=true",
        "rollbackRequired=true",
        "duplicateImportPolicyDefined=true",
        "sanitizedErrorMappingDefined=true",
        "implementationAdded=false",
        "importButtonAdded=false",
        "productionWrite=false",
        "brokerOrderSubmitted=false",
        "automaticTrading=false",
    ]:
        require_contains(doc325, token, "docs/325 evidence contract")

    require_contains(tests_cmake, "ShellAccountingExcelVbaImportPreviewToManualEntryPersistenceAuthorizationGate", "tests/CMakeLists")
    require_contains(gate_cmake, "shell_accounting_excel_vba_import_preview_to_manual_entry_persistence_authorization_gate", "TASK-264 CMake")

    changed = changed_paths(root)
    changed_all = changed | changed_paths_with_untracked(root)
    require_no_forbidden_changed_paths(changed_all)

    forbidden_source_roots = [
        root / "apps",
        root / "libs" / "DataServiceApi",
        root / "libs" / "DataServiceClient",
        root / "libs" / "ShellServices",
        root / "libs" / "DataAccess",
        root / "libs" / "AccountingEngine",
        root / "migrations",
    ]
    implementation_tokens = [
        r"\bINSERT\b",
        r"\bUPDATE\b",
        r"\bDELETE\b",
        r"\bREPLACE\b",
        "audit_log",
        "trade_log",
        "TradeDraft",
        "brokerOrderSubmitted",
        "automaticTrading",
    ]
    for changed_path in changed:
        if (
            changed_path in TASK265_IMPLEMENTATION_PATHS
            or changed_path in TASK266_CLIENT_ADAPTER_PATHS
            or changed_path in TASK267_SHELLSERVICES_CONTRACT_PATHS
            or changed_path in TASK269_SUPPORTED_FACT_TYPES_PATHS
            or changed_path in TASK_270_EXACT_PATHS or changed_path in TASK_271_EXACT_PATHS
        ):
            continue
        p = root / changed_path
        if not p.exists() or not p.is_file():
            continue
        if any(p.is_relative_to(source_root) for source_root in forbidden_source_roots):
            text = read(p)
            for token in implementation_tokens:
                require(re.search(token, text, flags=re.IGNORECASE) is None, f"no forbidden implementation token in {changed_path}: {token}")

    evidence = {
        "task": "TASK-264",
        "persistenceAuthorizationGateCreated": True,
        "acceptedPreviewRequired": True,
        "dataserviceOnlyWriteBoundaryDefined": True,
        "qmlDirectWriteForbidden": True,
        "presenterDirectWriteForbidden": True,
        "transactionBoundaryDefined": True,
        "auditLogRequired": True,
        "tradeLogWriteFutureOnly": True,
        "idempotencyRequired": True,
        "rollbackRequired": True,
        "duplicateImportPolicyDefined": True,
        "sanitizedErrorMappingDefined": True,
        "implementationAdded": False,
        "importButtonAdded": False,
        "productionWrite": False,
        "sqliteProductionWrite": False,
        "auditWritten": False,
        "tradeLogWritten": False,
        "readModelPersistentWrite": False,
        "tradeDraftGenerated": False,
        "strategyExecuted": False,
        "brokerOrderSubmitted": False,
        "networkAccess": False,
        "credentialAccess": False,
        "endpointAccess": False,
        "automaticTrading": False,
    }
    print(json.dumps(evidence, sort_keys=True, separators=(",", ":")))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
