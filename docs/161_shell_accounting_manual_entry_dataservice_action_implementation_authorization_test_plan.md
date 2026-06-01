# TASK-181 ShellAccounting Manual Entry DataService Action Implementation Authorization Test Plan

## Document Purpose

This plan defines TASK-181 gate-only verification for future manual entry
DataService action implementation. It does not implement action logic, payload
parsing, repositories, writes, replay, UI, broker access, or automatic trading.

## Test Matrix

### A. Documentation and registration

- docs/160 exists.
- docs/161 exists.
- README, docs/README, and docs/12 register TASK-181.
- docs/158 and docs/159 record that TASK-181 added a future implementation
  authorization gate.
- Gate documentation defines the future implementation split.

### B. Production source evolution

- `DataServiceActions.cpp` was allowed to evolve in TASK-182 only for payload
  parsing and TASK-178 validation wiring.
- `DataServiceActions.h` is not changed for TASK-181.
- `DataServiceActionRegistrar.cpp` is not changed for TASK-181.
- TASK-178 validation production source remains pure in-memory.
- No production C++ manual entry persistence implementation is added.

### C. TASK-180 scaffold semantics evolved by TASK-182

- Manual transaction action now validates payloads but still returns
  unavailable / write not implemented for accepted validation.
- Manual cash movement action now validates payloads but still returns
  unavailable / write not implemented for accepted validation.
- Validation responses do not return `success=true` for persistence.
- Validation responses do not write SQLite.
- Validation responses do not write `trade_log`.
- Validation responses do not write cash facts or cash ledger records.
- Validation responses do not write audit or ledger records.
- Validation responses do not call a repository.
- Validation responses do not call AccountingEngine replay.

### D. No persistence expansion

- Payload parsing exists only for validation and must not become a write path.
- No DataAccess write repository is added.
- No DB write path is added.
- `migrations/001_initial_schema.sql` is not modified.
- No new migration is added.
- No trade log, cash facts, audit, or ledger write implementation exists.

### E. No UI / replay / strategy drift

- Production QML is not modified.
- Production startup is not modified.
- Presenter and Controller behavior remain unchanged.
- AccountingEngine replay is not modified.
- StrategyEngine and MarketEngine are not modified.
- No TradeDraft or trading suggestion implementation is added.

### F. No broker / network / credentials / automatic trading

- No broker SDK or broker adapter is connected.
- No network, endpoint, host, port, URL, or remote call is added.
- No credentials, secrets, tokens, keys, passwords, or usable examples are
  introduced.
- No real broker order id is generated or stored.
- No real order placement exists.
- No automatic trading exists.
- Broker sandbox new capability work remains paused.

### G. Regression and gate retention

- TASK-180 scaffold tests pass.
- TASK-179 authorization gate tests pass.
- TASK-178 validation scaffold tests pass.
- TASK-177 MVP authorization gate tests pass.
- Existing broker / real broker / no-network / no-credentials /
  no-order-placement gates remain present, are not weakened, and are not skipped.
- Full CTest passes.
- `transport_local_socket_echo` 50 repeat passes.

## Required Probes

- Static source scan.
- Documentation registration scan.
- No production source change marker scan.
- Disabled scaffold response scan.
- No `success=true` implementation scan.
- No payload-to-write scan.
- No repository scan.
- No write SQL scan.
- No migration scan.
- No QML / startup scan.
- No replay scan.
- No StrategyEngine / MarketEngine scan.
- No TradeDraft / suggestion scan.
- No broker SDK scan.
- No network / endpoint scan.
- No credential / secret scan.
- No real order placement scan.
- No automatic trading scan.
- Broker gate retention scan.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/160 merged.
- [ ] docs/161 merged.
- [ ] README, docs/README, and docs/12 register TASK-181.
- [ ] `DataServiceActions.cpp` changes remain limited to TASK-182 validation
  wiring.
- [ ] `DataServiceActions.h` is not modified by TASK-181.
- [ ] `DataServiceActionRegistrar.cpp` is not modified by TASK-181.
- [ ] TASK-180 scaffold actions have evolved only into validation-only
  unavailable / write-not-implemented responses.
- [ ] No persistence implementation exists.
- [ ] No payload-to-write path exists.
- [ ] No DataAccess write repository exists.
- [ ] No database, trade log, cash facts, audit, or ledger writes exist.
- [ ] No schema migration is changed or added.
- [ ] No production QML or startup change exists.
- [ ] No broker SDK, network, credentials, endpoint, real order, or automatic
  trading exists.
- [ ] Existing broker gates remain present and passing.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean.
