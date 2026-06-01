# ShellAccounting Broker Sandbox Runtime Enablement State Scaffold (TASK-174)

## Document Purpose

TASK-174 adds a direct-test-only sandbox runtime enablement state scaffold in DataServiceApi. It is not sandbox runtime enablement and it is not connected to `DataServiceActions.cpp`.

The scaffold exists so later tasks can reason about sandbox runtime enablement state while the current production runtime remains disabled, unavailable, and fail-closed.

## Current Status

- TASK-173 added the sandbox runtime mode enablement authorization gate.
- TASK-172 disabled-default selector wiring remains unchanged.
- `DataServiceActions.cpp` is not modified by TASK-174.
- `ShellAccountingBrokerRuntimeModeSource.h` and `ShellAccountingBrokerRuntimeModeSource.cpp` are not modified by TASK-174.
- The default runtime source remains disabled-only.
- The TASK-170 selector scaffold remains direct-test-only.
- The sandbox runtime remains disabled.

## Scaffold API

TASK-174 adds:

- `ShellAccountingBrokerSandboxRuntimeEnablementState`
- `defaultShellAccountingBrokerSandboxRuntimeEnablementState()`

The default state is stable and directly testable:

- `enabled = false`
- `available = false`
- `failClosed = true`
- `runtimeMode = sandbox_runtime_disabled`
- `status = disabled`
- `errorCode = BROKER_SANDBOX_RUNTIME_DISABLED`

The scaffold has no constructor arguments, no setter, no registry, no global override, and no runtime injection API.

## Runtime Isolation

The scaffold is not read by `DataServiceActions.cpp`, not read by the runtime mode source selector, and not wired into broker dry-run behavior. It does not change `brokerPortMode`, `brokerPortDisabled`, `brokerPortDryRunOnly`, or `BROKER_ORDER_DISABLED` semantics.

## No External Source Policy

The scaffold does not read payload, QML, config, environment, command line, files, database, secret stores, credentials, endpoints, account data, or order data.

## No Side Effect Policy

TASK-174 adds no broker SDK, network call, endpoint, URL, host, port, credentials, secrets, token, key, password, real broker order id, real order placement, database write, audit write, ledger write, reconciliation, cancellation, correction, strategy execution, automatic trading, or schema change.

Real credentials, secrets, tokens, keys, passwords, endpoints, and usable examples must not enter code, repository files, documentation, logs, or test data. Policy keywords may appear in docs/tests, but real values and usable credential examples must not.

## Future Implementation Requirements

Future sandbox runtime enablement, sandbox adapter implementation, external mode source, credentials provider, endpoint provider, real broker order id storage, reconciliation, cancellation, correction, broker SDK, network, order placement, strategy execution, automatic trading, and schema changes still require separate TASK authorization.

TASK-175 adds a gate-only authorization boundary for future enablement state wiring. It does not wire this scaffold into `DataServiceActions.cpp`, does not modify the scaffold production code, and does not enable sandbox runtime.

## Rollback Policy

Rollback is to remove the enablement state scaffold and keep TASK-172 disabled-default selector wiring plus TASK-166 disabled-only runtime source behavior. Rollback must not enable sandbox runtime or affect TradeDraft, confirmation, broker dry-run, audit, ledger, UI, or broker order behavior.
