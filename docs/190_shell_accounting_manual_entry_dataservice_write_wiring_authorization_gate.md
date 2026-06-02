# ShellAccounting Manual Entry DataService Write Wiring Authorization Gate

## Document Purpose

TASK-197 is a DataService write wiring authorization gate-only task for
ShellAccounting manual entry actions. It defines the future boundary for
connecting validated manual entry DataService actions to the already authorized
DataAccess repositories.

This task does not implement DataService runtime write wiring.

- It does not modify `DataServiceActions.cpp`.
- It does not modify `DataServiceActions.h`.
- It does not modify `DataServiceActionRegistrar.cpp`.
- It does not wire repositories.
- It does not write database rows.
- It does not execute runtime SQL.
- It does not return persistent id values from DataService action responses.

## Current State

TASK-182 completed validation-only wiring for:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

Valid payloads still return validation accepted with `writeImplemented=false`
and `writeEnabled=false`.

TASK-192 implemented the DataAccess-only
`ShellAccountingManualTransactionRepository` for manual BUY / SELL repository
writes. TASK-196 implemented the DataAccess-only
`ShellAccountingManualCashMovementRepository` for Deposit / Withdrawal
`trade_log + cash_adjustment` dual-write.

The repositories are not wired into DataService runtime actions. QML, startup,
Presenter, Controller, AccountingEngine replay, read model, UI, broker, network,
credentials, endpoint, real order placement, and automatic trading remain out of
scope.

## Future DataService Write Wiring Boundary

Future DataService write wiring must be separately authorized in a later TASK.
That future task must preserve TASK-178 / TASK-182 validation-first behavior and
may call only the authorized DataAccess repository boundary after validation
succeeds.

Future DataService write wiring must not scatter SQL in DataService action handlers.
SQL location remains DataAccess only. DataService responses must be
explicit, sanitized local write results, and failure mapping must not leak raw
SQL, raw payloads, credentials, endpoint values, authorization tokens, or
internal stack traces.

Future audit write and ledger policy must remain separate TASKs. Future replay,
read model, and UI integration must remain separate TASKs. First phase manual
entry work remains no real broker, no automatic trading, and no real order
placement. Broker sandbox new capability development remains paused. Existing broker disabled, fail-closed,
no-real-order, no-network, no-credentials, no-order-placement, and
no-automatic-trading gates must remain retained.

Future DataService write wiring remains no real order placement.

## Manual Transaction DataService Wiring Policy

- Action: `accounting.manual_transaction.create`.
- Validation: existing TASK-182 validation remains first.
- Repository: TASK-192 `ShellAccountingManualTransactionRepository`.
- SQL location: DataAccess only.
- DataService response: sanitized local write result only.
- No broker semantics.
- No replay trigger.

Future manual transaction wiring must map a valid payload to a DataAccess
repository command for manual BUY / SELL. Duplicate idempotency keys must map to
a stable idempotent DataService response. Repository failure must map to a safe
DataService error.

## Manual Cash Movement DataService Wiring Policy

- Action: `accounting.manual_cash_movement.create`.
- Validation: existing TASK-182 validation remains first.
- Repository: TASK-196 `ShellAccountingManualCashMovementRepository`.
- SQL location: DataAccess only.
- DataService response: sanitized local write result only.
- No broker semantics.
- No replay trigger.

Future manual cash movement wiring must map a valid payload to the DataAccess
repository command for Deposit / Withdrawal and must preserve the TASK-196
`trade_log + cash_adjustment` dual-write boundary. Duplicate idempotency keys
must map to a stable idempotent DataService response. Repository failure must
map to a safe DataService error.

## Failure / Rollback / Idempotency Policy

Repository transaction boundaries remain authoritative. DataService must not
split repository transactions. Duplicate idempotency must map to a stable
DataService response. Repository validation or database errors must map to a
stable protocol error. No silent success is allowed.

Future error mapping must not leak raw SQL, raw request payloads, internal
exception stacks, credentials, endpoint values, tokens, or broker payloads.

## Explicit Non-Goals

TASK-197 does not modify migrations, does not add a migration or schema file,
does not modify production QML or startup, does not modify Presenter or
Controller behavior, does not modify DataServiceActions, does not modify
DataServiceActionRegistrar, does not modify TASK-178 validation production code,
does not modify TASK-192 or TASK-196 repositories, does not add DataService
runtime write wiring, does not add runtime SQL / SQLite writes, does not write
`trade_log`, `cash_adjustment`, `audit_log`, or ledger rows through a
DataService action, does not modify AccountingEngine replay, does not add
StrategyEngine or MarketEngine implementation, does not add TradeDraft or
suggestion implementation, does not add broker SDK, network, credentials, or
endpoint capability, does not place real orders, does not store real broker
order ids, and does not add automatic trading.

## Required Follow-Up

Future DataService write wiring implementation must be a separate TASK. Future
replay / read model / UI integration must be a separate TASK. Future audit write
or ledger policy must be a separate TASK. Future broker sandbox capability,
real broker order placement, reconciliation, cancellation, correction, or
automatic trading must remain separately authorized.
