# ShellAccounting Broker Sandbox Runtime Selector Authorization Test Plan (TASK-163)

## Document Purpose

This test plan defines the TASK-163 gate-only checks for future broker sandbox runtime selector wiring. The plan does not implement runtime wiring, does not enable sandbox runtime, does not add broker SDK or network access, and does not change production QML / startup / Presenter behavior.

## Test Matrix

### Gate Documentation

Verify docs/124 and docs/125 exist, mention TASK-163, and state that runtime selector wiring requires a separate authorization task.

### Index Coverage

Verify README, docs/README, and docs/12 include TASK-163 and link the new gate and test plan.

### DataServiceActions Runtime Not Wired

Verify `DataServiceActions.cpp` does not include or call `ShellAccountingBrokerOrderPortModeSelector`, `shellAccountingBrokerOrderPortForMode`, or runtime sandbox selector wiring.

### Default Provider Disabled

Verify the default broker provider still returns disabled/null provider semantics and that TASK-163 does not replace the default provider.

### Selector Not Exposed to Production Startup / QML / Presenter

Verify production startup, QML, ShellServices, ShellCore, Presenter, and Controller paths do not expose the broker selector or broker mode configuration.

### No Broker SDK

Verify TASK-163 does not add broker SDK includes, vendor APIs, SDK initialization, sandbox broker adapter implementation, paper broker adapter implementation, or real broker adapter implementation.

### No Network

Verify TASK-163 does not add network clients, sockets, endpoint values, host values, API path values, broker URLs, or live broker I/O. CI no-network remains mandatory.

### No Credentials

Verify TASK-163 does not read credentials, environment variables, config files, settings, secret stores, token stores, keys, passwords, or usable credential examples.

### No Real Order / Write Side Effects

Verify TASK-163 does not add real broker order ids, order placement, database writes, audit writes, ledger writes, reconciliation, cancellation, correction, strategy execution, or automatic trading.

### Fail-Closed Policy

Verify docs require disabled default behavior and require empty / unknown / unsupported / paper / real modes to fail closed until separately authorized.

### Runtime Mode Source Policy

Verify docs require a separate task before adding runtime mode source wiring or credentials injection.

### Regression

Verify TASK-160, TASK-161, and TASK-162 docs/tests remain valid and that policy keywords in docs/tests are not treated as production implementation violations.

## Required Probes

- Static docs probe for docs/124 and docs/125.
- Static index probe for README, docs/README, and docs/12.
- Static source scan over `DataServiceActions.cpp`.
- Static source scan over production QML/startup/ShellServices/ShellCore.
- Direct default provider disabled response probe.
- Forbidden broker SDK token scan.
- Forbidden broker network token scan.
- Forbidden credential read and credential value scan.
- Forbidden endpoint / secret value scan.
- Forbidden real broker order id scan.
- Forbidden order placement scan.
- Forbidden database / audit / ledger write scan.
- Forbidden reconciliation / cancellation / correction scan.
- Forbidden strategy / automatic trading scan.
- Documentation probes for fail-closed, runtime mode source, credentials injection, no-network, and rollback policies.
- Regression probes for docs/118 through docs/123.

## Go / No-Go Checklist

- [ ] TASK-162 is merged into main.
- [ ] docs/124 merged.
- [ ] docs/125 merged.
- [ ] TASK-163 tests pass.
- [ ] TASK-162 selector tests pass.
- [ ] TASK-161 scaffold tests pass.
- [ ] TASK-160 sandbox authorization tests pass.
- [ ] TASK-159 disabled wiring tests pass.
- [ ] Broker order and real broker gates pass.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo 50 repeat passes.
- [ ] git diff --check passes.
- [ ] User explicitly authorizes future runtime selector wiring before implementation.

## TASK-164 Baseline Update

After TASK-164, this gate allows disabled-default selector wiring in DataServiceActions. The updated negative probe is no sandbox runtime mode source / no external mode source, not no selector reference at all. DataServiceActions may use `defaultShellAccountingBrokerOrderPortMode()` and `shellAccountingBrokerOrderPortForMode(mode)` only.
