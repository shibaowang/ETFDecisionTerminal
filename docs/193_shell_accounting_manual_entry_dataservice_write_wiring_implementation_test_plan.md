# ShellAccounting Manual Entry DataService Write Wiring Implementation Test Plan

## Document Purpose

This document defines the TASK-198 test plan for ShellAccounting manual entry
DataService write wiring implementation.

The plan verifies validation-first DataService wiring into DataAccess
repositories, sanitized responses, idempotency, and retained no-broker and
no-runtime-SQL boundaries.

## Test Matrix

### Documentation And Registration

- docs/192 exists.
- docs/193 exists.
- README, docs/README, and docs/12 register TASK-198.
- tests/CMake registers `ShellAccountingManualEntryDataServiceWriteWiringImplementation`.
- TASK-197 docs record that the gate has evolved to allow TASK-198 write
  wiring.

### DataService Wiring

- Existing action names are retained:
  - `accounting.manual_transaction.create`
  - `accounting.manual_cash_movement.create`
- `DataServiceActionRegistrar.cpp` remains unchanged and already registered.
- `DataServiceActions.cpp` validates payloads before repository invocation.
- Valid manual transaction payloads call `ShellAccountingManualTransactionRepository`.
- Valid manual cash movement payloads call `ShellAccountingManualCashMovementRepository`.
- `DataServiceActions.cpp` does not scatter SQL.
- `DataServiceActions.cpp` does not directly execute `INSERT`, `UPDATE`,
  `DELETE`, or `REPLACE`.

### Runtime Writes

- Valid manual transaction writes through the repository to `trade_log` and
  `trade_execution_group`.
- Valid manual transaction does not write `cash_adjustment`, `audit_log`, or
  ledger rows.
- Valid manual cash movement writes through the repository to `trade_log` and
  `cash_adjustment`.
- Valid manual cash movement does not write `audit_log` or ledger rows.
- Invalid payload, non-object payload, and sensitive memo payload do not call
  repository write and do not write database rows.

### Response Mapping

- Manual transaction success returns `writeImplemented=true`,
  `writeEnabled=true`, `validationAccepted=true`, `databaseWritten=true`,
  `repositoryWrite=true`, `tradeLogWritten=true`,
  `cashAdjustmentWritten=false`, `auditWritten=false`, and
  `ledgerWritten=false`.
- Manual cash movement success returns `writeImplemented=true`,
  `writeEnabled=true`, `validationAccepted=true`, `databaseWritten=true`,
  `repositoryWrite=true`, `tradeLogWritten=true`,
  `cashAdjustmentWritten=true`, `auditWritten=false`, and
  `ledgerWritten=false`.
- Repository failures map to safe DataService errors.
- Duplicate idempotency keys return stable idempotent responses and do not
  duplicate rows.

### Privacy And Forbidden Integration

- Responses do not expose raw SQL, raw payloads, credentials, endpoint values,
  tokens, internal stack traces, broker order ids, or real order ids.
- No AccountingEngine replay or read model integration is triggered.
- No production QML, startup, Presenter, or Controller changes are introduced.
- No TradeDraft or suggestion implementation is introduced.
- No broker SDK, network, credentials, endpoint, real order placement, or
  automatic trading is introduced.

### Regression

- TASK-197 authorization gate passes after evolving to allow TASK-198.
- TASK-196 manual cash movement repository tests pass.
- TASK-192 manual transaction repository tests pass.
- TASK-182 validation wiring tests pass after evolving to validation-first write
  wiring.
- TASK-195 / TASK-194 gates pass.
- Broker disabled, broker order, real broker authorization, and real broker
  implementation gates pass.
- transport local socket echo 50 repeat passes.
- Full CTest passes.

## Required Probes

- Static source scan.
- Repository boundary scan.
- Direct SQL / SQLite write scan in `DataServiceActions.cpp`.
- Temp SQLite database write probe with migrations 001 and 002.
- Invalid payload no-write probe.
- Sensitive memo no-write probe.
- Idempotency duplicate probe.
- Repository failure safe error probe.
- Sanitized response probe.
- No migration / schema file probe.
- No production QML / startup / Presenter / Controller probe.
- No replay / read model probe.
- No TradeDraft / suggestion probe.
- No broker / network / credentials / endpoint / real order / automatic trading
  probe.

## Go / No-Go Checklist

- [ ] docs/192 exists.
- [ ] docs/193 exists.
- [ ] README, docs/README, and docs/12 register TASK-198.
- [ ] TASK-198 implementation CTest passes.
- [ ] TASK-197 authorization gate passes.
- [ ] TASK-196 repository tests pass.
- [ ] TASK-192 repository tests pass.
- [ ] TASK-182 validation wiring tests pass.
- [ ] TASK-195 / TASK-194 gates pass.
- [ ] Broker gates pass.
- [ ] transport 50 passes.
- [ ] Full CTest passes.
- [ ] git diff --check passes.
- [ ] git status is clean.
