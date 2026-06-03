# ShellAccounting Manual Entry Post-Write Readback Refresh Implementation Test Plan

## Document Purpose

This document defines TASK-202 test coverage for the ShellAccounting manual
entry post-write readback refresh implementation.

The tests verify a minimal implementation through existing DataService read
boundaries only. They must not depend on real user data, trigger network calls,
call broker code, write audit or ledger rows, or execute AccountingEngine
replay.

## Test Matrix

### Documentation And Registration

- docs/200 exists.
- docs/201 exists.
- README, docs/README, and docs/12 register TASK-202.
- docs/198 and docs/199 mention the TASK-202 implementation update.
- tests/CMake registers `ShellAccountingManualEntryPostWriteReadbackRefreshImplementation`.

### QML Status Boundary

- Production QML has post-write refresh status UI.
- QML object names are stable:
  - `shellAccountingPostWriteRefreshStatus`
  - `shellAccountingPostWriteRefreshIssue`
  - `shellAccountingPostWriteRefreshSummary`
- QML does not call DataServiceClient directly.
- QML does not access SQLite.
- QML does not import or call DataAccess.
- QML does not calculate position, cash, PnL, or derived accounting facts.
- QML does not mention broker order submission, real order placement, or
  automatic trading.

### Presenter And Controller Boundary

- `ShellAccountingPresenter` exposes post-write refresh status properties.
- `ShellAccountingPresenter` exposes reset/readback invokables.
- Successful manual transaction writes trigger post-write refresh.
- Successful manual cash movement writes trigger post-write refresh.
- Validation failures do not trigger refresh.
- Write failures do not trigger refresh.
- Manual transaction refresh uses only:
  - `position.list`
  - `cash.summary`
  - `portfolio.pnl.summary`
- Manual cash movement refresh uses only:
  - `cash.summary`
  - `portfolio.pnl.summary`
- Presenter / Controller do not include DataAccess repository headers.
- Presenter / Controller do not execute SQL.
- Presenter / Controller do not read SQLite directly.
- Presenter / Controller do not call AccountingEngine replay.
- Presenter / Controller do not call broker, network, endpoint, or credentials
  paths.

### No Production Drift Outside Scope

- DataServiceActions.cpp is not modified by TASK-202.
- DataServiceActions.h is not modified by TASK-202.
- DataServiceActionRegistrar.cpp is not modified by TASK-202.
- TASK-192 repository files are not modified.
- TASK-196 repository files are not modified.
- migrations/001 and migrations/002 are not modified.
- No migration or schema file is added.
- No audit_log or ledger write is introduced.
- No TradeDraft or suggestion behavior is introduced.
- No snapshot rebuild after write is introduced.
- No derived table refresh after write is introduced.
- No real broker order id is introduced.
- No real order placement is introduced.
- No automatic trading is introduced.

### Regression

- TASK-201 authorization gate passes after allowing TASK-202 implementation.
- TASK-200 QML Presenter implementation tests pass.
- TASK-198 DataService write wiring implementation tests pass.
- TASK-196 manual cash movement repository tests pass.
- TASK-192 manual transaction repository tests pass.
- Broker disabled, broker order, real broker authorization, and real broker
  implementation gates pass.
- transport local socket echo 50 repeat passes.
- Full CTest passes.

## Required Probes

- Static source scan.
- Changed-path allowlist scan.
- QML objectName scan.
- Presenter property and invokable scan.
- Presenter post-write success-only refresh scan.
- Controller DataService read action scan.
- DataServiceActions no-drift scan.
- DataAccess repository no-drift scan.
- Migration / schema no-drift scan.
- Direct SQLite / DataAccess scan.
- AccountingEngine replay scan.
- Snapshot rebuild / derived table refresh scan.
- Audit / ledger write scan.
- Broker / network / credentials / endpoint scan.
- Real order id / real order placement scan.
- Automatic trading scan.
- Documentation policy probe.
- DevDocs acceptance probe.

## Go / No-Go Checklist

- [ ] docs/200 exists.
- [ ] docs/201 exists.
- [ ] README, docs/README, and docs/12 register TASK-202.
- [ ] TASK-202 implementation tests pass.
- [ ] TASK-201 authorization gate passes.
- [ ] TASK-200 QML Presenter implementation tests pass.
- [ ] TASK-198 DataService write wiring tests pass.
- [ ] TASK-196 repository tests pass.
- [ ] TASK-192 repository tests pass.
- [ ] Broker gates pass.
- [ ] transport 50 passes.
- [ ] Full CTest passes.
- [ ] git diff --check passes.
- [ ] git status is clean.
