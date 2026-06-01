# TASK-176 ShellAccounting Broker Sandbox Runtime Enablement State Disabled Wiring

## Document Purpose

TASK-176 wires the TASK-174 sandbox runtime enablement state into the
`DataServiceActions.cpp` broker dry-run path in a disabled-only, fail-closed
form. This implementation reads only
`defaultShellAccountingBrokerSandboxRuntimeEnablementState()` and preserves the
existing disabled/null broker port response.

This document does not authorize sandbox runtime enablement, external runtime
mode sources, credentials, endpoints, broker SDK access, network calls, real
order placement, reconciliation, cancellation, correction, strategy execution,
automatic trading, or schema changes.

TASK-176 does not enable sandbox runtime.

## Current State

TASK-175 authorized a future wiring task for the TASK-174 enablement state.
TASK-176 is that limited wiring task. The default enablement state remains:

- `enabled=false`
- `available=false`
- `failClosed=true`
- `runtimeMode=sandbox_runtime_disabled`
- `status=disabled`
- `errorCode=BROKER_SANDBOX_RUNTIME_DISABLED`

`DataServiceActions.cpp` reads this default disabled state only as a
fail-closed guard. It does not convert the state into Sandbox, Paper, or Real
runtime mode and does not expose the state in the broker dry-run response.

## DataServiceActions Boundary

The broker dry-run path may include
`ShellAccountingBrokerSandboxRuntimeEnablement.h` and may read:

`defaultShellAccountingBrokerSandboxRuntimeEnablementState()`

The read is limited to preserving the disabled, unavailable, fail-closed
contract. The broker port mode still comes from:

`defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()`

and the runtime source selection still uses:

`shellAccountingBrokerRuntimeModeSourceForMode(defaultBrokerPortMode)`

The final broker port still comes from:

`shellAccountingBrokerOrderPortForMode(brokerPortMode)`

## Response Compatibility

TASK-176 does not add request fields or response fields. The broker dry-run
response keeps the existing disabled/null provider semantics:

- `brokerPortMode` remains `disabled`
- `brokerPortDisabled` remains `true`
- `brokerPortDryRunOnly` remains `true`
- `brokerPortErrorCode` remains `BROKER_ORDER_DISABLED`
- `brokerOrderId` remains `null`
- `realOrderPlacement` remains `false`
- `brokerSdkCalled` remains `false`
- `brokerOrderSubmitted` remains `false`

## Sandbox Runtime Policy

Sandbox runtime remains disabled. The TASK-170 selector scaffold remains
direct-test-only. Direct sandbox selector probes may still return scaffold
unavailable or not configured results, but production runtime remains driven by
the disabled default source only.

TASK-176 does not implement Paper or Real runtime sources and does not implement
a sandbox broker adapter.

## No External Mode Source Policy

TASK-176 does not read broker mode from payload, QML, config, environment,
command line, files, database, or secret stores. It does not add an external
mode source and does not read credentials, endpoint, account, or order data to
choose broker runtime mode.

## No Broker / No Network Policy

TASK-176 adds no broker SDK, network client, endpoint, URL, host, port, real
credentials, secret values, broker order id storage, or order placement path.
Real credentials, secrets, tokens, keys, passwords, and usable examples must not
appear in code, docs, logs, or test data.

## No Write / No Trading Policy

TASK-176 adds no DB, audit, or ledger write path. It adds no reconciliation,
cancellation, correction, strategy execution, automatic trading, real order
placement, or schema change.

## Rollback / Disable Policy

Rollback is to remove the disabled enablement-state read and keep the TASK-172
disabled-default selector wiring and TASK-166 disabled-only runtime source. The
fallback remains the existing disabled/null broker dry-run behavior.

## Follow-Up Scope

Sandbox runtime enablement, external mode source, credentials provider,
endpoint provider, sandbox adapter implementation, real broker order id storage,
reconciliation, cancellation, correction, strategy execution, automatic
trading, and real broker order placement require separate TASK authorization.
