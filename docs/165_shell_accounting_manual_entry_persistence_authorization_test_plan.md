# TASK-183 ShellAccounting Manual Entry Persistence Authorization Test Plan

## Document Purpose

This plan defines the static gates and regression probes for future
ShellAccounting manual entry persistence. It does not implement repositories,
SQLite writes, manual transaction writes, cash fact writes, audit writes,
ledger writes, replay, UI, broker access, network calls, credentials, endpoints,
real order placement, or automatic trading.

## Test Matrix

### A. Documentation and registration

- `docs/164_shell_accounting_manual_entry_persistence_authorization_gate.md`
  exists.
- `docs/165_shell_accounting_manual_entry_persistence_authorization_test_plan.md`
  exists.
- README, docs/README, and docs/12 register TASK-183.
- docs/162 and docs/163 record that TASK-183 added the future persistence gate
  while TASK-182 remains validation-only.

### B. Production source unchanged

- `DataServiceActions.cpp` is not modified for TASK-183.
- `DataServiceActions.h` is not modified for TASK-183.
- `DataServiceActionRegistrar.cpp` is not modified for TASK-183.
- TASK-178 validation header and source are not modified for TASK-183.
- No new DataService action implementation is added.

### C. TASK-182 validation-only contract

- `accounting.manual_transaction.create` still calls validation but does not
  write.
- `accounting.manual_cash_movement.create` still calls validation but does not
  write.
- Valid manual transaction payloads still return `writeImplemented=false`.
- Valid manual cash movement payloads still return `writeImplemented=false`.
- Valid payload responses do not include `tradeLogId`, `cashFactId`,
  `auditLogId`, `ledgerId`, or `persistentId`.

### D. No persistence added

- No DataAccess write repository is added.
- No manual transaction persistence repository is added.
- No manual cash movement persistence repository is added.
- No SQL `INSERT`, `UPDATE`, `DELETE`, `REPLACE`, `CREATE`, `DROP`, or `ALTER`
  is added for manual entry persistence.
- No SQLite write path is added.
- No `trade_log` write is added.
- No cash facts or cash ledger write is added.
- No audit or ledger write is added.
- No migration is added.
- `migrations/001_initial_schema.sql` remains unchanged.

### E. No UI, replay, strategy, or broker expansion

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

### F. Future persistence policy

- Future persistence must be DataService-only.
- Future persistence must be validation-first.
- Future persistence must use repository boundaries instead of scattered SQL in
  action handlers.
- Future persistence must define transaction / rollback behavior.
- Future persistence must define idempotency / duplicate handling.
- Future persistence must define sanitized audit behavior.
- Future persistence must define privacy / memo sanitization.
- Future repository, write implementation, replay/read-model, and UI tasks must
  remain split and separately authorized.

## Required Probes

- Static source scan.
- Docs consistency scan.
- Dispatcher validation-only probe.
- No persistent id probe.
- No repository scan.
- No write SQL scan.
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

- [ ] docs/164 merged.
- [ ] docs/165 merged.
- [ ] TASK-184 docs/166 and docs/167 merged as the repository scaffold
  authorization gate.
- [ ] README, docs/README, and docs/12 register TASK-183.
- [ ] docs/162 and docs/163 mention the TASK-183 persistence gate.
- [ ] DataServiceActions.cpp is not changed by TASK-183.
- [ ] DataServiceActions.h is not changed by TASK-183.
- [ ] DataServiceActionRegistrar.cpp is not changed by TASK-183.
- [ ] TASK-178 validation production code is not changed by TASK-183.
- [ ] TASK-182 valid payload responses still have `writeImplemented=false`.
- [ ] TASK-182 valid payload responses still return no persistent ids.
- [ ] No DataAccess write repository exists for manual entry persistence.
- [ ] No SQL or SQLite write path exists for manual entry persistence.
- [ ] No `trade_log`, cash facts, cash ledger, audit, or ledger write exists.
- [ ] No schema migration is changed or added.
- [ ] No production QML, startup, Presenter, or Controller change exists.
- [ ] No replay, StrategyEngine, MarketEngine, TradeDraft, suggestion, broker
  SDK, network, credential, endpoint, real order placement, or automatic
  trading change exists.
- [ ] TASK-183 persistence authorization gate tests pass.
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

## TASK-184 Regression Note

TASK-184 adds a repository scaffold authorization gate but still does not
implement a repository scaffold, repository header/source, repository
implementation, write implementation, DataAccess CMake registration, SQL,
SQLite writes, or schema changes. TASK-182 validation-only responses must still
return `writeImplemented=false` and no persistent ids.
