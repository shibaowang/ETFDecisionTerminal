# ShellAccounting Broker Sandbox Runtime Mode Source Scaffold Test Plan (TASK-168)

## Document Purpose

TASK-168 defines tests for the sandbox runtime mode source scaffold. This test plan verifies a direct-test scaffold only; it is not runtime enablement and it does not authorize sandbox adapter implementation, credentials, endpoints, broker SDK calls, network calls, real order placement, reconciliation, cancellation, correction, strategy execution, or automatic trading.

## Test Matrix

### Docs And Index

- docs/134 exists and documents TASK-168.
- docs/135 exists and contains this Test Matrix.
- README, docs/README, and docs/12 index TASK-168.
- docs/132 and docs/133 record that TASK-168 adds a scaffold while sandbox runtime remains disabled.

### Scaffold Shape

- Header declares `SandboxShellAccountingBrokerRuntimeModeSourceScaffold`.
- Source defines `SandboxShellAccountingBrokerRuntimeModeSourceScaffold::brokerOrderPortMode`.
- Direct instantiation returns `ShellAccountingBrokerOrderPortMode::Sandbox`.
- The scaffold has no constructor arguments and no runtime injection API.

### Runtime Remains Disabled

- `defaultShellAccountingBrokerRuntimeModeSource()` returns disabled.
- `DataServiceActions` does not instantiate or wire the sandbox scaffold.
- Runtime dry-run still reaches the disabled/null provider.
- `brokerPortMode` remains `disabled`.
- `brokerPortDisabled` remains `true`.
- `brokerPortDryRunOnly` remains `true`.
- Direct sandbox source use only reaches sandbox order port scaffold unavailable / not configured.

### No External Mode Source

- No payload mode source.
- No QML, startup, Presenter, or Controller mode source.
- No config source.
- No environment source.
- No command-line source.
- No file source.
- No database source.
- No secret-store source.
- No credentials, endpoint, account, or order data source.

### No Broker Side Effects

- No paper or real runtime source.
- No broker SDK.
- No network or endpoint.
- No real credentials, secrets, token, key, password, or usable credential example.
- No real broker order id storage.
- No order placement.
- No database, audit, or ledger write path.
- No reconciliation, cancellation, or correction.
- No strategy execution or automatic trading.
- No schema modification.

### Regression

- TASK-167 authorization gate remains valid.
- TASK-166 disabled scaffold remains valid.
- TASK-165, TASK-164, TASK-163, and TASK-162 regressions pass.
- Broker disabled / broker order / real broker gates pass.
- Full CTest passes.
- Transport echo passes 50 repeats.
- `git diff --check` passes.

## Required Probes

- Static source scan for scaffold declaration and implementation.
- Direct instantiation runtime probe.
- Default source disabled probe.
- Disabled provider dry-run probe.
- Static `DataServiceActions` scan proving no sandbox scaffold wiring.
- Static QML/startup/Presenter/Controller scan.
- Static payload/config/env/CLI/file/DB/secret-store scan.
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

- [ ] docs/134 merged.
- [ ] docs/135 merged.
- [ ] TASK-168 scaffold tests pass.
- [ ] Direct scaffold returns sandbox mode.
- [ ] Default runtime source remains disabled-only.
- [ ] `DataServiceActions` does not wire sandbox source.
- [ ] Sandbox runtime remains disabled.
- [ ] No payload/QML/config/env/CLI/file/DB/secret-store mode source.
- [ ] No broker SDK, network, endpoint, credentials, real broker order id, or order placement.
- [ ] No DB, audit, or ledger write path.
- [ ] No reconciliation, cancellation, correction, strategy execution, or automatic trading.
- [ ] Full CTest passes.
- [ ] Transport 50 passes.
- [ ] Any future sandbox runtime enablement opens a separate TASK.

## TASK-169 Follow-Up Gate

TASK-169 adds a gate-only test plan for a future sandbox runtime mode source selector. The TASK-168 scaffold remains direct-test-only, the default runtime source remains disabled-only, `DataServiceActions` remains unwired from any selector, and sandbox runtime remains disabled.

## TASK-170 Follow-Up Scaffold

TASK-170 adds direct-test selector scaffold tests. The selector scaffold may return the TASK-168 sandbox source scaffold only for an explicit sandbox enum in direct tests; default runtime source and `DataServiceActions` runtime behavior remain disabled-only.
