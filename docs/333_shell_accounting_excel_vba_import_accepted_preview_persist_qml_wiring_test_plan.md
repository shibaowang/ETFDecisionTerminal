# TASK-268 Test Plan

CTest:

`shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring`

The test validates the TASK-268 QML wiring and Presenter wrapper contract.

## Required Assertions

- QML persist panel, confirmation control, persist button, persist status,
  summary, issue codes, duplicate state, and idempotency conflict state exist.
- QML calls `persistAcceptedExcelVbaImportPreview()` on the Presenter only.
- QML does not reference DataServiceClient, DataServiceApi, SQLite,
  AccountingEngine, or `accounting.excel_vba_import.persist_manual_entry`.
- Persist enablement requires Presenter availability, `ACCEPTED` preview state,
  non-empty Presenter preview digest, available sanitized preview payload,
  at least one preview fact, no busy state, and explicit confirmation.
- Reset preview clears the QML confirmation state and Presenter persist state.
- Presenter exposes `persistAcceptedExcelVbaImportPreview()` as `Q_INVOKABLE`.
- Presenter generates a stable idempotency key from the accepted preview digest.
- Non-`ACCEPTED` preview state fails closed and does not call the fake client
  port.
- Duplicate import and idempotency conflict responses are mapped into sanitized
  Presenter state.

## Evidence JSON

The test must print compact JSON containing:

```json
{
  "task": "TASK-268",
  "qmlPersistButtonWired": true,
  "presenterPersistInvokableCreated": true,
  "acceptedPreviewRequired": true,
  "explicitUserConfirmationRequired": true,
  "persistButtonDisabledBeforeAccepted": true,
  "persistButtonDisabledWithoutConfirmation": true,
  "persistButtonEnabledAfterAcceptedAndConfirmed": true,
  "presenterPersistMethodInvokedFromQml": true,
  "persistStatusVisible": true,
  "persistSummaryVisible": true,
  "persistIssueCodesVisible": true,
  "duplicateImportVisible": true,
  "idempotencyConflictVisible": true,
  "resetClearsPersistState": true,
  "qmlDirectDataServiceClient": false,
  "qmlDirectDataServiceApi": false,
  "qmlDirectSQLite": false,
  "qmlDirectAccountingEngine": false,
  "automaticPersistence": false,
  "brokerOrderSubmitted": false,
  "networkAccessBeyondLocalDataServiceTransport": false,
  "automaticTrading": false
}
```

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_accepted_preview_persist_qml_wiring --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```
