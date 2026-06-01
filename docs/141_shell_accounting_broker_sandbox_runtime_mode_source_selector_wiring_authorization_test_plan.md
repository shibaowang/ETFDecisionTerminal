# ShellAccounting Broker Sandbox Runtime Mode Source Selector Wiring Authorization Test Plan (TASK-171)

## Document Purpose

This plan defines the TASK-171 gate tests for future wiring of the TASK-170 selector scaffold into `DataServiceActions`. It does not implement runtime wiring, does not modify `DataServiceActions.cpp`, does not enable sandbox runtime, and does not add external mode sources.

TASK-172 evolves this gate to allow only disabled-default selector wiring in `DataServiceActions`. Sandbox runtime, external mode sources, broker SDK, network, credentials, endpoint, order placement, and schema changes remain unauthorized.

## Test Matrix

### Documentation And Indexing

- docs/140 exists.
- docs/141 exists.
- README links docs/140 and docs/141.
- docs/README links docs/140 and docs/141.
- docs/12 mentions TASK-171 and the selector wiring authorization boundary.
- docs/138 and docs/139 mention that TASK-171 adds a wiring authorization gate while TASK-170 remains direct-test-only.

### Wiring Isolation

- `DataServiceActions.cpp` may call `shellAccountingBrokerRuntimeModeSourceForMode` only with `defaultBrokerPortMode` from `defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()`.
- `DataServiceActions.cpp` does not read payload, QML, config, environment, command-line, file, database, or secret-store runtime mode sources.
- `ShellAccountingBrokerRuntimeModeSource.h/.cpp` still contain the TASK-170 selector scaffold.
- The selector scaffold remains direct-test-only.
- `defaultShellAccountingBrokerRuntimeModeSource()` remains disabled-only.
- Sandbox runtime remains disabled.
- Direct sandbox selector path still reaches sandbox scaffold unavailable / not configured behavior.
- Paper and real runtime mode sources remain unimplemented.

### Forbidden Side Effects

- Broker SDK remains absent.
- Network, endpoint, URL, host, and port remain absent.
- Real credentials, secrets, tokens, keys, passwords, or usable examples remain absent.
- Real broker order id storage remains absent.
- Order placement remains absent.
- DB, audit, and ledger write paths are not added by the runtime mode source selector path.
- Reconciliation, cancellation, and correction remain absent.
- Strategy execution and automatic trading remain absent.
- Schema remains unchanged.

### Regression

- TASK-170 selector scaffold semantics remain valid.
- TASK-171 authorization gate semantics remain valid after evolving to allow disabled-default selector wiring.
- TASK-169 authorization gate semantics remain valid.
- TASK-166 disabled source baseline remains valid.
- TASK-165 through TASK-162 broker selector regressions pass.
- Broker disabled / broker order / real broker order gates pass.
- Full CTest passes.
- `transport_local_socket_echo` 50-repeat passes.

## Required Probes

- Static `DataServiceActions.cpp` scan proving only disabled-default selector wiring.
- Static external-source scan for payload, QML, config, environment, command-line, file, database, and secret-store mode tokens.
- Static production source scan for broker SDK, network, endpoint, credentials, order id, order placement, writes, reconciliation, cancellation, correction, strategy, and automatic trading.
- Direct runtime probes proving the default source remains disabled and sandbox selector remains unavailable / not configured.
- Documentation probes for disabled-default future wiring, no sandbox runtime enablement, fail-closed behavior, rollback, and separate TASK requirements.
- Scope probe proving docs/tests policy keywords are not treated as production implementation violations.

## Go / No-Go Checklist

- [ ] docs/140 merged.
- [ ] docs/141 merged.
- [ ] TASK-171 tests pass.
- [ ] `DataServiceActions.cpp` uses only disabled-default selector wiring.
- [ ] `ShellAccountingBrokerRuntimeModeSource.h/.cpp` are not modified by TASK-171.
- [ ] Default runtime source remains disabled-only.
- [ ] TASK-170 selector scaffold remains direct-test-only.
- [ ] Sandbox runtime remains disabled.
- [ ] No external mode source is added.
- [ ] No SDK, network, credentials, endpoint, real order id, or order placement is added.
- [ ] No DB, audit, ledger, reconciliation, cancellation, correction, strategy, automatic trading, or schema change is added.
- [ ] TASK-170 through TASK-162 regressions pass.
- [ ] Broker disabled / broker order / real broker order gates pass.
- [ ] Full CTest passes.
- [ ] transport 50 repeat passes.
