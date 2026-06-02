# ShellAccounting Manual Entry DataService Write Wiring Implementation

## Document Purpose

TASK-198 implements ShellAccounting manual entry DataService write wiring. It
connects the existing DataService actions to the already authorized DataAccess
repositories after existing validation succeeds.

This task does not add migrations, does not change repository implementation,
does not modify production QML or startup, does not add replay or read model
integration, and does not add broker, network, credentials, endpoint, real order
placement, or automatic trading capability.

## Current State

TASK-197 completed the authorization gate for manual entry DataService write
wiring. TASK-192 implemented the DataAccess-only
`ShellAccountingManualTransactionRepository`. TASK-196 implemented the
DataAccess-only `ShellAccountingManualCashMovementRepository` dual-write
boundary.

TASK-198 wires:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

through validation-first DataService handlers into those repository boundaries.

## DataService Action Boundary

The DataService action handlers remain the only runtime action entry point. The
handlers validate structured payloads first. Validation failure returns a safe
DataService error and does not call any repository.

Validation success maps the payload to a repository command:

- Manual transaction writes use `ShellAccountingManualTransactionRepository`.
- Manual cash movement writes use `ShellAccountingManualCashMovementRepository`.

`DataServiceActions.cpp` does not scatter SQL and does not issue direct
`INSERT`, `UPDATE`, `DELETE`, or `REPLACE` statements. SQL stays inside the
DataAccess repositories.

## Manual Transaction Write Mapping

`accounting.manual_transaction.create` maps a valid payload to:

- account id
- portfolio id
- instrument id
- security code
- trade side `BUY` / `SELL`
- quantity
- price
- gross amount
- fee
- tax
- occurred-at timestamp
- source memo
- request id
- idempotency key

The response reports a sanitized local write result. It includes
`writeImplemented=true`, `writeEnabled=true`, `validationAccepted=true`,
`databaseWritten=true`, `repositoryWrite=true`, `tradeLogWritten=true`,
`cashAdjustmentWritten=false`, `auditWritten=false`, and `ledgerWritten=false`
for successful writes.

## Manual Cash Movement Write Mapping

`accounting.manual_cash_movement.create` maps a valid payload to:

- account id
- portfolio id
- movement type `Deposit` / `Withdrawal`
- amount
- currency
- occurred-at timestamp
- source memo
- source reference
- request id
- idempotency key

The response reports a sanitized local write result. It includes
`writeImplemented=true`, `writeEnabled=true`, `validationAccepted=true`,
`databaseWritten=true`, `repositoryWrite=true`, `tradeLogWritten=true`,
`cashAdjustmentWritten=true`, `auditWritten=false`, and `ledgerWritten=false`
for successful writes.

## Error Mapping And Idempotency

Invalid payloads, non-object payloads, and sensitive memo payloads fail before
repository invocation. Repository failures map to safe DataService errors and
must not leak raw SQL, raw payloads, credentials, endpoint values, tokens, broker
payloads, or internal stack traces.

Duplicate idempotency keys map to a stable idempotent DataService response and
must not create duplicate rows.

## Explicit Non-Goals

TASK-198 does not modify `migrations/001_initial_schema.sql` or
`migrations/002_shell_accounting_manual_entry_schema.sql`, does not add a
migration or schema file, does not modify TASK-178 validation production code,
does not modify TASK-192 or TASK-196 repository implementation, does not modify
`DataServiceActionRegistrar.cpp`, does not modify production QML, startup,
Presenter, or Controller, does not trigger AccountingEngine replay, does not add
read model or UI integration, does not write `audit_log` or ledger rows, does
not add TradeDraft or suggestion implementation, does not add broker SDK,
network, credentials, or endpoint capability, does not place real orders, does
not store real broker order ids, and does not add automatic trading.

Broker sandbox new capability development remains paused.

## Future Work

Future replay/read-model integration, UI exposure, audit write, ledger policy,
broker sandbox capability, real broker order placement, or automatic trading
must be separately authorized in future TASKs.
