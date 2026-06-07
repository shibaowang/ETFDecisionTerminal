# TASK-266 ShellAccounting Excel/VBA Import Persist Manual Entry Client Adapter Test Plan

## Test Objective

Prove TASK-266 adds a typed DataServiceClient adapter for
`accounting.excel_vba_import.persist_manual_entry` without adding UI wiring,
direct SQLite access, DataAccess access from the client, AccountingEngine
calls, broker access, network access beyond local DataService transport, or
automatic trading.

## Required CTest

`shell_accounting_excel_vba_import_persist_manual_entry_client_adapter`

## Required Checks

The test must verify:

- typed request mapping includes `previewStatus`, `previewDigest`,
  `idempotencyKey`, `schemaVersion`, source, accepted timestamp, import batch
  label, fact summary, and sanitized import payload.
- the typed adapter calls
  `accounting.excel_vba_import.persist_manual_entry` through local
  DataService transport.
- successful persistence maps transaction commit, trade_log write, audit_log
  write, and single-transaction evidence.
- duplicate same idempotency key and digest maps
  `duplicateImportPrevented=true` without duplicate rows.
- same idempotency key and different digest maps
  `idempotencyConflictRejected=true` with sanitized issue codes.
- non-`ACCEPTED` preview rejection maps to a typed sanitized result.
- malformed client requests fail before a DataService write call.
- client source does not include SQLite, DataAccess, AccountingEngine, broker,
  or direct SQL write dependencies.
- tests use only a temporary SQLite database through the local service harness.

## Evidence JSON

The test must emit sanitized evidence containing:

- `task=TASK-266`
- `persistClientAdapterCreated=true`
- `dataServicePersistActionCalled=true`
- `requestMapped=true`
- `responseParsed=true`
- `transactionCommittedMapped=true`
- `tradeLogWrittenMapped=true`
- `auditLogWrittenMapped=true`
- `duplicateImportMapped=true`
- `idempotencyConflictMapped=true`
- `sanitizedIssuesMapped=true`
- `clientDirectSQLite=false`
- `clientDirectDataAccess=false`
- `clientDirectAccountingEngine=false`
- `qmlWiringChanged=false`
- `presenterWiringChanged=false`
- `productionDbTouched=false`
- `brokerOrderSubmitted=false`
- `networkAccessBeyondLocalDataServiceTransport=false`
- `automaticTrading=false`

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_persist_manual_entry_client_adapter --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Formal Conclusion

TASK-266 implements only the typed client adapter and mapping tests for the
TASK-265 persistence action. It does not add QML, Presenter, Controller,
ShellServices UI wiring, DataServiceApi action implementation changes,
DataAccess changes, migrations, AccountingEngine calls, broker capability,
real order placement, or automatic trading.
