# ShellAccounting Snapshot Rebuild Implementation Test Plan

## Document Purpose

TASK-141 defines the future snapshot rebuild implementation test plan. This
document does not implement rebuild, does not implement snapshot write, and does
not authorize database writes or trading behavior.

## Test Matrix

### A. No Rebuild Yet

- No snapshot rebuild implementation exists in TASK-141.
- No `rebuildSnapshot`, `rebuildFromReplay`, `rebuildFromFacts`, or
  `snapshot.rebuild` action is present in production ShellAccounting paths.
- No UI-triggered rebuild exists.

### B. DataService-Only Rebuild

- Future rebuild runs only through DataService.
- No Shell / QML / Presenter direct rebuild.
- No Shell / QML / Presenter direct SQLite or DataAccess path.
- No generic write-enabled escape hatch.
- Future rebuild action is allowlisted and test-covered.

### C. Read-Only Replay Input

- Future rebuild input comes from TASK-139 read-only replay.
- Future rebuild does not fabricate facts.
- Future rebuild does not read facts from Shell, QML, or Presenter.
- Replay unavailable maps to a structured issue.

### D. In-Memory Preview Output

- Future rebuild output is an in-memory derived snapshot preview only.
- Preview output is not persisted.
- Preview output is not TradeDraft.
- Preview output is not trade suggestion.
- Preview output is not strategy or broker order input.

### E. No DB Write

- No `INSERT`, `UPDATE`, `DELETE`, `REPLACE`, `CREATE`, `DROP`, or `ALTER`.
- No transaction write.
- No write repository.
- No `data.audit.append`.
- No `writeEnabled: true`.

### F. No Snapshot Write

- No `cash_snapshot` write.
- No `position_snapshot` write.
- No `portfolio_summary` write.
- No `audit_log` write.
- Snapshot write remains separately authorized.

### G. No TradeDraft / No Trading

- No TradeDraft.
- No trade suggestion.
- No broker order.
- No strategy execute.
- No order confirmation.
- No buy / sell action.
- No trading buttons.

### H. Error Mapping

- Replay unavailable.
- Missing facts.
- Stale facts.
- Inconsistent facts.
- Unsupported currency.
- Missing market price.
- Missing FX.
- Rebuild disabled.
- No silent success.

### I. Rollback

- Disable rebuild.
- Fallback to TASK-139 read-only replay.
- No fake data.
- No write fallback.
- No TradeDraft fallback.
- No trading fallback.

### J. Regression

- TASK-119 through TASK-140 pass.
- TASK-139 read-only replay tests pass.
- TASK-140 snapshot rebuild/write gate tests pass.
- Full CTest passes.
- Transport 50 passes.

## Required Probes

- Static source scan.
- Forbidden write token scan.
- No snapshot write scan.
- No UI-triggered rebuild scan.
- No trade / strategy scan.
- DataService-only policy scan.
- Read-only replay input policy scan.
- In-memory preview output policy scan.
- Error mapping documentation probe.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/89 merged.
- [ ] docs/90 merged.
- [ ] TASK-119 through TASK-140 gates pass.
- [ ] TASK-139 read-only replay implementation passes.
- [ ] TASK-140 snapshot rebuild/write gate passes.
- [ ] TASK-141 gate passes.
- [ ] Full CTest passes.
- [ ] Transport 50 passes.
- [ ] User explicitly authorized snapshot rebuild implementation.
- [ ] User explicitly confirmed snapshot write remains out of scope.
- [ ] No write SQL tests ready.
- [ ] No TradeDraft / trading tests ready.
