# TASK-258 ShellAccounting Excel/VBA Import Read-Only DataService Preview Action Test Plan

## Purpose

This test plan verifies that TASK-258 registers a read-only DataService preview
action for sanitized Excel/VBA-style import payloads and that the action calls
the TASK-257 parser boundary without adding writes, replay execution, UI
wiring, broker access, network access, or automatic trading.

## Test Matrix

- Action registration: `accounting.excel_vba_import.readonly_preview` is
  registered by `registerDataServiceReadOnlyActions`.
- Write policy: the action is not registered by write action policy.
- Parser boundary: action invokes `parseShellAccountingExcelVbaImportReadOnly`.
- Valid sample: accepted payload returns one cash fact and one trade fact in
  preview summary.
- Invalid sample: missing required header returns sanitized diagnostics and no
  replay facts.
- Sanitization: response does not include raw workbook payload or memo text.
- No AccountingEngine: preview response reports `accountingEngineCalled=false`.
- No writes: protected SQLite table row counts remain unchanged around the
  action calls.
- No runtime integration: preview response reports
  `productionRuntimeIntegrationImplemented=false`.
- No trading: preview response reports no broker, network, credential,
  endpoint, real order, strategy, or automatic trading behavior.

## Required CTest

Run:

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_dataservice_preview_action --output-on-failure
```

The CTest must pass and emit TASK-258 evidence with
`dataServiceReadOnlyActionRegistered=true`, `parserBoundaryCalled=true`,
`previewActionExecuted=true`, `diagnosticsBuilt=true`,
`replayFactSummaryBuilt=true`, `accountingEngineCalled=false`,
`productionRuntimeIntegrationImplemented=false`, `productionWrite=false`,
`sqliteProductionWrite=false`, `auditWritten=false`, `ledgerWritten=false`,
`snapshotWritten=false`, `tradeLogWritten=false`,
`readModelPersistentWrite=false`, `tradeDraftGenerated=false`,
`strategyExecuted=false`, `brokerOrderSubmitted=false`, `networkAccess=false`,
`credentialAccess=false`, `endpointAccess=false`, `automaticTrading=false`,
and `rawUserDataExposed=false`.

## Required Regression Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_dataservice_preview_action --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Static Review Probes

- `DataServiceActionRegistrar.cpp` registers the action only in read-only
  action registration.
- `WriteActionPolicy.cpp` does not include the preview action.
- DataService preview action source must not contain SQLite write tokens such
  as `INSERT`, `UPDATE`, `DELETE`, `REPLACE`, or `CREATE TABLE`.
- Preview action source must not call AccountingEngine.
- Preview action source must not access broker, network, credentials, or
  endpoints.
- TASK-258 fixtures must be local to the TASK-258 CTest directory.
- Historical fixture JSON must remain unchanged.

## Go / No-Go Checklist

- GO only if the targeted TASK-258 CTest passes.
- GO only if full CTest passes.
- GO only if transport repeat 50 passes.
- GO only if `git diff --check` passes.
- GO only if `git status --short` is clean after commit.
- NO-GO if apps, production QML, startup, Presenter, Controller,
  AccountingEngine production code, migrations, historical fixtures, broker
  code, network code, credentials, endpoints, real order placement, automatic
  trading, or production writes are added.
