# ShellAccounting Manual Entry MVP Runtime E2E Acceptance Test Plan

## Document Purpose

This document defines TASK-204 runtime E2E acceptance coverage for the
ShellAccounting manual entry MVP chain. TASK-204 implements tests only. It does
not add production functionality, does not modify production code, and does not
connect replay, audit, ledger, broker, network, credentials, endpoint, real
order placement, or automatic trading behavior.

## Test Matrix

### Documentation And Registration

- docs/204 exists.
- docs/205 exists.
- README, docs/README, and docs/12 register TASK-204.
- docs/202 and docs/203 mention TASK-204 runtime E2E acceptance implementation.
- tests/CMake registers `ShellAccountingManualEntryMvpRuntimeE2eAcceptance`.

### Runtime E2E Temporary DB

- Runtime acceptance uses a temporary SQLite database.
- The test database applies migrations 001 + 002.
- Test fixtures seed synthetic account, portfolio, and instrument data only.
- Tests do not use real user data.
- Tests do not perform network calls.
- Tests do not require live broker credentials or external endpoints.

### Manual Transaction E2E

- A valid manual BUY payload through the accepted DataService action boundary
  writes expected `trade_execution_group` and `trade_log` rows.
- Manual BUY does not write `cash_adjustment`.
- Duplicate idempotencyKey submission does not duplicate `trade_log` or
  `trade_execution_group`.
- Invalid manual transaction payload writes no rows.
- Response flags include `databaseWritten`, `tradeLogWritten`, and
  `cashAdjustmentWritten` as expected.

### Manual Cash Movement E2E

- A valid Deposit payload through the accepted DataService action boundary
  writes expected `trade_log` and `cash_adjustment` rows.
- `cash_adjustment.trade_log_id` references `trade_log.id`.
- Duplicate idempotencyKey submission does not duplicate `trade_log` or
  `cash_adjustment`.
- Invalid manual cash movement payload writes no rows.
- Response flags include `databaseWritten`, `tradeLogWritten`, and
  `cashAdjustmentWritten` as expected.

### Sanitized Response

- Runtime responses do not expose raw SQL.
- Runtime responses do not expose raw payload.
- Runtime responses do not expose credentials.
- Runtime responses do not expose endpoint values.
- Runtime responses do not expose internal stack trace.
- Runtime responses do not expose real broker order ids.
- Invalid and duplicate responses remain explicit and safe.

### Post-Write Readback Boundary

- QML / Presenter exposes post-write refresh status fields.
- Refresh action names remain allowlisted DataService read actions:
  `position.list`, `cash.summary`, and `portfolio.pnl.summary`.
- TASK-204 does not add new read model refresh or replay integration.

### No Production Drift

- production QML is not modified by TASK-204.
- production startup is not modified by TASK-204.
- Presenter / Controller are not modified by TASK-204.
- ShellServices adapter / port are not modified by TASK-204.
- DataServiceActions and DataServiceActionRegistrar are not modified by
  TASK-204.
- DataAccess repositories are not modified by TASK-204.
- migrations are not modified by TASK-204.
- no new migration or schema file is added.
- no new production runtime SQL / SQLite write path is added.
- no new AccountingEngine replay integration is added.
- no new audit or ledger write is added.
- no new broker SDK, network, credentials, endpoint, real order, or automatic
  trading capability is added.

### Regression

- TASK-203 acceptance authorization gate passes.
- TASK-202 post-write refresh implementation tests pass.
- TASK-200 QML Presenter implementation tests pass.
- TASK-198 DataService write wiring implementation tests pass.
- TASK-196 manual cash movement repository tests pass.
- TASK-192 manual transaction repository tests pass.
- Broker disabled, broker order, real broker authorization, and real broker
  implementation gates pass.
- transport local socket echo 50 repeat passes.
- Full CTest passes.

## Required Probes

- Documentation existence probe.
- README / docs index / prompt template registration probe.
- Temporary SQLite DB probe.
- Migration 001 + 002 application probe.
- Manual transaction valid payload write probe.
- Manual transaction duplicate idempotency probe.
- Manual transaction invalid no-write probe.
- Manual cash movement valid payload write probe.
- Manual cash movement linkage probe.
- Manual cash movement duplicate idempotency probe.
- Manual cash movement invalid no-write probe.
- Sanitized response probe.
- Post-write readback boundary probe.
- Changed-path allowlist scan.
- Production QML no-drift scan.
- Presenter / Controller no-drift scan.
- ShellServices adapter / port no-drift scan.
- DataServiceActions no-drift scan.
- DataAccess repository no-drift scan.
- Migration / schema no-drift scan.
- Runtime SQL / SQLite write scan outside tests.
- Direct SQLite / DataAccess scan in UI / Presenter.
- AccountingEngine replay scan.
- Audit / ledger write scan.
- Broker / network / credentials / endpoint scan.
- Real order id / real order placement scan.
- Automatic trading scan.
- Broker gate retention probe.
- DevDocs acceptance probe.

## Go / No-Go Checklist

- [ ] docs/204 exists.
- [ ] docs/205 exists.
- [ ] README, docs/README, and docs/12 register TASK-204.
- [ ] TASK-204 runtime E2E acceptance tests pass.
- [ ] TASK-203 authorization gate tests pass.
- [ ] TASK-202 implementation tests pass.
- [ ] TASK-200 implementation tests pass.
- [ ] TASK-198 implementation tests pass.
- [ ] TASK-196 repository tests pass.
- [ ] TASK-192 repository tests pass.
- [ ] Broker gates pass.
- [ ] transport 50 passes.
- [ ] Full CTest passes.
- [ ] git diff --check passes.
- [ ] git status is clean.
