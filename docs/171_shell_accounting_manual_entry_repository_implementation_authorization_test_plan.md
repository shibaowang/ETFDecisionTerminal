# TASK-186 ShellAccounting Manual Entry Repository Implementation Authorization Test Plan

## Document Purpose

This document defines the TASK-186 gate-only test plan for future manual entry
DataAccess repository implementation. It does not implement repository writes,
SQL, SQLite mutation, schema changes, DataService action writes, QML, broker
integration, strategy execution, or automatic trading.

## Test Matrix

### Documentation And Indexing

- `docs/170` and `docs/171` exist.
- README, docs index, and Codex prompt template reference TASK-186.
- TASK-185 docs state that repository implementation still requires a later
  authorization task.
- TASK-186 documents the split between repository implementation, schema
  authorization, DataService action write implementation, replay, and UI.

### TASK-185 Scaffold Regression

- TASK-185 scaffold header and source are not modified by TASK-186.
- The scaffold still returns `success=false`.
- The scaffold still returns `writeImplemented=false`.
- The scaffold still returns `databaseWritten=false`.
- The scaffold still returns `tradeLogWritten=false`.
- The scaffold still returns `cashFactsWritten=false`.
- The scaffold still returns `auditWritten=false`.
- The scaffold still returns `ledgerWritten=false`.
- The scaffold returns no persistent ids.

### DataService And Validation Regression

- `DataServiceActions.cpp` is not modified by TASK-186.
- `DataServiceActions.h` is not modified by TASK-186.
- `DataServiceActionRegistrar.cpp` is not modified by TASK-186.
- TASK-178 validation code is not modified by TASK-186.
- TASK-182 validation wiring remains validation-only.
- Valid manual entry payloads still return `writeImplemented=false`.
- Valid manual entry payloads still do not return persistent ids.

### No Write Path

- No repository implementation is added.
- No SQL insert, update, delete, replace, create, drop, or alter path is added.
- No SQLite write path is added.
- No `trade_log` write path is added.
- No cash facts or cash ledger write path is added.
- No audit or ledger write path is added.
- `migrations/001_initial_schema.sql` is not modified.
- No migration is added.

### Production Boundary

- Production QML is not modified.
- Production startup is not modified.
- Presenter and controller code are not modified.
- AccountingEngine replay is not modified.
- StrategyEngine and MarketEngine are not modified.
- No TradeDraft or suggestion implementation is added.

### Broker And Runtime Safety

- No broker SDK is added.
- No network call or endpoint is added.
- No credentials, secrets, token values, keys, or password examples are added.
- No real broker order id is added.
- No real order placement is added.
- No automatic trading is added.
- Existing broker, real broker, no-network, no-credentials, and
  no-order-placement gates are not deleted, weakened, or bypassed.

## Required Probes

- Static source scan.
- Documentation consistency scan.
- Forbidden SQL token scan.
- Forbidden SQLite write token scan.
- Forbidden persistent id scan.
- Forbidden broker, network, credential, endpoint, order placement, and
  automatic trading token scan.
- DataServiceActions / registrar drift scan.
- TASK-178 validation drift scan.
- TASK-185 scaffold direct behavior probe.
- TASK-182 valid payload response probe.
- Broker gate retention probe.

## Go / No-Go Checklist

- [ ] docs/170 merged.
- [ ] docs/171 merged.
- [ ] TASK-186 gate tests pass.
- [ ] TASK-185 scaffold tests pass.
- [ ] TASK-184 repository scaffold authorization tests pass.
- [ ] TASK-183 persistence authorization tests pass.
- [ ] TASK-182 validation wiring tests pass.
- [ ] TASK-178 validation scaffold remains pure in-memory.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` passes 50 repeats.
- [ ] User explicitly authorizes future repository implementation.
- [ ] User explicitly authorizes any schema task if schema is insufficient.

