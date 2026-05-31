# ShellAccounting Broker Runtime Mode Source Authorization Test Plan (TASK-165)

## Document Purpose

TASK-165 defines tests for future broker runtime mode source authorization. This test plan does not implement mode source selection, sandbox runtime, credentials injection, endpoint configuration, broker SDK integration, or order placement.

## Test Matrix

### Gate And Docs

- docs/128 exists and documents TASK-165.
- docs/129 exists and documents this Test Matrix.
- README, docs/README, and docs/12 index TASK-165.
- TASK-126 and TASK-127 record that TASK-165 adds a gate only.

### Disabled-Default Baseline

- DataServiceActions still uses `defaultShellAccountingBrokerOrderPortMode()`.
- DataServiceActions still uses `shellAccountingBrokerOrderPortForMode(brokerPortMode)`.
- DataServiceActions does not select sandbox, paper, or real through a runtime string.
- Default provider remains disabled.

### No Mode Source Yet

- No request payload broker mode source.
- No QML or Presenter broker mode source.
- No config, environment, command-line, file, database, or secret-store broker mode source.
- No runtime source wiring in production startup.

### Sandbox Runtime Not Enabled

- Sandbox scaffold remains non-runtime.
- Sandbox runtime still requires a separate task.
- Unknown, empty, unsupported, paper, and real values must fail closed.

### No External Side Effects

- No broker SDK.
- No network call.
- No credentials, secrets, token, key, password, endpoint, host, or API path.
- No real broker order id.
- No order placement.
- No database, audit, or ledger write introduced by this gate.
- No reconciliation, cancellation, or correction.
- No strategy execution or automatic trading.

### Policy Keyword Isolation

- Docs and tests may contain policy keywords.
- Production source must not contain usable mode source, credentials, endpoint, or broker implementation values.

### Regression

- TASK-160 through TASK-164 docs and tests remain valid.
- Full CTest passes.
- Transport echo passes 50 repeats.
- `git diff --check` passes.

## Required Probes

- Static source scan for mode source tokens.
- Static QML/Shell scan for mode exposure.
- Static selector/DataServiceActions scan for disabled-default wiring.
- Static forbidden broker token scan.
- Static no-network and no-credentials scan.
- Static no real broker order id and no order placement scan.
- Static docs/tests policy keyword isolation probe.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/128 merged.
- [ ] docs/129 merged.
- [ ] TASK-164 disabled-default selector wiring remains unchanged.
- [ ] No runtime mode source implementation.
- [ ] Sandbox runtime remains disabled.
- [ ] No SDK, network, credentials, endpoint, real order id, or order placement.
- [ ] TASK-165 gate tests pass.
- [ ] TASK-160 through TASK-164 regression tests pass.
- [ ] Full CTest passes.
- [ ] Transport 50 passes.
- [ ] User explicitly authorizes any future runtime mode source implementation.

