# ShellAccounting Broker Sandbox Runtime Mode Source Selector Disabled Wiring Test Plan (TASK-172)

## Document Purpose

This plan defines tests for TASK-172 disabled-default selector wiring in `DataServiceActions`. It does not authorize sandbox runtime enablement, external mode sources, broker SDK, network, credentials, endpoint, real broker order id storage, order placement, database write beyond the existing audit dry-run path, ledger write, reconciliation, cancellation, correction, strategy execution, automatic trading, or schema changes.

## Test Matrix

### Documentation And Indexing

- docs/142 exists.
- docs/143 exists.
- README links docs/142 and docs/143.
- docs/README links docs/142 and docs/143.
- docs/12 mentions TASK-172 and disabled-default selector wiring.
- docs/140 and docs/141 explain that TASK-172 implemented only disabled-default selector wiring.

### Disabled-Default Runtime Wiring

- `DataServiceActions.cpp` calls `shellAccountingBrokerRuntimeModeSourceForMode`.
- `DataServiceActions.cpp` first reads `defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()`.
- `DataServiceActions.cpp` passes only `defaultBrokerPortMode` into the selector.
- `DataServiceActions.cpp` does not pass Sandbox, Paper, Real, string literals, payload fields, or external input into the selector.
- Default runtime source still returns Disabled.
- Selected source from the default mode still returns Disabled.
- Broker dry-run runtime still reaches the disabled/null provider.
- Runtime `brokerPortMode` remains `disabled`.
- Runtime `brokerPortDisabled` remains true.
- Runtime `brokerPortDryRunOnly` remains true.

### Isolation And Forbidden Sources

- `DataServiceActions.cpp` does not read payload mode fields.
- `DataServiceActions.cpp` does not read QML, config, environment, command-line, file, database, or secret-store broker mode.
- `DataServiceActions.cpp` does not read credentials, endpoint, account data, or order data as mode source.
- Sandbox source scaffold remains direct-test-only.
- Direct sandbox selector path still reaches scaffold unavailable / not configured behavior.
- Sandbox runtime remains disabled.
- Paper and real runtime sources remain unimplemented.

### Forbidden Side Effects

- Broker SDK remains absent.
- Network, endpoint, URL, host, and port remain absent.
- Real credentials, secrets, tokens, keys, passwords, or usable examples remain absent.
- Real broker order id storage remains absent.
- Order placement remains absent.
- New DB, audit, and ledger write paths are not added by selector wiring.
- Reconciliation, cancellation, and correction remain absent.
- Strategy execution and automatic trading remain absent.
- Schema remains unchanged.

### Regression

- TASK-171 authorization gate semantics evolve to allow only disabled-default selector wiring.
- TASK-170 selector scaffold semantics remain valid.
- TASK-166 disabled source baseline remains valid.
- TASK-165 through TASK-162 broker selector regressions pass.
- Broker disabled / broker order / real broker order gates pass.
- Full CTest passes.
- `transport_local_socket_echo` 50-repeat passes.

## Required Probes

- Static `DataServiceActions.cpp` scan for disabled-default selector wiring.
- Static source scan proving no external mode sources.
- Direct runtime probes for default source, selected source, disabled broker port mode, disabled flag, and dry-run-only flag.
- Static broker side-effect scan for SDK, network, endpoint, credentials, order id, order placement, writes, reconciliation, cancellation, correction, strategy, and automatic trading.
- Documentation probes for rollback, no sandbox runtime enablement, and separate TASK requirements.

## Go / No-Go Checklist

- [ ] docs/142 merged.
- [ ] docs/143 merged.
- [ ] TASK-172 tests pass.
- [ ] `DataServiceActions.cpp` calls the selector only with default disabled source mode.
- [ ] `ShellAccountingBrokerRuntimeModeSource.h/.cpp` are not modified by TASK-172.
- [ ] Default runtime source remains disabled-only.
- [ ] Selected source from default remains disabled.
- [ ] Runtime broker port remains disabled/null provider.
- [ ] TASK-170 selector scaffold remains direct-test-only.
- [ ] Sandbox runtime remains disabled.
- [ ] No external mode source is added.
- [ ] No SDK, network, credentials, endpoint, real order id, or order placement is added.
- [ ] No new DB, audit, ledger, reconciliation, cancellation, correction, strategy, automatic trading, or schema change is added.
- [ ] TASK-171 through TASK-162 regressions pass.
- [ ] Broker disabled / broker order / real broker order gates pass.
- [ ] Full CTest passes.
- [ ] transport 50 repeat passes.
