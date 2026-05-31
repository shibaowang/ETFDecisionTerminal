# ShellAccounting Broker Runtime Mode Source Disabled Scaffold Test Plan (TASK-166)

## Document Purpose

TASK-166 defines tests for a disabled-only internal broker runtime mode source. This plan verifies the source exists, returns only disabled mode, is used by `DataServiceActions`, and does not enable sandbox runtime or any external source.

## Test Matrix

### Docs And Index

- docs/130 exists and documents TASK-166.
- docs/131 exists and contains this Test Matrix.
- README, docs/README, and docs/12 index TASK-166.
- docs/128 and docs/129 are updated from gate-only to note the disabled-only scaffold.

### Source Boundary

- `ShellAccountingBrokerRuntimeModeSource` compiles.
- `DisabledShellAccountingBrokerRuntimeModeSource` can be instantiated.
- `defaultShellAccountingBrokerRuntimeModeSource()` returns a source.
- The default source returns `ShellAccountingBrokerOrderPortMode::Disabled`; default source returns disabled.
- The source contains no sandbox, paper, or real runtime selector behavior.

### DataServiceActions Wiring

- `DataServiceActions` includes `ShellAccountingBrokerRuntimeModeSource.h`.
- `DataServiceActions` calls `defaultShellAccountingBrokerRuntimeModeSource()`.
- `DataServiceActions` calls `.brokerOrderPortMode()`.
- `DataServiceActions` passes `brokerPortMode` to `shellAccountingBrokerOrderPortForMode(brokerPortMode)`.
- `DataServiceActions` does not call `shellAccountingBrokerOrderPortForMode("sandbox")`, `"paper"`, or `"real"`.

### Runtime Behavior

- Broker dry-run remains disabled/null provider behavior.
- `brokerPortMode` remains `disabled`.
- `brokerPortDisabled` remains true.
- brokerPortDryRunOnly remains true for the disabled/null provider.
- `BROKER_ORDER_DISABLED` remains unchanged.
- Direct sandbox selector access still returns scaffold unavailable behavior and is not runtime mode.
- Unknown mode still fails closed.

### No External Source

- No payload mode source.
- No QML, Presenter, or startup mode source.
- No config, environment, command-line, file, database, or secret-store source.
- No sandbox, paper, or real runtime source.

### No External Side Effects

- No broker SDK.
- No network.
- No credentials, secrets, token, key, password, endpoint, host, or API path.
- No real broker order id.
- No order placement.
- No database, audit, or ledger write.
- No reconciliation, cancellation, or correction.
- No strategy execution or automatic trading.

### Regression

- TASK-160 through TASK-165 regression docs and tests remain valid under the disabled-only source baseline.
- Full CTest passes.
- Transport echo passes 50 repeats.
- `git diff --check` passes.

## Required Probes

- Static source scan.
- Static forbidden runtime source scan.
- Static DataServiceActions wiring scan.
- Static QML/ShellServices/ShellCore scan.
- Static no broker SDK scan.
- Static no network scan.
- Static no credentials or endpoint scan.
- Static no real broker order id scan.
- Static no order placement scan.
- Static no write side effect scan.
- Static no reconciliation/cancellation/correction scan.
- Static no strategy/automatic trading scan.
- Runtime disabled dry-run probe.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/130 merged.
- [ ] docs/131 merged.
- [ ] Disabled-only runtime mode source compiles.
- [ ] Default source returns disabled.
- [ ] `DataServiceActions` uses the source and still selects disabled/null provider.
- [ ] No external runtime mode source.
- [ ] Sandbox runtime remains disabled.
- [ ] No SDK, network, credentials, endpoint, real order id, or order placement.
- [ ] No database, audit, or ledger write.
- [ ] TASK-166 tests pass.
- [ ] TASK-165 and TASK-164 regression tests pass.
- [ ] Full CTest passes.
- [ ] Transport 50 passes.
