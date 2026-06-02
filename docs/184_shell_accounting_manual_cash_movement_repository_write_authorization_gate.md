# ShellAccounting Manual Cash Movement Repository Write Authorization Gate

## Document Purpose

TASK-194 is a manual cash movement repository write implementation
authorization gate-only task. This task does not implement manual cash movement
repository write, does not modify the TASK-192 manual transaction repository,
does not modify DataAccess production implementation, does not modify DataServiceActions.cpp / DataServiceActions.h / DataServiceActionRegistrar.cpp,
does not modify TASK-178 validation production code, and does not modify
`migrations/001_initial_schema.sql` or
`migrations/002_shell_accounting_manual_entry_schema.sql`.

This task does not write SQLite, does not execute runtime SQL, and does not write `cash_adjustment`, `trade_log`, `audit_log`, cash facts, cash ledger, or
ledger rows. It does not modify production QML, startup, Presenter, Controller,
AccountingEngine replay, StrategyEngine, or MarketEngine. It does not add
TradeDraft, trade suggestion, broker SDK, network, credentials, endpoint, real
order placement, broker order id storage, or automatic trading.

Current task does not implement manual cash movement repository write. Current
task does not modify TASK-192 manual transaction repository. Current task does
not modify DataServiceActions.cpp / DataServiceActions.h /
DataServiceActionRegistrar.cpp. Current task does not modify TASK-178
validation production code. Current task does not modify DataAccess production
implementation.

## Current State

TASK-190 completed `migrations/002_shell_accounting_manual_entry_schema.sql`.
TASK-192 implemented DataAccess-only manual transaction repository write for
manual BUY / SELL entries and writes only `trade_execution_group` and
`trade_log`. TASK-193 aligned the TASK-191 post-migration gate so that the
TASK-192 DataAccess-only repository is allowed while DataServiceActions runtime
write remains forbidden.

Manual cash movement repository write is still not implemented. DataService
runtime write wiring is still not implemented. `cash_adjustment`, audit, cash
ledger, replay, read model, UI, broker, network, credentials, endpoint, real
order placement, and automatic trading remain out of scope.

Broker sandbox new capability development remains paused. Existing broker,
real broker, no-network, no-credentials, and no-order-placement gates must stay
retained and must not be weakened, skipped, or deleted.

## Future Implementation Boundary

Future manual cash movement repository write implementation must be a separate
TASK. Future implementation may only live inside the DataAccess repository boundary.
It must not scatter SQL in DataService action handlers, and future
DataService action write wiring must be a separate TASK.

Future implementation must stay split from AccountingEngine replay, read model,
production QML, Presenter, Controller, and UI work. It must use the TASK-190 002 migration schema fields and must coordinate with TASK-192 manual
transaction repository write without modifying TASK-192 repository behavior.

Future implementation must preserve no broker, no network, no credentials, no endpoint, no real order placement, no broker order id, and no automatic trading.

## Cash Adjustment Mapping Boundary

Future repository implementation must use or explicitly map:

- `cash_adjustment.request_id`
- `cash_adjustment.idempotency_key`
- `cash_adjustment.occurred_at_utc`
- `cash_adjustment.source_memo_sanitized`
- `cash_adjustment.trade_log_uid`
- `cash_adjustment.amount_cents`
- `cash_adjustment.adjustment_type`
- account, portfolio, currency, and created_at_utc fields already present in
  the 001 schema

## Trade Log Cash Fact Mapping Boundary

Future repository implementation must explicitly decide whether it also writes
`trade_log` cash facts. If it writes those facts, it must use or explicitly map:

- `trade_log.request_id`
- `trade_log.idempotency_key`
- `trade_log.occurred_at_utc`
- `trade_log.cash_adjustment_uid`
- `trade_log.action_type`
- `trade_log.trade_source`
- `trade_log.manual_entry`
- `trade_log.amount_cents`
- `trade_log.net_cash_impact_cents`
- `trade_log.source_memo_sanitized`

Any dual write between `cash_adjustment` and `trade_log` must be atomic and must
not produce partial facts.

## Movement Type Policy

Future repository implementation must define Deposit mapping and Withdrawal mapping.
Adjustment support must be explicit; if Adjustment is not authorized,
the implementation must fail closed.

Future implementation must define amount sign policy, net cash impact policy,
currency constraints, account constraints, portfolio constraints,
request_id trace behavior, and idempotency_key duplicate behavior.

## Transaction / Rollback / Idempotency Policy

Future repository implementation must define:

- transaction boundary for `cash_adjustment` only or
  `cash_adjustment` + `trade_log` dual write
- rollback / failure atomicity
- idempotency behavior on duplicate idempotency_key
- request_id trace behavior
- unique index conflict handling
- safe error mapping without leaking raw SQL or raw payload
- no partial facts on failure

## Privacy / Sanitization Policy

Future repository implementation must define sourceMemo sanitization. It must
not persist raw credential, token, key, password, endpoint, broker payload,
raw SQL, raw manual entry payload, or internal exception stack. Sensitive memo
handling must be explicit through rejection or redaction policy.

## Forbidden Runtime Policy

TASK-194 does not authorize runtime SQL, SQLite runtime write, DataService
runtime write, cash facts write, cash ledger write, audit write, ledger write,
manual cash movement repository implementation, TradeDraft implementation,
trade suggestion implementation, broker SDK, network call, credentials,
endpoint, real order placement, broker order id persistence, reconciliation,
cancellation, correction, or automatic trading.

## Required Follow-Up

Future manual cash movement repository implementation must be separately
authorized. Future DataService action write implementation must be separately
authorized. Future replay, read model, UI integration, audit integration, and
broker work must remain separate authorization tasks.
