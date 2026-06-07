# TASK-267 ShellAccounting Excel/VBA Import Persist Manual Entry ShellServices Presenter Contract Test Plan

## Test Objective

Prove TASK-267 adds a ShellServices / Controller / Presenter C++ contract for
accepted Excel/VBA import preview persistence while keeping QML, production DB,
DataServiceApi implementation, DataServiceClient implementation, DataAccess,
AccountingEngine, broker, and automatic trading untouched.

## Required CTest

`shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract`

## Required Checks

The test must verify:

- ShellServices exposes a persist contract for
  `accounting.excel_vba_import.persist_manual_entry`.
- the Presenter has a production C++ persist method that is not QML invokable.
- the Controller has a persist method.
- `ShellAccountingDataServiceClientPortAdapter` calls the TASK-266 typed
  DataServiceClient adapter through the port boundary.
- Presenter and Controller do not directly include or call DataServiceClient,
  DataServiceApi, DataAccess, SQLite, or AccountingEngine.
- only `ACCEPTED` preview state can call the fake client port.
- non-`ACCEPTED` preview state fails closed before the client port is called.
- missing preview digest fails closed before the client port is called.
- missing idempotency key fails closed before the client port is called.
- malformed sanitized payload JSON fails closed before the client port is
  called.
- success maps transaction commit, trade_log write, audit_log write, and
  sanitized view-model summary.
- duplicate import maps `duplicateImportPrevented`.
- idempotency conflict maps `idempotencyConflictRejected` and sanitized issue
  code.
- tests use a fake client port and do not touch production DB.
- QML does not call the persist method and does not reference the persist
  action name.

## Evidence JSON

The test must emit sanitized evidence containing:

- `task=TASK-267`
- `shellServicesPersistContractCreated=true`
- `presenterPersistMethodCreated=true`
- `controllerPersistMethodCreated=true`
- `typedClientPersistAdapterCalledThroughPort=true`
- `presenterDirectDataServiceClient=false`
- `controllerDirectDataServiceClient=false`
- `acceptedPreviewRequired=true`
- `nonAcceptedPreviewFailsClosed=true`
- `missingDigestFailsClosed=true`
- `missingIdempotencyKeyFailsClosed=true`
- `persistViewModelMapped=true`
- `transactionCommittedMapped=true`
- `tradeLogWrittenMapped=true`
- `auditLogWrittenMapped=true`
- `duplicateImportMapped=true`
- `idempotencyConflictMapped=true`
- `sanitizedIssuesMapped=true`
- `fakeClientPortUsed=true`
- `productionDbTouched=false`
- `qmlWiringChanged=false`
- `importButtonAdded=false`
- `accountingEngineCalled=false`
- `brokerOrderSubmitted=false`
- `networkAccessBeyondLocalDataServiceTransport=false`
- `automaticTrading=false`

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_persist_manual_entry_shellservices_presenter_contract --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Formal Conclusion

TASK-267 adds only the ShellServices / Controller / Presenter C++ contract and
fake-port mapping tests for accepted preview persistence. It does not add QML
wiring, UI buttons, DataServiceApi action implementation changes,
DataServiceClient implementation changes, DataAccess changes, migrations,
AccountingEngine calls, broker capability, real order placement, or automatic
trading.
