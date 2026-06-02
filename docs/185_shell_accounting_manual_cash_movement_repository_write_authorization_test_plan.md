# ShellAccounting Manual Cash Movement Repository Write Authorization Test Plan

## Document Purpose

This document defines the TASK-194 test plan for the manual cash movement
repository write implementation authorization gate. It is a static gate and
documentation consistency plan. It does not implement manual cash movement
repository write and does not write a database.

## Test Matrix

### Documentation And Indexing

- `docs/184_shell_accounting_manual_cash_movement_repository_write_authorization_gate.md` exists.
- `docs/185_shell_accounting_manual_cash_movement_repository_write_authorization_test_plan.md` exists.
- README, docs index, and prompt template register TASK-194.
- docs/184 mentions TASK-192 and TASK-190 migration.
- docs/184 states authorization-only and gate-only.
- docs/184 states the current task does not implement cash movement repository write.
- docs/184 states the current task does not modify TASK-192 repository behavior.
- docs/184 states future cash movement repository implementation must be a separate TASK.
- docs/184 states future DataService write implementation must be a separate TASK.

### Mapping Boundary

- Cash adjustment mapping includes request_id, idempotency_key,
  occurred_at_utc, source_memo_sanitized, trade_log_uid, amount_cents, and
  adjustment_type.
- Trade log cash fact mapping includes request_id, idempotency_key,
  occurred_at_utc, cash_adjustment_uid, trade_source, manual_entry,
  amount_cents, net_cash_impact_cents, and source_memo_sanitized.
- Deposit and Withdrawal policy is documented.
- Adjustment must be explicit and must fail closed unless separately
  authorized.
- Amount sign, net cash impact, currency, account, portfolio, request_id trace,
  and idempotency_key duplicate behavior are documented.

### Transaction / Rollback / Idempotency

- Future implementation must define a transaction boundary for
  `cash_adjustment` only or `cash_adjustment` + `trade_log` dual write.
- Rollback and failure atomicity must be documented.
- Duplicate idempotency_key behavior and unique index conflict handling must be
  documented.
- No partial facts on failure must be documented.

### Privacy And Sanitization

- sourceMemo sanitization must be documented.
- Raw credential, token, key, password, endpoint, broker payload, raw SQL, raw
  manual entry payload, and internal exception stack persistence are forbidden.
- Sensitive memo rejection or redaction policy must be documented.

### Forbidden Changes

- `migrations/001_initial_schema.sql` is not modified.
- `migrations/002_shell_accounting_manual_entry_schema.sql` is not modified.
- No new migration is added.
- No new schema file is added.
- DataServiceActions.cpp is not modified by TASK-194.
- DataServiceActions.h is not modified by TASK-194.
- DataServiceActionRegistrar.cpp is not modified by TASK-194.
- TASK-178 validation production code is not modified by TASK-194.
- TASK-192 manual transaction repository header/source are not modified by
  TASK-194.
- Manual cash movement repository implementation is not added.
- Runtime SQL INSERT / UPDATE / DELETE / REPLACE is not added.
- SQLite runtime write is not added.
- Runtime cash_adjustment write is not added.
- Runtime audit / ledger write is not added.
- Production QML and startup are not modified.
- Presenter and Controller are not modified.
- AccountingEngine replay is not modified.
- StrategyEngine and MarketEngine are not modified.
- TradeDraft or suggestion implementation is not added.
- Broker SDK, network, endpoint, credentials, secrets, token, key, password,
  real broker order id, real order placement, and automatic trading are not
  added.

### Regression

- TASK-193 gate alignment semantics remain valid.
- TASK-192 manual transaction repository write semantics remain valid.
- TASK-190 migration implementation semantics remain valid.
- TASK-182 validation wiring semantics remain valid.
- TASK-177 MVP authorization gate remains valid.
- Existing broker / real broker / no-network / no-credentials /
  no-order-placement gates remain retained and are not weakened, skipped, or
  deleted.

## Required Probes

- Static source scan.
- Documentation token scan.
- Forbidden migration and schema file probe.
- Forbidden DataServiceActions / registrar diff probe.
- Forbidden TASK-178 validation diff probe.
- Forbidden TASK-192 repository diff probe.
- Forbidden manual cash movement repository implementation probe.
- Forbidden runtime SQL / SQLite write probe.
- Forbidden cash_adjustment / audit / ledger write probe.
- Forbidden QML / startup / Presenter / Controller probe.
- Forbidden replay / strategy / market probe.
- Forbidden TradeDraft / suggestion probe.
- Forbidden broker / network / credentials / endpoint / order probe.
- Broker gate retention probe.

## Go / No-Go Checklist

- [ ] docs/184 exists.
- [ ] docs/185 exists.
- [ ] README, docs index, and prompt template register TASK-194.
- [ ] TASK-194 gate tests pass.
- [ ] TASK-192 manual transaction repository write tests pass.
- [ ] TASK-191/TASK-193 post-migration gate tests pass.
- [ ] TASK-190 migration implementation tests pass.
- [ ] TASK-182 validation wiring tests pass.
- [ ] TASK-178 validation scaffold tests pass.
- [ ] Broker disabled / broker order / real broker gates pass.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo repeat 50 passes.
- [ ] git diff --check passes.
- [ ] git status is clean.
- [ ] User separately authorizes any future manual cash movement repository
  implementation.
