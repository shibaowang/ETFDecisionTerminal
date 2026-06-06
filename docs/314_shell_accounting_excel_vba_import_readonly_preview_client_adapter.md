# TASK-259 ShellAccounting Excel/VBA Import Read-Only Preview Client Adapter

## Purpose

TASK-259 adds a read-only client adapter and typed response contract for the
TASK-258 DataService preview action:

sanitized in-memory import payload -> read-only preview client adapter ->
`accounting.excel_vba_import.readonly_preview` -> typed preview diagnostics /
replay fact summary -> sanitized CTest evidence.

The adapter prepares the Shell side and existing DataService client boundary
for a future UI preview. This task does not add UI wiring.

## Current State

TASK-257 added the production read-only parser boundary. TASK-258 registered
the DataService read-only action `accounting.excel_vba_import.readonly_preview`
and returned normalized diagnostics plus replay fact counts. TASK-259 keeps
that action unchanged and adds a typed client-side mapping around the existing
DataService client transport boundary.

## Adapter Boundary

The client adapter may accept only an in-memory sanitized JSON payload object.
It calls the existing read-only DataService action and maps the response into a
typed preview result containing:

- action name.
- task and mode fields from the service response.
- accepted / rejected preview status.
- sanitized diagnostics and diagnostic codes.
- replay fact summary counts.
- no-write / no-broker / no-network evidence fields.

The adapter must fail closed when the service response payload is malformed or
missing required preview fields.

## Forbidden Scope

TASK-259 does not modify apps, production QML, startup, Presenter, Controller,
production file loading, AccountingEngine production code, DataAccess
repositories, migrations, or historical fixtures.

TASK-259 does not call AccountingEngine, refresh read models, write SQLite,
write audit / ledger / snapshot / trade_log data, generate TradeDrafts, execute
strategies, submit broker orders, use broker SDKs, access networks, read
credentials, read endpoints, place real orders, or enable automatic trading.

## Evidence Contract

The CTest
`shell_accounting_excel_vba_import_readonly_preview_client_adapter` must emit
sanitized stdout JSON evidence with:

- `task=TASK-259`
- `mode=readonly-import-preview-client-adapter-vertical-slice`
- `previewClientAdapterCreated=true`
- `dataServicePreviewActionCalled=true`
- `previewResponseParsed=true`
- `diagnosticsMapped=true`
- `replayFactSummaryMapped=true`
- `accountingEngineCalled=false`
- `qmlWiringChanged=false`
- `productionFileLoading=false`
- `productionWrite=false`
- `sqliteProductionWrite=false`
- `auditWritten=false`
- `ledgerWritten=false`
- `snapshotWritten=false`
- `tradeLogWritten=false`
- `readModelPersistentWrite=false`
- `tradeDraftGenerated=false`
- `brokerOrderSubmitted=false`
- `networkAccess=false`
- `credentialAccess=false`
- `endpointAccess=false`
- `automaticTrading=false`
- `rawUserDataExposed=false`

## Follow-Up Boundary

Future UI preview, QML wiring, production file loading, AccountingEngine
runtime integration, read model refresh, import persistence, TradeDraft
generation, broker integration, strategy execution, real order placement, or
automatic trading must be separately authorized.
