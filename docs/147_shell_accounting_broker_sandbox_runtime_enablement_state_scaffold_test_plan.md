# ShellAccounting Broker Sandbox Runtime Enablement State Scaffold Test Plan (TASK-174)

## Document Purpose

TASK-174 defines tests for the direct-test-only sandbox runtime enablement state scaffold. This plan does not authorize runtime wiring, sandbox runtime enablement, external mode sources, credentials providers, endpoint providers, sandbox adapters, SDK access, network calls, order placement, or database writes.

## Test Matrix

### A. Documentation and Indexing

- docs/146 exists and documents TASK-174.
- docs/147 exists and contains this Test Matrix.
- README, docs/README, and docs/12 reference TASK-174.
- docs/144 and docs/145 state that TASK-174 adds only a disabled state scaffold.

### B. Scaffold API

- Header declares `ShellAccountingBrokerSandboxRuntimeEnablementState`.
- Source defines `defaultShellAccountingBrokerSandboxRuntimeEnablementState()`.
- Default state has `enabled=false`.
- Default state has `available=false`.
- Default state has `failClosed=true`.
- Default state has stable `runtimeMode`, `status`, and `errorCode`.
- No setter, registry, global override, runtime injection API, or constructor arguments are added.

### C. Runtime Isolation

- `DataServiceActions.cpp` is not modified by TASK-174.
- `DataServiceActions.cpp` does not include or call the enablement state scaffold.
- `DataServiceActions.cpp` still performs only disabled-default selector wiring.
- `DataServiceActions.cpp` does not pass Sandbox, Paper, or Real.
- `ShellAccountingBrokerRuntimeModeSource.h/.cpp` are not modified by TASK-174.
- Default runtime source remains disabled.
- Selected runtime source remains disabled when driven by the default source.
- The sandbox runtime remains disabled.
- Direct sandbox selector path remains scaffold unavailable / not configured.

### D. No External Inputs or Side Effects

- No payload, QML, config, environment, command line, file, database, or secret-store mode source.
- No credentials, endpoint, account data, or order data reads.
- No broker SDK.
- No network or endpoint.
- No real credential, secret, token, key, password values, or usable examples.
- No real broker order id storage.
- No order placement.
- No DB, audit, or ledger write path.
- No reconciliation, cancellation, or correction.
- No strategy execution or automatic trading.
- No schema change.

### E. Regression

- TASK-173 enablement authorization gate evolves to allow the TASK-174 disabled state scaffold.
- TASK-175 adds only a future wiring authorization gate; TASK-174 remains direct-test-only.
- TASK-172 disabled-default selector wiring remains valid.
- TASK-166 disabled source baseline remains valid.
- Broker disabled / broker order / real broker order gates pass.
- Full CTest passes.
- transport_local_socket_echo 50-repeat passes.

## Required Probes

- Static scaffold header/source scan.
- Direct default state probe.
- Static `DataServiceActions.cpp` no-integration scan.
- Static runtime source no-change scan.
- Static no external source scan.
- Static no SDK / network / endpoint scan.
- Static no credential value scan.
- Static no real order id / order placement scan.
- Static no DB / audit / ledger write scan.
- Static no reconciliation / cancellation / correction scan.
- Static no strategy / automatic trading scan.
- Schema unchanged probe.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/146 merged.
- [ ] docs/147 merged.
- [ ] TASK-174 scaffold tests pass.
- [ ] TASK-173 authorization gate tests pass.
- [ ] TASK-172 disabled-default selector wiring tests pass.
- [ ] TASK-166 disabled source scaffold tests pass.
- [ ] Broker disabled / broker order / real broker order gates pass.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo 50 repeat passes.
- [ ] User explicitly authorizes a future sandbox runtime enablement TASK before the scaffold is connected to runtime.
