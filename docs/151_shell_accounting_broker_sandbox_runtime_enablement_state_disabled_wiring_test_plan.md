# TASK-176 ShellAccounting Broker Sandbox Runtime Enablement State Disabled Wiring Test Plan

## Document Purpose

This test plan defines TASK-176 verification for the disabled/fail-closed
enablement-state wiring in `DataServiceActions.cpp`. It verifies that the
default sandbox runtime enablement state is read without enabling sandbox
runtime or changing broker dry-run behavior.

## Test Matrix

### Documentation and Indexes

- docs/150 exists and documents TASK-176.
- docs/151 exists and contains this test matrix.
- README, docs/README, and docs/12 register TASK-176.
- docs/148 and docs/149 evolve TASK-175 from future authorization to completed
  disabled/fail-closed wiring.

### DataServiceActions Wiring

- `DataServiceActions.cpp` includes
  `ShellAccountingBrokerSandboxRuntimeEnablement.h`.
- `DataServiceActions.cpp` reads only
  `defaultShellAccountingBrokerSandboxRuntimeEnablementState()`.
- The read checks the disabled, unavailable, fail-closed fields.
- The enablement state is not converted into Sandbox, Paper, or Real mode.
- The enablement state is not written into the response.
- No request or response fields are added.

### Disabled Runtime Behavior

- Default enablement state remains `enabled=false`.
- Default enablement state remains `available=false`.
- Default enablement state remains `failClosed=true`.
- Broker dry-run port response remains disabled/null provider behavior.
- `brokerPortMode` remains `disabled`.
- `brokerPortDisabled` remains `true`.
- `brokerPortDryRunOnly` remains `true`.
- `BROKER_ORDER_DISABLED` remains the disabled port error code.

### Source Isolation

- Selector runtime remains driven only by the disabled default source.
- Sandbox runtime remains disabled.
- TASK-170 selector scaffold remains direct-test-only.
- Direct sandbox selector remains scaffold unavailable or not configured.
- Sandbox broker adapter is not implemented.
- Paper and Real runtime sources are not implemented.
- `ShellAccountingBrokerSandboxRuntimeEnablement.h/.cpp` are not changed by
  TASK-176.
- `ShellAccountingBrokerRuntimeModeSource.h/.cpp` are not changed by TASK-176.

### Forbidden Probes

- No payload, QML, config, environment, command-line, file, DB, or secret store
  mode source.
- No credentials, endpoint, account, or order data runtime source.
- No broker SDK.
- No network or endpoint.
- No real credential, secret, token, key, password value, or usable example.
- No real broker order id storage.
- No order placement.
- No DB, audit, or ledger write path.
- No reconciliation, cancellation, or correction.
- No strategy execution or automatic trading.
- No schema modification.

### Regression

- TASK-175 wiring authorization gate has evolved to allow disabled/fail-closed
  enablement state reads only.
- TASK-174 enablement state scaffold remains disabled, unavailable, and
  fail-closed.
- TASK-172 disabled-default selector wiring remains valid.
- TASK-166 disabled-only runtime source remains valid.
- Broker disabled, broker order, and real broker gates remain valid.

## Required Probes

- Static source scan.
- DataServiceActions include and default-state read scan.
- Response field scan.
- Forbidden token scan.
- Runtime source scan.
- Selector scan.
- Direct disabled/null provider response probe.
- Direct sandbox selector unavailable probe.
- Schema unchanged probe.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/150 merged.
- [ ] docs/151 merged.
- [ ] `DataServiceActions.cpp` reads only the default disabled enablement state.
- [ ] Broker dry-run response is unchanged.
- [ ] TASK-176 tests pass.
- [ ] TASK-175 through TASK-162 regressions pass.
- [ ] Broker disabled, broker order, and real broker gates pass.
- [ ] Full CTest passes.
- [ ] transport local socket echo 50 repeat passes.
- [ ] `git diff --check` passes.
- [ ] `git status --short` is clean.
