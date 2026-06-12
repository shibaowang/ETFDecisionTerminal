#!/usr/bin/env python3

import argparse
import re
import subprocess
from pathlib import Path


TASK_258_EXACT_PATHS = {
    "docs/312_shell_accounting_excel_vba_import_readonly_dataservice_preview_action.md",
    "docs/313_shell_accounting_excel_vba_import_readonly_dataservice_preview_action_test_plan.md",
    "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
    "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportReadOnlyPreviewAction.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/excel_vba_import_readonly_dataservice_preview_action.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_missing_required_header_preview_payload.json",
    "tests/ShellAccountingExcelVbaImportReadOnlyDataServicePreviewAction/fixtures/TASK258_valid_buy_preview_payload.json",
    "docs/314_shell_accounting_excel_vba_import_readonly_preview_client_adapter.md",
    "docs/315_shell_accounting_excel_vba_import_readonly_preview_client_adapter_test_plan.md",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClient.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientJson.h",
    "libs/DataServiceClient/include/DataServiceClient/DataServiceClientTypes.h",
    "libs/DataServiceClient/src/DataServiceClient.cpp",
    "libs/DataServiceClient/src/DataServiceClientJson.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter/excel_vba_import_readonly_preview_client_adapter.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter/fixtures/TASK259_missing_required_header_preview_client_payload.json",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewClientAdapter/fixtures/TASK259_valid_buy_preview_client_payload.json",
    "README.md",
    "docs/README.md",
    "docs/12_codex_prompt_template.md",
    "docs/316_shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract.md",
    "docs/317_shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract_test_plan.md",
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
    "libs/ShellServices/src/ShellAccountingServiceTypes.cpp",
    "tests/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewShellServicesPresenterContract/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewShellServicesPresenterContract/excel_vba_import_readonly_preview_shellservices_presenter_contract.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewShellServicesPresenterContract/fixtures/TASK260_missing_required_header_presenter_preview_payload.json",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewShellServicesPresenterContract/fixtures/TASK260_valid_buy_presenter_preview_payload.json",
    "docs/326_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_implementation.md",
    "docs/327_shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence_test_plan.md",
    "libs/DataAccess/CMakeLists.txt",
    "libs/DataAccess/include/DataAccess/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.h",
    "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
    "libs/DataAccess/src/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.cpp",
    "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
    "libs/DataServiceApi/CMakeLists.txt",
    "libs/DataServiceApi/src/ShellAccountingExcelVbaImportPersistManualEntryAction.cpp",
    "libs/DataServiceApi/src/WriteActionPolicy.cpp",
    "tests/DevDocs/test_readonly_demo_acceptance.py",
    "tests/ShellAccountingExcelVbaImportAcceptedPreviewManualEntryPersistence/CMakeLists.txt",
    "tests/ShellAccountingExcelVbaImportAcceptedPreviewManualEntryPersistence/excel_vba_import_accepted_preview_manual_entry_persistence.cpp",
    "tests/ShellAccountingExcelVbaImportReadOnlyPreviewAcceptanceUxExportFormatContract/excel_vba_import_readonly_preview_acceptance_ux_export_format_contract.py",
    "tests/ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate/manual_cash_movement_repository_write_authorization_gate.py",
    "tests/ShellAccountingManualCashMovementSchemaContractAlignmentGate/manual_cash_movement_schema_contract_alignment_gate.py",
    "tests/ShellAccountingManualEntryDataServiceActionAuthorizationGate/manual_entry_dataservice_action_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryDataServiceActionImplementationAuthorizationGate/manual_entry_dataservice_action_implementation_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryDataServiceActionScaffold/manual_entry_dataservice_action_scaffold.cpp",
    "tests/ShellAccountingManualEntryPersistenceAuthorizationGate/manual_entry_persistence_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryRepositoryImplementationAuthorizationGate/manual_entry_repository_implementation_authorization_gate.cpp",
    "tests/ShellAccountingManualEntryRepositoryScaffoldAuthorizationGate/manual_entry_repository_scaffold_authorization_gate.cpp",
    "tests/ShellAccountingManualEntrySchemaAdequacyReviewGate/manual_entry_schema_adequacy_review_gate.cpp",
    "tests/ShellAccountingManualEntrySchemaGapAuthorizationGate/manual_entry_schema_gap_authorization_gate.cpp",
    "tests/ShellAccountingManualEntrySchemaImplementationAuthorizationGate/manual_entry_schema_implementation_authorization_gate.cpp",
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


def diff_text(root: Path, *paths: str) -> str:
    completed = subprocess.run(
        ["git", "diff", "main", "--", *paths],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    return completed.stdout


def diff_text_excluding(root: Path, paths: list[str], excluded_paths: set[str]) -> str:
    completed = subprocess.run(
        ["git", "diff", "main", "--", *paths],
        cwd=root,
        check=True,
        capture_output=True,
        text=True,
    )
    kept: list[str] = []
    keep_block = True
    for line in completed.stdout.splitlines():
        if line.startswith("diff --git "):
            parts = line.split(" b/", 1)
            changed = parts[1].strip() if len(parts) == 2 else ""
            keep_block = changed not in excluded_paths
        if keep_block:
            kept.append(line)
    return "\n".join(kept)


def files_under(root: Path, suffixes: set[str] | None = None) -> list[Path]:
    if not root.exists():
        return []
    files = [path for path in root.rglob("*") if path.is_file()]
    if suffixes is not None:
        files = [path for path in files if path.suffix in suffixes]
    return files


def joined(files: list[Path]) -> str:
    return "\n".join(read(path) for path in files if path.exists())


def assert_not_changed(changes: set[str], relative_path: str) -> None:
    normalized = relative_path.replace("\\", "/")
    if normalized in TASK_258_EXACT_PATHS:
        return
    require(normalized not in changes, f"{relative_path} must not be changed by TASK-195")


def function_body(source: str, function_name: str) -> str:
    start = source.find(function_name + "(")
    require(start >= 0, f"{function_name} must exist")
    brace = source.find("{", start)
    require(brace >= 0, f"{function_name} body must start")
    depth = 0
    for index in range(brace, len(source)):
        char = source[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return source[brace : index + 1]
    raise AssertionError(f"{function_name} body must end")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--source-root", required=True)
    args = parser.parse_args()
    root = Path(args.source_root)

    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc184 = read(root / "docs" / "184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md")
    doc185 = read(root / "docs" / "185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md")
    doc186_path = root / "docs" / "186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md"
    doc187_path = root / "docs" / "187_shell_accounting_manual_cash_movement_schema_contract_alignment_test_plan.md"
    doc186 = read(doc186_path)
    doc187 = read(doc187_path)
    migration001 = read(root / "migrations" / "001_initial_schema.sql")
    migration002 = read(root / "migrations" / "002_shell_accounting_manual_entry_schema.sql")
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    dataaccess_cmake = read(root / "libs" / "DataAccess" / "CMakeLists.txt")

    require(doc186_path.exists(), "docs/186 exists")
    require(doc187_path.exists(), "docs/187 exists")
    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-195", context)
    require_contains(readme, "docs/186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md", "README")
    require_contains(readme, "docs/187_shell_accounting_manual_cash_movement_schema_contract_alignment_test_plan.md", "README")
    require_contains(docs_index, "186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md", "docs/README")
    require_contains(docs_index, "187_shell_accounting_manual_cash_movement_schema_contract_alignment_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualCashMovementSchemaContractAlignmentGate", "tests/CMakeLists")

    require_contains(migration001, "trade_log_id INTEGER NOT NULL", "migrations/001")
    require_contains(migration001, "FOREIGN KEY (trade_log_id) REFERENCES trade_log(id)", "migrations/001")
    require_contains(migration002, "ALTER TABLE cash_adjustment ADD COLUMN trade_log_uid TEXT", "migrations/002")
    require_contains(migration002, "ALTER TABLE trade_log ADD COLUMN cash_adjustment_uid TEXT", "migrations/002")

    for token in [
        "TASK-195",
        "schema-contract alignment gate-only",
        "cash_adjustment.trade_log_id INTEGER NOT NULL",
        "FOREIGN KEY (trade_log_id) REFERENCES trade_log(id)",
        "current schema blocks cash_adjustment-only write",
        "must not use a cash_adjustment-only design",
        "write `trade_log` cash movement fact",
        "write `cash_adjustment`",
        "cash_adjustment.trade_log_id",
        "trade_log.id",
        "trade_log.cash_adjustment_uid",
        "atomic transaction",
        "rollback on any failure",
        "no partial facts",
        "Future DataService action write wiring must be a separate TASK",
        "Future replay, read model, UI integration",
        "does not authorize",
        "repository implementation",
        "runtime SQL / SQLite write",
        "broker SDK, network, credentials, endpoint",
        "automatic trading",
        "Broker sandbox new capability development remains paused",
    ]:
        require_contains(doc186, token, "docs/186")

    for token in [
        "TASK-195",
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "cash_adjustment-only implementation",
        "trade_log_id INTEGER NOT NULL",
        "FOREIGN KEY (trade_log_id) REFERENCES trade_log(id)",
        "current schema blocks cash_adjustment-only write",
        "future implementation must dual-write",
        "no partial facts",
        "No manual cash movement repository implementation is added",
        "Broker disabled / broker order / real broker gates pass",
    ]:
        require_contains(doc187, token, "docs/187")

    for token in [
        "current schema blocks cash_adjustment-only write",
        "must atomically dual-write",
        "cash_adjustment.trade_log_id",
        "trade_log.id",
        "trade_log.cash_adjustment_uid",
    ]:
        require_contains(doc184, token, "docs/184")
    require(
        "cash_adjustment-only implementation is viable" not in doc184,
        "docs/184 must not claim cash_adjustment-only implementation is viable",
    )
    require_contains(doc185, "current schema blocks cash_adjustment-only write", "docs/185")
    require_contains(doc185, "`cash_adjustment` + `trade_log` dual write", "docs/185")

    changes = changed_paths(root)
    for protected in [
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
        "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
    ]:
        assert_not_changed(changes, protected)
    require(not any(path.startswith("migrations/") for path in changes), "TASK-195 must not modify migrations")
    require(not any(path.endswith(".sql") for path in changes), "TASK-195 must not add schema or SQL files")
    allowed_task196_dataaccess = {
        "libs/DataAccess/CMakeLists.txt",
        "libs/DataAccess/include/DataAccess/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.h",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualCashMovementRepository.h",
        "libs/DataAccess/src/ShellAccountingExcelVbaImportManualEntryPersistenceRepository.cpp",
        "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
        "libs/DataAccess/src/ShellAccountingManualCashMovementRepository.cpp",
    }
    require(
        not any(
            path.startswith("libs/DataAccess/")
            and path not in allowed_task196_dataaccess
            and path not in EPIC_278_EXACT_PATHS
            for path in changes
        ),
        "TASK-195/TASK-196 alignment only allows the authorized DataAccess manual cash movement repository",
    )
    allowed_task200_qml = {"apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml"}
    unexpected_qml_changes = [
        path for path in changes
        if path.startswith("apps/ETFDecisionShell/qml/") and path not in allowed_task200_qml
    ]
    require(
        not unexpected_qml_changes,
        "TASK-195/TASK-200 must not modify unauthorized production QML: "
        + ", ".join(unexpected_qml_changes),
    )
    require(not any(path.startswith("libs/AccountingEngine/") and path not in EPIC_276_EXACT_PATHS for path in changes), "TASK-195 must not modify AccountingEngine")

    dataservice_actions = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp")
    require_contains(
        dataservice_actions,
        "ShellAccountingManualTransactionRepository repository(connection)",
        "TASK-198 DataServiceActions manual transaction wiring",
    )
    require_contains(
        dataservice_actions,
        "ShellAccountingManualCashMovementRepository repository(connection)",
        "TASK-198 DataServiceActions manual cash movement wiring",
    )
    for handler in [
        function_body(dataservice_actions, "handleAccountingManualEntryTransactionCreate"),
        function_body(dataservice_actions, "handleAccountingManualEntryCashMovementCreate"),
    ]:
        require("executeStatement" not in handler, "TASK-198 DataServiceActions handler must not execute SQL directly")
        require(re.search(r"\b(INSERT|UPDATE|DELETE|REPLACE)\b", handler, re.IGNORECASE) is None,
                "TASK-198 DataServiceActions handler must not contain direct DML")

    require("ShellAccountingManualCashMovementRepository.cpp" in dataaccess_cmake, "DataAccess CMake registers TASK-196 cash movement repository")
    dataaccess_text = joined(
        files_under(root / "libs" / "DataAccess" / "include", {".h", ".hpp"})
        + files_under(root / "libs" / "DataAccess" / "src", {".cpp", ".cc", ".cxx"})
    )
    require("ShellAccountingManualCashMovementRepository" in dataaccess_text, "TASK-196 manual cash movement repository implementation must exist")
    require("ManualCashMovementWriteRepository" not in dataaccess_text, "unauthorized manual cash movement write repository name must not exist")

    production_diff = diff_text_excluding(root, ["libs", "apps"], TASK_258_EXACT_PATHS)
    dataservice_shell_diff = diff_text_excluding(
        root,
        ["libs/DataServiceApi", "apps", "libs/ShellServices", "libs/ShellCore"],
        TASK_258_EXACT_PATHS,
    )
    require(re.search(r"\b(INSERT|UPDATE|DELETE|REPLACE)\b", dataservice_shell_diff, re.IGNORECASE) is None,
            "TASK-195 must not add DataService/Shell runtime DML")
    added_production_lines = "\n".join(
        line for line in production_diff.splitlines() if line.startswith("+") and not line.startswith("+++")
    )
    for token in [
        "INSERT INTO cash_adjustment",
        "INSERT INTO trade_log",
        "INSERT INTO audit_log",
        "ledgerWritten = true",
        "brokerOrder",
        "placeOrder",
        "http://",
        "https://",
        "api_key",
        "password=",
        "secret=",
        "endpoint=",
        "automaticTrading\\\":true",
    ]:
        require(token not in added_production_lines, f"TASK-195 must not add forbidden production token {token}")

    for required_subdir in [
        "ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate",
        "ShellAccountingManualTransactionRepositoryWriteImplementation",
        "ShellAccountingManualEntryDataServiceActionValidationWiring",
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
    ]:
        require_contains(tests_cmake, required_subdir, "tests/CMakeLists retained related gates")

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
if __name__ == "__main__":
    raise SystemExit(main())
