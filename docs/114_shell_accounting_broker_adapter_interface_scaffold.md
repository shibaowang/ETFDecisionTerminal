# TASK-157 ShellAccounting Broker Adapter Interface Scaffold

## Purpose

TASK-157 adds only the ShellAccounting broker adapter interface scaffold. It
prepares an abstract boundary for a future real broker order implementation
without implementing a real broker adapter, importing a broker SDK, calling
sandbox / paper / real broker endpoints, making broker network calls, placing
orders, changing production QML, changing production startup, changing schema,
or adding credentials / secrets.

TASK-156 real broker order implementation gate is complete. This task keeps
that gate intact: real order placement remains unauthorized and unimplemented.

## Interface Ownership / Location Rationale

The scaffold lives in `libs/DataServiceApi` because future broker placement
must remain behind the DataService boundary. The production Shell, QML,
Presenter, ShellServices, ShellCore, DataAccess, and tests must not directly
depend on concrete broker SDK types.

The public boundary is `ShellAccountingBrokerOrderPort` with request and
response DTOs. It describes an order intent envelope only; it is not an order
placement implementation and it is not a broker SDK wrapper.

## Request DTO Policy

`ShellAccountingBrokerOrderRequest` may describe:

- account id
- portfolio id
- instrument id / symbol
- side
- quantity
- limit price
- reason
- source TradeDraft id
- source ledger id
- source dry-run id
- user confirmation id
- idempotency key
- final user confirmation flag
- sanitized risk flags

It must not include credentials, raw broker payload, raw QML payload, raw SQL,
or secrets.

## Response DTO Policy

`ShellAccountingBrokerOrderResponse` supports:

- disabled
- dry-run-only
- rejected
- accepted placeholder
- pending placeholder
- unknown state placeholder
- error code
- sanitized message
- disabled broker mode
- sanitized correlation id only

The scaffold does not expose real broker order id, exchange order id, full
broker payload, credentials, raw SQL, full `trade_log` payload, full TradeDraft
payload, or internal exception stack traces.

## Disabled / Null Adapter Policy

`DisabledShellAccountingBrokerOrderPort` is the only production implementation
added by TASK-157. It always returns disabled, dry-run-only, non-success output
and has no side effects. It does not write any database table, does not call a
broker SDK, does not call a broker endpoint, does not place an order, and does
not execute strategy.

## Test-only Fake Adapter Policy

The fake adapter is test-only under
`tests/ShellAccountingBrokerAdapterInterfaceScaffold`. It can return placeholder
responses for contract tests, but it must not access network, must not write a
database, must not generate a real broker order id, must not expose credentials,
and must not be linked into production code.

## No Broker SDK Policy

TASK-157 does not import, link, wrap, or reference a concrete broker SDK. Any
future real broker SDK integration requires a separate TASK, a separate
authorization token, dedicated tests, credential isolation, and explicit user
approval.

## No Network Call Policy

The scaffold must not make broker network calls. Tests and CI must not call
sandbox, paper, or real broker endpoints. Future network access requires a
separate TASK and must be disabled in CI unless explicitly authorized.

## No Credentials Policy

No credentials, secrets, broker tokens, private keys, endpoint secrets, or
passwords are introduced by TASK-157. The request and response DTOs must not
carry credentials, and logs must not print credential-like material.

## No Real Order Placement Policy

TASK-157 does not place real orders and does not create a real broker order
identifier. The accepted / pending states are placeholders only for future
mapping. A future real implementation must be a separate TASK and must preserve
TASK-154 dry-run preflight, confirmed-ledger input, final user confirmation,
independent authorization, idempotency, risk preflight, audit, reconciliation,
and kill-switch behavior.

## Error Mapping Policy

The scaffold supports disabled, dry-run-only, rejected, accepted placeholder,
pending placeholder, unknown state placeholder, and error status mapping. Error
responses must use an error code and sanitized message. They must not expose raw
broker payload, raw SQL, credentials, full TradeDraft payload, full `trade_log`
payload, or stack traces.

## Audit / Reconciliation Future Handoff Policy

TASK-157 does not write audit or reconciliation records. Future real broker
implementation must hand off sanitized order intent, dry-run reference,
confirmed ledger reference, final user confirmation, idempotency key, broker
mode marker, risk preflight result, and mapped broker state to the authorized
audit and reconciliation path.

## Rollback Strategy

Rollback is removing the interface scaffold and returning to the TASK-154
dry-run-only boundary. The disabled/null adapter is safe to leave in place
because it returns disabled and has no side effects. Disabling or removing this
scaffold does not affect TASK-154 dry-run, does not affect TradeDraft create,
does not affect TradeDraft confirm, does not affect production trading UI, and
does not fake broker success. Any future real implementation requires a
separate TASK.

## Future Implementation Requirements

Future real broker implementation must:

- open a separate user-authorized TASK
- remain DataService-only
- use a concrete adapter behind the abstract port
- keep concrete broker SDK types out of QML, Shell, Presenter, ShellServices,
  ShellCore, and DataAccess
- require TASK-154 dry-run preflight
- require confirmed TradeDraft / ledger fact input
- require final user confirmation
- require independent authorization
- remain default-disabled and kill-switchable
- enforce risk preflight
- use idempotency and duplicate prevention
- write sanitized audit
- define reconciliation and unknown-state handling
- keep CI broker network calls disabled
- avoid credentials in source, docs, tests, QML, and logs
- avoid StrategyEngine execution and automatic trading
