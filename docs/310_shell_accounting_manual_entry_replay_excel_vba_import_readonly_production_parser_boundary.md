# TASK-257 ShellAccounting Manual Entry Replay Excel/VBA Import Read-Only Production Parser Boundary Slice

## Purpose

TASK-257 moves the Excel/VBA-style import proof from a test-only importer to a
minimal production parser boundary while keeping replay execution test-only and
read-only:

sanitized Excel/VBA-style import payload -> production read-only parser
boundary -> normalized diagnostics -> replay fact DTOs -> test-only CTest
AccountingEngine read-only replay -> in-memory projection / diagnostic
assertions -> sanitized stdout JSON evidence.

## Current State

TASK-256 proved a test-only Excel/VBA-style sample matrix can be loaded,
diagnosed, converted to replay fact DTOs, replayed through AccountingEngine
read-only replay, and compared against expected sanitized projection or issue
output. TASK-257 adds only a production parser boundary for read-only parsing
and diagnostic normalization. It does not add production runtime integration.

## Parser Boundary

The parser boundary lives in DataServiceApi and is named explicitly as
read-only. It accepts in-memory import payload DTOs and returns normalized
parser results:

- sheet / table identity detection.
- TradeLog-style header detection.
- stable field and Chinese header mapping.
- replay facts DTO construction.
- sanitized diagnostics for missing required headers.
- sanitized diagnostics for invalid action, non-numeric amount, and missing
  cash fields.

The parser boundary does not register a DataService action, does not read or
write files, does not access SQLite, and does not call AccountingEngine. The
TASK-257 CTest calls AccountingEngine read-only replay only after parser output
is produced.

## Sample Matrix

TASK-257 owns all sanitized samples under its local CTest directory. The sample
set covers:

- valid buy-only import.
- valid Chinese-header buy plus partial-sell import.
- valid cash-adjustment import with expected sanitized replay issue behavior.
- missing required header rejection.
- invalid action, non-numeric amount, and missing cash field diagnostics.

Samples are synthetic and sanitized. They must not contain real user accounts,
real trade history, real broker data, credentials, endpoints, account numbers,
tokens, secrets, or personally identifying payloads.

## Diagnostics Boundary

Diagnostics contain only normalized fields such as level, code, field, sheet
name, and row id. Diagnostics must not include raw user payloads, raw SQL,
stack traces, credentials, endpoints, broker payloads, or secrets.

Invalid samples must be rejected before AccountingEngine replay. Accepted
samples may be replayed only by the TASK-257 test helper and must match the
expected in-memory projection or sanitized issue summary.

## Read-Only Boundary

TASK-257 must not write SQLite, audit rows, ledger rows, snapshots, trade_log,
read models, TradeDrafts, or broker orders. The only output is in-memory parser
and replay summary data plus sanitized stdout JSON evidence.

## Forbidden Scope

TASK-257 does not modify apps, production QML, startup, Presenter, Controller,
ShellServices production wiring, DataService action registration, DataAccess
repositories, migrations, AccountingEngine production code, or historical
fixtures.

TASK-257 does not implement production Excel import runtime integration,
production parser file loading, read model refresh, strategy execution, broker
integration, network access, credential access, endpoint access, real order
placement, or automatic trading.

## Evidence Contract

The CTest
`shell_accounting_manual_entry_replay_excel_vba_import_readonly_production_parser_boundary`
must emit sanitized stdout JSON evidence with:

- `task=TASK-257`
- `mode=readonly-production-parser-boundary-vertical-slice`
- `productionParserBoundaryCreated=true`
- `parserReadOnly=true`
- `parserNoSqliteAccess=true`
- `parserNoWrite=true`
- `sampleMatrixLoaded=true`
- `sampleCount>=5`
- `parserExecuted=true`
- `diagnosticsBuilt=true`
- `replayFactsBuilt=true`
- `accountingEngineCalledFromTestOnly=true`
- `replayExecutedFromTestOnly=true`
- `validSamplesParityPassed=true`
- `invalidSamplesRejected=true`
- `diagnosticsSanitized=true`
- `rawUserDataExposed=false`
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

## Follow-Up Boundary

Future production Excel import runtime integration, file loading, UI exposure,
read model refresh, larger sample suites, broker integration, strategy
execution, real order placement, or automatic trading must be separately
authorized.
