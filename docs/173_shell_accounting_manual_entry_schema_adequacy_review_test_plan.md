# TASK-187 ShellAccounting Manual Entry Schema Adequacy Review Test Plan

## Document Purpose

This test plan defines static checks for the TASK-187 manual entry schema
adequacy review gate. It does not implement repository writes, does not execute
SQL, does not write SQLite, and does not modify schema.

## Test Matrix

### Documentation And Indexing

- `docs/172_shell_accounting_manual_entry_schema_adequacy_review_gate.md`
  exists.
- `docs/173_shell_accounting_manual_entry_schema_adequacy_review_test_plan.md`
  exists.
- `README.md`, `docs/README.md`, and `docs/12_codex_prompt_template.md`
  reference TASK-187.
- `docs/172` states schema review-only.
- `docs/172` states `migrations/001_initial_schema.sql` is the source of
  truth.

### Schema Adequacy Review

- `docs/172` documents manual transaction schema adequacy evidence or gaps.
- `docs/172` documents manual cash movement schema adequacy evidence or gaps.
- `docs/172` documents audit policy schema review.
- `docs/172` documents idempotency / duplicate handling schema review.
- `docs/172` documents privacy / memo sanitization schema review.
- `docs/172` states schema gaps require a future schema authorization TASK.
- Static probes inspect `migrations/001_initial_schema.sql` for the cited
  tables, fields, constraints, and gaps.

### No Schema Drift

- `migrations/001_initial_schema.sql` is not modified by TASK-187.
- No migration is added.
- No schema file is added.
- No table, column, index, or constraint is added.

### Production Boundary

- `DataServiceActions.cpp` is not modified by TASK-187.
- `DataServiceActions.h` is not modified by TASK-187.
- `DataServiceActionRegistrar.cpp` is not modified by TASK-187.
- TASK-178 validation production code is not modified by TASK-187.
- TASK-185 repository scaffold header/source are not modified by TASK-187.
- TASK-185 scaffold remains disabled / write-not-implemented.
- TASK-182 validation wiring remains `writeImplemented=false`.
- No repository implementation is added.
- No SQL insert/update/delete/replace is added.
- No SQLite write is added.
- No `trade_log`, cash facts, cash ledger, audit, or ledger write is added.
- Production QML, startup, Presenter, and Controller code are not modified.
- AccountingEngine replay is not modified.
- StrategyEngine and MarketEngine are not modified.
- No TradeDraft or suggestion implementation is added.

### Broker And Runtime Safety

- No broker SDK is added.
- No network call or endpoint is added.
- No credentials, secrets, tokens, keys, or passwords are added.
- No real broker order id is added.
- No real order placement is added.
- No automatic trading is added.
- Existing broker, real broker, no-network, no-credentials, and
  no-order-placement gates are not deleted, weakened, or skipped.

### Regression

- TASK-186 repository implementation authorization gate remains valid.
- TASK-185 repository scaffold remains valid.
- TASK-184 authorization gate remains valid.
- TASK-183 persistence authorization gate remains valid.
- TASK-182 validation wiring remains valid.
- TASK-178 validation scaffold remains pure.
- TASK-177 MVP authorization gate remains valid.
- Broker disabled, broker order, and real broker gates remain valid.

## Required Probes

- Static document existence scan.
- README / docs index / prompt registration scan.
- Static schema source-of-truth scan.
- Manual transaction table / field / gap scan.
- Manual cash movement table / field / gap scan.
- Audit, idempotency, privacy, and rollback documentation probe.
- Migration filename scan.
- Forbidden schema file scan.
- Forbidden TASK-187 marker scan in production code.
- Forbidden repository implementation token scan.
- Forbidden write SQL token scan.
- Forbidden SQLite write token scan.
- Forbidden `trade_log` / cash facts / audit / ledger write scan.
- Forbidden QML / startup / Presenter / Controller drift scan.
- Forbidden replay / StrategyEngine / MarketEngine drift scan.
- Forbidden TradeDraft / suggestion scan.
- Forbidden broker / network / credentials / endpoint scan.
- Existing broker gate retention probe.

## Go / No-Go Checklist

- [ ] docs/172 merged.
- [ ] docs/173 merged.
- [ ] TASK-187 schema adequacy review gate tests pass.
- [ ] TASK-186 repository implementation authorization tests pass.
- [ ] TASK-185 repository scaffold tests pass.
- [ ] TASK-184 authorization tests pass.
- [ ] TASK-183 persistence authorization tests pass.
- [ ] TASK-182 validation wiring tests pass.
- [ ] TASK-178 validation scaffold tests pass.
- [ ] TASK-177 gate tests pass.
- [ ] Broker disabled / broker order / real broker gates pass.
- [ ] Full CTest passes.
- [ ] transport 50 repeat passes.
- [ ] User explicitly authorizes any future schema change.
- [ ] User explicitly authorizes any future repository implementation.

