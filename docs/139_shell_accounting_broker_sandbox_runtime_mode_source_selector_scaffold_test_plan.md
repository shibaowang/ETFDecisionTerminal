# ShellAccounting Broker Sandbox Runtime Mode Source Selector Scaffold Test Plan (TASK-170)

## Document Purpose

This plan defines tests for the TASK-170 direct-test runtime mode source selector scaffold. It does not authorize runtime wiring, sandbox runtime enablement, external mode sources, broker SDK, network, credentials, endpoint, real broker order id storage, order placement, database write, audit write, ledger write, reconciliation, cancellation, correction, strategy execution, automatic trading, or schema changes.

TASK-171 adds a wiring authorization gate. TASK-172 later implements only disabled-default selector wiring, while the TASK-170 sandbox selector path remains direct-test-only.

## Test Matrix

### Documentation And Indexing

- docs/138 exists.
- docs/139 exists.
- README links docs/138 and docs/139.
- docs/README links docs/138 and docs/139.
- docs/12 mentions TASK-170 and the selector scaffold boundary.

### Selector Scaffold

- Header declares `shellAccountingBrokerRuntimeModeSourceForMode`.
- Source defines `shellAccountingBrokerRuntimeModeSourceForMode`.
- `Disabled` returns disabled runtime mode source.
- `Sandbox` returns TASK-168 sandbox runtime mode source scaffold.
- `Unsupported` fails closed to disabled runtime mode source.
- Unknown enum values fail closed to disabled runtime mode source.
- No constructor parameters, setter, registry, global override, or runtime injection API are added.

### Runtime Isolation

- `defaultShellAccountingBrokerRuntimeModeSource()` remains disabled-only.
- `DataServiceActions.cpp` calls the selector scaffold only with the disabled default source mode.
- Runtime dry-run remains disabled/null-provider behavior.
- Runtime broker port mode remains disabled.
- Runtime `brokerPortDisabled` remains true.
- Runtime `brokerPortDryRunOnly` remains true.
- Sandbox runtime remains disabled.
- TASK-168 sandbox source scaffold remains direct-test-only.
- Direct sandbox selector path still reaches sandbox order port scaffold unavailable / not configured behavior.

### External Source And Side Effects

- Selector does not read payload.
- Selector does not read QML, startup, Presenter, or Controller state.
- Selector does not read config, environment, command line, file, database, or secret store.
- Selector does not carry credentials, endpoint, account data, order data, or broker payload.
- Paper and real runtime mode sources remain unimplemented.
- Broker SDK remains absent.
- Network, endpoint, URL, host, and port remain absent.
- Real credentials, secrets, tokens, keys, passwords, or usable examples remain absent.
- Real broker order id storage remains absent.
- Order placement remains absent.
- DB, audit, and ledger write paths remain absent.
- Reconciliation, cancellation, and correction remain absent.
- Strategy execution and automatic trading remain absent.
- Schema remains unchanged.

### Regression

- TASK-169 authorization gate semantics remain valid while allowing the TASK-170 direct-test selector scaffold.
- TASK-171 wiring authorization gate semantics remain valid after allowing disabled-default selector wiring.
- TASK-168 scaffold semantics remain valid.
- TASK-166 disabled scaffold semantics remain valid.
- TASK-165 through TASK-162 broker selector regressions pass.
- Broker disabled / broker order / real broker order gates pass.
- Full CTest passes.
- `transport_local_socket_echo` 50-repeat passes.

## Required Probes

- Static source scan for the selector scaffold declaration and definition.
- Runtime direct probes for Disabled, Sandbox, Unsupported, and unknown enum values.
- Static `DataServiceActions.cpp` scan proving no selector runtime wiring.
- Static external-source scan for payload, QML, config, environment, command-line, file, database, and secret-store tokens.
- Static broker side-effect scan for SDK, network, endpoint, credentials, order id, order placement, writes, reconciliation, cancellation, correction, strategy, and automatic trading.
- Documentation probes for rollback, fail-closed behavior, and future TASK requirements.

## Go / No-Go Checklist

- [ ] docs/138 merged.
- [ ] docs/139 merged.
- [ ] Header declares the direct-test selector scaffold.
- [ ] Source defines the direct-test selector scaffold.
- [ ] Default runtime source remains disabled-only.
- [ ] `DataServiceActions.cpp` remains unwired from the selector.
- [ ] Sandbox runtime remains disabled.
- [ ] No external mode source is added.
- [ ] No SDK, network, credentials, endpoint, real order id, or order placement is added.
- [ ] No DB, audit, ledger, reconciliation, cancellation, correction, strategy, automatic trading, or schema change is added.
- [ ] TASK-170 tests pass.
- [ ] TASK-169 through TASK-162 regressions pass.
- [ ] Full CTest passes.
- [ ] transport 50 repeat passes.
