# ShellAccounting Broker Runtime Mode Source Disabled Scaffold (TASK-166)

## Document Purpose

TASK-166 establishes a disabled-only internal broker runtime mode source for ShellAccounting. The scaffold is intentionally narrow: it only returns `ShellAccountingBrokerOrderPortMode::Disabled`, it does not read any external runtime input, and it keeps the TASK-159 disabled/null provider semantics for broker dry-run.

This task does not authorize sandbox runtime, paper runtime, real runtime, broker SDK access, network access, credentials, endpoint configuration, real broker order id generation, order placement, database write, audit write, ledger write, reconciliation, cancellation, correction, strategy execution, automatic trading, production QML changes, production startup changes, Presenter changes, Controller changes, or schema changes.

## Current Status

- TASK-165 completed the runtime mode source authorization gate.
- TASK-166 adds `ShellAccountingBrokerRuntimeModeSource`, `DisabledShellAccountingBrokerRuntimeModeSource`, and `defaultShellAccountingBrokerRuntimeModeSource()`.
- The default source always returns disabled mode.
- `DataServiceActions` may ask the source for the broker port mode and then call `shellAccountingBrokerOrderPortForMode(brokerPortMode)`.
- Runtime dry-run remains equivalent to the TASK-159 disabled/null provider.
- Existing response semantics such as `brokerPortMode`, `brokerPortDisabled`, `brokerPortDryRunOnly`, and `BROKER_ORDER_DISABLED` remain unchanged.

## Disabled-Only Source Boundary

The source has no configuration state and no external input dependency. It does not read request payload, QML, Presenter state, config, environment, command line, files, database rows, or secret store values. It does not expose sandbox, paper, or real selection.

The only current runtime mode returned by the source is `ShellAccountingBrokerOrderPortMode::Disabled`.

## DataServiceActions Wiring

`DataServiceActions` uses the source only as an internal disabled-default boundary:

- call `defaultShellAccountingBrokerRuntimeModeSource()`;
- call `brokerOrderPortMode()` on that source;
- pass the returned mode to `shellAccountingBrokerOrderPortForMode(brokerPortMode)`;
- keep the selected broker port disabled/null for runtime dry-run.

No request or UI field can override this value in TASK-166.

## No External Runtime Source Policy

External runtime mode source remains unauthorized. The following remain forbidden:

- payload mode source;
- QML mode source;
- config or settings source;
- environment source;
- command-line source;
- file source;
- database source;
- secret-store source;
- sandbox, paper, or real runtime mode source.

## No Broker / No Network / No Credential Policy

TASK-166 adds no broker SDK, network call, host, API path, endpoint, credentials, token, key, password, certificate, real broker order id, order placement, reconciliation, cancellation, correction, strategy execution, or automatic trading. CI remains no-network, and credentials isolation remains required for any future mode source task.

## No Write Policy

The disabled-only source and selector wiring do not write any database table. TASK-166 adds no audit write, ledger write, broker order status write, trade draft write, trade log write, or execution group write.

## Rollback Policy

Rollback is to keep using the TASK-159 disabled/null provider semantics. If the source is removed or disabled, broker dry-run must still fail closed to disabled/null behavior and must not infer sandbox, paper, or real mode.

## Test Coverage

TASK-166 tests cover source compilation, direct instantiation, default disabled mode, `DataServiceActions` source usage, unchanged disabled dry-run semantics, unchanged `BROKER_ORDER_DISABLED`, no payload/QML/config/env/CLI/file/DB/secret-store source, no broker SDK, no network, no credentials or endpoint, no real broker order id, no order placement, no write side effects, no reconciliation/cancellation/correction, no strategy execution, no automatic trading, and TASK-160 through TASK-165 regression.

## Unauthorized Future Scope

Sandbox runtime, paper runtime, real runtime, external runtime mode source, broker credentials, broker endpoints, real broker SDK integration, real broker order placement, reconciliation, cancellation, correction, strategy execution, and automatic trading all require separate tasks and explicit authorization.

## TASK-167 Follow-Up Gate

TASK-167 adds a sandbox runtime mode source authorization gate only. It does not implement sandbox mode source selection, does not enable sandbox runtime, does not implement a sandbox adapter, and does not change the TASK-166 disabled-only source. Credentials, endpoints, SDK, network, real order placement, real broker order id storage, reconciliation, cancellation, correction, strategy execution, and automatic trading remain separately unauthorized.

## TASK-168 Follow-Up Scaffold

TASK-168 adds a direct-test sandbox runtime mode source scaffold only. The default runtime source remains disabled-only, `DataServiceActions` does not wire the sandbox scaffold, and sandbox runtime remains disabled. External mode sources, credentials, endpoints, SDK, network, real order placement, real broker order id storage, reconciliation, cancellation, correction, strategy execution, and automatic trading remain separately unauthorized.
