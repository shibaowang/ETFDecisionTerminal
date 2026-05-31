# ShellAccounting Broker Sandbox Adapter Scaffold Test Plan (TASK-161)

## Document Purpose

This test plan defines the TASK-161 scaffold checks for the ShellAccounting sandbox broker adapter class. The plan is scaffold-only and does not authorize usable sandbox trading, paper trading, real broker placement, credential wiring, broker SDK integration, network calls, reconciliation, cancellation, correction, strategy execution, or automatic trading.

## Test Matrix

### Scaffold Compile / Instantiate

Verify `ShellAccountingBrokerSandboxOrderPortScaffold` compiles, instantiates through the existing broker order port interface, and returns a deterministic unavailable response.

### Sandbox Unavailable Response

Verify every scaffold response is `success=false`, `disabled=true`, `dryRunOnly=true`, `brokerMode=sandbox`, and contains a sanitized not-configured error.

### No Real Broker Order Id

Verify the scaffold returns no real broker order id or correlation id that could be interpreted as a broker acceptance.

### No Order Placement

Verify scaffold source contains no real order placement, order submit, broker request, or broker acceptance token.

### No SQLite / Database Write

Verify scaffold source contains no SQL write token, SQLite write call, audit write, ledger write, execution group write, or TradeDraft write.

### No Credentials

Verify scaffold source reads no credentials, secrets, tokens, keys, passwords, environment variables, settings, secret stores, or local credential files. Documentation and tests may mention these policy words only as forbidden behavior.

### No Network

Verify scaffold source contains no network client, socket, broker endpoint, broker URL, request sender, or external broker I/O token. CI must remain broker-network-free.

### No Broker SDK Dependency

Verify scaffold source imports, links, initializes, and calls no broker SDK or vendor API.

### No Endpoint / Secret Values

Verify TASK-161 source and docs contain no usable endpoint, token, key, password, private key, or credential sample.

### Disabled Default Unchanged

Verify `defaultShellAccountingBrokerOrderPort()` still returns the TASK-159 disabled/null provider and not the sandbox scaffold.

### TASK-154 / TASK-159 Regression

Verify the disabled provider and dry-run boundary still return disabled / dry-run-only semantics.

### TASK-160 Gate Regression

Verify docs/118 and docs/119 still define sandbox authorization, disabled default, credential isolation, CI no-network, DataService-only access, and no real order placement.

### Fake Adapter Test-Only

Verify the scaffold does not reuse the fake adapter or expose test-only fake behavior as a production sandbox implementation.

### No QML / Presenter / Startup Exposure

Verify production QML, ShellServices, ShellCore, and startup do not instantiate or expose the sandbox scaffold, broker endpoint, broker credential, or broker SDK.

### Static Scan Scope

Verify implementation scans are scoped to production source while docs and tests may contain forbidden-policy words as explanatory text.

## Required Probes

- Direct scaffold response probe.
- Static source scan over `libs/DataServiceApi` scaffold files.
- Static source scan over production QML, ShellServices, ShellCore, and startup.
- Forbidden order placement token scan.
- Forbidden network token scan.
- Forbidden broker SDK token scan.
- Forbidden credential value and credential read scan.
- Forbidden database / audit / ledger write token scan.
- Documentation probe for docs/120 and docs/121.
- Regression probe for docs/118 and docs/119.

## Go / No-Go Checklist

- [ ] TASK-160 is merged into main.
- [ ] docs/120 exists and documents scaffold-only behavior.
- [ ] docs/121 exists and defines this test matrix.
- [ ] TASK-161 scaffold tests pass.
- [ ] TASK-160 sandbox authorization tests pass.
- [ ] TASK-159 disabled wiring tests pass.
- [ ] TASK-154 dry-run broker order tests pass.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo 50 repeat passes.
- [ ] git diff --check passes.
- [ ] No production QML or startup changes.
- [ ] No broker SDK, network, credential, real order placement, or database write is introduced.
