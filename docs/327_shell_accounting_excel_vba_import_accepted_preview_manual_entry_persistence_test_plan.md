# TASK-265 ShellAccounting Excel/VBA Import Accepted Preview Manual Entry Persistence Test Plan

## Test Objective

Prove TASK-265 implements the accepted Excel/VBA import preview persistence
slice through a DataService action and a DataAccess single-transaction
composition boundary, without UI wiring, direct DataService `trade_log` SQL,
production database writes, broker access, network access, or automatic
trading.

## Required CTest

`shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence`

## Required Checks

The test must verify:

- `accounting.excel_vba_import.persist_manual_entry` is registered as a
  DataService write action.
- only `ACCEPTED` previews can enter persistence.
- `previewDigest` and `idempotencyKey` are required.
- `ShellAccountingExcelVbaImportReadOnlyParser` is reused.
- the DataService action does not contain direct `INSERT INTO trade_log`.
- the manual transaction repository active-transaction method fails closed
  outside an active transaction.
- the existing `persistManualTransaction()` API still commits standalone
  manual transaction writes.
- manual transaction facts and sanitized audit event commit together in one
  outer transaction.
- audit failure rolls back manual transaction facts.
- trade fact failure rolls back audit and import marker effects.
- duplicate same idempotency key and digest does not duplicate trade or audit
  rows.
- same idempotency key and different digest returns a sanitized conflict.
- tests write only temporary SQLite databases.

## Evidence JSON

The test must emit sanitized evidence containing:

- `task=TASK-265`
- `dataServicePersistActionRegistered=true`
- `acceptedPreviewRequired=true`
- `parserBoundaryReused=true`
- `manualTransactionRepositoryUsed=true`
- `directTradeLogSqlInDataService=false`
- `tempDbOnly=true`
- `productionDbTouched=false`
- `transactionCommitted=true`
- `tradeLogWritten=true`
- `auditLogWritten=true`
- `idempotencyRequired=true`
- `duplicateImportPrevented=true`
- `idempotencyConflictRejected=true`
- `rollbackOnTradeFailureVerified=true`
- `rollbackOnAuditFailureVerified=true`
- `nonAcceptedPreviewRejected=true`
- `supplementalDataAccessTransactionAuthorizationUsed=true`
- `manualTransactionActiveTransactionMethodCreated=true`
- `existingManualTransactionApiPreserved=true`
- `dataAccessCompositionRepositoryCreated=true`
- `singleTransactionBoundaryUsed=true`
- `manualFactsAndAuditCommittedTogether=true`
- `auditFailureRollsBackManualFacts=true`
- `directTradeLogSqlInDataService=false`
- `nestedTransactionAttempted=false`
- `qmlWiringChanged=false`
- `brokerOrderSubmitted=false`
- `networkAccess=false`
- `automaticTrading=false`

## Required Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R shell_accounting_excel_vba_import_accepted_preview_manual_entry_persistence --output-on-failure
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
git diff --check
git status --short
```

## Formal Conclusion

TASK-265 implements only accepted preview manual entry persistence through the
DataService and DataAccess repository boundaries. It does not add Import UI,
does not touch production databases in tests, does not call AccountingEngine
replay, and does not add broker, network, real order placement, or automatic
trading capability.
