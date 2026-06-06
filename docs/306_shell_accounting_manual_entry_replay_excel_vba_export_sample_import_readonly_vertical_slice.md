# TASK-255 ShellAccounting Manual Entry Replay Excel/VBA Export Sample Import Read-Only Vertical Slice

## Purpose

TASK-255 advances the manual entry replay work from TASK-254 fixture parity
matrix input to a sanitized Excel/VBA-style export sample import vertical
slice. The slice stays test-only and read-only:

sanitized Excel/VBA-style JSON export sample -> test-only importer/parser ->
replay fact DTOs -> AccountingEngine read-only replay -> in-memory projection
summary -> expected parity assertions -> sanitized stdout JSON evidence.

## Current State

TASK-254 proved that TASK-local fixture facts can drive AccountingEngine
read-only replay and match expected projection output across a small parity
matrix. TASK-255 keeps that read-only boundary but changes the input source to a
TASK-255-only sanitized export sample that mimics Excel/VBA sheet headers and
rows.

## Authorized Scope

- Add TASK-255-only documentation and CTest registration.
- Add a TASK-255-only sanitized Excel/VBA-style JSON export sample.
- Add a test-only importer/parser under the TASK-255 test directory.
- Build replay fact DTOs in memory.
- Call AccountingEngine read-only replay from the TASK-255 test helper.
- Compare the replay result to expected projection output.
- Emit sanitized stdout JSON evidence.
- Repair old gate exact-path or exact-fixture predicates only when required by
  TASK-255 exact paths.

## Export Sample Policy

The sample is synthetic and sanitized. It must not include real user accounts,
real trade history, real broker data, credentials, endpoints, account numbers,
tokens, secrets, or personally identifying payloads.

The importer treats the sample as an Excel/VBA-style workbook export encoded as
JSON sheets. Headers and row cells are parsed by the test helper, not by
production code.

## Read-Only Boundary

TASK-255 must not write SQLite, audit rows, ledger rows, snapshots, trade_log,
read models, TradeDrafts, or broker orders. The only output is in-memory summary
data and sanitized stdout JSON evidence.

## Forbidden Scope

TASK-255 does not modify apps, production QML, startup, Presenter, Controller,
ShellServices production wiring, DataServiceActions, DataAccess repositories,
migrations, AccountingEngine production code, or historical fixtures.

TASK-255 does not implement production Excel import, production parser /
loader / reader code, production runtime integration, read model refresh,
strategy execution, broker integration, network access, credential access,
endpoint access, real order placement, or automatic trading.

## Evidence Contract

The CTest
`shell_accounting_manual_entry_replay_excel_vba_export_sample_import_readonly_vertical_slice`
must emit sanitized stdout JSON evidence with:

- `task=TASK-255`
- `mode=test-only-excel-vba-export-sample-import-readonly-vertical-slice`
- `sampleLoaded=true`
- `importerExecuted=true`
- `replayFactsBuilt=true`
- `replayExecuted=true`
- `accountingEngineCalled=true`
- `parityPassed=true`
- `productionWrite=false`
- `sqliteProductionWrite=false`
- `auditWritten=false`
- `ledgerWritten=false`
- `snapshotWritten=false`
- `tradeLogWritten=false`
- `readModelPersistentWrite=false`
- `brokerOrderSubmitted=false`
- `networkAccess=false`
- `credentialAccess=false`
- `endpointAccess=false`
- `automaticTrading=false`
- `rawUserDataExposed=false`

## Follow-Up Boundary

Future production Excel import, production runtime integration, read model
refresh, production parser / loader / reader work, larger sample suites,
broker integration, strategy execution, real order placement, or automatic
trading must be separately authorized.
