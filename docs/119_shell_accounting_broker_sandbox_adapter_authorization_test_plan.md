# ShellAccounting Broker Sandbox Adapter Authorization Test Plan (TASK-160)

## Document Purpose

This test plan defines the static and documentation probes for the future ShellAccounting sandbox broker adapter authorization boundary. TASK-160 does not implement a sandbox adapter, paper adapter, real adapter, SDK integration, network call, or order placement.

## Test Matrix

### Sandbox Adapter Not Implemented Yet

Verify production source contains no sandbox, paper, or real broker adapter implementation and only keeps the TASK-159 disabled/null wiring.

### Disabled Remains Default

Verify docs and production wiring keep the disabled/null provider as the default mode.

### Sandbox / Paper / Real Mode Isolation

Verify docs require separate mode authorization, configuration, tests, audit policy, and rollback strategy for sandbox, paper, and real modes.

### Fake Adapter Remains Test-Only

Verify the fake adapter remains limited to tests and does not appear in production runtime wiring.

### No Broker SDK

Verify production source contains no broker SDK import, link, initialization, or runtime token.

### No Broker Network Call

Verify production source contains no broker endpoint, broker HTTP call, broker socket call, or external broker I/O token. CI must remain broker-network-free.

### No Real Credential / Secret / Token / Key / Password Value

Verify docs and production source contain no real credential values, usable examples, default environment values, private keys, or token-like samples. Policy words may appear only as forbidden-boundary descriptions.

### No Real Order Placement

Verify production source contains no real broker order placement call or real placement success marker.

### No Real Broker Order ID Storage

Verify production source contains no real broker order id persistence, accepted order id storage, or broker order id reconciliation state.

### No Reconciliation

Verify production source contains no broker reconciliation implementation, polling loop, or broker status reconciliation job.

### No Cancellation / Correction

Verify production source contains no broker cancellation or correction implementation. Existing schema words are not treated as broker flow implementations unless production broker flow code is added.

### No Strategy Execution

Verify production source contains no StrategyEngine execution from broker adapter paths.

### No Automatic Trading

Verify production source contains no automatic trading, unattended broker submission, or background broker placement path.

### No QML / Presenter Exposure

Verify production QML, ShellServices, and ShellCore do not expose broker adapter objects, broker SDK clients, endpoints, or credential handles.

### TASK-154 / TASK-159 Regression Preservation

Verify TASK-154 dry-run semantics remain dryRun=true, brokerDisabled=true, dryRunOnly=true, and brokerOrderId=null. Verify TASK-159 disabled provider remains wired.

### Rollback / Fail-Closed Policy

Verify docs require disabling sandbox mode to fall back to the TASK-159 disabled/null provider without fake broker success or order placement retry.

## Required Probes

- Static source scan over apps, libs, migrations, and tools.
- Explicit exclusion of docs, tests, README, build output, and generated logs from forbidden implementation scans.
- Documentation consistency checks for docs/118, docs/119, README, docs index, and docs/12.
- Forbidden broker SDK token scan.
- Forbidden broker network token scan.
- Forbidden credential value scan.
- No QML / Presenter direct adapter exposure scan.
- TASK-154 dry-run preservation scan.
- TASK-159 disabled provider preservation scan.
- Rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/118 merged.
- [ ] docs/119 merged.
- [ ] TASK-159 disabled/null wiring remains unchanged.
- [ ] TASK-160 gate tests pass.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo 50 repeat passes.
- [ ] No sandbox, paper, or real adapter implementation exists.
- [ ] No broker SDK, network call, credential value, real order id storage, real order placement, reconciliation, cancellation, correction, strategy execution, or automatic trading exists.
- [ ] User explicitly authorizes any future sandbox adapter implementation task.
