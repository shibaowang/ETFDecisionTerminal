# TASK-259 ShellAccounting Excel/VBA Import Read-Only Preview Client Adapter Test Plan

## Purpose

This test plan verifies that TASK-259 adds a typed read-only client adapter for
the TASK-258 DataService preview action without adding UI wiring, production
file loading, database writes, AccountingEngine replay, broker access, network
access, or automatic trading.

## Test Matrix

- Client adapter: `DataServiceClient` exposes a typed read-only preview wrapper.
- Action boundary: the wrapper calls
  `accounting.excel_vba_import.readonly_preview`.
- Response mapping: task, mode, action, accepted status, diagnostics, diagnostic
  codes, replay fact summary, and evidence flags map into typed fields.
- Valid sample: accepted preview maps one cash fact and one trade fact.
- Invalid sample: missing required header maps sanitized diagnostic codes and
  zero replay fact counts.
- Response parser: malformed or incomplete preview payloads fail closed.
- Client guard: raw SQL payload fields are rejected before dispatch.
- No writes: protected SQLite table row counts remain unchanged around preview
  calls.
- No UI wiring: apps, QML, startup, Presenter, and Controller remain untouched.
- No replay or trading: AccountingEngine, broker, network, credentials,
  endpoints, real order placement, and automatic trading remain absent.

## Required CTest

Run:

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_preview_client_adapter --output-on-failure
```

The CTest must pass and emit TASK-259 evidence with
`previewClientAdapterCreated=true`, `dataServicePreviewActionCalled=true`,
`previewResponseParsed=true`, `diagnosticsMapped=true`,
`replayFactSummaryMapped=true`, `accountingEngineCalled=false`,
`qmlWiringChanged=false`, `productionFileLoading=false`,
`productionWrite=false`, `sqliteProductionWrite=false`, `auditWritten=false`,
`ledgerWritten=false`, `snapshotWritten=false`, `tradeLogWritten=false`,
`readModelPersistentWrite=false`, `tradeDraftGenerated=false`,
`brokerOrderSubmitted=false`, `networkAccess=false`,
`credentialAccess=false`, `endpointAccess=false`, `automaticTrading=false`,
and `rawUserDataExposed=false`.

## Required Regression Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_preview_client_adapter --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Static Review Probes

- DataService action name remains
  `accounting.excel_vba_import.readonly_preview`.
- TASK-259 client adapter code must not contain SQLite write SQL tokens such as
  `INSERT`, `UPDATE`, `DELETE`, `REPLACE`, or `CREATE TABLE`.
- TASK-259 client adapter code must not call AccountingEngine.
- TASK-259 client adapter code must not access broker, network, credentials, or
  endpoints.
- TASK-259 fixtures must be local to the TASK-259 CTest directory.
- Historical fixture JSON must remain unchanged.

## Go / No-Go Checklist

- GO only if the targeted TASK-259 CTest passes.
- GO only if full CTest passes.
- GO only if transport repeat 50 passes.
- GO only if `git diff --check` passes.
- GO only if `git status --short` is clean after commit.
- NO-GO if apps, production QML, startup, Presenter, Controller,
  AccountingEngine production code, migrations, historical fixtures, broker
  code, network code, credentials, endpoints, real order placement, automatic
  trading, or production writes are added.
