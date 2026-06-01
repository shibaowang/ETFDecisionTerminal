# ShellAccounting Broker Sandbox Runtime Mode Source Selector Scaffold (TASK-170)

## Document Purpose

TASK-170 adds a directly testable broker runtime mode source selector scaffold. It is not runtime enablement, does not wire the selector into `DataServiceActions`, and does not change production QML, startup, Presenter, Controller, broker SDK, network, credentials, order placement, database write, audit write, ledger write, reconciliation, cancellation, correction, strategy execution, automatic trading, or schema behavior.

TASK-171 adds a wiring authorization gate after this scaffold. TASK-172 then wires `DataServiceActions.cpp` to the selector using only the disabled default source mode. The selector scaffold remains direct-test-only for sandbox, and sandbox runtime remains disabled.

## Current Status

TASK-169 authorized a future selector boundary. TASK-170 implements only the scaffold needed for direct tests: `shellAccountingBrokerRuntimeModeSourceForMode`.

- `defaultShellAccountingBrokerRuntimeModeSource()` remains disabled-only.
- `ShellAccountingBrokerRuntimeModeSourceForMode` behavior is wired into `DataServiceActions.cpp` only through disabled-default selector wiring after TASK-172.
- `DataServiceActions.cpp` continues to be driven only by the disabled default runtime source.
- The TASK-168 sandbox runtime mode source scaffold remains direct-test-only.
- Sandbox runtime remains disabled.
- Paper and real runtime mode sources remain unimplemented.

## Selector Scaffold Boundary

The selector scaffold accepts only an explicit `ShellAccountingBrokerOrderPortMode` value. It does not read payload, QML, config, environment, command line, files, database, secret store, credentials, endpoint, account data, order data, or broker payloads.

The scaffold maps modes as follows:

- `Disabled` returns the disabled-only runtime mode source.
- `Sandbox` returns the TASK-168 sandbox runtime mode source scaffold.
- `Unsupported` and any unknown enum value fail closed to the disabled-only runtime mode source.

The sandbox path through this scaffold is direct-test-only. TASK-172 allows production `DataServiceActions.cpp` to use the selector only with the disabled default source mode.

## Default Runtime Policy

The default runtime source remains disabled-only. TASK-170 does not alter the behavior of `defaultShellAccountingBrokerRuntimeModeSource()`, and it does not make sandbox the default.

## DataServiceActions Policy

`DataServiceActions.cpp` is not modified by TASK-170. TASK-172 may call the selector scaffold only with `defaultShellAccountingBrokerRuntimeModeSource().brokerOrderPortMode()`. Runtime broker mode source selection remains disabled-only through the existing default source.

## No External Mode Source Policy

TASK-170 adds no payload mode source, QML mode source, config source, environment source, command-line source, file source, database source, or secret-store source. Future external mode source work requires a separate authorization task and must remain fail-closed.

## No Broker Side Effect Policy

TASK-170 adds no broker SDK, network call, endpoint, URL, host, port, credentials, secrets, token, key, password, real broker order id, real order placement, database write, audit write, ledger write, reconciliation, cancellation, correction, strategy execution, or automatic trading.

Real credentials, secrets, tokens, keys, passwords, endpoints, and usable examples must not enter code, repository files, documentation, logs, or test data. Policy keywords may appear in docs/tests, but real values and usable credential examples must not.

## Error And Fail-Closed Policy

Unsupported, blank, unknown, paper, and real modes must fail closed to disabled or an explicit unavailable/error result. No selector path may silently infer sandbox, paper, or real mode.

## Rollback Policy

Rollback is to continue using `defaultShellAccountingBrokerRuntimeModeSource()`, which remains disabled-only. Removing or disabling the selector scaffold must not affect TradeDraft, confirmation, broker dry-run, or existing disabled broker behavior because TASK-170 does not wire the scaffold into runtime.

## Future Implementation Requirements

Any future runtime selector wiring, sandbox adapter implementation, credentials provider, endpoint provider, real broker order id storage, reconciliation, cancellation, correction, strategy execution, automatic trading, or real broker order placement must open a separate TASK and reference docs/138 and docs/139.

Future DataServiceActions selector wiring must also reference docs/140 and docs/141. The first future wiring step may only be disabled-default selector wiring unless a separate TASK explicitly authorizes sandbox runtime mode source enablement.
