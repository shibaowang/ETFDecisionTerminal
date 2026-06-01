# ShellAccounting Broker Sandbox Runtime Enablement State Wiring Authorization Gate (TASK-175)

## Document Purpose

TASK-175 is an authorization gate-only task for a future step that may wire the TASK-174 sandbox runtime enablement state scaffold into `DataServiceActions.cpp`.

This document does not implement runtime wiring, does not modify `DataServiceActions.cpp`, does not modify `ShellAccountingBrokerSandboxRuntimeEnablement.h/.cpp`, does not enable sandbox runtime, and does not allow the enablement state to affect production broker dry-run behavior.

## Current Status

- TASK-174 added `ShellAccountingBrokerSandboxRuntimeEnablementState` and `defaultShellAccountingBrokerSandboxRuntimeEnablementState()` as a direct-test-only scaffold.
- The default enablement state remains disabled, unavailable, and fail-closed.
- `DataServiceActions.cpp` does not include, call, or read the enablement state scaffold.
- `ShellAccountingBrokerRuntimeModeSource.h/.cpp` remain unchanged by this gate.
- TASK-172 disabled-default selector wiring remains the production path.
- TASK-166 disabled-only runtime source remains the baseline.
- Sandbox runtime remains disabled.

## Future Wiring Boundary

Future wiring must be a separate TASK with explicit authorization. The first authorized wiring step may only read the default disabled/fail-closed enablement state inside `DataServiceActions.cpp` and must preserve existing production behavior.

Future wiring must not:

- Change the disabled/null provider response.
- Change `brokerPortDisabled`, `brokerPortDryRunOnly`, or disabled broker error semantics.
- Enable sandbox runtime.
- Read broker mode from payload, QML, config, environment, command line, files, database, or secret stores.
- Carry credentials, endpoints, account payload, order payload, or usable examples.
- Trigger broker SDK, network, endpoint access, order placement, DB write, audit write, or ledger write.
- Add reconciliation, cancellation, correction, strategy execution, or automatic trading.

Unknown, blank, unsupported, paper, and real modes must continue to fail closed.

## DataServiceActions Policy

`DataServiceActions.cpp` remains outside this TASK. It must not be modified by TASK-175 and must not call `defaultShellAccountingBrokerSandboxRuntimeEnablementState()`.

The current broker dry-run path may continue to read only `defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()`, pass that value to `shellAccountingBrokerRuntimeModeSourceForMode(defaultBrokerPortMode)`, and then call `shellAccountingBrokerOrderPortForMode(brokerPortMode)`.

Passing Sandbox, Paper, Real, request payload values, QML values, config values, environment values, command-line values, file values, database values, or secret-store values into the runtime selector remains unauthorized.

## Enablement State Policy

The TASK-174 scaffold remains direct-test-only for this gate. Its default state must remain:

- `enabled=false`
- `available=false`
- `failClosed=true`
- `runtimeMode=sandbox_runtime_disabled`
- `status=disabled`
- `errorCode=BROKER_SANDBOX_RUNTIME_DISABLED`

The scaffold must not gain setter, registry, override, injection, payload, config, environment, file, database, secret-store, credential, endpoint, account, or order-data APIs in TASK-175.

## Future Independent Tasks

The following remain independent future TASKs and are not authorized here:

- Sandbox runtime enablement.
- External mode source.
- Credentials provider.
- Endpoint provider.
- Sandbox broker adapter implementation.
- Broker SDK or network access.
- Real broker order id storage.
- Reconciliation.
- Cancellation or correction.
- Strategy execution.
- Automatic trading.
- Schema changes.

## Static Scan Policy

Static scans must distinguish production source from docs/tests:

- Production source is scanned for forbidden implementation and runtime wiring tokens.
- Docs/tests may contain policy keywords only to describe forbidden behavior.
- Docs/tests policy keywords must not be treated as production implementation.

Real credentials, secrets, tokens, keys, passwords, endpoints, URLs, host names, port values, and usable examples must not appear in code, repository files, documentation, logs, or test data.

## Error And Fail-Closed Policy

Future wiring must preserve fail-closed behavior for:

- Unknown mode.
- Blank mode.
- Unsupported mode.
- Paper mode.
- Real mode.
- Missing enablement state.
- Disabled enablement state.
- Unavailable enablement state.

No future wiring may return silent success.

## Rollback Policy

Rollback must return to TASK-172 disabled-default selector wiring and TASK-166 disabled-only runtime source behavior. Removing future wiring must not alter TradeDraft, confirmation, broker dry-run, audit, ledger, UI, broker order, or sandbox selector behavior.

## Required Gates For Future Wiring

Future wiring requires:

- TASK-166 through TASK-175 gates passing.
- Full CTest passing.
- `transport_local_socket_echo` 50-repeat passing.
- Clean git status.
- Explicit user authorization for a new implementation TASK.
