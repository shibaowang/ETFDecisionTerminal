# ShellAccounting Broker Sandbox Runtime Selector Disabled Wiring (TASK-164)

## Document Purpose

TASK-164 wires the TASK-162 broker order port mode selector into the DataService broker dry-run path with a disabled-default mode only. This is runtime selector plumbing, not sandbox runtime enablement.

Summary phrases for static gates: disabled-default runtime selector wiring; runtime mode source not authorized; sandbox runtime not enabled; sandbox scaffold does not enter runtime; default remains disabled; CI no-network; credentials isolation; no real broker order id; no order placement.

## Current Status

- TASK-159 disabled/null provider remains the effective runtime behavior.
- TASK-161 sandbox scaffold remains unavailable / not configured and is not on the runtime request path.
- TASK-162 selector exists.
- TASK-163 authorization gate has been updated to allow only disabled-default selector wiring.
- TASK-164 connects `DataServiceActions` to the selector using `defaultShellAccountingBrokerOrderPortMode()` only.

## DataService Runtime Wiring

The broker dry-run path now obtains the mode from `defaultShellAccountingBrokerOrderPortMode()` and obtains the port from `shellAccountingBrokerOrderPortForMode(mode)`. Because the default mode is `Disabled`, the selected port is still equivalent to the TASK-159 disabled/null provider.

No request field, QML setting, config value, environment variable, command-line argument, database value, file value, or secret-store value is read as broker mode.

## Disabled-Default Behavior

Runtime behavior remains disabled/null provider behavior. Existing dry-run response semantics remain unchanged: disabled broker state, dry-run-only behavior, `BROKER_ORDER_DISABLED`, no broker SDK call, no order submission, no real broker order id, and no real order placement.

## Sandbox Runtime Policy

Sandbox runtime is not enabled by TASK-164. The sandbox scaffold can still be selected only by explicit selector unit tests or direct test probes. It must not enter the production runtime request path until a later task separately authorizes a runtime mode source and sandbox adapter behavior.

## Forbidden Mode Sources

TASK-164 does not add runtime mode source wiring. The dry-run path must not read broker mode from payload fields, QML, config files, environment variables, command-line arguments, local files, databases, or secret stores.

## No Broker / No Write Policy

TASK-164 adds no broker SDK, network call, endpoint, host, API path, credentials, secrets, token, key, password, real broker order id, order placement, database write, audit write, ledger write, reconciliation, cancellation, correction, strategy execution, automatic trading, production QML change, startup change, Presenter change, Controller change, schema change, or TradeDraft create / confirm behavior change.

## CI No-Network And Credentials Isolation

CI no-network remains mandatory. Credentials isolation remains mandatory. Documentation and tests may mention policy words, but production code must not contain usable credential examples, endpoint values, credential reads, network clients, or broker SDK calls.

## Rollback Policy

Rollback is to return the dry-run path to directly using the TASK-159 disabled/null provider or to keep `defaultShellAccountingBrokerOrderPortMode()` disabled. Either rollback path must preserve dry-run-only disabled behavior and must not affect TradeDraft creation / confirmation, audit write, snapshot write, read-only replay, or production UI.

## Test Checklist

- DataServiceActions uses selector wiring.
- DataServiceActions mode source is only `defaultShellAccountingBrokerOrderPortMode()`.
- Runtime dry-run still returns disabled/null provider semantics.
- `brokerPortMode` remains disabled.
- No sandbox / paper / real runtime string is used.
- No payload / QML / config / env / command-line / secret-store mode source is read.
- Sandbox scaffold remains unavailable in direct selector tests and does not enter runtime.
- No production QML / startup / Presenter exposure.
- No SDK / network / credentials / endpoint / real order id / order placement.
- No database / audit / ledger write introduced by this wiring.
- No reconciliation / cancellation / correction / strategy execution / automatic trading.

## Unauthorized Follow-Up Scope

Runtime mode source wiring, sandbox runtime enablement, credentials injection, sandbox adapter implementation, paper adapter implementation, real broker adapter implementation, broker SDK integration, order placement, broker order id storage, reconciliation, cancellation, correction, strategy execution, automatic trading, and schema changes remain separately unauthorized.

## TASK-165 Follow-Up Gate

TASK-165 adds a broker runtime mode source authorization gate after this disabled-default wiring. It does not implement mode source selection, does not enable sandbox runtime, and does not change the TASK-164 DataServiceActions disabled-default selector wiring.

## TASK-166 Follow-Up Scaffold

TASK-166 adds a disabled-only internal broker runtime mode source. `DataServiceActions` may obtain the broker mode from that source, but the source only returns disabled and the selected provider remains the TASK-159 disabled/null provider. External runtime mode source, sandbox runtime, SDK/network/credentials/endpoint, real broker order id, order placement, database/audit/ledger write, reconciliation, cancellation, correction, strategy execution, and automatic trading remain unauthorized.
