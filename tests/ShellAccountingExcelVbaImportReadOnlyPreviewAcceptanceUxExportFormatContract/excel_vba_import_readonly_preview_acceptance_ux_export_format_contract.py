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


if __name__ == "__main__":
    raise SystemExit(main())
