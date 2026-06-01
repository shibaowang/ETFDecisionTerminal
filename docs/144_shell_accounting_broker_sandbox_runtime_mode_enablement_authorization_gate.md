# ShellAccounting Broker Sandbox Runtime Mode Enablement Authorization Gate (TASK-173)

## Document Purpose

TASK-173 is a gate-only authorization boundary for a future task that may enable ShellAccounting broker sandbox runtime mode. It does not enable sandbox runtime, does not modify `DataServiceActions.cpp`, and does not modify `ShellAccountingBrokerRuntimeModeSource.h` or `ShellAccountingBrokerRuntimeModeSource.cpp`.

This document exists to keep TASK-172 disabled-default selector wiring isolated from future sandbox runtime enablement work.

## Current Status

- TASK-172 wires the runtime mode source selector only through `defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()`.
- The default runtime source remains disabled-only.
- The selected runtime source remains disabled when driven by the default source.
- `brokerPortMode` remains `disabled`.
- `brokerPortDisabled` remains true.
- `brokerPortDryRunOnly` remains true.
- The TASK-170 sandbox selector scaffold remains direct-test-only and unavailable / not configured for runtime use.
- Sandbox runtime, paper runtime, real runtime, external mode sources, credentials providers, endpoint providers, sandbox adapters, broker SDK, network calls, real order placement, real broker order id storage, reconciliation, cancellation, correction, strategy execution, automatic trading, and schema changes remain unimplemented.

## Future Enablement Boundary

Future sandbox runtime enablement must be a separate TASK. It is not authorized by TASK-173.

Future enablement is not the same as sandbox adapter implementation, credentials provider implementation, endpoint provider implementation, broker SDK integration, network access, order placement, real broker order id storage, reconciliation, cancellation, correction, strategy execution, or automatic trading. Each of those areas requires separate explicit authorization.

Future enablement may only proceed after explicit mode source, credentials provider, endpoint provider, sandbox adapter boundary, kill switch, and rollback policy are documented and tested. It must fail closed.

## Fail-Closed Policy

Unknown, blank, unsupported, paper, and real modes must fail closed. Before sandbox runtime is separately authorized, direct sandbox selector access may only return the existing scaffold unavailable / not configured behavior.

`DataServiceActions.cpp` must not pass Sandbox, Paper, Real, string literals, request payload fields, or externally supplied values into `shellAccountingBrokerRuntimeModeSourceForMode`.

## Mode Source Isolation

`DataServiceActions.cpp` must not read payload, QML, config, environment, command line, files, database, or secret stores as broker mode sources.

QML, Presenter, Controller, ShellServices, ShellCore, and startup code must not directly provide broker mode, access broker adapters, or bypass the DataService boundary.

## Forbidden Side Effects

TASK-173 adds no broker SDK, network call, endpoint, URL, host, port, credentials, secrets, token, key, password, real broker order id, real order placement, database write, audit write, ledger write, reconciliation, cancellation, correction, strategy execution, automatic trading, or schema change.

Real credentials, secrets, tokens, keys, passwords, endpoints, and usable examples must not enter code, repository files, documentation, logs, or test data. Policy keywords may appear in docs/tests, but real values and usable credential examples must not.

## CI Policy

CI must continue to enforce no-network, no-credentials, and no-real-order-placement checks. Static scans must distinguish production source from docs/tests: production source is scanned for forbidden implementation tokens, while docs/tests are checked only for policy text and expected gate coverage.

## Rollback Policy

Rollback is to keep TASK-172 disabled-default selector wiring or remove the selector call and return to the TASK-166 disabled-only source plus TASK-159 disabled/null provider behavior. Rollback must not enable sandbox runtime or change TradeDraft, confirmation, broker dry-run, audit, ledger, UI, or broker order behavior.

## Future Implementation Requirements

Any future sandbox runtime enablement must open a separate TASK, reference docs/144 and docs/145, preserve fail-closed behavior, and include static gates proving no unauthorized payload, QML, config, environment, command line, file, database, or secret-store mode source is introduced.
