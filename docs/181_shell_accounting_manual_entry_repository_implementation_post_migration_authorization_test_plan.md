# ShellAccounting Manual Entry Repository Implementation Post-Migration Authorization Test Plan

## Document Purpose

This document defines the TASK-191 test plan for the manual entry repository
implementation post-migration authorization gate. After TASK-192, the plan
allows the DataAccess-only manual transaction repository write implementation
for manual BUY / SELL entries while still verifying that manual cash movement
write, DataService write wiring, UI, broker, network, credentials, real order
placement, and automatic trading remain out of scope.

## Test Matrix

### Documentation And Indexing

- `docs/180_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_gate.md` exists.
- `docs/181_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_test_plan.md` exists.
- README, docs index, and prompt template register TASK-191.
- docs/180 states post-migration authorization-only.
- docs/180 states TASK-190 migration exists.
- docs/180 states `migrations/001_initial_schema.sql` and
  `migrations/002_shell_accounting_manual_entry_schema.sql` must not be modified.
- docs/180 states repository implementation must be a separate TASK.
- docs/180 states DataService action write implementation must be a separate TASK.

### Mapping Boundary

- Manual transaction mapping includes request_id, idempotency_key,
  occurred_at_utc, tax_cents, source_memo_sanitized, quantity_1e6, price_1e6,
  amount_cents, fee_cents, net_cash_impact_cents, action_type, trade_source,
  and manual_entry.
- Manual cash movement mapping includes request_id, idempotency_key,
  occurred_at_utc, source_memo_sanitized, trade_log_uid, amount_cents,
  adjustment_type, cash_adjustment_uid, action_type, trade_source, and
  manual_entry.
- Audit mapping includes request_id, idempotency_key, payload_classification,
  redaction_status, sanitized payload shape, audit failure policy, and
  transaction boundary.
- Transaction / rollback / idempotency policy includes duplicate
  idempotency_key behavior, request_id trace behavior, unique index conflict
  handling, safe error mapping, and no partial facts on failure.

### Static Boundary

- `migrations/001_initial_schema.sql` remains unmodified by TASK-191.
- `migrations/002_shell_accounting_manual_entry_schema.sql` remains unmodified by TASK-191.
- No new migration is added.
- No schema file is added.
- DataServiceActions.cpp remains unmodified by TASK-191.
- DataServiceActions.h remains unmodified by TASK-191.
- DataServiceActionRegistrar.cpp remains unmodified by TASK-191.
- TASK-178 validation production code remains unmodified by TASK-191.
- TASK-185 repository scaffold header/source remain unmodified by TASK-191.
- TASK-185 scaffold remains disabled / write-not-implemented.
- TASK-182 validation wiring remains `writeImplemented=false`.
- The only repository implementation added is the TASK-192 DataAccess-only
  manual transaction repository write implementation.
- Runtime SQL INSERT / UPDATE / DELETE / REPLACE is not added outside the
  DataAccess repository implementation.
- SQLite runtime write is not added outside direct repository tests.
- Runtime `trade_log` write is not added through DataService actions.
- No runtime `cash_adjustment` / cash facts / cash ledger write is added.
- No runtime `audit_log` / ledger write is added.
- Production QML / startup / Presenter / Controller remain unmodified.
- AccountingEngine replay remains unmodified.
- StrategyEngine and MarketEngine remain unmodified.
- No TradeDraft or suggestion implementation is added.
- No broker SDK, network, endpoint, credentials, secrets, token, key, password,
  real broker order id, real order placement, or automatic trading is added.

### Regression

- TASK-190 migration implementation semantics remain valid.
- TASK-192 manual transaction repository write implementation semantics remain
  limited to DataAccess direct tests.
- TASK-189 schema implementation authorization semantics remain valid.
- TASK-188 schema gap authorization semantics remain valid.
- TASK-187 schema adequacy review semantics remain valid.
- TASK-186 repository implementation authorization semantics remain valid.
- TASK-185 repository scaffold semantics remain valid.
- TASK-182 validation wiring semantics remain valid.
- TASK-177 MVP authorization gate remains valid.
- Existing broker / real broker / no-network / no-credentials /
  no-order-placement gates are not deleted, weakened, or skipped.

## Required Probes

- Static source scan.
- Documentation keyword probe.
- Git diff path probe against main.
- Forbidden migration and schema file probe.
- TASK-190 migration column and index probe.
- Authorized TASK-192 repository implementation token scan.
- Forbidden runtime SQL / SQLite write scan outside DataAccess direct
  repository implementation.
- Forbidden broker / network / credentials / endpoint / order scan.
- Broker gate retention probe.

## Go / No-Go Checklist

- [ ] TASK-190 commit is present in main.
- [ ] docs/180 is present.
- [ ] docs/181 is present.
- [ ] README / docs/README / docs/12 register TASK-191.
- [ ] `migrations/001_initial_schema.sql` is unchanged.
- [ ] `migrations/002_shell_accounting_manual_entry_schema.sql` is unchanged.
- [ ] No new migration or schema file is added.
- [ ] Only TASK-192 manual transaction repository implementation is added.
- [ ] No DataService write implementation is added.
- [ ] No runtime SQL / SQLite write is added outside DataAccess direct
  repository implementation.
- [ ] No broker SDK, network, credentials, endpoint, real order, or automatic
  trading is added.
- [ ] TASK-191 gate tests pass.
- [ ] Full CTest passes.
- [ ] transport local socket echo 50 repeat passes.
