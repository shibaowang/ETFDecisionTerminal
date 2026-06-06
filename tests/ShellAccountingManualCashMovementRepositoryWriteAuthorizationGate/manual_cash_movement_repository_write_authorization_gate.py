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


def files_under(root: Path, suffixes: set[str] | None = None) -> list[Path]:
    if not root.exists():
        return []
    files = [path for path in root.rglob("*") if path.is_file()]
    if suffixes is not None:
        files = [path for path in files if path.suffix in suffixes]
    return files


def joined(root: Path, paths: list[Path]) -> str:
    return "\n".join(read(path) for path in paths if path.exists())


def assert_not_changed(root: Path, relative_path: str) -> None:
    normalized = relative_path.replace("\\", "/")
    if normalized in TASK_258_EXACT_PATHS:
        return
    require(normalized not in changed_paths(root), f"{relative_path} must not be changed by TASK-194")


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

    docs184 = root / "docs" / "184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md"
    docs185 = root / "docs" / "185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md"
    readme = read(root / "README.md")
    docs_index = read(root / "docs" / "README.md")
    prompt = read(root / "docs" / "12_codex_prompt_template.md")
    doc184 = read(docs184)
    doc185 = read(docs185)
    tests_cmake = read(root / "tests" / "CMakeLists.txt")
    dataaccess_cmake = read(root / "libs" / "DataAccess" / "CMakeLists.txt")

    require(docs184.exists(), "docs/184 exists")
    require(docs185.exists(), "docs/185 exists")
    for text, context in [(readme, "README"), (docs_index, "docs/README"), (prompt, "docs/12")]:
        require_contains(text, "TASK-194", context)
    require_contains(readme, "docs/184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md", "README")
    require_contains(readme, "docs/185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md", "README")
    require_contains(docs_index, "184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md", "docs/README")
    require_contains(docs_index, "185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md", "docs/README")
    require_contains(tests_cmake, "ShellAccountingManualCashMovementRepositoryWriteAuthorizationGate", "tests/CMakeLists")

    doc184_tokens = [
        "TASK-194",
        "authorization gate-only",
        "does not implement manual cash movement repository write",
        "does not modify the TASK-192 manual transaction repository",
        "does not modify DataAccess production implementation",
        "does not modify DataServiceActions.cpp / DataServiceActions.h / DataServiceActionRegistrar.cpp",
        "does not modify TASK-178 validation production code",
        "`migrations/001_initial_schema.sql`",
        "`migrations/002_shell_accounting_manual_entry_schema.sql`",
        "does not write SQLite",
        "does not execute runtime SQL",
        "does not write `cash_adjustment`, `trade_log`, `audit_log`",
        "TASK-190 completed",
        "TASK-192 implemented DataAccess-only manual transaction repository write",
        "TASK-193 aligned",
        "Future manual cash movement repository write implementation must be a separate",
        "DataAccess repository boundary",
        "must not scatter SQL in DataService action handlers",
        "DataService action write wiring must be a separate TASK",
        "TASK-190 002 migration schema fields",
        "without modifying TASK-192 repository behavior",
        "no broker",
        "no network",
        "no credentials",
        "no endpoint",
        "no real order placement",
        "no automatic trading",
        "cash_adjustment.request_id",
        "cash_adjustment.idempotency_key",
        "cash_adjustment.occurred_at_utc",
        "cash_adjustment.source_memo_sanitized",
        "cash_adjustment.trade_log_uid",
        "cash_adjustment.amount_cents",
        "cash_adjustment.adjustment_type",
        "trade_log.request_id",
        "trade_log.idempotency_key",
        "trade_log.occurred_at_utc",
        "trade_log.cash_adjustment_uid",
        "trade_log.trade_source",
        "trade_log.manual_entry",
        "Deposit mapping",
        "Withdrawal mapping",
        "fail closed",
        "amount sign policy",
        "net cash impact policy",
        "request_id trace behavior",
        "idempotency_key duplicate behavior",
        "transaction boundary",
        "rollback / failure atomicity",
        "unique index conflict handling",
        "no partial facts on failure",
        "sourceMemo sanitization",
        "raw credential",
        "raw SQL",
        "internal exception stack",
        "Broker sandbox new capability development remains paused",
    ]
    for token in doc184_tokens:
        require_contains(doc184, token, "docs/184")

    doc185_tokens = [
        "TASK-194",
        "Test Matrix",
        "Required Probes",
        "Go / No-Go Checklist",
        "docs/184 exists",
        "docs/185 exists",
        "Cash adjustment mapping includes request_id",
        "Trade log cash fact mapping includes request_id",
        "Deposit and Withdrawal policy",
        "Rollack" if False else "Rollback",
        "Duplicate idempotency_key behavior",
        "sourceMemo sanitization",
        "Runtime SQL INSERT / UPDATE / DELETE / REPLACE is not added",
        "Runtime cash_adjustment write is not added",
        "Broker disabled / broker order / real broker gates pass",
    ]
    for token in doc185_tokens:
        require_contains(doc185, token, "docs/185")

    protected_paths = [
        "migrations/001_initial_schema.sql",
        "migrations/002_shell_accounting_manual_entry_schema.sql",
        "libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h",
        "libs/DataServiceApi/src/DataServiceActionRegistrar.cpp",
        "libs/DataServiceApi/include/DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h",
        "libs/DataServiceApi/src/ShellAccountingManualTransactionCashMovementValidation.cpp",
        "libs/DataAccess/include/DataAccess/ShellAccountingManualTransactionRepository.h",
        "libs/DataAccess/src/ShellAccountingManualTransactionRepository.cpp",
        "apps/ETFDecisionShell/src/main.cpp",
    ]
    for path in protected_paths:
        assert_not_changed(root, path)

    changes = changed_paths(root)
    require(not any(path.startswith("migrations/") for path in changes), "TASK-194 must not modify migrations")
    require(not any(path.endswith(".sql") for path in changes), "TASK-194 must not add schema or SQL files")
    allowed_task200_qml = {"apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml"}
    unexpected_qml_changes = [
        path for path in changes
        if path.startswith("apps/ETFDecisionShell/qml/") and path not in allowed_task200_qml
    ]
    require(
        not unexpected_qml_changes,
        "TASK-194/TASK-200 must not modify unauthorized production QML: "
        + ", ".join(unexpected_qml_changes),
    )
    allowed_task200_presenter_controller = {
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
    unexpected_presenter_controller_changes = [
        path for path in changes
        if path not in allowed_task200_presenter_controller
        and (path.startswith("apps/") or path.startswith("libs/"))
        and ("Presenter" in path or "Controller" in path)
    ]
    require(
        not unexpected_presenter_controller_changes,
        "TASK-194/TASK-200 must not modify unauthorized Presenter / Controller: "
        + ", ".join(unexpected_presenter_controller_changes),
    )
    require(not any(path.startswith("libs/AccountingEngine/") for path in changes), "AccountingEngine must not change")
    require(not any(path.startswith("libs/StrategyEngine/") or path.startswith("libs/MarketEngine/") for path in changes),
            "StrategyEngine / MarketEngine must not change")

    require("ShellAccountingManualCashMovementRepository.cpp" in dataaccess_cmake, "TASK-196 DataAccess CMake registers cash movement repository")
    dataaccess_prod_text = joined(
        root,
        files_under(root / "libs" / "DataAccess" / "include", {".h", ".hpp"})
        + files_under(root / "libs" / "DataAccess" / "src", {".cpp", ".cc", ".cxx"}),
    )
    require("ShellAccountingManualCashMovementRepository" in dataaccess_prod_text,
            "TASK-196 manual cash movement repository implementation must exist")
    require("ManualCashMovementWriteRepository" not in dataaccess_prod_text,
            "manual cash movement write repository implementation must not exist")

    dataservice_actions = read(root / "libs" / "DataServiceApi" / "src" / "DataServiceActions.cpp")
    require_contains(
        dataservice_actions,
        "ShellAccountingManualCashMovementRepository repository(connection)",
        "TASK-198 DataServiceActions manual cash movement wiring",
    )
    require_contains(
        dataservice_actions,
        "ShellAccountingManualTransactionRepository repository(connection)",
        "TASK-198 DataServiceActions manual transaction wiring",
    )
    for handler in [
        function_body(dataservice_actions, "handleAccountingManualEntryTransactionCreate"),
        function_body(dataservice_actions, "handleAccountingManualEntryCashMovementCreate"),
    ]:
        require("executeStatement" not in handler, "TASK-198 DataServiceActions handler must not execute SQL directly")
        require(re.search(r"\b(INSERT|UPDATE|DELETE|REPLACE)\b", handler, re.IGNORECASE) is None,
                "TASK-198 DataServiceActions handler must not contain direct DML")

    production_diff = diff_text_excluding(root, ["libs", "apps"], TASK_258_EXACT_PATHS)
    dataservice_shell_diff = diff_text_excluding(
        root,
        ["libs/DataServiceApi", "apps", "libs/ShellServices", "libs/ShellCore"],
        TASK_258_EXACT_PATHS,
    )
    require(re.search(r"\b(INSERT|UPDATE|DELETE|REPLACE)\b", dataservice_shell_diff, re.IGNORECASE) is None,
            "TASK-194 must not add DataService/Shell runtime DML")
    for token in ["INSERT INTO audit_log", "ledgerId", "auditLogId"]:
        require(token not in production_diff, f"TASK-194 must not add runtime token {token}")
    for token in ["sqlite3_exec", "SQLite::Statement", "prepareStatement", "executeWrite"]:
        require(token not in dataservice_shell_diff, f"TASK-194 must not add SQLite runtime write token {token}")

    added_lines = "\n".join(
        line for line in production_diff.splitlines() if line.startswith("+") and not line.startswith("+++")
    )
    forbidden_added = [
        "TradeDraft",
        "trade suggestion",
        "strategy recommendation",
        "brokerOrder",
        "placeOrder",
        "realBrokerOrderId",
        "automatic trading",
        "auto trading",
        "http://",
        "https://",
        "api_key",
        "password=",
        "secret=",
        "endpoint=",
    ]
    for token in forbidden_added:
        require(token not in added_lines, f"TASK-194 must not add forbidden production token {token}")

    for required_subdir in [
        "ShellAccountingManualTransactionRepositoryWriteImplementation",
        "ShellAccountingManualEntryRepositoryImplementationPostMigrationAuthorizationGate",
        "ShellAccountingManualEntrySchemaMigrationImplementation",
        "ShellAccountingManualEntryDataServiceActionValidationWiring",
        "ShellAccountingManualTransactionCashMovementValidationScaffold",
        "ShellAccountingBrokerAdapterDisabledWiring",
        "ShellAccountingBrokerOrderAuthorizationGate",
        "ShellAccountingBrokerOrderImplementation",
        "ShellAccountingRealBrokerOrderAuthorizationGate",
        "ShellAccountingRealBrokerOrderImplementationGate",
    ]:
        require_contains(tests_cmake, required_subdir, "tests/CMakeLists retained related gates")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
