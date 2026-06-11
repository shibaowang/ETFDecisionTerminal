# TASK-271 Test Plan

CTest:

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
```

The test uses a temporary SQLite database plus an in-process local DataService
action host. It uses the real DataServiceClient typed methods through the
ShellServices port adapter and does not use a fake client port.

## Assertions

- The local database is initialized from migrations and required seed data.
- The local DataService host registers read-only and write actions.
- `ShellAccountingPresenter.previewExcelVbaImportReadOnly(...)` executes
  `accounting.excel_vba_import.readonly_preview` through the real client.
- The preview reaches `ACCEPTED` with one trade fact and one cash fact.
- `ShellAccountingPresenter.persistAcceptedExcelVbaImportPreview()` executes
  `accounting.excel_vba_import.persist_manual_entry`.
- The temp database gains exactly two `trade_log` rows, one
  `cash_adjustment` row, and one `audit_log` row.
- Post-write readback refresh is attempted and reaches a successful sanitized
  Presenter state.
- Duplicate idempotent import does not grow `trade_log`, `cash_adjustment`, or
  `audit_log` rows and does not claim new rows.
- Idempotency conflict does not grow rows and does not trigger refresh.
- The local service host is closed and no residual listener remains.

## Required Evidence JSON

```json
{
  "task": "TASK-271",
  "mvpLocalServiceE2eAcceptanceCreated": true,
  "tempDbOnly": true,
  "productionDbTouched": false,
  "localServiceHostUsed": true,
  "realDataServiceClientUsed": true,
  "presenterPreviewExecuted": true,
  "previewAccepted": true,
  "presenterPersistExecuted": true,
  "persistActionCalled": true,
  "tradeLogRowsWritten": true,
  "cashAdjustmentRowsWritten": true,
  "auditLogRowsWritten": true,
  "postPersistRefreshAttempted": true,
  "postPersistRefreshSucceeded": true,
  "duplicateImportNoGrowth": true,
  "idempotencyConflictNoGrowth": true,
  "noResidualProcess": true,
  "brokerOrderSubmitted": false,
  "networkAccess": false,
  "credentialAccess": false,
  "endpointAccess": false,
  "automaticTrading": false
}
```

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_mvp_local_service_e2e_acceptance --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```
