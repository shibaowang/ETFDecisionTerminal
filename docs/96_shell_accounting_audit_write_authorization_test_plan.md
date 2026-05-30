# ShellAccounting Audit Write Authorization Test Plan

## Document Purpose

TASK-145 defines the tests required before any future ShellAccounting audit
write implementation. This test plan does not implement audit write, does not
write `audit_log`, does not modify production QML, and does not add TradeDraft,
trade suggestions, strategy execution, or broker orders.

## Test Matrix

### A. No Audit Write Yet

- current ShellAccounting path has no audit write implementation
- no `audit_log` write from snapshot write
- no `data.audit.append` side effect from snapshot write
- no ShellAccounting-specific audit action
- no audit write repository dependency in Shell / QML / Presenter

### B. DataService-Only Audit

- DataService-only audit
- future audit only through DataService
- no Shell / QML / Presenter direct audit write
- no ShellServices or ShellCore direct SQLite / DataAccess write path
- no generic write-enabled escape hatch
- every future audit action is allowlisted and statically scannable

### C. Authorized Write Result Input Only

- audit input must be an authorized DataService write result
- current future candidate input is TASK-144 snapshot write result
- source write unavailable blocks audit write
- source write failed blocks or records a sanitized failure according to the
  future implementation policy
- audit must not consume raw SQL, raw facts, or full snapshot payloads

### D. Explicit Authorization Required

- user explicitly authorizes audit write implementation
- user explicitly authorizes audit payload fields
- schema migration, if needed, is a separate TASK
- audit append cannot be enabled by generic write fallback

### E. Sanitized Payload Policy

- action name
- authorization token
- snapshot uid / batch id
- result status
- sanitized issue code
- timestamp
- no raw SQL
- no raw trade_log payload
- no full snapshot payload
- no internal stack trace

### F. Forbidden Payload Policy

- no full `trade_log` payload
- no full snapshot payload
- no raw SQL
- no internal exception stack
- no sensitive raw payload dump
- no generic JSON mirror of source write payload

### G. No Trade / No Strategy

- no TradeDraft
- no trade suggestion
- no broker order
- no strategy execute
- no order confirmation
- no buy / sell buttons
- no trading UI

### H. Transaction Rollback

- audit rollback policy documented
- source write / audit consistency boundary documented by the future task
- no silent success after rollback
- no partial generic write fallback

### I. Idempotency

- idempotency key defined before implementation
- duplicate audit event explicitly handled
- duplicate audit event does not silently overwrite unrelated audit records
- duplicate audit event does not mutate `trade_log`

### J. Error Mapping

- audit disabled
- authorization missing
- source write unavailable
- source write failed
- audit payload invalid
- DB write failure
- transaction rollback
- schema mismatch
- duplicate audit event
- no silent success

### K. Privacy

- no raw SQL in QML
- no full `trade_log` payload in QML
- no full snapshot payload in QML
- no internal exception stack in QML
- no full sensitive payload dump in default logs
- audit diagnostics are redacted

### L. Rollback

- disable switch exists before audit rollout
- disable audit write keeps TASK-144 snapshot write available
- rollback policy documented
- no fake audit data
- no generic write fallback
- no trading fallback

### M. Regression

- TASK-119 through TASK-144 pass
- TASK-144 snapshot write tests pass
- all ShellAccounting gates pass
- full CTest passes
- transport 50 passes

## Required Probes

- static source scan
- forbidden audit write token scan
- no UI-triggered audit write scan
- no generic write action scan
- no raw payload scan
- no TradeDraft / broker / strategy scan
- sanitized payload documentation probe
- rollback documentation probe

## Go / No-Go Checklist

- [ ] docs/95 merged
- [ ] docs/96 merged
- [ ] TASK-144 snapshot write tests pass
- [ ] TASK-145 authorization gate passes
- [ ] full CTest passes
- [ ] transport 50 passes
- [ ] user explicitly authorized audit write implementation
- [ ] user explicitly authorized audit payload fields
- [ ] user explicitly authorized any schema migration, if needed
