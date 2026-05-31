# ShellAccounting Broker Sandbox Runtime Mode Source Selector Authorization Test Plan (TASK-169)

## Document Purpose

TASK-169 defines tests for the future sandbox runtime mode source selector authorization boundary. This test plan is gate-only: it does not implement a selector, does not enable sandbox runtime, does not wire the TASK-168 scaffold into `DataServiceActions`, and does not add external mode sources.

## Test Matrix

### Docs And Index

- docs/136 exists and documents TASK-169.
- docs/137 exists and contains this Test Matrix.
- README, docs/README, and docs/12 index TASK-169.
- docs/134 and docs/135 mention TASK-169 as a future selector authorization gate.

### No Selector Implementation Yet

- Production source has no runtime mode source selector implementation.
- `ShellAccountingBrokerRuntimeModeSource.h/.cpp` are not modified by TASK-169.
- `DataServiceActions.cpp` is not wired to a sandbox source selector.
- `defaultShellAccountingBrokerRuntimeModeSource()` returns disabled.
- Sandbox runtime remains disabled.

### TASK-168 Scaffold Remains Direct-Test-Only

- `SandboxShellAccountingBrokerRuntimeModeSourceScaffold` remains present.
- Direct sandbox scaffold instantiation returns sandbox.
- Direct sandbox mode through the order port selector remains scaffold unavailable / not configured.
- The sandbox scaffold does not become the default runtime source.

### Future Selector Policy

- Unknown, blank, unsupported, paper, and real modes must fail closed to disabled or explicit unavailable/error.
- Future selector may only choose explicitly authorized sources.
- Future selector must not read payload, QML, config, environment, command line, files, database rows, or secret stores.
- Future selector must not carry credentials, endpoints, account data, order data, portfolio data, or broker payloads.

### No Broker Side Effects

- No broker SDK.
- No network call.
- No endpoint, URL, host, or port.
- No real credentials, secrets, token, key, password, or usable credential example.
- No real broker order id storage.
- No order placement.
- No database, audit, or ledger write path.
- No reconciliation, cancellation, or correction.
- No strategy execution or automatic trading.
- No schema modification.

### Regression

- TASK-168 scaffold tests pass.
- TASK-167 authorization gate tests pass.
- TASK-166 disabled scaffold tests pass.
- TASK-165, TASK-164, TASK-163, and TASK-162 regressions pass.
- Broker disabled / broker order / real broker gates pass.
- Full CTest passes.
- Transport echo passes 50 repeats.
- `git diff --check` passes.

## Required Probes

- Static production source scan for selector implementation tokens.
- Static runtime source scan proving no selector class or factory.
- Static `DataServiceActions` scan proving no sandbox selector wiring.
- Runtime default source disabled probe.
- Direct TASK-168 scaffold probe.
- Direct sandbox order port scaffold unavailable probe.
- Future fail-closed policy documentation probe.
- Static payload/QML/config/env/CLI/file/DB/secret-store scan.
- Static credential/endpoint/account/order data scan.
- Static broker SDK scan.
- Static network/endpoint scan.
- Static real broker order id scan.
- Static order placement scan.
- Static DB/audit/ledger write scan.
- Static reconciliation/cancellation/correction scan.
- Static strategy/automatic trading scan.
- Schema unchanged probe.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/136 merged.
- [ ] docs/137 merged.
- [ ] TASK-169 gate tests pass.
- [ ] No runtime mode source selector implementation.
- [ ] `ShellAccountingBrokerRuntimeModeSource.h/.cpp` unchanged by TASK-169.
- [ ] `DataServiceActions.cpp` does not wire a sandbox selector.
- [ ] Default runtime source remains disabled-only.
- [ ] TASK-168 sandbox scaffold remains direct-test-only.
- [ ] Sandbox runtime remains disabled.
- [ ] No payload/QML/config/env/CLI/file/DB/secret-store mode source.
- [ ] No broker SDK, network, endpoint, credentials, real broker order id, or order placement.
- [ ] No DB, audit, or ledger write path.
- [ ] No reconciliation, cancellation, correction, strategy execution, or automatic trading.
- [ ] Full CTest passes.
- [ ] Transport 50 passes.
- [ ] Any future selector implementation opens a separate TASK.
