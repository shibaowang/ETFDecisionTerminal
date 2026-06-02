# ShellAccounting Manual Entry Repository Implementation Post-Migration Authorization Gate

## Document Purpose

TASK-191 is a repository implementation post-migration authorization gate only.
It records the boundary for future ShellAccounting manual entry repository
implementation after TASK-190 landed
`migrations/002_shell_accounting_manual_entry_schema.sql`.

TASK-191 does not implement a repository, does not modify DataAccess production
code, does not modify the TASK-185 repository scaffold, does not modify
DataServiceActions, does not write SQLite, does not execute runtime SQL, does
not write runtime `trade_log` / `cash_adjustment` / `audit_log`, and does not
add persistent id responses.

TASK-191 does not modify DataAccess production code.
TASK-191 does not modify DataServiceActions.
TASK-191 does not write runtime `trade_log` / `cash_adjustment` / `audit_log`.
TASK-191 does not add persistent id responses.

TASK-191 does not modify `migrations/001_initial_schema.sql` or
`migrations/002_shell_accounting_manual_entry_schema.sql`.

TASK-191 does not modify `migrations/001_initial_schema.sql`.
TASK-191 does not modify `migrations/002_shell_accounting_manual_entry_schema.sql`.

## Current State

TASK-190 completed the manual entry schema migration implementation. The
independent `migrations/002_shell_accounting_manual_entry_schema.sql` migration
adds request tracing, idempotency, occurred-at, sanitized memo, linkage, tax,
payload classification, redaction status, and index support for future manual
entry writes.

Current production code still has no manual entry repository implementation.
Current TASK-185 repository scaffold remains disabled and write-not-implemented.
Current TASK-182 DataService validation wiring remains validation-only and
write-not-implemented. Current DataServiceActions handlers do not contain
manual entry repository SQL.

TASK-192 implements manual transaction repository write for manual BUY / SELL
entries only. That implementation is DataAccess-only and direct-test-only. It
does not implement manual cash movement write, does not modify DataService
actions, and does not authorize runtime DataService write wiring.

## Future Repository Implementation Boundary

Future repository implementation must be a separate TASK. That future TASK may
implement only the DataAccess repository boundary unless a separate
authorization expands scope. DataService action write implementation must remain
a separate TASK. Future DataService action write implementation must be a
separate TASK. AccountingEngine replay, read model, and UI work must also remain
separately authorized.

DataService action write implementation must remain a separate TASK.

Future repository implementation must use the TASK-190 schema fields. It must
not scatter SQL in DataService action handlers. Future repository implementation
must not scatter SQL in DataService action handlers. It must not add broker
SDKs, network calls, credentials, endpoints, real order placement, automatic
trading, TradeDraft implementation, or trade suggestions.

## DataAccess-Only Repository Policy

Future repository implementation must live behind a DataAccess repository
boundary. DataService action handlers may coordinate authorized calls, but they
must not own scattered SQL strings or direct SQLite write mechanics. A later
DataService write TASK must explicitly authorize any DataService action wiring.

The future repository must not be used as a generic write escape hatch. It must
only expose the specific manual transaction and manual cash movement write
operations authorized by the future implementation TASK.

## Manual Transaction Mapping Boundary

Future repository implementation must use or explicitly map:

- `trade_log.request_id`
- `trade_log.idempotency_key`
- `trade_log.occurred_at_utc`
- `trade_log.tax_cents`
- `trade_log.source_memo_sanitized`
- `trade_log.cash_adjustment_uid` only when applicable
- `trade_log.quantity_1e6`
- `trade_log.price_1e6`
- `trade_log.amount_cents`
- `trade_log.fee_cents`
- `trade_log.net_cash_impact_cents`
- `trade_log.action_type`
- `trade_log.trade_source`
- `trade_log.manual_entry`

## Manual Cash Movement Mapping Boundary

Future repository implementation must use or explicitly map:

- `cash_adjustment.request_id`
- `cash_adjustment.idempotency_key`
- `cash_adjustment.occurred_at_utc`
- `cash_adjustment.source_memo_sanitized`
- `cash_adjustment.trade_log_uid`
- `cash_adjustment.amount_cents`
- `cash_adjustment.adjustment_type`
- `trade_log.request_id`
- `trade_log.idempotency_key`
- `trade_log.occurred_at_utc`
- `trade_log.cash_adjustment_uid`
- `trade_log.action_type`
- `trade_log.trade_source`
- `trade_log.manual_entry`

## Audit Mapping Boundary

Future repository implementation or a later DataService write implementation
must define:

- `audit_log.request_id`
- `audit_log.idempotency_key`
- `audit_log.payload_classification`
- `audit_log.redaction_status`
- sanitized payload shape
- audit failure policy
- transaction boundary between business facts and audit facts

Audit output must not include raw SQL, raw manual entry payloads, credentials,
broker payloads, endpoint values, or internal exception stacks.

## Transaction / Rollback / Idempotency Policy

Future repository implementation must define:

- transaction boundary for manual transaction write
- transaction boundary for manual cash movement write
- rollback / failure atomicity
- idempotency behavior on duplicate idempotency_key
- request_id trace behavior
- unique index conflict handling
- safe error mapping without leaking raw SQL or raw payload
- no partial facts on failure

Duplicate request handling must use the TASK-190 idempotency schema support and
must not insert duplicate business facts.

## Forbidden Current Scope

TASK-191 does not authorize:

- modification of `migrations/001_initial_schema.sql`
- modification of `migrations/002_shell_accounting_manual_entry_schema.sql`
- any new migration or schema file
- repository implementation
- runtime SQL execution
- SQLite runtime write
- runtime `trade_log` write
- runtime `cash_adjustment` / cash facts / cash ledger write
- runtime `audit_log` / ledger write
- persistent id response fields such as tradeLogId, cashFactId, auditLogId, or ledgerId
- DataServiceActions.cpp / DataServiceActions.h / DataServiceActionRegistrar.cpp changes
- TASK-178 validation production code changes
- TASK-185 repository scaffold changes
- production QML, startup, Presenter, or Controller changes
- AccountingEngine replay changes
- StrategyEngine or MarketEngine implementation
- TradeDraft or suggestion implementation
- broker SDK, network, credentials, endpoint, real order id, real order placement, or automatic trading

## Broker And Trading Policy

Broker sandbox new capability development remains paused. Existing broker,
real broker, no-network, no-credentials, no-endpoint, no-order-placement, and
no-automatic-trading gates remain retained and must not be deleted, weakened, or
skipped.

## Future Implementation Requirements

A future repository implementation PR must reference this document and
`docs/181_shell_accounting_manual_entry_repository_implementation_post_migration_authorization_test_plan.md`.
It must prove the TASK-190 migration fields are used, keep DataService write
wiring in a separate TASK, and preserve all broker / network / credentials /
real-order safety gates.

After TASK-192, future manual cash movement repository write remains a separate
TASK. Future DataService action write implementation also remains a separate
TASK and must not scatter SQL in action handlers.
