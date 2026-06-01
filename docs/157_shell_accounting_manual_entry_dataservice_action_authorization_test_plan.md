# TASK-179 ShellAccounting Manual Entry DataService Action Authorization Test Plan

## Document Purpose

This test plan defines TASK-179 static gate coverage for future manual
transaction and cash movement DataService actions. It is a gate-only plan and
does not implement a DataService action, repository, write path, production UI,
broker connection, strategy execution, automatic trading, or real order
placement.

## Test Matrix

### A. Documentation and Registration

- docs/156 exists.
- docs/157 exists.
- README, docs/README, and docs/12 register TASK-179.
- docs/154 and docs/155 record that TASK-179 is only a future DataService
  action authorization gate.

### B. TASK-180 Scaffold-Only DataService Runtime Action

- TASK-179 originally had no DataService runtime action.
- TASK-180 allows only disabled scaffold action registration.
- `accounting.manual_transaction.create` may be registered as disabled
  scaffold.
- `accounting.manual_cash_movement.create` may be registered as disabled
  scaffold.
- No write implementation, repository call, SQLite write, replay, TradeDraft,
  suggestion, broker, network, credentials, endpoint, order placement, or
  automatic trading behavior is allowed.

### C. No Production Write Path

- No DataAccess production manual write repository is added.
- No SQLite write is added.
- No `trade_log` write is added.
- No cash facts or cash ledger write is added.
- No audit / ledger write is added.
- TASK-178 validation remains pure in-memory validation.

### D. No Schema / UI / Replay Drift

- `migrations/001_initial_schema.sql` is not modified.
- No migration is added.
- Production QML is not modified for manual entry UI.
- Production startup is not modified for manual entry registration.
- AccountingEngine production replay is not modified for manual entries.
- StrategyEngine and MarketEngine are not modified.

### E. No TradeDraft / Suggestions

- No TradeDraft implementation is added.
- No trading suggestion implementation is added.
- No golden-tower, sniper, on-exchange, or off-exchange suggestion
  implementation is added.

### F. Broker Safety Regression

- No broker SDK is added.
- No network client, endpoint, host, port, or URL is added.
- No credentials, secrets, tokens, keys, passwords, or usable examples are
  added.
- No real broker order id is generated or stored.
- No real order placement is added.
- No automatic trading is added.
- Existing broker / real broker / no-network / no-credentials /
  no-order-placement gates are not deleted, weakened, or skipped.

### G. Future Task Split

- Gate documentation states that future DataService action scaffold,
  implementation, repository, replay, read model, and UI work require separate
  tasks.
- Broker sandbox new capability development remains paused.

## Required Probes

- Static source scan.
- Documentation registration scan.
- Forbidden action token scan.
- Dispatcher route scan.
- DataServiceActions scaffold scan.
- DataAccess repository scan.
- No write SQL scan.
- No migration scan.
- Production QML scan.
- Startup scan.
- AccountingEngine replay scan.
- StrategyEngine / MarketEngine scan.
- TradeDraft / suggestion scan.
- Broker SDK scan.
- Network / endpoint scan.
- Credential / secret scan.
- Real broker order id scan.
- Real order placement scan.
- Automatic trading scan.
- Existing broker gate retention scan.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/156 merged.
- [ ] docs/157 merged.
- [ ] README, docs/README, and docs/12 register TASK-179.
- [ ] TASK-179 DataService action authorization gate passes.
- [ ] TASK-180 action scaffold tests pass.
- [ ] TASK-178 validation scaffold tests pass.
- [ ] TASK-177 MVP authorization gate passes.
- [ ] Broker disabled, broker order, and real broker gates pass.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean.

