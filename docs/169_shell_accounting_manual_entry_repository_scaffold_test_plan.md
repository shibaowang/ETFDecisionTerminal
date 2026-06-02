# TASK-185 ShellAccounting Manual Entry Repository Scaffold Test Plan

## Document Purpose

This plan defines the tests for the TASK-185 manual entry repository scaffold.
The scaffold is a disabled DataAccess boundary only. It does not implement
database persistence, SQL execution, DataService action writes, replay, UI,
broker access, network calls, credentials, endpoints, real order placement, or
automatic trading.

## Test Matrix

### A. Documentation and registration

- `docs/168_shell_accounting_manual_entry_repository_scaffold.md` exists.
- `docs/169_shell_accounting_manual_entry_repository_scaffold_test_plan.md`
  exists.
- README, docs/README, and docs/12 register TASK-185.
- docs/166 and docs/167 record that TASK-185 implemented only the disabled
  repository scaffold.

### B. Scaffold files and CMake

- `ShellAccountingManualEntryRepositoryScaffold.h` exists.
- `ShellAccountingManualEntryRepositoryScaffold.cpp` exists.
- DataAccess CMake registers the scaffold source.
- The scaffold compiles in `ETFDataAccess`.

### C. DTO boundary

- Manual transaction command DTO exists.
- Manual cash movement command DTO exists.
- Persistence result DTO exists.
- Command DTOs include account, portfolio, source, memo, and idempotency fields.
- Result DTO does not include persistent id fields.

### D. Disabled behavior

- `persistManualTransaction(...)` exists.
- `persistManualCashMovement(...)` exists.
- Manual transaction scaffold returns `success=false`.
- Manual transaction scaffold returns `writeImplemented=false`.
- Manual cash movement scaffold returns `success=false`.
- Manual cash movement scaffold returns `writeImplemented=false`.
- Both results return `databaseWritten=false`.
- Both results return `tradeLogWritten=false`.
- Both results return `cashFactsWritten=false`.
- Both results return `auditWritten=false`.
- Both results return `ledgerWritten=false`.
- Both results return `E9001_SERVICE_UNAVAILABLE`.

### E. No write path

- Scaffold source contains no SQL execution.
- Scaffold source contains no `INSERT`, `UPDATE`, or `DELETE`.
- Scaffold source contains no SQLite mutation API use.
- Scaffold source writes no `trade_log`.
- Scaffold source writes no cash facts / cash ledger.
- Scaffold source writes no audit / ledger.
- Scaffold source produces no persistent ids.

### F. Existing behavior unchanged

- DataServiceActions.cpp is not modified by TASK-185.
- DataServiceActions.h is not modified by TASK-185.
- DataServiceActionRegistrar.cpp is not modified by TASK-185.
- TASK-178 validation production code is not modified by TASK-185.
- TASK-182 validation wiring still returns `writeImplemented=false`.
- `migrations/001_initial_schema.sql` is not modified.
- No migration is added.
- Production QML and startup are not modified.
- Presenter / Controller are not modified.
- AccountingEngine replay is not modified.
- StrategyEngine / MarketEngine are not modified.
- No TradeDraft / suggestion implementation is added.

### G. Broker boundaries retained

- No broker SDK is added.
- No network / endpoint is added.
- No credentials, secrets, token, key, or password examples are added.
- No real broker order id is added.
- No real order placement is added.
- No automatic trading is added.
- Existing broker / real broker / no-network / no-credentials /
  no-order-placement gates remain present and passing.

## Required Probes

- Direct repository scaffold unit probe.
- Static source scan.
- Header DTO scan.
- CMake registration scan.
- No SQL token scan.
- No SQLite mutation scan.
- No persistent id scan.
- No DataService action drift scan.
- No validation production drift scan.
- No schema / migration scan.
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

## Go / No-Go Checklist

- [ ] docs/168 merged.
- [ ] docs/169 merged.
- [ ] README, docs/README, and docs/12 register TASK-185.
- [ ] docs/166 and docs/167 record the TASK-185 disabled scaffold evolution.
- [ ] TASK-186 repository implementation authorization gate passes before any
  future write repository implementation starts.
- [ ] Scaffold header/source merged.
- [ ] DataAccess CMake registers the scaffold source.
- [ ] Scaffold methods return disabled / write-not-implemented results.
- [ ] No SQL or SQLite write exists.
- [ ] No `trade_log`, cash facts, cash ledger, audit, or ledger write exists.
- [ ] No persistent ids are returned.
- [ ] DataServiceActions.cpp is not changed by TASK-185.
- [ ] DataServiceActions.h is not changed by TASK-185.
- [ ] DataServiceActionRegistrar.cpp is not changed by TASK-185.
- [ ] TASK-178 validation production code is not changed by TASK-185.
- [ ] TASK-182 valid payload responses still have `writeImplemented=false`.
- [ ] No schema migration is changed or added.
- [ ] No production QML, startup, Presenter, or Controller change exists.
- [ ] No replay, StrategyEngine, MarketEngine, TradeDraft, suggestion, broker
  SDK, network, credential, endpoint, real order placement, or automatic
  trading change exists.
- [ ] TASK-185 repository scaffold tests pass.
- [ ] TASK-184 authorization tests pass.
- [ ] TASK-183 persistence authorization tests pass.
- [ ] TASK-182 validation wiring tests pass.
- [ ] TASK-178 validation scaffold tests pass.
- [ ] TASK-177 MVP authorization tests pass.
- [ ] Existing broker gates remain present and passing.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean.
