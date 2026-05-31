# ShellAccounting Broker Sandbox Runtime Mode Source Authorization Test Plan (TASK-167)

## Document Purpose

TASK-167 defines the test plan for a future sandbox runtime mode source authorization boundary. This plan is gate-only: it does not implement sandbox runtime, sandbox mode source selection, sandbox adapter behavior, credentials, endpoints, SDK calls, network calls, order placement, reconciliation, cancellation, correction, strategy execution, or automatic trading.

TASK-168 updates this boundary by authorizing only a direct-test sandbox runtime mode source scaffold. The scaffold is not runtime wiring, and sandbox runtime remains disabled.

## Test Matrix

### Docs And Index

- docs/132 exists and documents TASK-167.
- docs/133 exists and contains this Test Matrix.
- README, docs/README, and docs/12 index TASK-167.
- docs/130 and docs/131 mention TASK-167 as a future authorization gate.

### Current Disabled-Only Runtime Source

- `ShellAccountingBrokerRuntimeModeSource` may contain the TASK-168 direct-test sandbox scaffold.
- `defaultShellAccountingBrokerRuntimeModeSource()` returns disabled.
- `DataServiceActions` continues to use disabled-only source behavior and does not read external broker mode.
- `DataServiceActions` does not instantiate or wire the TASK-168 sandbox scaffold.
- Sandbox runtime remains disabled.
- Sandbox direct selector access remains scaffold unavailable / not configured.
- Paper and real runtime source remain unimplemented.

### No External Mode Source

- No payload mode source.
- No QML, Presenter, Controller, or startup mode source.
- No config source.
- No environment source.
- No command-line source.
- No file source.
- No database source.
- No secret-store source.

### No Broker Side Effects

- No broker SDK.
- No network call.
- No endpoint, URL, host, or port.
- No real credentials, secrets, token, key, password, or usable credential example.
- No real broker order id storage.
- No order placement.
- No DB, audit, or ledger write path.
- No reconciliation, cancellation, or correction.
- No strategy execution or automatic trading.

### Policy Keyword Isolation

- Docs and tests may contain policy keywords.
- Production source must not contain usable sandbox runtime source implementation or usable broker credentials / endpoints.
- Static scans must distinguish production source from docs/tests policy text.

### Regression

- TASK-166 disabled scaffold tests pass.
- TASK-165 authorization gate tests pass.
- TASK-164, TASK-163, and TASK-162 selector regressions pass.
- Broker disabled / broker order / real broker gates pass.
- Full CTest passes.
- Transport echo passes 50 repeats.
- `git diff --check` passes.

## Required Probes

- Static production source scan allowing only the TASK-168 direct-test sandbox runtime mode source scaffold.
- Static `DataServiceActions` scan for forbidden payload/QML/config/env/CLI/file/DB/secret-store source tokens.
- Static runtime source scan proving disabled-only source behavior.
- Runtime default source disabled probe.
- Runtime direct sandbox scaffold unavailable probe.
- Static paper/real source scan.
- Static broker SDK scan.
- Static network / endpoint scan.
- Static credential / secret scan.
- Static real broker order id scan.
- Static order placement scan.
- Static DB / audit / ledger write scan.
- Static reconciliation / cancellation / correction scan.
- Static strategy / automatic trading scan.
- Documentation policy probe.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/132 merged.
- [ ] docs/133 merged.
- [ ] TASK-167 gate tests pass.
- [ ] TASK-166 disabled scaffold remains disabled-only.
- [ ] TASK-165 authorization gate remains valid.
- [ ] No sandbox runtime mode source wiring beyond the TASK-168 direct-test scaffold.
- [ ] No sandbox runtime enablement.
- [ ] No payload/QML/config/env/CLI/file/DB/secret-store mode source.
- [ ] No broker SDK, network, endpoint, credentials, real broker order id, or order placement.
- [ ] No DB, audit, or ledger write path.
- [ ] No reconciliation, cancellation, correction, strategy execution, or automatic trading.
- [ ] Full CTest passes.
- [ ] Transport 50 passes.
- [ ] User explicitly authorizes any future sandbox runtime source implementation in a separate TASK.
