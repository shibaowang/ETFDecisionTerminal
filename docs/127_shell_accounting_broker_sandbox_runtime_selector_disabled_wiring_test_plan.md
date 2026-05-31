# ShellAccounting Broker Sandbox Runtime Selector Disabled Wiring Test Plan (TASK-164)

## Document Purpose

This test plan verifies TASK-164 disabled-default runtime selector wiring. It confirms that DataServiceActions uses the selector with only `defaultShellAccountingBrokerOrderPortMode()`, while runtime behavior remains disabled and sandbox runtime remains unavailable.

## Test Matrix

### Documentation

Verify docs/126 and docs/127 exist, mention TASK-164, and document disabled-default runtime selector wiring.

### DataServiceActions Selector Use

Verify `DataServiceActions.cpp` includes the selector header, calls `defaultShellAccountingBrokerOrderPortMode()`, and calls `shellAccountingBrokerOrderPortForMode(mode)`.

### Mode Source Restriction

Verify `DataServiceActions.cpp` does not use `"sandbox"`, `"paper"`, or `"real"` runtime mode strings, and does not read broker mode from payload fields, QML, config, environment variables, command-line arguments, files, databases, or secret stores.

### Runtime Disabled Semantics

Verify `accounting.broker_order.dry_run` still returns disabled/null provider semantics: `brokerDisabled=true`, `dryRunOnly=true`, `brokerPortMode="disabled"`, no broker order id, no broker SDK call, no order submitted, and no real order placement.

### Existing Response Contract

Verify `BROKER_ORDER_DISABLED`, disabled broker mode, dry-run-only response fields, and existing broker order dry-run response semantics remain stable.

### Sandbox Not Runtime

Verify direct selector sandbox tests still return scaffold unavailable / success false, while runtime dry-run does not select sandbox.

### Gate Regression

Verify TASK-163 authorization gate is updated to allow disabled-default selector wiring and still rejects sandbox mode source / external mode source.

### No Side Effects

Verify no production QML / startup / Presenter exposure, no broker SDK, no network, no credentials, no endpoint values, no real broker order id, no order placement, no database / audit / ledger write, no reconciliation / cancellation / correction, and no strategy execution / automatic trading.

## Required Probes

- Static source scan of `DataServiceActions.cpp`.
- Runtime broker dry-run response probe.
- Direct selector disabled / sandbox / unknown probes.
- Static production QML/startup/ShellServices/ShellCore scan.
- Forbidden broker SDK token scan.
- Forbidden network token scan.
- Forbidden credential and endpoint value scan.
- Forbidden order placement token scan.
- Forbidden database / audit / ledger write token scan.
- Forbidden reconciliation / cancellation / correction token scan.
- Forbidden strategy / automatic trading token scan.
- Documentation probes for docs/124 through docs/127.

## Go / No-Go Checklist

- [ ] TASK-163 is merged into main.
- [ ] docs/126 merged.
- [ ] docs/127 merged.
- [ ] DataServiceActions uses selector through default disabled mode only.
- [ ] Runtime dry-run remains disabled.
- [ ] TASK-163 authorization gate passes after update.
- [ ] TASK-162 selector tests pass.
- [ ] TASK-161 scaffold tests pass.
- [ ] TASK-160 sandbox authorization tests pass.
- [ ] Broker disabled / broker order / real broker gates pass.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo 50 repeat passes.
- [ ] git diff --check passes.

## TASK-165 Follow-Up Gate

TASK-165 adds a separate broker runtime mode source authorization gate. The TASK-164 tests remain the disabled-default selector wiring baseline and must continue to prove that no payload, QML, config, environment, command-line, file, database, or secret-store mode source is implemented.
