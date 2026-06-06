# TASK-261 ShellAccounting Excel/VBA Import Read-Only Preview QML Panel Wiring

## Purpose

TASK-261 wires the TASK-260 ShellAccounting Presenter preview contract into the
production ShellAccounting read-only QML page. The vertical slice accepts pasted
sanitized in-memory JSON payload text, calls
`ShellAccountingPresenter.previewExcelVbaImportReadOnly(payloadJson)`, and shows
the Presenter preview status, issue, summary, diagnostic codes, and replay fact
counts.

This is read-only QML panel wiring. It does not add startup wiring, production
file loading, database writes, replay integration, TradeDraft generation,
strategy execution, broker integration, network access, real order placement,
or automatic trading.

## Current State

TASK-258 registered the read-only DataService action
`accounting.excel_vba_import.readonly_preview`. TASK-259 added the typed
client adapter. TASK-260 exposed a ShellServices / Controller / Presenter
contract. TASK-261 connects that existing Presenter contract to the read-only
QML page without changing startup wiring or adding direct service access in
QML.

## QML Panel Contract

The panel in `ShellAccountingReadOnlyPage.qml` must provide:

- a text input for sanitized in-memory JSON payload text.
- a Preview control that calls
  `accountingPresenter.previewExcelVbaImportReadOnly(payloadText)`.
- a Reset control that calls
  `accountingPresenter.resetExcelVbaImportPreviewState()`.
- visible preview status, issue, summary, diagnostic codes, and trade / cash /
  market price / FX fact counts.

The QML page must bind only to Presenter properties. It must not directly
reference `DataServiceClient`, DataServiceApi, SQLite, DataAccess,
AccountingEngine, broker code, network code, credentials, endpoints, file
dialogs, or filesystem import paths.

## Forbidden Scope

TASK-261 does not modify startup wiring, `main.cpp`, DataServiceApi action
implementation, DataServiceClient, ShellServices production C++, DataAccess
repositories, migrations, AccountingEngine production code, historical
fixtures, broker code, network code, credentials, endpoints, or production file
loading.

TASK-261 does not call AccountingEngine, refresh read models, write SQLite,
write audit / ledger / snapshot / trade_log data, generate TradeDrafts, execute
strategies, submit broker orders, place real orders, or enable automatic
trading.

## Evidence Contract

The CTest
`shell_accounting_excel_vba_import_readonly_preview_qml_panel_wiring` must emit
sanitized stdout JSON evidence with:

- `task=TASK-261`
- `mode=readonly-import-preview-qml-panel-wiring-vertical-slice`
- `qmlPreviewPanelWired=true`
- `presenterPreviewMethodInvokedFromQml=true`
- `presenterPreviewPropertiesBound=true`
- `previewStatusVisible=true`
- `previewSummaryVisible=true`
- `diagnosticCodesVisible=true`
- `factCountsVisible=true`
- `resetPreviewStateWired=true`
- `qmlDirectDataServiceClient=false`
- `qmlDirectSQLite=false`
- `qmlDirectDataServiceApi=false`
- `qmlDirectAccountingEngine=false`
- `productionFileLoading=false`
- `fileDialogUsed=false`
- `startupWiringChanged=false`
- `accountingEngineCalled=false`
- `productionWrite=false`
- `sqliteProductionWrite=false`
- `auditWritten=false`
- `ledgerWritten=false`
- `snapshotWritten=false`
- `tradeLogWritten=false`
- `readModelPersistentWrite=false`
- `tradeDraftGenerated=false`
- `strategyExecuted=false`
- `brokerOrderSubmitted=false`
- `networkAccess=false`
- `credentialAccess=false`
- `endpointAccess=false`
- `automaticTrading=false`
- `rawUserDataExposed=false`

## Follow-Up Boundary

Future startup wiring changes, production file loading, AccountingEngine
runtime integration, read model refresh, import persistence, TradeDraft
generation, broker integration, strategy execution, real order placement, or
automatic trading must be separately authorized.
