# ShellAccounting Snapshot Write Authorization Test Plan

## Document Purpose

TASK-143 defines the tests required before any future ShellAccounting snapshot
write implementation. This test plan does not implement snapshot write, does
not write database tables, does not modify production QML, and does not add
TradeDraft, trade suggestions, strategy execution, or broker orders.

## Test Matrix

### A. No Write Yet

- current ShellAccounting path has no snapshot write implementation
- no `cash_snapshot` write
- no `position_snapshot` write
- no `portfolio_summary` write
- no `audit_log` write
- no `trade_log`, `trade_execution_group`, or `trade_draft` write

### B. DataService-Only Write

- DataService-only write
- future write only through DataService
- no Shell / QML / Presenter direct write
- no ShellServices or ShellCore direct SQLite / DataAccess write path
- no generic write-enabled escape hatch
- every future write action is allowlisted and statically scannable

### C. Preview Input Only

- write input must be TASK-142 snapshot rebuild preview
- `snapshotRebuildPreview` is the only approved source
- preview input only means no direct replay write path
- snapshot write must not read directly from replay or facts query when preview
  is unavailable
- snapshot write must not read directly from replay or facts query when preview is unavailable
- stale or unavailable preview blocks write

### D. Explicit Authorization Required

- user explicitly authorizes snapshot write implementation
- user explicitly authorizes each table write
- schema migration, if needed, is a separate TASK
- audit append, if needed, is a separate TASK

### E. Allowlist Table Policy

- candidate allowlist contains `cash_snapshot`
- candidate allowlist contains `position_snapshot`
- candidate allowlist contains `portfolio_summary`
- current TASK-143 does not write these tables
- future implementation proves each table is authorized

### F. Forbidden Table Policy

- no `trade_log` write
- no `trade_execution_group` write
- no `trade_draft` write
- no `audit_log` write unless a future task separately authorizes audit writes
- no generic write action
- no write-enabled escape hatch

### G. No Trade / No Strategy

- no TradeDraft
- no trade suggestion
- no broker order
- no strategy execute
- no order confirmation
- no buy / sell buttons
- no trading UI

### H. Transaction Rollback

- partial write forbidden
- transaction rollback produces an issue
- no silent success after rollback
- no partial derived snapshot persistence

### I. Idempotency

- idempotency key defined before implementation
- duplicate snapshot explicitly handled
- duplicate snapshot does not silently overwrite
- duplicate snapshot does not mutate `trade_log`

### J. Error Mapping

- write disabled
- authorization missing
- preview unavailable
- stale preview
- DB write failure
- transaction rollback
- schema mismatch
- duplicate snapshot
- partial write forbidden
- no silent success

### K. Privacy

- no raw SQL in QML
- no full `trade_log` payload in QML
- no internal exception stack in QML
- no full sensitive payload dump in default logs
- write diagnostics are redacted

### L. Rollback

- disable switch exists before write rollout
- disable snapshot write keeps TASK-142 snapshot rebuild preview
- rollback policy documented
- no fake data
- no write fallback
- no trading fallback

### M. Regression

- TASK-119 through TASK-142 pass
- TASK-142 preview tests pass
- all ShellAccounting gates pass
- full CTest passes
- transport 50 passes

## Required Probes

- static source scan
- forbidden write token scan
- no UI-triggered write scan
- no generic write action scan
- no TradeDraft / broker / strategy scan
- allowlist table documentation probe
- forbidden table documentation probe
- rollback documentation probe

## Go / No-Go Checklist

- [ ] docs/92 merged
- [ ] docs/93 merged
- [ ] TASK-142 snapshot rebuild preview tests pass
- [ ] TASK-143 authorization gate passes
- [ ] full CTest passes
- [ ] transport 50 passes
- [ ] user explicitly authorized snapshot write implementation
- [ ] user explicitly authorized snapshot write allowlist tables
- [ ] user explicitly authorized any audit write, if needed
- [ ] user explicitly authorized any schema migration, if needed

## TASK-144 Post-Implementation Matrix

TASK-144 implements `accounting.snapshot.write` as an authorized DataService-only
snapshot write. The authorization gate now verifies that the write remains
limited to TASK-142 `snapshotRebuildPreview` input and the allowlist tables
`cash_snapshot`, `position_snapshot`, and `portfolio_summary`. The matrix still
requires checks for forbidden tables (`trade_log`, `trade_execution_group`,
`trade_draft`, `audit_log`), no production QML/startup trigger, transaction
rollback, idempotency, duplicate handling, privacy, no TradeDraft, no strategy,
no broker order, full CTest, and transport 50 repeat.

## TASK-145 Audit Write Authorization Gate

TASK-145 adds a separate audit write authorization gate and test plan. Audit
write is still not implemented, `audit_log` remains unwritten, and TASK-144
snapshot write tests must continue to prove that snapshot write does not append
audit rows. Future audit write tests must cover sanitized payloads,
DataService-only coordination, rollback, idempotency, and no TradeDraft /
strategy / broker order behavior.
