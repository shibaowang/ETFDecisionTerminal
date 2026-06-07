# TASK-269 Test Plan

CTest:

`shell_accounting_excel_vba_import_persist_supported_fact_types_alignment`

## Required Assertions

- Manual cash movement repository exposes an active-transaction-only method and
  preserves its existing public API behavior.
- Cash-only accepted preview persists through the manual cash movement path.
- Trade+cash accepted preview persists both fact types and one sanitized audit
  marker in a single transaction.
- Duplicate import does not duplicate trade, cash adjustment, or audit rows.
- Market price and FX rate persistence fail closed with sanitized issue codes.
- Cash failure rolls back trade and audit rows.
- Audit failure rolls back trade and cash rows.
- DataService action does not contain direct `INSERT INTO trade_log`.
- QML and Presenter disable or reject persistence when market/FX fact counts
  are non-zero.

## Evidence JSON

The test prints compact JSON containing:

```json
{
  "task": "TASK-269",
  "cashFactsPersistenceSupported": true,
  "cashOnlyAcceptedPreviewPersists": true,
  "tradeAndCashPersistTogether": true,
  "manualCashMovementRepositoryUsed": true,
  "cashMovementActiveTransactionMethodCreated": true,
  "singleTransactionBoundaryUsed": true,
  "cashFailureRollsBackTradeAndAudit": true,
  "auditFailureRollsBackTradeAndCash": true,
  "marketPricePersistenceRejected": true,
  "fxRatePersistenceRejected": true,
  "qmlPersistDisabledForUnsupportedMarketFx": true,
  "presenterRejectsUnsupportedMarketFx": true,
  "duplicateImportPreventsTradeCashAuditDuplicates": true,
  "directTradeLogSqlInDataService": false,
  "qmlDirectSQLite": false,
  "qmlDirectDataServiceClient": false,
  "accountingEngineCalled": false,
  "brokerOrderSubmitted": false,
  "automaticTrading": false
}
```

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_persist_supported_fact_types_alignment --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```
