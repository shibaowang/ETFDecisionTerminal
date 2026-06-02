# ShellAccounting Manual Transaction Repository Write Implementation Test Plan

## Document Purpose

This document defines the TASK-192 test plan for the ShellAccounting manual
transaction repository write implementation. The plan verifies only DataAccess
direct repository writes for manual BUY / SELL entries and preserves all
runtime, UI, broker, strategy, and automatic trading boundaries.

## Test Matrix

### Documentation And Indexing

- docs/182 exists.
- docs/183 exists.
- README, docs/README, and docs/12 register TASK-192.
- docs/180 and docs/181 state TASK-192 has implemented manual transaction
  repository write only.

### Schema Boundary

- `migrations/001_initial_schema.sql` remains unchanged.
- `migrations/002_shell_accounting_manual_entry_schema.sql` remains unchanged.
- No new migration is added.
- No schema side file is added.
- Direct tests apply migrations 001 + 002 to a temporary SQLite database.

### Repository Boundary

- `ShellAccountingManualTransactionRepository` exists in DataAccess.
- Repository implementation is not added to DataServiceApi, QML, Shell,
  Presenter, or Controller code.
- DataServiceActions.cpp, DataServiceActions.h, and
  DataServiceActionRegistrar.cpp remain unchanged.
- TASK-178 validation production code remains unchanged.
- TASK-182 validation wiring remains `writeImplemented=false`.
- Manual cash movement write remains disabled / not implemented.

### Manual Transaction Write

- manual BUY command writes successfully.
- manual SELL command writes successfully.
- BUY / SELL write `trade_log`.
- `trade_execution_group` is written for manual transaction facts.
- `trade_log.request_id` is written.
- `trade_log.idempotency_key` is written.
- `trade_log.occurred_at_utc` is written.
- `trade_log.tax_cents` is written.
- `trade_log.source_memo_sanitized` is written.
- `trade_log.manual_entry` is set.
- `trade_log.trade_source` is `MANUAL`.
- `trade_log.action_type` distinguishes `BUY` and `SELL`.
- quantity, price, amount, fee, and net cash impact follow the documented rules.

### Idempotency / Duplicate Handling

- duplicate idempotencyKey is covered.
- duplicate non-empty idempotencyKey does not create another `trade_log` row.
- duplicate non-empty idempotencyKey returns stable idempotent / duplicate
  result semantics.
- empty idempotencyKey remains stable and does not break the unique index.
- requestId is retained as trace data, not the unique dedupe key.

### Rollback / Failure

- rollback / failure case coverage is required.
- invalid command writes no `trade_log`.
- missing referenced account / portfolio / instrument writes no partial rows.
- forced transaction failure leaves no partial `trade_execution_group` or
  `trade_log` rows.
- SQLite failures are mapped without exposing raw SQL.

### Forbidden Writes And Calls

- repository does not write `cash_adjustment`.
- repository does not write `audit_log`.
- repository does not write cash facts / cash ledger.
- repository does not call AccountingEngine replay.
- repository does not generate TradeDraft or trade suggestions.
- repository does not call broker.
- repository does not access network / endpoint.
- repository does not read credentials, secrets, tokens, keys, or passwords.
- repository does not place real orders.
- repository does not implement automatic trading.

### Regression

- TASK-191 post-migration repository authorization gate passes.
- TASK-194 manual cash movement repository write authorization gate passes and
  does not modify TASK-192 repository behavior.
- TASK-190 migration implementation passes.
- TASK-185 repository scaffold passes with cash movement disabled.
- TASK-182 validation wiring passes and remains validation-only.
- TASK-178 validation scaffold passes.
- Existing broker disabled / broker order / real broker gates pass and are not
  deleted, weakened, or skipped.

## Required Probes

- direct SQLite temporary database write probe.
- migration 001 + 002 apply probe.
- repository success path probe.
- idempotency duplicate probe.
- rollback / partial row probe.
- sanitized memo probe.
- forbidden table write probe.
- DataServiceActions unchanged probe.
- QML / Shell / Presenter / Controller boundary probe.
- no broker / network / credentials / endpoint / real order probe.
- broker gate retention probe.

## Go / No-Go Checklist

- [ ] TASK-191 commit is present in main.
- [ ] docs/182 is present.
- [ ] docs/183 is present.
- [ ] `migrations/001_initial_schema.sql` is unchanged.
- [ ] `migrations/002_shell_accounting_manual_entry_schema.sql` is unchanged.
- [ ] No new migration or schema file is added.
- [ ] Manual BUY / SELL direct repository writes pass.
- [ ] Manual cash movement write remains disabled.
- [ ] TASK-194 cash movement repository authorization gate passes.
- [ ] DataService action write wiring is not implemented.
- [ ] No audit, broker, strategy, UI, network, credentials, real order, or
  automatic trading implementation is added.
- [ ] TASK-192 tests pass.
- [ ] Full CTest passes.
- [ ] transport local socket echo 50 repeat passes.
