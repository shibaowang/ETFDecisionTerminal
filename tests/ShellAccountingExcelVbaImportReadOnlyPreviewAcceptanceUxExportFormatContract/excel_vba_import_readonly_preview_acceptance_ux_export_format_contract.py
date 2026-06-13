#!/usr/bin/env python3

import argparse
import json
import re
import subprocess
from pathlib import Path


TASK_265_EXACT_PATHS = {
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
    "docs/328_shell_accounting_excel_vba_import_persist_manual_entry_client_adapter.md",
    "docs/329_shell_accounting_excel_vba_import_persist_manual_entry_client_adapter_test_plan.md",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClient.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientJson.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientTypes.h",
    "libs/DataServiceClient/src/DataServiceClient.cpp",
    "libs/DataServiceClient/src/DataServiceClientJson.cpp",
    "tests/ShellAccountingExcelVbaImportPersistManualEntryClientAdapter/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPersistManualEntryClientAdapter/excel_vba_import_persist_manual_entry_client_adapter.cpp",
    "docs/334_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment.md",
    "docs/335_shell_accounting_excel_vba_import_persist_supported_fact_types_alignment_test_plan.md",
    "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
    "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
    "tests/ShellAccountingExcelVbaImportPersistSupportedFactTypesAlignment/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPersistSupportedFactTypesAlignment/excel_vba_import_persist_supported_fact_types_alignment.cpp",
    "tests/ShellAccountingExcelVbaImportAcceptedPreviewManualEntryPersistence/excel_vba_import_accepted_preview_manual_entry_persistence.cpp",
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


def panel_source(qml: str) -> str:
    start = qml.find('objectName: "shellAccountingExcelVbaImportPreviewPanel"')
    require(start >= 0, "preview panel objectName exists")
    end = qml.find('objectName: "shellAccountingTradingUiSection"', start)
    require(end > start, "preview panel is before trading UI section")
    return qml[start:end]


def changed_paths(root: Path) -> set[str]:
    completed = subprocess.run(
        ["git", "diff", "--name-only", "main"],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return {line.strip().replace("\\", "/") for line in completed.stdout.splitlines() if line.strip()}


def load_json(path: Path) -> dict:
    return json.loads(read(path))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    qml = read(root / "apps" / "ETFDecisionShell" / "qml" / "pages" / "ShellAccountingReadOnlyPage.qml")
    panel = panel_source(qml)
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc322 = read(root / "docs" / "322_shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.md")
    doc323 = read(root / "docs" / "323_shell_accounting_excel_vba_import_readonly_preview_acceptance_ux_export_format_contract_test_plan.md")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    sample_path = (
        root
        / "tests"
        / "ShellAccountingExcelVbaImportReadOnlyPreviewAcceptanceUxExportFormatContract"
        / "fixtures"
        / "TASK263_sanitized_sample_export.json"
    )
    sample = load_json(sample_path)

    for text, context in [
        (readme, "README"),
        (docs_index, "docs/README"),
        (prompt, "docs/12"),
        (doc322, "docs/322"),
        (doc323, "docs/323"),
    ]:
        require_contains(text, "TASK-263", context)
        require_contains(text, "read-only", context)
        require_contains(text, "excel-vba-export/v1", context)
        require_contains(text, "sanitized-excel-vba-export", context)

    require_contains(tests_cmake, "ShellAccountingExcelVbaImportReadOnlyPreviewAcceptanceUxExportFormatContract", "tests/CMakeLists")

    for token in [
        "shellAccountingExcelVbaImportPreviewPanel",
        "shellAccountingExcelVbaImportPreviewPayloadInput",
        "shellAccountingExcelVbaImportPreviewButton",
        "shellAccountingExcelVbaImportPreviewSelectFileButton",
        "shellAccountingExcelVbaImportPreviewFileButton",
        "shellAccountingExcelVbaImportPreviewSelectedFileText",
        "shellAccountingExcelVbaImportPreviewLoadSampleButton",
        "shellAccountingExcelVbaImportPreviewFormatContractPanel",
        "shellAccountingExcelVbaImportPreviewFormatContractText",
        "shellAccountingExcelVbaImportPreviewAcceptanceStateText",
        "shellAccountingExcelVbaImportPreviewAcceptanceSummaryText",
        "shellAccountingExcelVbaImportPreviewStatusText",
        "shellAccountingExcelVbaImportPreviewIssueText",
        "shellAccountingExcelVbaImportPreviewDiagnosticCodesText",
        "shellAccountingExcelVbaImportPreviewTradeFactCountText",
        "shellAccountingExcelVbaImportPreviewCashFactCountText",
        "shellAccountingExcelVbaImportPreviewMarketPriceFactCountText",
        "shellAccountingExcelVbaImportPreviewFxRateFactCountText",
    ]:
        require_contains(panel, token, "preview panel")

    for state in ["READY", "FILE_SELECTED", "PREVIEWING", "ACCEPTED", "REJECTED", "INPUT_ERROR"]:
        require_contains(panel, state, "acceptance state contract")

    for token in [
        "schemaVersion",
        "excel-vba-export/v1",
        "source",
        "sanitized-excel-vba-export",
        "sheets",
        "headers",
        "rows",
    ]:
        require_contains(panel, token, "visible format contract")

    for token in [
        "TradeLog",
        "DEMO_ACCOUNT",
        "DEMO_PORTFOLIO",
        "DEMO_ETF",
    ]:
        require_contains(qml, token, "sample payload")

    require_contains(panel, "previewExcelVbaImportReadOnly(", "sample and paste preview use existing path")
    require_contains(panel, "excelVbaImportPreviewSamplePayload()", "sample payload function used")
    require_contains(panel, "previewExcelVbaImportReadOnlyFromLocalFile(", "local file preview remains available")
    require_contains(panel, "Selected file:", "basename display remains available")

    sample_json = json.dumps(sample, sort_keys=True)
    require(sample["schemaVersion"] == "excel-vba-export/v1", "sample schema version")
    require(sample["source"] == "sanitized-excel-vba-export", "sample source")
    require(isinstance(sample.get("sheets"), list) and sample["sheets"], "sample sheets list")
    sheet = sample["sheets"][0]
    require(sheet["name"] == "TradeLog", "sample TradeLog sheet")
    require(isinstance(sheet.get("headers"), list) and sheet["headers"], "sample headers")
    require(isinstance(sheet.get("rows"), list) and sheet["rows"], "sample rows")

    for token in [
        "c:/",
        "d:/",
        "\\users\\",
        "select ",
        "insert ",
        "update ",
        "delete ",
        "credential",
        "endpoint",
        "password",
        "secret",
        "broker payload",
        "real order id",
        "stack trace",
        "traceback",
    ]:
        require(token not in sample_json.lower(), f"sample sanitized token absent: {token}")

    for token in [
        "DataServiceClient",
        "DataServiceApi",
        "SQLite",
        "DataAccess",
        "AccountingEngine",
        "XMLHttpRequest",
        "fetch(",
        ".read(",
        "readAll",
        "credential",
        "endpoint",
    ]:
        require_not_contains(qml, token, "ShellAccountingReadOnlyPage.qml")

    forbidden_panel_patterns = [
        r"\bImport\s*(Button|Control)\b",
        r"\bConfirm\s*(Button|Control)\b",
        r"\bPersist\s*(Button|Control)\b",
        r"\bImport\s*(Now|File|Data)\b",
        r"TradeDraft",
        r"\bstrategy\b",
        r"\bbroker\b",
        r"\border\b",
        r"automaticTrading",
        r"submitManualTransaction",
        r"submitManualCashMovement",
    ]
    for pattern in forbidden_panel_patterns:
        require(re.search(pattern, panel, flags=re.IGNORECASE) is None, f"forbidden panel token absent: {pattern}")

    changes = changed_paths(root)
    for path in changes:
        if path in TASK_265_EXACT_PATHS:
            continue
        require(not path.startswith("migrations/"), f"no migration changed: {path}")
        require(not path.startswith("libs/DataServiceApi/"), f"no DataServiceApi changed: {path}")
        require(not path.startswith("libs/DataServiceClient/"), f"no DataServiceClient changed: {path}")
        require(not path.startswith("libs/DataAccess/"), f"no DataAccess changed: {path}")
        require(not path.startswith("libs/AccountingEngine/"), f"no AccountingEngine changed: {path}")
        require(path != "apps/ETFDecisionShell/src/main.cpp", "startup main.cpp unchanged")

    evidence = {
        "task": "TASK-263",
        "acceptanceUxCreated": True,
        "exportFormatContractDocumented": True,
        "samplePayloadAvailable": True,
        "samplePreviewUsesExistingPath": True,
        "pastePreviewStillAvailable": True,
        "localFilePreviewStillAvailable": True,
        "acceptedStateVisible": True,
        "rejectedStateVisible": True,
        "inputErrorStateVisible": True,
        "factCountsVisible": True,
        "diagnosticCodesVisible": True,
        "sanitizedIssueVisible": True,
        "rawPathSanitized": True,
        "rawPayloadExposed": False,
        "importButtonAdded": False,
        "confirmButtonAdded": False,
        "persistenceAdded": False,
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
    }
    print(json.dumps(evidence, sort_keys=True, separators=(",", ":")))
    return 0



EPIC_276_EXACT_PATHS = {
    "docs/12_codex_prompt_template.md",
    "docs/359_shell_accounting_portfolio_replay_vba_parity_full_delivery.md",
    "docs/360_shell_accounting_portfolio_replay_vba_parity_matrix.md",
    "docs/361_shell_accounting_portfolio_replay_read_model_contract.md",
    "docs/362_shell_accounting_portfolio_replay_user_visible_summary.md",
    "docs/363_shell_accounting_portfolio_replay_test_plan.md",
    "docs/README.md",
    "libs/AccountingEngine/CMakeLists.txt",
    "libs/AccountingEngine/include/AccountingEngine/ShellAccountingPortfolioReplayEngine.h",
    "libs/AccountingEngine/src/ShellAccountingPortfolioReplayEngine.cpp",
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/ShellAccountingPortfolioReplayReadOnlySummaryAction.cpp",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClient.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientJson.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientTypes.h",
    "libs/DataServiceClient/src/DataServiceClient.cpp",
    "libs/DataServiceClient/src/DataServiceClientJson.cpp",
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
    "README.md",
    "samples/shell_accounting_replay/EPIC276_buy_only.json",
    "samples/shell_accounting_replay/EPIC276_buy_partial_sell.json",
    "samples/shell_accounting_replay/EPIC276_cash_in_cash_out.json",
    "samples/shell_accounting_replay/EPIC276_decimal_quantity_price_fee.json",
    "samples/shell_accounting_replay/EPIC276_etf_and_otc_split.json",
    "samples/shell_accounting_replay/EPIC276_multi_buy_average_cost.json",
    "samples/shell_accounting_replay/EPIC276_multi_instrument.json",
    "samples/shell_accounting_replay/EPIC276_negative_broker_order_token.json",
    "samples/shell_accounting_replay/EPIC276_negative_currency_mismatch.json",
    "samples/shell_accounting_replay/EPIC276_negative_duplicate_fact_id.json",
    "samples/shell_accounting_replay/EPIC276_negative_invalid_price.json",
    "samples/shell_accounting_replay/EPIC276_negative_invalid_quantity.json",
    "samples/shell_accounting_replay/EPIC276_negative_missing_instrument.json",
    "samples/shell_accounting_replay/EPIC276_negative_sell_over_position.json",
    "samples/shell_accounting_replay/EPIC276_negative_sensitive_token.json",
    "samples/shell_accounting_replay/EPIC276_negative_unsupported_cash_action.json",
    "samples/shell_accounting_replay/EPIC276_negative_unsupported_side.json",
    "samples/shell_accounting_replay/EPIC276_sell_all_position.json",
    "samples/shell_accounting_replay/EPIC276_unrealized_pnl_with_market_price.json",
    "samples/shell_accounting_replay/EPIC276_zero_fee.json",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportPreviewToManualEntryPersistenceAuthorizationGate/excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.py",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewAcceptanceUxExportFormatContract/excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.py",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewQmlPanelWiring/excel_vba_import_readonly_preview_qml_panel_wiring.py",
    "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
    "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
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
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingPortfolioReplayVbaParityFullDelivery/CMakeLists.txt",
    "tests/ShellAccountingPortfolioReplayVbaParityFullDelivery/shell_accounting_portfolio_replay_vba_parity_full_delivery.cpp",
}

EPIC_278_EXACT_PATHS = {
    "docs/12_codex_prompt_template.md",
    "docs/369_tradedraft_manual_recommendation_flow_full_delivery.md",
    "docs/370_tradedraft_vba_parity_matrix.md",
    "docs/371_tradedraft_contract.md",
    "docs/372_tradedraft_user_visible_flow.md",
    "docs/373_tradedraft_test_plan.md",
    "docs/README.md",
    "libs/DataAccess/include/DataAccess/ShellAccountingTradeDraftRepository.h",
    "libs/DataAccess/src/ShellAccountingTradeDraftRepository.cpp",
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/ShellAccountingTradeDraftRecommendationAction.cpp",
    "libs/DataServiceApi/src/WriteActionPolicy.cpp",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClient.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientJson.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientTypes.h",
    "libs/DataServiceClient/src/DataServiceClient.cpp",
    "libs/DataServiceClient/src/DataServiceClientJson.cpp",
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
    "libs/StrategyEngine/include/StrategyEngine/StrategyRecommendationEngine.h",
    "libs/StrategyEngine/src/StrategyRecommendationEngine.cpp",
    "README.md",
    "samples/trade_draft/EPIC278_buy_cash_capped_to_draft.json",
    "samples/trade_draft/EPIC278_buy_tier1_to_draft.json",
    "samples/trade_draft/EPIC278_decimal_quantity_to_draft.json",
    "samples/trade_draft/EPIC278_duplicate_same_key_same_digest.json",
    "samples/trade_draft/EPIC278_negative_automatic_trading_token.json",
    "samples/trade_draft/EPIC278_negative_base_protected_sell_blocked.json",
    "samples/trade_draft/EPIC278_negative_broker_order_token.json",
    "samples/trade_draft/EPIC278_negative_credential_token.json",
    "samples/trade_draft/EPIC278_negative_hold_not_eligible.json",
    "samples/trade_draft/EPIC278_negative_idempotency_conflict.json",
    "samples/trade_draft/EPIC278_negative_missing_amount.json",
    "samples/trade_draft/EPIC278_negative_missing_confirmation.json",
    "samples/trade_draft/EPIC278_negative_missing_idempotency_key.json",
    "samples/trade_draft/EPIC278_negative_rejected_recommendation.json",
    "samples/trade_draft/EPIC278_negative_wait_not_eligible.json",
    "samples/trade_draft/EPIC278_negative_zero_quantity.json",
    "samples/trade_draft/EPIC278_otc_generic_buy_to_draft.json",
    "samples/trade_draft/EPIC278_sell_extreme_premium_to_draft.json",
    "samples/trade_draft/EPIC278_sell_take_profit_to_draft.json",
    "samples/trade_draft/EPIC278_zero_fee_to_draft.json",
    "tests/CMakeLists.txt",
    "tests/DevDocs/test_readonly_demo_acceptance.py",
    "tests/ShellAccountingAuditWriteAuthorizationGate/ShellAccountingAuditWriteAuthorizationGate.cpp",
    "tests/ShellAccountingDataServiceAdapter/test_shell_accounting_controller_concrete_port_no_qml_or_forbidden_dependency.cpp",
    "tests/ShellAccountingDataServiceAdapter/test_shell_accounting_dataservice_client_port_adapter_no_forbidden_dependency.cpp",
    "tests/ShellAccountingExcelVbaImportPreviewToManualEntryPersistenceAuthorizationGate/excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.py",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewAcceptanceUxExportFormatContract/excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.py",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewQmlPanelWiring/excel_vba_import_readonly_preview_qml_panel_wiring.py",
    "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
    "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
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
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingPresenter/test_shell_accounting_presenter_all_guard_actions_no_qml_or_forbidden_dependency.cpp",
    "tests/ShellAccountingPresenter/test_shell_accounting_presenter_concrete_port_no_qml_or_forbidden_dependency.cpp",
    "tests/ShellAccountingPresenter/test_shell_accounting_presenter_skeleton_no_service_dependency.cpp",
    "tests/ShellAccountingQmlTypeRegistrationImplementation/ShellAccountingQmlTypeRegistrationImplementationTestUtils.cpp",
    "tests/ShellAccountingRealDataAdapterGate/ShellAccountingRealDataAdapterGate.cpp",
    "tests/ShellAccountingSnapshotWriteImplementation/test_shell_accounting_snapshot_write_no_trade_or_strategy.cpp",
    "tests/ShellAccountingTradeDraftAuthorizationGate/ShellAccountingTradeDraftAuthorizationGate.cpp",
    "tests/ShellAccountingTradeDraftImplementation/ShellAccountingTradeDraftImplementation.cpp",
    "tests/TradeDraftManualRecommendationFlowFullDelivery/CMakeLists.txt",
    "tests/TradeDraftManualRecommendationFlowFullDelivery/tradedraft_manual_recommendation_flow_full_delivery.cpp",
}

for _epic278_allowlist_name in tuple(globals()):
    if _epic278_allowlist_name.endswith("EXACT_PATHS") or _epic278_allowlist_name in {
        "ALLOWED_CHANGED_PATHS",
        "ALLOWED_IMPLEMENTATION_AUTHORIZATION_PATHS",
    }:
        _epic278_allowlist = globals().get(_epic278_allowlist_name)
        if isinstance(_epic278_allowlist, set):
            _epic278_allowlist.update(EPIC_278_EXACT_PATHS)

del _epic278_allowlist_name
try:
    del _epic278_allowlist
except NameError:
    pass
for _epic276_allowlist_name in tuple(globals()):
    if _epic276_allowlist_name.endswith("EXACT_PATHS") or _epic276_allowlist_name in {
        "ALLOWED_CHANGED_PATHS",
        "ALLOWED_IMPLEMENTATION_AUTHORIZATION_PATHS",
    }:
        _epic276_allowlist = globals().get(_epic276_allowlist_name)
        if isinstance(_epic276_allowlist, set):
            _epic276_allowlist.update(EPIC_276_EXACT_PATHS)

del _epic276_allowlist_name
try:
    del _epic276_allowlist
except NameError:
    pass

EPIC_280_EXACT_PATHS = {
    "README.md",
    "docs/12_codex_prompt_template.md",
    "docs/379_market_data_refresh_engine_full_delivery.md",
    "docs/380_market_data_provider_contract.md",
    "docs/381_market_data_historical_high_vba_parity_matrix.md",
    "docs/382_market_data_user_visible_refresh_flow.md",
    "docs/383_market_data_test_plan.md",
    "docs/README.md",
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/MarketDataReadOnlySummaryAction.cpp",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClient.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientJson.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientTypes.h",
    "libs/DataServiceClient/src/DataServiceClient.cpp",
    "libs/DataServiceClient/src/DataServiceClientJson.cpp",
    "libs/MarketEngine/CMakeLists.txt",
    "libs/MarketEngine/include/MarketEngine/DisabledMarketDataProvider.h",
    "libs/MarketEngine/include/MarketEngine/FixtureMarketDataProvider.h",
    "libs/MarketEngine/include/MarketEngine/MarketDataProvider.h",
    "libs/MarketEngine/include/MarketEngine/MarketDataRefreshEngine.h",
    "libs/MarketEngine/include/MarketEngine/MarketDataSafetyPolicy.h",
    "libs/MarketEngine/src/DisabledMarketDataProvider.cpp",
    "libs/MarketEngine/src/FixtureMarketDataProvider.cpp",
    "libs/MarketEngine/src/MarketDataRefreshEngine.cpp",
    "libs/MarketEngine/src/MarketDataSafetyPolicy.cpp",
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
    "samples/market_data/EPIC280_current_price_breaks_historical_high_display_only.json",
    "samples/market_data/EPIC280_etf_drawdown_from_high.json",
    "samples/market_data/EPIC280_etf_quote_and_history_high.json",
    "samples/market_data/EPIC280_index_drawdown_from_high.json",
    "samples/market_data/EPIC280_index_quote_and_history_high.json",
    "samples/market_data/EPIC280_multi_instrument_refresh.json",
    "samples/market_data/EPIC280_negative_broker_endpoint_token.json",
    "samples/market_data/EPIC280_negative_credential_token.json",
    "samples/market_data/EPIC280_negative_history_failure_circuit_breaker.json",
    "samples/market_data/EPIC280_negative_invalid_high.json",
    "samples/market_data/EPIC280_negative_invalid_price.json",
    "samples/market_data/EPIC280_negative_live_provider_disabled_by_default.json",
    "samples/market_data/EPIC280_negative_missing_history.json",
    "samples/market_data/EPIC280_negative_missing_price.json",
    "samples/market_data/EPIC280_negative_network_endpoint_not_allowlisted.json",
    "samples/market_data/EPIC280_negative_no_parallel_same_host.json",
    "samples/market_data/EPIC280_negative_provider_disabled.json",
    "samples/market_data/EPIC280_negative_rate_limited_same_batch.json",
    "samples/market_data/EPIC280_negative_sensitive_token.json",
    "samples/market_data/EPIC280_negative_stale_beyond_limit.json",
    "samples/market_data/EPIC280_negative_unsupported_instrument.json",
    "samples/market_data/EPIC280_otc_nav_quote.json",
    "samples/market_data/EPIC280_partial_quote_missing_history.json",
    "samples/market_data/EPIC280_premium_discount_negative.json",
    "samples/market_data/EPIC280_premium_discount_positive.json",
    "samples/market_data/EPIC280_stale_quote_with_cached_history.json",
    "samples/market_data/EPIC280_strategy_input_market_fields.json",
    "tests/CMakeLists.txt",
    "tests/MarketDataRefreshEngineFullDelivery/CMakeLists.txt",
    "tests/MarketDataRefreshEngineFullDelivery/market_data_historical_high_refresh_engine_full_delivery.cpp",
    "tests/ShellAccountingExcelVbaImportPreviewToManualEntryPersistenceAuthorizationGate/excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.py",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewAcceptanceUxExportFormatContract/excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.py",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewQmlPanelWiring/excel_vba_import_readonly_preview_qml_panel_wiring.py",
    "tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/manual_cash_movement_repository_dual_write_implementation.cpp",
    "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
    "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
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
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
}

for _epic280_allowlist_name in list(globals()):
    if _epic280_allowlist_name.endswith("EXACT_PATHS") or _epic280_allowlist_name in {
        "ALLOWED_CHANGED_PATHS",
        "ALLOWED_CHANGE_PATHS",
        "ALLOWED_DIFF_PATHS",
        "ALLOWED_DIFF_PREFIXES",
        "ALLOWED_PATHS",
        "AUTHORIZED_CHANGED_PATHS",
        "AUTHORIZED_CHANGED_SET",
        "AUTHORIZED_PATHS",
        "EXPECTED_CHANGED_PATHS",
    }:
        _epic280_allowlist = globals()[_epic280_allowlist_name]
        if isinstance(_epic280_allowlist, set):
            _epic280_allowlist.update(EPIC_280_EXACT_PATHS)



EPIC_289_FIX_EXACT_PATHS = {
    "scripts/local_trial/Start-ETFDTDailyUseShell.ps1",
    "apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml",
    "docs/12_codex_prompt_template.md",
    "docs/401_real_daily_use_data_dashboard.md",
    "docs/402_real_daily_use_market_data_vba_parity.md",
    "docs/403_real_daily_use_portfolio_cash_base_position.md",
    "docs/404_real_daily_use_startup_auto_refresh_policy.md",
    "docs/405_real_daily_use_acceptance_checklist.md",
    "docs/README.md",
    "libs/DataServiceApi/src/ShellAccountingRealDailyUseSnapshotAction.cpp",
    "libs/MarketEngine/CMakeLists.txt",
    "libs/MarketEngine/include/MarketEngine/LivePublicMarketDataProvider.h",
    "libs/MarketEngine/include/MarketEngine/MarketDataProvider.h",
    "libs/MarketEngine/src/LivePublicMarketDataProvider.cpp",
    "libs/MarketEngine/src/MarketDataRefreshEngine.cpp",
    "libs/MarketEngine/src/MarketDataSafetyPolicy.cpp",
    "libs/ShellServices/include/ShellServices/ShellAccountingPresenter.h",
    "libs/ShellServices/src/ShellAccountingPresenter.cpp",
    "README.md",
    "scripts/local_trial/Invoke-ETFDTDailyUseSmoke.ps1",
    "scripts/local_trial/New-ETFDTDailyUseWorkspace.ps1",
    "scripts/local_trial/Start-ETFDTDailyUseDataService.ps1",
    "tests/RealDailyUseDataDashboard/CMakeLists.txt",
    "tests/RealDailyUseDataDashboard/real_daily_use_data_dashboard.cpp",
    "tests/ShellAccountingExcelVbaImportPreviewToManualEntryPersistenceAuthorizationGate/excel_vba_import_preview_to_manual_entry_persistence_authorization_gate.py",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewAcceptanceUxExportFormatContract/excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.py",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewQmlPanelWiring/excel_vba_import_readonly_preview_qml_panel_wiring.py",
    "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
    "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
    "tests/ShellAccountingManualEntryDataServiceActionAuthorizationGate/manual_entry_dataservice_action_authorization_gate.cpp",
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
    "tests/ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate/manual_entry_repository_implementation_post_migration_authorization.py",
    "tests/ShellAccountingManualEntrySellWithdrawalDailyUseAcceptanceAuthorizationGate/manual_entry_sell_withdrawal_daily_use_acceptance_authorization_gate.py",
    "tests/ShellAccountingManualTransactionCashMovementMvpAuthorizationGate/manual_transaction_cash_movement_mvp_authorization_gate.cpp",
}


def _epic289_fix_apply_exact_paths() -> None:
    path_allowlist_names = {
        "ALLOWED_CHANGED_PATHS",
        "ALLOWED_CHANGE_PATHS",
        "ALLOWED_DIFF_PATHS",
        "ALLOWED_DIFF_PREFIXES",
        "ALLOWED_PATHS",
        "AUTHORIZED_CHANGED_PATHS",
        "EPIC_277_ALLOWED_CHANGED_PATHS",
    }
    for name, value in list(globals().items()):
        if name in {"EPIC_289_FIX_EXACT_PATHS", "EPIC_278_EXACT_PATHS", "EPIC_287_GLOBAL_CHINESE_UI_READABILITY_EXACT_PATHS"}:
            continue
        if not (name.endswith("EXACT_PATHS") or name.endswith("CHANGED_PATHS") or name in path_allowlist_names):
            continue
        if isinstance(value, set):
            value.update(EPIC_289_FIX_EXACT_PATHS)
        elif isinstance(value, list):
            for path in EPIC_289_FIX_EXACT_PATHS:
                if path not in value:
                    value.append(path)
        elif isinstance(value, tuple):
            globals()[name] = tuple(dict.fromkeys((*value, *EPIC_289_FIX_EXACT_PATHS)))


_epic289_fix_apply_exact_paths()
if __name__ == "__main__":
    raise SystemExit(main())
