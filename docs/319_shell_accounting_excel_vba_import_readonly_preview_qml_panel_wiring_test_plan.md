# TASK-261 ShellAccounting Excel/VBA Import Read-Only Preview QML Panel Wiring Test Plan

## Purpose

This test plan verifies that TASK-261 wires the TASK-260 read-only preview
Presenter contract into `ShellAccountingReadOnlyPage.qml` without adding
startup wiring, direct service access from QML, production file loading,
database writes, AccountingEngine replay, broker access, network access, or
automatic trading.

## Test Matrix

- QML panel: the page exposes an Excel/VBA import preview panel with payload
  input, Preview, Reset, and preview result fields.
- Presenter invocation: Preview calls
  `accountingPresenter.previewExcelVbaImportReadOnly(payloadText)`.
- Presenter bindings: QML binds status, issue, summary, diagnostic codes, and
  trade / cash / market price / FX fact counts to the TASK-260 Presenter
  properties.
- Reset wiring: QML calls `accountingPresenter.resetExcelVbaImportPreviewState()`.
- Direct dependency block: QML does not reference DataServiceClient,
  DataServiceApi, SQLite, DataAccess, AccountingEngine, broker, network,
  credentials, endpoints, FileDialog, or filesystem import paths.
- Production boundary: startup, DataServiceApi action implementation,
  DataServiceClient, ShellServices C++, DataAccess, migrations, AccountingEngine
  production code, and historical fixtures are not changed by the panel wiring.

## Required CTest

Run:

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring --output-on-failure
```

The CTest must pass and emit TASK-261 evidence with
`qmlPreviewPanelWired=true`,
`presenterPreviewMethodInvokedFromQml=true`,
`presenterPreviewPropertiesBound=true`, `previewStatusVisible=true`,
`previewSummaryVisible=true`, `diagnosticCodesVisible=true`,
`factCountsVisible=true`, `resetPreviewStateWired=true`,
`qmlDirectDataServiceClient=false`, `qmlDirectSQLite=false`,
`qmlDirectDataServiceApi=false`, `qmlDirectAccountingEngine=false`,
`productionFileLoading=false`, `fileDialogUsed=false`,
`startupWiringChanged=false`, `accountingEngineCalled=false`,
`productionWrite=false`, `sqliteProductionWrite=false`, `auditWritten=false`,
`ledgerWritten=false`, `snapshotWritten=false`, `tradeLogWritten=false`,
`readModelPersistentWrite=false`, `tradeDraftGenerated=false`,
`strategyExecuted=false`, `brokerOrderSubmitted=false`, `networkAccess=false`,
`credentialAccess=false`, `endpointAccess=false`, `automaticTrading=false`, and
`rawUserDataExposed=false`.

## Required Regression Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Go / No-Go Checklist

- Go only if QML uses the Presenter contract and exposes all required preview
  fields.
- Go only if no startup, DataServiceApi action, DataServiceClient, ShellServices
  C++, DataAccess, migration, AccountingEngine, broker, network, credential, or
  endpoint implementation changed.
- Go only if the panel does not provide FileDialog, filesystem import, database
  write, TradeDraft, strategy, broker, real order placement, or automatic
  trading behavior.
