# ShellAccounting Broker Sandbox Runtime Enablement State Wiring Authorization Test Plan (TASK-175)

## Document Purpose

TASK-175 defines test-only gates for a future task that may wire the TASK-174 sandbox runtime enablement state scaffold into `DataServiceActions.cpp`.

This plan does not implement runtime wiring, sandbox runtime enablement, external mode sources, credentials providers, endpoint providers, sandbox adapters, SDK access, network calls, order placement, database writes, audit writes, or ledger writes.

## Test Matrix

### A. Documentation and Indexing

- docs/148 exists and documents TASK-175 as gate-only.
- docs/149 exists and contains this Test Matrix.
- README, docs/README, and docs/12 reference TASK-175.
- docs/146 and docs/147 state that TASK-175 adds a future wiring authorization gate but does not wire the scaffold.

### B. No Wiring Yet

- `DataServiceActions.cpp` does not include `ShellAccountingBrokerSandboxRuntimeEnablement.h`.
- `DataServiceActions.cpp` does not call `defaultShellAccountingBrokerSandboxRuntimeEnablementState()`.
- `DataServiceActions.cpp` does not read enablement state fields.
- `DataServiceActions.cpp` still performs only disabled-default selector wiring.
- `DataServiceActions.cpp` does not pass Sandbox, Paper, or Real into runtime selection.
- `ShellAccountingBrokerSandboxRuntimeEnablement.h/.cpp` are not modified by TASK-175.
- `ShellAccountingBrokerRuntimeModeSource.h/.cpp` are not modified by TASK-175.

### C. Default Enablement State

- Default enablement state remains `enabled=false`.
- Default enablement state remains `available=false`.
- Default enablement state remains `failClosed=true`.
- Default runtime mode, status, and error code remain stable.
- The scaffold remains direct-test-only and has no setter, registry, override, or runtime injection API.

### D. Runtime Isolation

- Sandbox runtime is not enabled.
- TASK-170 selector scaffold remains direct-test-only.
- Direct sandbox selector path remains unavailable / not configured.
- Sandbox broker adapter implementation is not added.
- Paper and real runtime sources are not implemented.
- Unknown, blank, unsupported, paper, and real modes fail closed.

### E. No External Inputs or Side Effects

- No payload, QML, config, environment, command line, file, database, or secret-store mode source.
- No credential, endpoint, account, or order-data source.
- No broker SDK.
- No network, endpoint, URL, host, or port.
- No real credential, secret, token, key, password value, or usable example.
- No real broker order id storage.
- No order placement.
- No DB, audit, or ledger write path.
- No reconciliation, cancellation, or correction.
- No strategy execution or automatic trading.
- No schema change.

### F. Regression

- TASK-174 enablement state scaffold semantics remain valid.
- TASK-173 enablement authorization gate semantics remain valid.
- TASK-172 disabled-default selector wiring semantics remain valid.
- TASK-166 disabled source baseline remains valid.
- Broker disabled, broker order, and real broker order gates pass.
- Full CTest passes.
- `transport_local_socket_echo` 50-repeat passes.

## Required Probes

- Static production source scan.
- Static docs/tests policy keyword scan.
- Static `DataServiceActions.cpp` no-wiring scan.
- Static enablement state scaffold no-runtime-injection scan.
- Direct default state probe.
- Static runtime source no-change scan.
- Static no external mode source scan.
- Static no SDK / network / endpoint scan.
- Static no credential value scan.
- Static no real order id / order placement scan.
- Static no DB / audit / ledger write scan.
- Static no reconciliation / cancellation / correction scan.
- Static no strategy / automatic trading scan.
- Schema unchanged probe.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/148 merged.
- [ ] docs/149 merged.
- [ ] TASK-175 wiring authorization gate tests pass.
- [ ] TASK-174 scaffold tests pass.
- [ ] TASK-173 authorization gate tests pass.
- [ ] TASK-172 disabled-default selector wiring tests pass.
- [ ] TASK-166 disabled source scaffold tests pass.
- [ ] Broker disabled / broker order / real broker order gates pass.
- [ ] Full CTest passes.
- [ ] `transport_local_socket_echo` 50 repeat passes.
- [ ] User explicitly authorizes a future runtime wiring implementation TASK before the scaffold is connected to runtime.
