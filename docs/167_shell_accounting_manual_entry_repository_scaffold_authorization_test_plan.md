# TASK-184 ShellAccounting Manual Entry Repository Scaffold Authorization Test Plan

## Document Purpose

This plan defines the static gates and regression probes for a future
ShellAccounting manual entry DataAccess repository scaffold. It does not
implement a repository scaffold, does not add repository headers or sources,
does not write SQLite, does not modify DataService action implementation, does
not modify schema, does not modify production QML or startup, does not connect
broker, and does not add automatic trading.

## Test Matrix

### A. Documentation and registration

- `docs/166_shell_accounting_manual_entry_repository_scaffold_authorization_gate.md`
  exists.
- `docs/167_shell_accounting_manual_entry_repository_scaffold_authorization_test_plan.md`
  exists.
- README, docs/README, and docs/12 register TASK-184.
- docs/164 and docs/165 record that TASK-184 added the future repository
  scaffold authorization gate while TASK-183 remains persistence-gate only.

### B. Production source unchanged

- `DataServiceActions.cpp` is not modified for TASK-184.
- `DataServiceActions.h` is not modified for TASK-184.
- `DataServiceActionRegistrar.cpp` is not modified for TASK-184.
- TASK-178 validation header and source are not modified for TASK-184.
- DataAccess production code and DataAccess CMake are not modified for
  TASK-184.
- No DataService action implementation is added.

### C. TASK-182 validation-only contract

- `accounting.manual_transaction.create` still validates but does not write.
- `accounting.manual_cash_movement.create` still validates but does not write.
- Valid manual transaction payloads still return `writeImplemented=false`.
- Valid manual cash movement payloads still return `writeImplemented=false`.
- Valid payload responses do not include `tradeLogId`, `cashFactId`,
  `auditLogId`, `ledgerId`, or `persistentId`.

### D. No repository scaffold added

- No DataAccess write repository is added.
- No manual entry repository scaffold is added.
- No manual transaction repository header/source is added.
- No manual cash movement repository header/source is added.
- No DataAccess CMake registration is added for manual entry repositories.
- No SQL `INSERT`, `UPDATE`, `DELETE`, `REPLACE`, `CREATE`, `DROP`, or `ALTER`
  is added.
- No SQLite write path is added.
- No `trade_log`, cash facts, cash ledger, audit, or ledger write is added.
- No migration is added.
- `migrations/001_initial_schema.sql` remains unchanged.

### E. No UI, replay, strategy, market, or broker expansion

- Production QML remains unchanged.
- Production startup remains unchanged.
- Presenter and Controller behavior remains unchanged.
- AccountingEngine replay remains unchanged.
- StrategyEngine and MarketEngine remain unchanged.
- No TradeDraft or suggestion implementation is added.
- No broker SDK, network, credential, endpoint, real broker order id, order
  placement, or automatic trading is added.
- Existing broker disabled, real broker, no-network, no-credentials,
  no-order-placement, and no-automatic-trading gates remain present and
  passing.

### F. Future repository policy

- Future repository scaffold must be DataService-only.
- Future repository scaffold must be validation-first.
- Future repository scaffold may define only interface / command / result
  boundaries and must not write DB.
- Future repository implementation must be separately authorized.
- Future write implementation must be separately authorized.
- Future transaction / rollback / idempotency / audit / privacy policies must
  remain explicit and testable.
- Future replay, UI, broker, and schema work must remain separate tasks.

## Required Probes

- Static source scan.
- Docs consistency scan.
- Dispatcher validation-only probe.
- No persistent id probe.
- No repository scaffold scan.
- No repository header/source scan.
- No DataAccess CMake drift scan.
- No write SQL scan.
- No SQLite write scan.
- No migration scan.
- No QML / startup scan.
- No Presenter / Controller scan.
- No replay scan.
- No StrategyEngine / MarketEngine scan.
- No TradeDraft / suggestion scan.
- No broker SDK scan.
- No network / endpoint / credential scan.
- No real order placement scan.
- No automatic trading scan.
- Broker gate retention scan.
- Future task split documentation probe.

## Go / No-Go Checklist

- [ ] docs/166 merged.
- [ ] docs/167 merged.
- [ ] TASK-185 docs/168 and docs/169 merged as the disabled repository
  scaffold implementation.
- [ ] README, docs/README, and docs/12 register TASK-184.
- [ ] docs/164 and docs/165 mention the TASK-184 repository scaffold gate.
- [ ] DataServiceActions.cpp is not changed by TASK-184.
- [ ] DataServiceActions.h is not changed by TASK-184.
- [ ] DataServiceActionRegistrar.cpp is not changed by TASK-184.
- [ ] TASK-178 validation production code is not changed by TASK-184.
- [ ] DataAccess production code and CMake are not changed by TASK-184.
- [ ] TASK-182 valid payload responses still have `writeImplemented=false`.
- [ ] TASK-182 valid payload responses still return no persistent ids.
- [ ] No DataAccess write repository or manual entry repository scaffold exists.
- [ ] No repository header/source exists for manual transaction or cash
  movement persistence.
- [ ] No SQL or SQLite write path exists for manual entry persistence.
- [ ] No `trade_log`, cash facts, cash ledger, audit, or ledger write exists.
- [ ] No schema migration is changed or added.
- [ ] No production QML, startup, Presenter, or Controller change exists.
- [ ] No replay, StrategyEngine, MarketEngine, TradeDraft, suggestion, broker
  SDK, network, credential, endpoint, real order placement, or automatic
  trading change exists.
- [ ] TASK-184 repository scaffold authorization gate tests pass.
- [ ] TASK-183 persistence authorization tests pass.
- [ ] TASK-182 validation wiring tests pass.
- [ ] TASK-181 authorization gate tests pass.
- [ ] TASK-180 scaffold tests pass.
- [ ] TASK-178 validation scaffold tests pass.
- [ ] TASK-177 MVP authorization tests pass.
- [ ] Existing broker gates remain present and passing.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean.

## TASK-185 Regression Note

TASK-185 evolves this authorization gate by allowing only
`ShellAccountingManualEntryRepositoryScaffold` command DTOs, result DTO, and
disabled scaffold methods. It still forbids repository write implementation,
SQL execution, SQLite writes, DataServiceActions behavior changes, persistent
ids, schema changes, QML/startup changes, replay, broker, network,
credentials, endpoints, real order placement, and automatic trading.
