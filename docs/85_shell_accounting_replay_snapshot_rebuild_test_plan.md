# ShellAccounting Replay Snapshot Rebuild Test Plan

## Document Purpose

TASK-138 defines the future replay / snapshot rebuild implementation tests. This
document does not implement replay, does not implement rebuild, and does not
implement any write path.

## Test Matrix

### A. Replay Boundary

- No replay yet.
- Future replay only through DataService.
- No Shell / QML / Presenter direct AccountingEngine dependency.
- No direct `replayFromFacts` from the UI path.

### B. Snapshot Rebuild Boundary

- No snapshot rebuild yet.
- No snapshot write yet.
- No QML-triggered write.
- No migration or schema modification.

### C. No Write Path

- No `INSERT`, `UPDATE`, `DELETE`, or `REPLACE`.
- No snapshot table writes.
- No audit append.
- No trade_log write.
- No trade_draft write.

### D. No Trade / Strategy

- No TradeDraft.
- No trade suggestion.
- No broker order.
- No strategy execute.
- No trading buttons.

### E. Error Mapping

- Missing facts.
- Inconsistent facts.
- Missing price / FX.
- Replay disabled.
- Rebuild disabled.
- Write authorization missing.
- Unavailable / timeout.

### F. Rollback

- Disable replay.
- Disable rebuild.
- Fallback to TASK-137 read-only facts query.
- No fake data.
- No write fallback.

### G. Regression

- TASK-137 facts query tests pass.
- All ShellAccounting gates pass.
- Full tests pass.
- Transport 50 passes.

## Required Fakes / Probes

- Static source scan.
- Forbidden token scan.
- Dependency scan.
- No write SQL scan.
- No AccountingEngine direct dependency scan.
- No snapshot write scan.
- No TradeDraft / broker / strategy scan.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/84 merged.
- [ ] docs/85 merged.
- [ ] TASK-137 facts query implementation passes.
- [ ] TASK-138 gate passes.
- [ ] full CTest passes.
- [ ] transport 50 passes.
- [ ] user explicitly authorized replay implementation.
- [ ] user explicitly authorized snapshot rebuild implementation.
- [ ] user explicitly authorized any write path, if snapshot writes are needed.
