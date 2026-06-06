# TASK-258 ShellAccounting Excel/VBA Import Read-Only DataService Preview Action

## Purpose

TASK-258 adds a production read-only DataService preview action for sanitized
Excel/VBA-style manual entry import payloads:

DataService read-only action request -> read-only import preview action ->
TASK-257 parser boundary -> normalized diagnostics -> replay fact summary only
-> sanitized response evidence.

The action is a preview boundary. It does not run AccountingEngine, does not
refresh read models, and does not write SQLite or any production table.

## Current State

TASK-257 added the DataServiceApi read-only parser boundary
`parseShellAccountingExcelVbaImportReadOnly`. TASK-257 did not register a
DataService action. TASK-258 registers the read-only preview action and proves
that service-side dispatch can invoke the parser boundary without introducing
runtime integration or write behavior.

## Action Name

The DataService action is:

`accounting.excel_vba_import.readonly_preview`

The action is registered only by `registerDataServiceReadOnlyActions`. It is
not a write action and must not be added to `WriteActionPolicy`.

## Request Boundary

The action accepts only an in-memory sanitized payload object. The expected
shape is:

- `rawUserDataExposed=false`
- `importPayload.sheets[].sheetName`
- `importPayload.sheets[].headers[]`
- `importPayload.sheets[].rows[][]`

The action must not load production files, inspect user directories, read
credentials, read endpoints, use broker data, or fetch network resources.

## Preview Response

The action calls the TASK-257 parser boundary and returns:

- normalized diagnostics.
- diagnostic codes.
- replay fact counts for trade, cash, market price, and FX facts.
- read-only evidence fields.
- no raw workbook payload.
- no AccountingEngine replay result.

Diagnostics must remain sanitized and must not include raw SQL, raw import
payloads, stack traces, credentials, endpoints, broker payloads, secrets, or
real user data.

## Read-Only Boundary

TASK-258 must not write:

- SQLite production data.
- `audit_log`.
- ledger rows.
- snapshots.
- `trade_log`.
- persistent read models.
- `trade_draft`.

The action does not call AccountingEngine. The response is a parser preview and
fact summary only.

## Forbidden Scope

TASK-258 does not modify apps, production QML, startup, Presenter, Controller,
ShellServices UI wiring, AccountingEngine production code, DataAccess
repositories, migrations, or historical fixture JSON.

TASK-258 does not generate TradeDrafts, execute strategies, submit broker
orders, use broker SDKs, access networks, read credentials, read endpoints,
place real orders, or enable automatic trading.

## Evidence Contract

The CTest
`shell_accounting_excel_vba_import_readonly_dataservice_preview_action` must
emit sanitized stdout JSON evidence with:

- `task=TASK-258`
- `mode=readonly-dataservice-import-preview-action-vertical-slice`
- `dataServiceReadOnlyActionRegistered=true`
- `parserBoundaryCalled=true`
- `previewActionExecuted=true`
- `diagnosticsBuilt=true`
- `replayFactSummaryBuilt=true`
- `accountingEngineCalled=false`
- `productionRuntimeIntegrationImplemented=false`
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

Future production file loading, UI exposure, AccountingEngine runtime
integration, read model refresh, import persistence, TradeDraft generation,
broker integration, strategy execution, real order placement, or automatic
trading must be separately authorized.
