# TASK-260 ShellAccounting Excel/VBA Import Read-Only Preview ShellServices Presenter Contract

## Purpose

TASK-260 adds a ShellServices presenter contract for the TASK-258 read-only
DataService preview action and the TASK-259 typed client adapter:

sanitized in-memory import payload JSON -> ShellAccountingPresenter preview
method -> ShellAccountingReadOnlyController -> ShellAccountingDataServiceAdapter
-> ShellAccountingDataServiceClientPort -> ShellAccountingDataServiceClientPortAdapter
-> `DataServiceClient.accountingExcelVbaImportReadOnlyPreview` -> typed preview
status, diagnostics, and replay fact summary -> UI-ready presenter state.

This is a read-only preview contract slice. It does not add QML wiring,
startup wiring, production file loading, replay integration, persistence, or
trading capability.

## Current State

TASK-257 added the production read-only Excel/VBA import parser boundary.
TASK-258 registered the read-only DataService action
`accounting.excel_vba_import.readonly_preview`. TASK-259 added the typed
DataService client preview adapter. TASK-260 maps that typed preview result
through ShellServices and exposes presenter state for a future UI preview.

## ShellServices Contract

The ShellServices contract carries:

- accepted / rejected preview status.
- sanitized diagnostics and diagnostic codes.
- trade, cash, market price, and FX replay fact counts.
- sanitized issue text and summary text for presenter state.
- read-only and no-write evidence flags.

Only `ShellAccountingDataServiceClientPortAdapter` may call the typed
DataService client preview adapter. Presenter and controller code must not
include or directly call `DataServiceClient`.

## Input Boundary

The presenter accepts only an in-memory JSON object string. Malformed JSON and
file-path style inputs fail closed before reaching the DataService boundary.
The task does not load production files and does not expose raw user data.

## Forbidden Scope

TASK-260 does not modify apps, production QML, startup, migrations,
DataServiceApi action implementation, DataAccess repositories, AccountingEngine
production code, broker code, network code, credentials, endpoints, or
historical fixtures.

TASK-260 does not call AccountingEngine, refresh read models, write SQLite,
write audit / ledger / snapshot / trade_log data, generate TradeDrafts, execute
strategies, submit broker orders, place real orders, or enable automatic
trading.

## Evidence Contract

The CTest
`shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract`
must emit sanitized stdout JSON evidence with:

- `task=TASK-260`
- `mode=readonly-import-preview-shellservices-presenter-contract-vertical-slice`
- `shellServicesPreviewContractCreated=true`
- `presenterPreviewMethodCreated=true`
- `controllerPreviewMethodCreated=true`
- `serviceAdapterPreviewMapped=true`
- `dataServiceClientPortExtended=true`
- `typedClientPreviewAdapterCalled=true`
- `previewViewModelMapped=true`
- `diagnosticsMapped=true`
- `replayFactSummaryMapped=true`
- `malformedInputFailsClosed=true`
- `accountingEngineCalled=false`
- `qmlWiringChanged=false`
- `startupWiringChanged=false`
- `productionFileLoading=false`
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

Future QML preview wiring, startup wiring, production file loading,
AccountingEngine replay integration, read model refresh, import persistence,
TradeDraft generation, broker integration, strategy execution, real order
placement, or automatic trading must be separately authorized.
