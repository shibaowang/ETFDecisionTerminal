# TASK-177 ShellAccounting Manual Transaction and Cash Movement MVP Authorization Test Plan

## Document Purpose

This test plan defines the TASK-177 static gate for pivoting ShellAccounting
back to the local assisted decision MVP. It verifies that manual transaction and
cash movement work is documented as a future authorization boundary while no
implementation, production UI, schema change, broker access, network,
credentials, real order placement, automatic trading, TradeDraft generation, or
suggestion implementation is added in this task.

TASK-178 is the next allowed step and adds only DTO / validation scaffold. It
does not change the TASK-177 no-runtime-write boundary.

## Test Matrix

### A. Documentation and Priority Shift

- docs/152 exists and documents TASK-177.
- docs/153 exists and contains this test matrix.
- README, docs/README, and docs/12 register TASK-177.
- The MVP priority list names manual buy/sell records, deposit/withdraw
  records, TradeLog/cash facts/position replay stability, daily holdings/cash
  PnL page usability, index retracement golden-tower/sniper suggestions,
  on-exchange/off-exchange buy/sell suggestions, and local
  backup/export/verification/package readiness.
- Broker sandbox new capability development is paused.
- Existing broker gates remain active and must pass.

### B. No Implementation Yet

- No production QML manual transaction UI is added.
- No production startup registration is added.
- `DataServiceActions.cpp` does not add manual transaction or cash movement
  actions.
- DataAccess does not add manual transaction or cash movement write
  repositories.
- AccountingEngine production replay behavior is not changed for manual
  transaction input.
- StrategyEngine and MarketEngine are not modified for this MVP gate.
- No TradeDraft or suggestion implementation is added.
- No migration or schema change is added.

### C. No Broker / Network / Credential Expansion

- No broker SDK is added.
- No network client, endpoint, host, port, or URL is added for broker access.
- No credentials, secrets, tokens, keys, passwords, or usable examples are
  added.
- No real broker order id is stored or returned.
- No real order placement is implemented.
- No automatic trading is implemented.

### D. Existing Safety Gates

- Existing broker-disabled, broker-order, real-broker, no-network,
  no-credentials, no-order-placement, and sandbox-disabled gates are retained.
- Existing gates are not deleted, weakened, or skipped.
- TASK-176 disabled runtime baseline remains valid.
- Policy keywords in docs and tests are allowed only as policy text, not as
  production implementation.

### E. Future Implementation Guardrails

- Future manual buy/sell and deposit/withdraw features must use DataService-only
  boundaries.
- Future production QML must not directly access SQLite, DataAccess, or
  DataServiceClient.
- Future writes must be action allowlisted and repository scoped.
- Future replay must consume authorized facts and must not be called directly by
  QML, Shell, Presenter, or Controller.
- Future suggestions must be separately authorized and must not equal execution.
- Future backup/export/package work must avoid broker/network/credential
  expansion unless separately authorized.

## Required Probes

- Static document existence scan.
- README, docs index, and prompt-template scan.
- MVP priority keyword scan.
- Production QML scan.
- Startup scan.
- DataServiceActions forbidden action scan.
- DataAccess repository scan.
- AccountingEngine replay scan.
- StrategyEngine and MarketEngine scan.
- Migration unchanged and no-new-migration scan.
- Forbidden broker SDK scan.
- Forbidden network and endpoint scan.
- Forbidden credentials, secrets, tokens, keys, passwords, and usable examples
  scan.
- Real broker order id scan.
- Real order placement scan.
- Automatic trading scan.
- TradeDraft and suggestion implementation scan.
- Existing broker gate retention scan.
- Existing gate skip/weaken scan.
- TASK-176 baseline scan.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/152 merged.
- [ ] docs/153 merged.
- [ ] README, docs/README, and docs/12 register TASK-177.
- [ ] TASK-177 gate tests pass.
- [ ] TASK-176 disabled runtime baseline tests pass.
- [ ] Broker disabled, broker order, real broker, no-network,
  no-credentials, and no-order-placement gates pass.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean.
- [ ] Future manual transaction implementation has explicit user
  authorization.
- [ ] Future cash movement implementation has explicit user authorization.
- [ ] Future suggestion implementation has explicit user authorization.
