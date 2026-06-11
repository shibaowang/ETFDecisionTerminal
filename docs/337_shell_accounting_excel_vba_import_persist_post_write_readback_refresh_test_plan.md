# TASK-270 Test Plan

CTest:

```powershell
ctest --test-dir build -R shell_accounting_excel_vba_import_persist_post_write_readback_refresh --output-on-failure
```

The test uses a fake ShellServices DataService client port and does not touch a
production database.

## Assertions

- A successful accepted preview persistence response sets persisted row-count
  state and calls the existing read-only position, cash, and PnL refresh path.
- Duplicate idempotent import maps to `DUPLICATE`, reports zero new
  `trade_log` and `cash_adjustment` rows, and does not call readback refresh.
- Idempotency conflict maps to a sanitized conflict state, reports zero new
  rows, and does not call readback refresh.
- Refresh failure after successful persistence leaves persistence state intact
  and shows a sanitized post-persist refresh issue.
- QML shows persisted row counts and post-persist refresh status / summary /
  issue through Presenter properties only.
- QML does not directly reference DataServiceClient, DataServiceApi, SQLite,
  AccountingEngine, credentials, endpoint, or raw persistence action names.

## Required Evidence JSON

```json
{
  "task": "TASK-270",
  "persistSuccessTriggersReadbackRefresh": true,
  "postPersistRefreshAttempted": true,
  "postPersistRefreshSucceeded": true,
  "persistedTradeLogRowsVisible": true,
  "persistedCashAdjustmentRowsVisible": true,
  "duplicateImportDoesNotClaimNewRows": true,
  "idempotencyConflictDoesNotRefresh": true,
  "refreshFailureSanitized": true,
  "readOnlyRefreshNoWrite": true,
  "qmlDirectSQLite": false,
  "qmlDirectDataServiceClient": false,
  "accountingEngineProductionCodeChanged": false,
  "brokerOrderSubmitted": false,
  "automaticTrading": false
}
```

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_persist_post_write_readback_refresh --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```
