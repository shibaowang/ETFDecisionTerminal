# ShellAccounting Snapshot Rebuild Write Test Plan

## Document Purpose

TASK-140 defines the future snapshot rebuild / write implementation tests. This
document does not implement rebuild or any write path.

## Test Matrix

### A. Snapshot Rebuild Boundary

- No snapshot rebuild yet.
- Future rebuild only through DataService.
- No Shell / QML / Presenter direct rebuild.
- No direct `rebuildFromReplay` from the UI path.

### B. Snapshot Write Boundary

- No snapshot write yet.
- No QML-triggered write.
- No migration or schema modification.
- Future write allowlist required.

### C. Forbidden Writes

- No trade_log write.
- No trade_draft write.
- No audit append unless separately authorized.
- No generic write action.
- No write-enabled escape hatch.

### D. No Trade / Strategy

- No TradeDraft.
- No trade suggestion.
- No broker order.
- No strategy execute.
- No trading buttons.

### E. Error Mapping

- Rebuild disabled.
- Write authorization missing.
- Replay unavailable.
- Missing facts.
- Stale facts.
- Inconsistent facts.
- DB write failure.
- Transaction rollback.
- Schema mismatch.

### F. Rollback

- Disable rebuild.
- Disable write.
- Fallback to TASK-139 read-only replay.
- No fake data.
- No write fallback.

### G. Regression

- TASK-139 read-only replay tests pass.
- TASK-137 facts query tests pass.
- All ShellAccounting gates pass.
- Full tests pass.
- Transport 50 passes.

## Required Fakes / Probes

- Static source scan.
- Forbidden token scan.
- Dependency scan.
- No write SQL scan.
- No snapshot write scan.
- No TradeDraft / broker / strategy scan.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/87 merged.
- [ ] docs/88 merged.
- [ ] TASK-139 read-only replay implementation passes.
- [ ] TASK-140 gate passes.
- [ ] full CTest passes.
- [ ] transport 50 passes.
- [ ] user explicitly authorized snapshot rebuild implementation.
- [ ] user explicitly authorized snapshot write implementation.
- [ ] user explicitly authorized any schema migration, if needed.

## TASK-141 Follow-Up Gate

TASK-141 adds the snapshot rebuild implementation gate and test plan.
Snapshot rebuild remains unimplemented in TASK-141, snapshot write remains
separately unauthorized, and TASK-139 read-only replay remains the fallback.
Future rebuild implementation must keep DataService-only coordination,
read-only replay input, in-memory preview output, no database write, no
TradeDraft, no trade suggestion, no strategy execution, no broker order, and no
trading UI.

## TASK-143 Snapshot Write Authorization Gate

TASK-143 adds the snapshot write authorization gate. Snapshot write remains
unimplemented. Future tests must prove preview-input-only behavior from
TASK-142, explicit authorization, candidate allowlist tables, forbidden
`trade_log` / `trade_execution_group` / `trade_draft` / `audit_log` writes,
transaction rollback, idempotency, no UI-triggered write, no TradeDraft, no
strategy, no broker order, and rollback readiness.
