# ShellAccounting Manual Cash Movement Repository Dual-Write Implementation Test Plan

## Document Purpose

This document defines the TASK-196 test plan for the manual cash movement
repository dual-write implementation. It verifies DataAccess-only repository
behavior and static boundaries. It does not authorize DataService runtime write
wiring, UI, replay, audit write, ledger write, broker, network, credentials,
endpoint, real order placement, or automatic trading.

## Test Matrix

### Documentation And Registration

- docs/188 exists.
- docs/189 exists.
- README, docs index, and prompt template register TASK-196.
- docs/186 and docs/187 record TASK-196 as the authorized DataAccess-only
  manual cash movement repository dual-write implementation.
- docs/184 and docs/185 evolve from future authorization to the TASK-196
  DataAccess-only dual-write implementation while keeping DataService/UI/broker
  work separate.

### Schema And Boundary

- `migrations/001_initial_schema.sql` is not modified.
- `migrations/002_shell_accounting_manual_entry_schema.sql` is not modified.
- No migration or schema side file is added.
- DataServiceActions.cpp is not modified.
- DataServiceActions.h is not modified.
- DataServiceActionRegistrar.cpp is not modified.
- TASK-178 validation production code is not modified.
- TASK-192 manual transaction repository is not modified.
- Repository implementation exists only in DataAccess.
- Repository implementation is not referenced from DataServiceApi, QML, Shell,
  Presenter, or Controller.

### Direct SQLite Repository Write

- Temporary SQLite DB applies 001 + 002 migration.
- Deposit command writes successfully.
- Withdrawal command writes successfully.
- Deposit and Withdrawal write `trade_log`.
- Deposit and Withdrawal write `cash_adjustment`.
- `cash_adjustment.trade_log_id` points to the inserted `trade_log.id`.
- `trade_log.cash_adjustment_uid` links to the inserted cash adjustment uid.
- `cash_adjustment.trade_log_uid` links to the inserted trade log uid.
- `trade_log.request_id`, `trade_log.idempotency_key`,
  `trade_log.occurred_at_utc`, and `trade_log.source_memo_sanitized` are
  written.
- `cash_adjustment.request_id`, `cash_adjustment.idempotency_key`,
  `cash_adjustment.occurred_at_utc`, and
  `cash_adjustment.source_memo_sanitized` are written.
- Deposit is positive; Withdrawal is negative.
- `trade_log.net_cash_impact_cents` reflects Deposit positive and Withdrawal
  negative.
- `cash_adjustment.adjustment_type` is stable as `CASH_IN` or `CASH_OUT`.

### Idempotency / Rollback

- Duplicate non-empty idempotencyKey does not create extra `trade_log`.
- Duplicate non-empty idempotencyKey does not create extra `cash_adjustment`.
- duplicate idempotencyKey coverage proves the repository does not create
  repeated facts for the same non-empty key.
- Duplicate result returns idempotent / duplicate status.
- Empty idempotencyKey remains stable and does not break unique indexes.
- Invalid command writes no `trade_log`.
- Invalid command writes no `cash_adjustment`.
- Invalid movement type fails closed.
- Unauthorized Adjustment movement fails closed.
- Reference failure leaves no partial rows.
- Cash adjustment insert failure rolls back the preceding trade_log insert.
- rollback / failure case leaves no partial facts.

### Forbidden Writes And Integrations

- Repository does not write `audit_log`.
- Repository does not write ledger rows.
- Repository does not call AccountingEngine replay.
- Repository does not create TradeDraft or trade suggestions.
- Repository does not call broker.
- Repository does not access network, endpoint, credentials, secrets, token,
  key, or password values.
- Repository does not place a real order.
- Repository does not add automatic trading.

### Regression

- TASK-195 schema contract alignment tests pass.
- TASK-194 authorization gate tests pass.
- TASK-192 manual transaction repository write tests pass.
- TASK-190 migration implementation tests pass.
- TASK-182 DataService validation-only wiring tests pass.
- TASK-178 validation scaffold tests pass.
- Broker disabled, broker order, real broker authorization, and real broker
  implementation gates pass.
- Full CTest passes.
- transport local socket echo 50 repeat passes.

## Required Probes

- Static source scan.
- Forbidden migration / schema file scan.
- Forbidden DataServiceActions diff scan.
- Forbidden TASK-192 repository diff scan.
- Direct SQLite dual-write probe.
- Idempotency duplicate probe.
- Rollback / partial fact probe.
- Sanitized memo probe.
- No audit / ledger write probe.
- No replay / TradeDraft / suggestion probe.
- No broker / network / credentials / endpoint / real order / automatic trading
  probe.
- Broker gate retention probe.

## Go / No-Go Checklist

- [ ] docs/188 exists.
- [ ] docs/189 exists.
- [ ] TASK-196 tests pass.
- [ ] TASK-195 gate tests pass.
- [ ] TASK-194 gate tests pass.
- [ ] TASK-192 manual transaction repository tests pass.
- [ ] TASK-190 migration implementation tests pass.
- [ ] TASK-182 validation wiring tests pass.
- [ ] TASK-178 validation scaffold tests pass.
- [ ] Broker gates pass.
- [ ] Full CTest passes.
- [ ] transport 50 passes.
- [ ] git diff --check passes.
- [ ] git status is clean.
