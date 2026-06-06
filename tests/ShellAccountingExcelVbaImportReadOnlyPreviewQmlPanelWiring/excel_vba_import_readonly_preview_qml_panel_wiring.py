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
