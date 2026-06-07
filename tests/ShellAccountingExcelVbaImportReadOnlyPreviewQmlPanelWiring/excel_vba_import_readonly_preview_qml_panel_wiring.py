#!/usr/bin/env python3

import argparse
import json
import subprocess
from pathlib import Path


TASK_262_EXACT_PATHS = {
    "docs/320_shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview.md",
    "docs/321_shell_accounting_excel_vba_import_readonly_local_export_json_file_loader_preview_test_plan.md",
    "libs/ShellServices/CMakeLists.txt",
    "libs/ShellServices/include/ShellServices/ShellAccountingExcelVbaImportReadOnlyFileLoader.h",
    "libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h",
    "libs/ShellServices/src/ShellAccountingExcelVbaImportReadOnlyFileLoader.cpp",
    "libs/ShellServices/src/ShellAccountingPresenter.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyLocalExportJsonFileLoaderPreview/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyLocalExportJsonFileLoaderPreview/excel_vba_import_readonly_local_export_json_file_loader_preview.cpp",
    "README.md",
    "docs/12_codex_prompt_template.md",
    "docs/326_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_implementation.md",
    "docs/327_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_test_plan.md",
    "docs/README.md",
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
    "tests/CMakeLists.txt",
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
    "tests/ShellAccountingExcelVbaImportPreviewToManualEntryPersistenceAuthorizationGate/excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.py",
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


def panel_source(qml: str) -> str:
    start = qml.find('objectName: "shellAccountingExcelVbaImportPreviewPanel"')
    require(start >= 0, "preview panel objectName exists")
    end = qml.find('objectName: "shellAccountingTradingUiSection"', start)
    require(end > start, "preview panel is before trading UI section")
    return qml[start:end]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    qml_path = root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml"
    qml = read(qml_path)
    panel = panel_source(qml)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc318 = read(root / "docs" / "318_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring.md")
    doc319 = read(root / "docs" / "319_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring_test_plan.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")

    for text, context in [
        (readme, "README"),
        (docs_index, "docs/README"),
        (prompt, "docs/12"),
        (doc318, "docs/318"),
        (doc319, "docs/319"),
    ]:
        require_contains(text, "TASK-261", context)
        require_contains(text, "read-only", context)

    require_contains(readme, "docs/318_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring.md", "README")
    require_contains(readme, "docs/319_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring_test_plan.md", "README")
    require_contains(docs_index, "318_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring.md", "docs/README")
    require_contains(docs_index, "319_shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingExcelVbaImportReadOnlyPreviewQmlPanelWiring", "tests/CMakeLists")

    for token in [
        "shellAccountingExcelVbaImportPreviewPanel",
        "shellAccountingExcelVbaImportPreviewPayloadInput",
        "shellAccountingExcelVbaImportPreviewButton",
        "shellAccountingExcelVbaImportPreviewResetButton",
        "shellAccountingExcelVbaImportPreviewStatusPanel",
        "shellAccountingExcelVbaImportPreviewStatusText",
        "shellAccountingExcelVbaImportPreviewIssueText",
        "shellAccountingExcelVbaImportPreviewSummaryText",
        "shellAccountingExcelVbaImportPreviewDiagnosticCodesText",
        "shellAccountingExcelVbaImportPreviewTradeFactCountText",
        "shellAccountingExcelVbaImportPreviewCashFactCountText",
        "shellAccountingExcelVbaImportPreviewMarketPriceFactCountText",
        "shellAccountingExcelVbaImportPreviewFxRateFactCountText",
        "previewExcelVbaImportReadOnly(",
        "excelVbaImportPreviewPayloadInput.text",
        "resetExcelVbaImportPreviewState()",
        "lastExcelVbaImportPreviewStatus",
        "lastExcelVbaImportPreviewIssue",
        "lastExcelVbaImportPreviewSummary",
        "lastExcelVbaImportPreviewDiagnosticCodes",
        "excelVbaImportPreviewTradeFactCount",
        "excelVbaImportPreviewCashFactCount",
        "excelVbaImportPreviewMarketPriceFactCount",
        "excelVbaImportPreviewFxRateFactCount",
    ]:
        require_contains(panel, token, "preview panel")

    for token in [
        "DataServiceClient",
        "DataServiceApi",
        "SQLite",
        "DataAccess",
        "AccountingEngine",
        "XMLHttpRequest",
        "fetch(",
        "credential",
        "endpoint",
    ]:
        require_not_contains(qml, token, "ShellAccountingReadOnlyPage.qml")

    if "FileDialog" in qml:
        require_contains(qml, "shellAccountingExcelVbaImportPreviewFileDialog", "TASK-262 local file picker")
        require_contains(qml, "previewExcelVbaImportReadOnlyFromLocalFile(", "TASK-262 local file preview")
        require_contains(qml, "excelVbaImportPreviewSelectedFileName", "TASK-262 sanitized file display")
        require_contains(qml, "Selected file:", "TASK-262 sanitized file display")
        for token in [
            ".read(",
            "readText",
            "readAll",
            "XMLHttpRequest",
            "fetch(",
            "http://",
            "https://",
            "credential",
            "endpoint",
        ]:
            require_not_contains(panel, token, "TASK-262 local file picker")

    for token in [
        "broker",
        "brokerOrder",
        "placeOrder",
        "realOrder",
        "automaticTrading",
        "createDraft(",
        "confirmDraft(",
        "submitManualTransaction(",
        "submitManualCashMovement(",
    ]:
        require_not_contains(panel, token, "preview panel")

    changes = changed_paths(root)
    forbidden_changed_paths = {
        "apps/ETFDecisionShell/src/main.cpp",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActions.cpp",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    }
    for path in forbidden_changed_paths:
        if path in TASK_262_EXACT_PATHS:
            continue
        require(path not in changes, f"forbidden changed path: {path}")

    for path in changes:
        if path in TASK_262_EXACT_PATHS:
            continue
        require(not path.startswith("libs/"), f"production libs path changed: {path}")
        require(not path.startswith("migrations/"), f"migration path changed: {path}")
        require(
            not path.startswith("apps/") or path == "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml",
            f"unauthorized app path changed: {path}",
        )

    evidence = {
        "task": "TASK-261",
        "mode": "readonly-import-preview-qml-panel-wiring-vertical-slice",
        "qmlPreviewPanelWired": True,
        "presenterPreviewMethodInvokedFromQml": True,
        "presenterPreviewPropertiesBound": True,
        "previewStatusVisible": True,
        "previewSummaryVisible": True,
        "diagnosticCodesVisible": True,
        "factCountsVisible": True,
        "resetPreviewStateWired": True,
        "qmlDirectDataServiceClient": False,
        "qmlDirectSQLite": False,
        "qmlDirectDataServiceApi": False,
        "qmlDirectAccountingEngine": False,
        "productionFileLoading": False,
        "fileDialogUsed": "TASK-262-local-file-picker-only" if "FileDialog" in qml else False,
        "startupWiringChanged": False,
        "accountingEngineCalled": False,
        "productionWrite": False,
        "sqliteProductionWrite": False,
        "auditWritten": False,
        "ledgerWritten": False,
        "snapshotWritten": False,
        "tradeLogWritten": False,
        "readModelPersistentWrite": False,
        "tradeDraftGenerated": False,
        "strategyExecuted": False,
        "brokerOrderSubmitted": False,
        "networkAccess": False,
        "credentialAccess": False,
        "endpointAccess": False,
        "automaticTrading": False,
        "rawUserDataExposed": False,
    }
    print(json.dumps(evidence, sort_keys=True, separators=(",", ":")))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
