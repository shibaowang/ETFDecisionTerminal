# ShellAccounting DataService Read-only Facts Query Test Plan

## Document Purpose

TASK-136 defines the future DataService read-only facts query implementation
test plan. This document does not implement query. This document does not access
SQLite. This document does not call AccountingEngine.

TASK-137 update: the read-only facts query implementation now exists. This test
plan remains the regression matrix for keeping that implementation SELECT-only,
no-write, no-snapshot-rebuild, no-AccountingEngine-replay, and no-TradeDraft.

TASK-138 adds the replay snapshot rebuild gate. TASK-137 facts query remains
the read-only fallback, while AccountingEngine replay and snapshot rebuild still
require separate authorization and tests.

## Test Matrix

### TASK-137 Post-implementation Matrix

- five authorized action success fixtures.
- empty data mapping.
- missing data mapping.
- query error mapping.
- no write SQL scan.
- no snapshot rebuild scan.
- no AccountingEngine replay scan.
- privacy / no raw payload scan.
- rollback / disable facts query policy.

### A. Read-only SQL / Facts Boundary

- SELECT-only policy.
- no INSERT / UPDATE / DELETE / REPLACE.
- no DROP / ALTER / CREATE TABLE.
- no transaction write.
- no snapshot rebuild.
- no trade_log mutation.

### B. Query Behavior

- success fixture.
- empty account.
- missing account.
- missing portfolio.
- missing instrument.
- stale snapshot.
- data inconsistency.
- query failure.
- query error.
- timeout.
- no silent success.
- no fake data.

### C. Data Source Policy

- allowed tables explicit.
- `trade_log` read-only fact source.
- `position_snapshot` read-only derived cache.
- `cash_snapshot` read-only derived cache.
- `portfolio_summary` read-only derived cache.
- `base_position_allocation` read-only base allocation fact.
- `trade_tier_allocation` read-only tier allocation fact.
- `grid_cycle` read-only cycle context if needed.
- strategy / portfolio / account metadata read-only context if needed.
- no unauthorized table read.
- no direct shell-side SQLite.
- no presenter-side DataAccess.
- no QML-side data query.

### D. AccountingEngine Boundary

- no AccountingReplayEngine call.
- no replayFromFacts.
- no replay rebuild.
- no rebuild summary.
- no correction / reversal behavior.
- replay integration must be separate TASK.

### E. Privacy

- no raw SQL in QML.
- no full trade_log payload in QML.
- no sensitive raw payload in issue text.
- no raw facts dump in default logs.
- no full account cash history in QML error.

### F. Regression

- TASK-119 through TASK-135 pass.
- real data adapter tests pass.
- full tests pass.
- transport 50 passes.

## Required Fakes / Probes

- controlled read-only fixture.
- empty fixture.
- missing account probe.
- missing portfolio probe.
- missing instrument probe.
- stale snapshot probe.
- data inconsistency probe.
- query error probe.
- timeout probe.
- SQL write token scanner.
- AccountingEngine call scanner.
- privacy payload scanner.

## Go / No-Go Checklist

- [ ] docs/81 merged.
- [ ] docs/82 merged.
- [ ] TASK-119 through TASK-135 gates pass.
- [ ] rollback strategy documented.
- [ ] user explicitly authorized DataService facts query implementation.
- [ ] allowed read source list approved.
- [ ] no write SQL tests ready.
- [ ] no AccountingEngine replay tests ready.

## TASK-139 Update

TASK-139 read-only replay implementation tests now cover the authorized
DataService-only replay path while keeping this facts query plan as the
SELECT-only fallback boundary. Snapshot rebuild/write remains separate future
scope.
