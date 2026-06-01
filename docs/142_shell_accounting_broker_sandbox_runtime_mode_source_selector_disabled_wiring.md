# ShellAccounting Broker Sandbox Runtime Mode Source Selector Disabled Wiring (TASK-172)

## Document Purpose

TASK-172 wires the TASK-170 runtime mode source selector scaffold into the DataService broker dry-run path using disabled-default input only. It is not sandbox runtime enablement and does not add any external runtime mode source.

## Current Status

- TASK-171 authorized only disabled-default selector wiring as the next implementation step.
- TASK-170 selector scaffold remains available through `shellAccountingBrokerRuntimeModeSourceForMode`.
- `defaultShellAccountingBrokerRuntimeModeSource()` remains disabled-only.
- `DataServiceActions.cpp` may now call the selector scaffold, but only with the mode returned by the disabled default source.
- Sandbox runtime remains disabled.
- Paper and real runtime mode sources remain unimplemented.

## Disabled-Default Wiring

`DataServiceActions.cpp` obtains broker mode in the broker dry-run path as follows:

1. Read `defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()`.
2. Pass that mode to `shellAccountingBrokerRuntimeModeSourceForMode(defaultBrokerPortMode)`.
3. Read `brokerOrderPortMode()` from the selected source.
4. Pass the selected mode to `shellAccountingBrokerOrderPortForMode(brokerPortMode)`.

Because the default source remains disabled-only, runtime selection still resolves to the disabled/null broker order port. Runtime `brokerPortMode` remains `disabled`, `brokerPortDisabled` remains true, and `brokerPortDryRunOnly` remains true.

## Runtime Source Policy

The selector runtime path is driven only by the default disabled source. It must not read payload, QML, config, environment, command line, files, database, or secret stores as mode sources.

`DataServiceActions.cpp` must not pass Sandbox, Paper, Real, string literals, request payload fields, or externally supplied values into `shellAccountingBrokerRuntimeModeSourceForMode`.

## Sandbox Isolation Policy

The TASK-168 sandbox source scaffold remains direct-test-only. The direct sandbox selector path may still be tested and must continue returning scaffold unavailable / not configured behavior. TASK-172 does not enable sandbox runtime.

Sandbox runtime enablement, external mode source, credentials provider, endpoint provider, real broker order id storage, reconciliation, cancellation, correction, broker SDK, network, order placement, strategy execution, and automatic trading still require separate TASK authorization.

## No Side Effect Policy

TASK-172 adds no broker SDK, network call, endpoint, URL, host, port, credentials, secrets, token, key, password, real broker order id, real order placement, database write, audit write, ledger write, reconciliation, cancellation, correction, strategy execution, automatic trading, or schema change.

Real credentials, secrets, tokens, keys, passwords, endpoints, and usable examples must not enter code, repository files, documentation, logs, or test data. Policy keywords may appear in docs/tests, but real values and usable credential examples must not.

## Rollback Policy

Rollback is to remove the disabled-default selector call and return directly to the TASK-166 disabled-only source plus TASK-159 disabled/null provider behavior. Disabling this wiring must not enable sandbox runtime or change TradeDraft, confirmation, broker dry-run, audit, ledger, or UI behavior.

## Future Implementation Requirements

Any future sandbox runtime enablement, external mode source, credentials provider, endpoint provider, paper or real runtime source, real broker order id storage, reconciliation, cancellation, correction, broker SDK, network, order placement, strategy execution, automatic trading, or schema change must open a separate TASK and reference docs/142 and docs/143.

TASK-173 adds the sandbox runtime mode enablement authorization gate. It does
not change TASK-172 behavior: selector input remains the default disabled
source, sandbox runtime remains disabled, and future enablement still requires a
separate implementation TASK.
