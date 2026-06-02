# ShellAccounting Manual Cash Movement Schema Contract Alignment Gate

## Document Purpose

TASK-195 is a schema-contract alignment gate-only task. It records the blocking
schema fact discovered after TASK-194: current `cash_adjustment` rows cannot be
written standalone when SQLite foreign keys are enabled.

This task does not implement manual cash movement repository write, does not
write `cash_adjustment`, does not write `trade_log`, does not write
`audit_log`, does not write ledger rows, does not modify DataServiceActions,
does not modify migrations, and does not add QML, replay, broker, network,
credentials, endpoint, real order, or automatic trading capability.

## Current Schema Contract

`migrations/001_initial_schema.sql` defines:

- `cash_adjustment.trade_log_id INTEGER NOT NULL`
- `FOREIGN KEY (trade_log_id) REFERENCES trade_log(id)`

Because `cash_adjustment.trade_log_id` is NOT NULL and references
`trade_log(id)`, the current schema blocks cash_adjustment-only write. A
manual cash movement repository cannot insert a valid `cash_adjustment` row
without a valid `trade_log.id` while foreign keys are enabled.

`migrations/002_shell_accounting_manual_entry_schema.sql` adds manual entry
trace and idempotency columns, including:

- `cash_adjustment.request_id`
- `cash_adjustment.idempotency_key`
- `cash_adjustment.occurred_at_utc`
- `cash_adjustment.source_memo_sanitized`
- `cash_adjustment.trade_log_uid`
- `trade_log.request_id`
- `trade_log.idempotency_key`
- `trade_log.occurred_at_utc`
- `trade_log.source_memo_sanitized`
- `trade_log.cash_adjustment_uid`

These fields support linkage, but they do not remove the NOT NULL
`cash_adjustment.trade_log_id` requirement.

## Alignment Decision

Future manual cash movement repository implementation must not use a cash_adjustment-only design under the current schema.

The current project priority is the dual-write boundary:

- write `trade_log` cash movement fact
- write `cash_adjustment`
- set `cash_adjustment.trade_log_id` to the written `trade_log.id`
- set or explicitly map `trade_log.cash_adjustment_uid` to the cash adjustment
  uid / linkage policy
- perform both writes in one atomic transaction
- rollback on any failure
- produce no partial facts

If a future task wants standalone cash_adjustment, that task must first
separately authorize a schema migration. TASK-195 does not modify
`migrations/001_initial_schema.sql` or
`migrations/002_shell_accounting_manual_entry_schema.sql`.

## Future Implementation Boundary

Future manual cash movement repository implementation must be a separate TASK.
It must stay inside the DataAccess repository boundary and must use the
TASK-190 schema fields. Future DataService action write wiring must be a separate TASK.
Future replay, read model, UI integration, audit integration,
and broker work must remain separate authorization tasks.

Future implementation must not modify TASK-192 manual transaction repository
behavior. It must not scatter SQL in DataService action handlers. It must not
write `audit_log` or ledger rows unless separately authorized.

## Forbidden Runtime Policy

TASK-195 does not authorize:

- repository implementation
- runtime SQL / SQLite write
- `cash_adjustment` write
- `trade_log` write
- `audit_log` write
- ledger write
- DataServiceActions changes
- QML / startup / Presenter / Controller changes
- AccountingEngine replay changes
- TradeDraft or suggestion implementation
- broker SDK, network, credentials, endpoint, real order, broker order id, or
  automatic trading

Broker sandbox new capability development remains paused. Existing broker,
real broker, no-network, no-credentials, and no-order-placement gates must stay
retained and must not be weakened, skipped, or deleted.

## Required Follow-Up

The next implementation task must choose one of two explicitly authorized
paths:

1. Implement DataAccess-only manual cash movement repository dual-write for
   `trade_log` + `cash_adjustment`.
2. Open a separate schema migration authorization task to allow standalone
   cash_adjustment before implementing cash_adjustment-only writes.

No future task may assume cash_adjustment-only write is valid under the current
schema.
