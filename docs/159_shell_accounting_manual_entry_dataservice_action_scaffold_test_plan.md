# TASK-180 ShellAccounting Manual Entry DataService Action Scaffold Test Plan

## Document Purpose

This plan defines TASK-180 verification for the ShellAccounting manual entry
DataService action scaffold. TASK-180 registers action names and disabled
handlers only. It does not implement a write path, repository, schema change,
production UI, broker connection, strategy execution, automatic trading, or
real order placement.

## Test Matrix

### A. DataService action scaffold

- `accounting.manual_transaction.create` is registered.
- `accounting.manual_cash_movement.create` is registered.
- Manual transaction returns unavailable / not implemented / disabled
  scaffold semantics.
- Manual cash movement returns unavailable / not implemented / disabled
  scaffold semantics.
- The dispatcher can resolve both actions.

### B. No write path

- No SQLite write.
- No `trade_log` write.
- No cash facts write.
- No cash ledger write.
- No audit write.
- No ledger write.
- No DataAccess write repository.
- No schema or migration modification.

### C. No replay / strategy / suggestion

- No AccountingEngine replay call.
- No read model update.
- No TradeDraft generation.
- No trading suggestion.
- No StrategyEngine or MarketEngine implementation change.

### D. No broker / network / credentials

- No broker SDK.
- No broker adapter.
- No network call, endpoint, host, port, or URL.
- No credentials, secrets, tokens, keys, passwords, or usable examples.
- No real broker order id.
- No real order placement.
- No automatic trading.

### E. No UI / startup drift

- Production QML is not modified.
- Production startup is not modified.
- Presenter and Controller behavior are not changed.

### F. Broker gate regression and gate evolution

- TASK-179 authorization gate evolves to allow only disabled action
  scaffold registration.
- TASK-181 adds the future implementation authorization gate while preserving
  TASK-180 disabled scaffold semantics.
- TASK-178 validation scaffold remains pure in-memory.
- TASK-177 MVP authorization gate remains valid.
- Existing broker / real broker / no-network / no-credentials /
  no-order-placement gates remain present and are not deleted, weakened, or
  skipped.

## Required Probes

- Static source scan.
- Documentation registration scan.
- Dispatcher registration scan.
- Disabled handler response scan.
- No write SQL scan.
- No repository scan.
- No replay scan.
- No TradeDraft / suggestion scan.
- No broker SDK scan.
- No network / endpoint scan.
- No credential / secret scan.
- No migration scan.
- Production QML scan.
- Startup scan.
- Broker gate retention scan.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/158 merged.
- [ ] docs/159 merged.
- [ ] README, docs/README, and docs/12 register TASK-180.
- [ ] Manual transaction action scaffold is registered.
- [ ] Manual cash movement action scaffold is registered.
- [ ] Both actions return disabled scaffold responses.
- [ ] TASK-181 implementation authorization gate passes without changing
      production action implementation.
- [ ] No database write exists.
- [ ] No DataAccess write repository exists.
- [ ] No schema migration is changed or added.
- [ ] No production QML or startup change exists.
- [ ] TASK-179 gate passes with scaffold-only evolution.
- [ ] TASK-178 validation scaffold tests pass.
- [ ] TASK-177 MVP authorization gate passes.
- [ ] Broker disabled, broker order, and real broker gates pass.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean.
