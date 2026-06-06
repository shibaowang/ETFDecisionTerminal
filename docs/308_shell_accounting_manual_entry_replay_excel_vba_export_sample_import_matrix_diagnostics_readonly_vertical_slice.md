# TASK-256 ShellAccounting Manual Entry Replay Excel/VBA Export Sample Import Matrix Diagnostics Read-Only Vertical Slice

## Purpose

TASK-256 extends TASK-255 from one sanitized Excel/VBA-style export sample to a
small test-only import matrix with normalized diagnostics. The slice remains
read-only:

sanitized Excel/VBA-style sample matrix -> test-only importer/parser ->
normalized import diagnostics -> replay fact DTOs -> AccountingEngine read-only
replay -> in-memory projection summaries -> per-sample parity / diagnostic
assertions -> sanitized stdout JSON evidence.

## Current State

TASK-255 proved that one sanitized Excel/VBA-style JSON export sample can be
loaded, parsed into replay fact DTOs, replayed through AccountingEngine
read-only replay, and compared to expected projection output. TASK-256 expands
that proof to a matrix with valid samples, replay issue samples, cash adjustment
unsupported coverage, and an importer diagnostic failure.

## Sample Matrix

TASK-256 owns all samples in its local test directory. The minimum matrix is:

- `buy_only_export_sample`
- `buy_partial_sell_export_sample`
- `cash_adjustment_export_sample`
- `missing_required_header_sample`
- `unsupported_or_issue_sample`

The samples are synthetic and sanitized. They must not contain real user
accounts, real trade history, real broker data, credentials, endpoints, account
numbers, tokens, secrets, or personally identifying payloads.

## Diagnostics Boundary

Diagnostics are normalized test-only objects containing only fields such as
level, code, field, sheet name, and row id. Diagnostics must not include raw
payloads, SQL, stack traces, credentials, endpoints, broker payloads, or raw
user data.

The missing required header sample must be rejected by the importer before
AccountingEngine replay. Accepted samples may call AccountingEngine read-only
replay and must match either expected projection output or expected sanitized
issue output.

## Read-Only Boundary

TASK-256 must not write SQLite, audit rows, ledger rows, snapshots, trade_log,
read models, TradeDrafts, or broker orders. The only output is in-memory summary
data and sanitized stdout JSON evidence.

## Forbidden Scope

TASK-256 does not modify apps, production QML, startup, Presenter, Controller,
ShellServices production wiring, DataServiceActions, DataAccess repositories,
migrations, AccountingEngine production code, or historical fixtures.

TASK-256 does not implement production Excel import, production parser /
loader / reader code, production runtime integration, read model refresh,
strategy execution, broker integration, network access, credential access,
endpoint access, real order placement, or automatic trading.

## Evidence Contract

The CTest
`shell_accounting_manual_entry_replay_excel_vba_export_sample_import_matrix_diagnostics_readonly_vertical_slice`
must emit sanitized stdout JSON evidence with:

- `task=TASK-256`
- `mode=test-only-excel-vba-export-sample-import-matrix-diagnostics-readonly-vertical-slice`
- `sampleMatrixLoaded=true`
- `sampleCount>=5`
- `importerExecuted=true`
- `diagnosticsBuilt=true`
- `replayFactsBuilt=true`
- `replayExecuted=true`
- `accountingEngineCalled=true`
- `validSamplesParityPassed=true`
- `invalidSamplesRejected=true`
- `diagnosticsSanitized=true`
- `rawUserDataExposed=false`
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

## Follow-Up Boundary

Future production Excel import, production runtime integration, read model
refresh, production parser / loader / reader work, larger sample suites,
broker integration, strategy execution, real order placement, or automatic
trading must be separately authorized.
