# ShellAccounting Manual Entry DataService Write Wiring Authorization Test Plan

## Document Purpose

This document defines the TASK-197 test plan for the manual entry DataService
write wiring authorization gate. TASK-198 has evolved the gate to allow only the
authorized DataService runtime write wiring through the TASK-192 and TASK-196
repositories. UI, replay, audit write, ledger write, broker, network,
credentials, endpoint, real order placement, and automatic trading remain out of
scope.

## Test Matrix

### Documentation And Registration

- docs/190 exists.
- docs/191 exists.
- README, docs/README, and docs/12 register TASK-197.
- docs/188 and docs/189 record that TASK-197 adds the DataService write wiring
  authorization gate while keeping TASK-196 DataAccess-only repository semantics.
- docs/190 records TASK-197 as authorization-only.
- docs/190 states TASK-198 implements the separately authorized wiring.
- docs/190 states DataServiceActions may call only repository boundaries.
- docs/190 states DataService wiring must stay validation-first.
- docs/190 states future wiring uses the TASK-192 manual transaction repository.
- docs/190 states future wiring uses the TASK-196 manual cash movement
  repository.
- docs/190 states validation-first.
- docs/190 states no scattered SQL in DataServiceActions.
- docs/190 states sanitized response and safe error mapping.
- docs/190 states no broker and no automatic trading.

### Protected Production Files

- DataServiceActions.cpp is modified only for TASK-198 repository wiring.
- DataServiceActions.h is not modified.
- DataServiceActionRegistrar.cpp is not modified.
- TASK-178 validation production code is not modified.
- TASK-192 repository header/source are not modified.
- TASK-196 repository header/source are not modified.
- Migrations 001 and 002 are not modified.
- No migration or schema file is added.
- Production QML, startup, Presenter, and Controller are not modified.
- AccountingEngine replay is not modified.
- StrategyEngine and MarketEngine are not modified.

### Validation-First Repository Runtime Semantics

- TASK-182 validation-first behavior is retained.
- Valid manual transaction payload writes through
  `ShellAccountingManualTransactionRepository`.
- Valid manual cash movement payload writes through
  `ShellAccountingManualCashMovementRepository`.
- Invalid payloads still do not write database rows.
- DataService action responses return sanitized local repository write results.
- DataServiceActions calls only `ShellAccountingManualTransactionRepository`.
- DataServiceActions calls only `ShellAccountingManualCashMovementRepository`.
- DataServiceApi does not add runtime SQL insert / update / delete / replace.
- DataServiceApi does not add SQLite runtime write path.
- DataService action writes only through the repository boundary. Manual
  transaction may write `trade_log` / `trade_execution_group`; manual cash
  movement may write `trade_log` / `cash_adjustment`. It still does not write
  `audit_log` or ledger rows.

### Forbidden Integrations

- No TradeDraft implementation or suggestion implementation is added.
- No broker SDK is added.
- No network or endpoint is added.
- No credentials, secrets, token, key, password value, or usable example is
  added.
- No real broker order id is added.
- No real order placement is added.
- No automatic trading is added.
- Existing broker disabled, broker order, real broker, no-network,
  no-credentials, no-order-placement, and no-automatic-trading gates are retained
  and passing.

### Regression

- TASK-196 dual-write repository tests pass.
- TASK-192 manual transaction repository tests pass.
- TASK-182 validation-only wiring tests pass.
- TASK-195 schema contract alignment tests pass.
- TASK-194 manual cash movement repository authorization tests pass.
- Broker disabled, broker order, real broker authorization, and real broker
  implementation gates pass.
- transport local socket echo 50 repeat passes.
- Full CTest passes.

## Required Probes

- Static source scan.
- Changed path allowlist scan.
- Forbidden DataServiceActions diff scan.
- Forbidden DataServiceActionRegistrar diff scan.
- Forbidden migration / schema file scan.
- Forbidden production QML / startup / Presenter / Controller scan.
- Validation-only DataService response probe.
- No persistent id response probe.
- Repository-only boundary in DataServiceActions probe.
- No runtime SQL / SQLite write probe.
- Repository-mediated trade_log / cash_adjustment write probe.
- No audit / ledger DataService action write probe.
- No replay / read model / UI probe.
- No TradeDraft / suggestion probe.
- No broker / network / credentials / endpoint / real order / automatic trading
  probe.
- Broker gate retention probe.

## Go / No-Go Checklist

- [ ] docs/190 exists.
- [ ] docs/191 exists.
- [ ] README, docs/README, and docs/12 register TASK-197.
- [ ] TASK-197 gate tests pass.
- [ ] TASK-196 repository tests pass.
- [ ] TASK-192 repository tests pass.
- [ ] TASK-182 validation-first wiring tests pass.
- [ ] TASK-195 gate tests pass.
- [ ] TASK-194 gate tests pass.
- [ ] Broker gates pass.
- [ ] transport 50 passes.
- [ ] Full CTest passes.
- [ ] git diff --check passes.
- [ ] git status is clean.
