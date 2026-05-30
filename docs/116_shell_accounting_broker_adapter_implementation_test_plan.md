# ShellAccounting Broker Adapter Implementation Test Plan (TASK-158)

## Document Purpose

This test plan defines the static and behavioral probes required before any future ShellAccounting broker adapter implementation can begin. TASK-158 does not implement a broker adapter, import a broker SDK, call broker network, place real orders, add credentials, modify production QML/startup, change TASK-148 / TASK-150 / TASK-152 / TASK-154 / TASK-157 behavior, or modify schema.

## Test Matrix

### No Adapter Implementation Yet

Verify production code still contains only the TASK-157 port scaffold and disabled/null adapter, with no concrete sandbox, paper, or real broker adapter implementation.

### Disabled Default

Verify docs and tests require every future adapter mode to default disabled, fail closed on missing mode/configuration/credentials, and preserve dry-run fallback.

### Fake Test-Only

Verify fake adapters remain test-only and cannot be linked into production runtime, QML, Presenter, ShellServices, or ShellCore as a broker implementation.

### Sandbox / Paper / Real Isolation

Verify sandbox, paper, and real modes are documented as distinct modes with no implicit promotion between them. Mode mismatch must fail closed.

### No Broker SDK

Verify no broker SDK imports, links, initialization, or SDK client tokens are present in production code for TASK-158.

### No Network Call

Verify no broker endpoint, submit-order HTTP helper, sandbox broker call, paper broker call, or real broker network call appears in production paths or CI-executed tests.

### No Credentials

Verify no broker API key, password, token, secret, private key, or credential file is committed to repo docs, tests, QML, logs, or production code.

### DataService-Only Boundary

Verify future adapter work is documented as DataService-only and cannot be accessed through QML, Presenter, ShellCore, or generic DataServiceClient escape hatches.

### No QML Direct Broker

Verify production QML has no broker adapter import, no broker SDK reference, no broker order direct call, and no credential handling.

### No Presenter Direct Broker

Verify ShellServices and ShellCore do not instantiate broker adapters, hold broker clients, or submit orders outside the DataService boundary.

### Order ID Policy

Verify no real broker order id is produced or stored by TASK-158. Future order id storage and reconciliation require separate authorization.

### Reconciliation Handoff

Verify broker status polling, fill reconciliation, partial fills, order id persistence, and external broker state reconciliation are documented as later tasks.

### Cancellation Handoff

Verify cancellation, replacement, correction, and reversal flows are documented as separate tasks and not implemented by TASK-158.

### Error Mapping

Verify docs require disabled, credentials missing, mode mismatch, broker unavailable, broker rejected, timeout, duplicate order, unknown broker state, audit failure, reconciliation required, and no silent success.

### Rollback / Kill Switch

Verify docs require adapter kill switch behavior, dry-run fallback, no fabricated broker success, no blind retries, and unaffected TradeDraft / ledger / audit features.

### No Strategy Execution

Verify adapter work is not allowed to execute StrategyEngine or generate strategy-driven orders.

### No Automatic Trading

Verify adapter work is not allowed to enable background submission, automatic trading, or unattended order placement.

### Regression

Verify TASK-157 broker adapter interface tests, TASK-154 broker order dry-run tests, TASK-155/TASK-156 real broker gates, full CTest, transport 50 repeat, git diff --check, and final clean status all pass.

## Required Probes

- static source scan
- SDK token scan
- network token scan
- credential token scan
- QML broker scan
- Presenter broker scan
- real order placement scan
- fake adapter production-link scan
- mode isolation documentation probe
- order id documentation probe
- reconciliation handoff documentation probe
- cancellation handoff documentation probe
- error mapping documentation probe
- rollback documentation probe

## Go / No-Go Checklist

- [ ] docs/115 merged.
- [ ] docs/116 merged.
- [ ] TASK-157 interface scaffold still passes.
- [ ] TASK-158 broker adapter implementation gate passes.
- [ ] No adapter implementation exists in production code.
- [ ] No broker SDK is imported or linked.
- [ ] No broker network call is reachable in CI.
- [ ] No credentials or secrets are committed.
- [ ] QML / Presenter / Shell have no direct broker access.
- [ ] No real broker order id is produced or stored.
- [ ] No strategy execution or automatic trading is introduced.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo 50 repeat passes.
- [ ] User explicitly authorizes any future adapter implementation task.
