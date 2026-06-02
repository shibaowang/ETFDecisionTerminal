# ShellAccounting Manual Cash Movement Schema Contract Alignment Test Plan

## Document Purpose

This document defines the TASK-195 schema-contract alignment gate test plan. It
is static and documentation-only. It does not implement manual cash movement
repository write and does not write a database.

## Test Matrix

### Documentation And Indexing

- `docs/186_shell_accounting_manual_cash_movement_schema_contract_alignment_gate.md` exists.
- `docs/187_shell_accounting_manual_cash_movement_schema_contract_alignment_test_plan.md` exists.
- README, docs index, and prompt template register TASK-195.
- docs/184 and docs/185 are aligned so they no longer describe
  cash_adjustment-only implementation as viable under the current schema.

### Schema Contract

- `migrations/001_initial_schema.sql` contains
  `trade_log_id INTEGER NOT NULL` for `cash_adjustment`.
- `migrations/001_initial_schema.sql` contains
  `FOREIGN KEY (trade_log_id) REFERENCES trade_log(id)`.
- docs/186 states current schema blocks cash_adjustment-only write.
- docs/186 states future implementation must dual-write `trade_log` and
  `cash_adjustment`.
- docs/186 states `cash_adjustment.trade_log_id` must point to `trade_log.id`.
- docs/186 states `trade_log.cash_adjustment_uid` linkage must be explicit.

### Atomicity / Rollback

- docs/186 requires one atomic transaction for dual-write.
- docs/186 requires rollback on any failure.
- docs/186 requires no partial facts.

### Forbidden Changes

- `migrations/001_initial_schema.sql` is not modified.
- `migrations/002_shell_accounting_manual_entry_schema.sql` is not modified.
- No new migration is added.
- No new schema file is added.
- DataServiceActions.cpp is not modified.
- DataServiceActions.h is not modified.
- DataServiceActionRegistrar.cpp is not modified.
- TASK-192 manual transaction repository is not modified.
- No manual cash movement repository implementation is added.
- Runtime SQL / SQLite write is not added.
- `cash_adjustment`, `trade_log`, `audit_log`, and ledger writes are not added.
- QML, startup, Presenter, and Controller are not modified.
- AccountingEngine replay is not modified.
- Broker SDK, network, credentials, endpoint, real order, broker order id, and
  automatic trading are not added.

### Regression

- TASK-194 authorization gate still passes.
- TASK-192 manual transaction repository write tests still pass.
- TASK-182 DataService validation-only wiring still passes.
- Existing broker / real broker / no-network / no-credentials /
  no-order-placement gates remain retained and are not weakened, skipped, or
  deleted.

## Required Probes

- Static documentation token scan.
- Migration schema fact scan.
- Forbidden migration and schema file probe.
- Forbidden DataServiceActions / registrar diff probe.
- Forbidden TASK-192 repository diff probe.
- Forbidden manual cash movement repository implementation probe.
- Forbidden runtime SQL / SQLite write probe.
- Forbidden cash_adjustment / trade_log / audit / ledger write probe.
- Forbidden QML / startup / Presenter / Controller probe.
- Forbidden broker / network / credentials / endpoint / order probe.
- Broker gate retention probe.

## Go / No-Go Checklist

- [ ] docs/186 exists.
- [ ] docs/187 exists.
- [ ] README, docs index, and prompt template register TASK-195.
- [ ] Schema contract facts are verified from `migrations/001_initial_schema.sql`.
- [ ] TASK-195 gate tests pass.
- [ ] TASK-194 gate tests pass.
- [ ] TASK-192 manual transaction repository write tests pass.
- [ ] TASK-182 validation wiring tests pass.
- [ ] Broker disabled / broker order / real broker gates pass.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo repeat 50 passes.
- [ ] git diff --check passes.
- [ ] git status is clean.
