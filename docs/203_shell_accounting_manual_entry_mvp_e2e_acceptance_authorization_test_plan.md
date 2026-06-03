# ShellAccounting Manual Entry MVP E2E Acceptance Authorization Test Plan

## Document Purpose

This document defines TASK-203 static test coverage for the ShellAccounting
manual entry MVP E2E acceptance authorization gate. TASK-203 is gate-only and
does not implement runtime E2E fixtures, new production behavior, database
writes, replay integration, audit or ledger policy, broker access, real order
placement, or automatic trading.

## Test Matrix

### Documentation And Registration

- docs/202 exists.
- docs/203 exists.
- README, docs/README, and docs/12 register TASK-203.
- docs/200 and docs/201 mention the TASK-203 acceptance gate update.
- tests/CMake registers `ShellAccountingManualEntryMvpE2eAcceptanceAuthorizationGate`.

### Authorization-Only Boundary

- docs/202 states TASK-203 is authorization-only / gate-only.
- docs/202 states the current task does not implement new functionality.
- docs/202 states the current task does not modify production code.
- docs/202 states the current task does not write a database.
- docs/202 states the current task does not connect replay.
- docs/202 states the current task does not write audit or ledger rows.
- docs/202 states the current task does not connect broker, network,
  credentials, or endpoints.
- docs/202 states the current task does not place real orders or enable
  automatic trading.

### Manual Transaction E2E Acceptance

- docs/202 states the chain:
  `QML -> Presenter -> Controller -> ShellServices adapter -> DataService action -> DataAccess repository`.
- docs/202 states no layer may bypass its boundary.
- docs/202 covers manual transaction BUY / SELL acceptance.
- docs/202 covers invalid payload does not write database rows.
- docs/202 covers duplicate idempotencyKey does not duplicate writes.
- docs/202 covers sensitive memo safe handling.
- docs/202 covers post-write readback refresh state after successful writes.
- docs/202 covers safe visible status and issue messages.

### Manual Cash Movement E2E Acceptance

- docs/202 covers manual cash movement Deposit / Withdrawal acceptance.
- docs/202 covers `trade_log + cash_adjustment` dual-write acceptance.
- docs/202 covers `cash_adjustment.trade_log_id` linkage to `trade_log.id`.
- docs/202 covers invalid payload does not write database rows.
- docs/202 covers duplicate idempotencyKey does not duplicate writes.
- docs/202 covers post-write readback refresh state after successful writes.

### Daily-Use MVP Acceptance

- docs/202 covers manual buy / sell record submission.
- docs/202 covers Deposit / Withdrawal record submission.
- docs/202 covers success and failure status visibility.
- docs/202 covers validation issue visibility.
- docs/202 covers duplicate and idempotent status visibility.
- docs/202 covers position, cash, and PnL read views through DataService read
  boundary or safe unavailable state.
- docs/202 forbids silent success.
- docs/202 forbids raw SQL, raw payload, credentials, endpoint values, and internal stack trace exposure.
- docs/202 forbids broker, real order, and automatic trading side effects.

### No Production Drift

- production QML is not modified by TASK-203.
- production startup is not modified by TASK-203.
- Presenter / Controller are not modified by TASK-203.
- ShellServices adapter / port are not modified by TASK-203.
- DataServiceActions and DataServiceActionRegistrar are not modified by
  TASK-203.
- DataAccess repositories are not modified by TASK-203.
- migrations are not modified by TASK-203.
- no new migration or schema file is added.
- no new runtime SQL / SQLite write path is added.
- no new AccountingEngine replay integration is added.
- no new audit or ledger write is added.
- no new broker SDK, network, credentials, endpoint, real order, or automatic
  trading capability is added.

### Regression

- TASK-204 runtime E2E acceptance tests pass after implementation.
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
- Authorization-only wording probe.
- Manual transaction E2E acceptance wording probe.
- Manual cash movement E2E acceptance wording probe.
- Daily-use MVP acceptance wording probe.
- Changed-path allowlist scan.
- Production QML no-drift scan.
- Presenter / Controller no-drift scan.
- ShellServices adapter / port no-drift scan.
- DataServiceActions no-drift scan.
- DataAccess repository no-drift scan.
- Migration / schema no-drift scan.
- Runtime SQL / SQLite write scan.
- Direct SQLite / DataAccess scan.
- AccountingEngine replay scan.
- Audit / ledger write scan.
- Broker / network / credentials / endpoint scan.
- Real order id / real order placement scan.
- Automatic trading scan.
- Broker gate retention probe.
- DevDocs acceptance probe.

## Go / No-Go Checklist

- [ ] docs/202 exists.
- [ ] docs/203 exists.
- [ ] README, docs/README, and docs/12 register TASK-203.
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
