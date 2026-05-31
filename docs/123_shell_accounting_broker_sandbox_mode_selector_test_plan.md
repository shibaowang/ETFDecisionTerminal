# ShellAccounting Broker Sandbox Mode Selector Test Plan (TASK-162)

## Document Purpose

This test plan defines the TASK-162 selector / factory boundary checks. TASK-162 is a disabled-default selector boundary, not runtime broker enablement. It does not authorize usable sandbox trading, broker SDK integration, network calls, credential reads, real broker order ids, order placement, reconciliation, cancellation, correction, strategy execution, automatic trading, production QML changes, startup wiring changes, or DataServiceActions runtime changes.

## Test Matrix

### Selector Compiles

Verify `ShellAccountingBrokerOrderPortModeSelector` compiles and exposes stable mode parsing and port selection functions.

### Default Disabled Mode

Verify the selector default mode is disabled and `defaultShellAccountingBrokerOrderPort()` remains the TASK-159 disabled/null provider.

### Empty / Unknown / Unsupported Fail Closed

Verify empty, unknown, unsupported, paper, or real mode fails closed to disabled provider semantics.

### Disabled Mode

Verify explicit disabled mode returns disabled/null provider semantics: disabled, dry-run-only, no success, and brokerMode disabled.

### Sandbox Scaffold Mode

Verify explicit sandbox mode only selects TASK-161 scaffold semantics: `success=false`, `disabled=true`, `dryRunOnly=true`, `brokerMode="sandbox"`, `BROKER_SANDBOX_NOT_CONFIGURED`, and no real broker order id.

### DataServiceActions Runtime Unchanged

Verify DataServiceActions still calls the default disabled provider and does not include or call the selector.

### No QML / Startup / Presenter Exposure

Verify production QML, startup, Presenter, ShellServices, and ShellCore do not expose selector objects, sandbox port objects, broker credentials, or broker endpoints.

### No Broker SDK

Verify selector source contains no broker SDK include, link, initialization, or vendor API token.

### No Network

Verify selector source contains no network client, socket, broker endpoint, broker URL, request sender, or external broker I/O token. CI no-network remains required.

### No Credentials

Verify selector source reads no credentials, secrets, tokens, keys, passwords, environment variables, settings, secret stores, or local credential files.

### No Endpoint / Secret Values

Verify TASK-162 source and docs contain no usable endpoint, token, key, password, private key, or credential sample.

### No Database / Audit / Ledger Write

Verify selector source contains no SQL write token, SQLite write call, audit write, ledger write, execution group write, or TradeDraft write.

### No Order Placement

Verify selector source contains no real order placement, order submit, broker request, or broker acceptance token.

### No Reconciliation / Cancellation / Correction

Verify selector source contains no broker reconciliation, broker status polling, cancellation, or correction implementation.

### No Strategy / Automatic Trading

Verify selector source contains no StrategyEngine execution, automatic trading, unattended order submission, or background broker placement path.

### Documentation

Verify docs/122 and docs/123 exist and describe disabled-default selector boundary, sandbox scaffold-only mode, unknown mode fail closed, CI no-network, credentials isolation, no real broker order id, and no order placement.

### TASK-160 / TASK-161 Regression

Verify docs/118 through docs/121 remain valid and continue to describe authorization gate and scaffold-only behavior.

## Required Probes

- Direct selector response probe.
- Static source scan over selector files.
- Static source scan over DataServiceActions.
- Static source scan over production QML, startup, ShellServices, and ShellCore.
- Forbidden broker SDK token scan.
- Forbidden broker network token scan.
- Forbidden credential read and credential value scan.
- Forbidden order placement token scan.
- Forbidden database / audit / ledger write token scan.
- Forbidden reconciliation / cancellation / correction token scan.
- Forbidden strategy / automatic trading token scan.
- Documentation probes for docs/122 and docs/123.
- Regression probes for docs/118 through docs/121.

## Go / No-Go Checklist

- [ ] TASK-161 is merged into main.
- [ ] docs/122 exists and documents disabled-default selector boundary.
- [ ] docs/123 exists and defines this test matrix.
- [ ] Selector tests pass.
- [ ] TASK-161 scaffold tests pass.
- [ ] TASK-160 sandbox authorization tests pass.
- [ ] TASK-159 disabled wiring tests pass.
- [ ] Broker order and real broker gates pass.
- [ ] Full CTest passes.
- [ ] transport_local_socket_echo 50 repeat passes.
- [ ] git diff --check passes.
- [ ] No production QML, startup, schema, DataServiceActions runtime, broker SDK, network, credential, real broker order id, order placement, strategy, or automatic trading changes are introduced.
