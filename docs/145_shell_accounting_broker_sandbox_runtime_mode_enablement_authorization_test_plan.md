# ShellAccounting Broker Sandbox Runtime Mode Enablement Authorization Test Plan (TASK-173)

## Document Purpose

TASK-173 defines the test-only gate for future ShellAccounting broker sandbox runtime mode enablement. This test plan does not implement sandbox runtime, sandbox adapters, external mode sources, credentials providers, endpoint providers, SDK access, network calls, order placement, or database writes.

## Test Matrix

### A. Documentation and Indexing

- docs/144 exists and describes TASK-173 as gate-only.
- docs/145 exists and contains this Test Matrix.
- README, docs/README, and docs/12 reference TASK-173.
- docs/142 and docs/143 state that TASK-173 adds an enablement gate while TASK-172 remains disabled-default selector wiring.

### B. Disabled-Default Runtime

- `DataServiceActions.cpp` still uses `defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()`.
- `DataServiceActions.cpp` still passes only `defaultBrokerPortMode` into `shellAccountingBrokerRuntimeModeSourceForMode`.
- `DataServiceActions.cpp` does not pass Sandbox, Paper, Real, string literals, payload fields, or external values into the selector.
- The default runtime source remains disabled-only.
- The selected source from the default mode remains disabled.
- `brokerPortDisabled` remains true.
- `brokerPortDryRunOnly` remains true.

### C. No Runtime Enablement

- Sandbox runtime is not enabled.
- The TASK-170 selector scaffold remains direct-test-only.
- Direct sandbox selector access remains unavailable / not configured.
- Paper and real runtime sources remain unimplemented.
- No sandbox adapter implementation is introduced.

### D. Mode Source Isolation

- Production source does not read payload, QML, config, environment, command line, files, database, or secret stores as broker mode sources.
- QML, Presenter, Controller, ShellServices, ShellCore, and startup code do not directly provide broker mode or broker adapter access.
- Docs/tests may contain policy keywords, but production source must not contain unauthorized runtime implementation.

### E. No Side Effects

- No broker SDK.
- No network, endpoint, URL, host, or port.
- No credentials, secrets, token, key, password values, or usable examples.
- No real broker order id storage.
- No real order placement.
- No DB, audit, or ledger write path.
- No reconciliation, cancellation, or correction.
- No strategy execution or automatic trading.
- No schema modification.

### F. Regression

- TASK-172 disabled-default selector wiring remains valid.
- TASK-171 wiring authorization gate remains valid.
- TASK-170 selector scaffold remains valid.
- TASK-166 disabled runtime source baseline remains valid.
- Broker disabled, broker order, and real broker gates continue to pass.
- Full CTest and transport_local_socket_echo repeat pass.

## Required Probes

- Static production source scan.
- Static docs/tests policy text probe.
- DataServiceActions disabled-default selector probe.
- Runtime default disabled provider probe.
- Direct sandbox scaffold unavailable probe.
- No external mode source scan.
- No broker SDK / network / endpoint scan.
- No credential value scan.
- No real order id / order placement scan.
- No DB / audit / ledger write scan.
- No reconciliation / cancellation / correction scan.
- No strategy / automatic trading scan.
- Schema unchanged probe.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/144 merged.
- [ ] docs/145 merged.
- [ ] TASK-173 gate tests pass.
- [ ] TASK-172 disabled-default selector wiring tests pass.
- [ ] TASK-171 through TASK-162 broker runtime regressions pass.
- [ ] Broker disabled / broker order / real broker gates pass.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo 50 repeat passes.
- [ ] User explicitly authorizes a future sandbox runtime enablement TASK before any runtime enablement implementation begins.
