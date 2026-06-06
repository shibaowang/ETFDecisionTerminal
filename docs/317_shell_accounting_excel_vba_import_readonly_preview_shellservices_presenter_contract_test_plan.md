# TASK-260 ShellAccounting Excel/VBA Import Read-Only Preview ShellServices Presenter Contract Test Plan

## Purpose

This test plan verifies that TASK-260 maps the existing read-only Excel/VBA
import preview action through ShellServices and presenter state without adding
QML wiring, startup wiring, production file loading, database writes,
AccountingEngine replay, broker access, network access, or automatic trading.

## Test Matrix

- Presenter contract: `ShellAccountingPresenter` exposes a preview method and
  UI-ready status, issue, diagnostic code, and fact count fields.
- Controller contract: `ShellAccountingReadOnlyController` delegates preview to
  `ShellAccountingServiceAdapter`.
- Service adapter mapping: `ShellAccountingDataServiceAdapter` maps preview
  request and response fields through `ShellAccountingDataServiceClientPort`.
- Typed client boundary: only `ShellAccountingDataServiceClientPortAdapter`
  calls `DataServiceClient.accountingExcelVbaImportReadOnlyPreview`.
- Valid sample: accepted preview maps one cash fact and one trade fact.
- Invalid sample: missing required header maps sanitized diagnostic codes and
  zero replay fact counts.
- Local fail-closed: malformed JSON and file-path style inputs fail closed
  before DataService dispatch.
- No writes: protected SQLite table row counts remain unchanged around preview
  calls.
- No production wiring: apps, QML, startup, production file loading,
  AccountingEngine replay, broker, network, credentials, endpoints, real order
  placement, and automatic trading remain absent.

## Required CTest

Run:

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract --output-on-failure
```

The CTest must pass and emit TASK-260 evidence with
`shellServicesPreviewContractCreated=true`,
`presenterPreviewMethodCreated=true`, `controllerPreviewMethodCreated=true`,
`serviceAdapterPreviewMapped=true`, `dataServiceClientPortExtended=true`,
`typedClientPreviewAdapterCalled=true`, `previewViewModelMapped=true`,
`diagnosticsMapped=true`, `replayFactSummaryMapped=true`,
`malformedInputFailsClosed=true`, `accountingEngineCalled=false`,
`qmlWiringChanged=false`, `startupWiringChanged=false`,
`productionFileLoading=false`, `productionWrite=false`,
`sqliteProductionWrite=false`, `auditWritten=false`, `ledgerWritten=false`,
`snapshotWritten=false`, `tradeLogWritten=false`,
`readModelPersistentWrite=false`, `tradeDraftGenerated=false`,
`strategyExecuted=false`, `brokerOrderSubmitted=false`,
`networkAccess=false`, `credentialAccess=false`, `endpointAccess=false`,
`automaticTrading=false`, and `rawUserDataExposed=false`.

## Required Regression Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_readonly_preview_shellservices_presenter_contract --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Static Review Probes

- Presenter and controller must not include `DataServiceClient`.
- Presenter and controller must not include DataServiceApi, DataAccess,
  AccountingEngine, or SQLite dependencies.
- `ShellAccountingDataServiceClientPortAdapter` must contain the typed preview
  client call.
- TASK-260 code must not contain SQLite write SQL tokens such as `INSERT`,
  `UPDATE`, `DELETE`, or `REPLACE`.
- TASK-260 fixtures must be local to the TASK-260 CTest directory.
- Historical fixture JSON must remain unchanged.

## Go / No-Go Checklist

- GO only if the targeted TASK-260 CTest passes.
- GO only if full CTest passes.
- GO only if transport repeat 50 passes.
- GO only if `git diff --check` passes.
- GO only if `git status --short` is clean after commit.
- NO-GO if apps, production QML, startup, production file loading,
  AccountingEngine production code, migrations, historical fixtures, broker
  code, network code, credentials, endpoints, real order placement, automatic
  trading, or production writes are added.
