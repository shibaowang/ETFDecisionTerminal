# ShellAccounting Manual Transaction Repository Write Implementation

## Document Purpose

TASK-192 implements the ShellAccounting manual transaction repository write
implementation for manual BUY / SELL entries. The implementation is limited to
the DataAccess repository boundary and is exercised only by direct repository
tests with a temporary SQLite database.
This is the manual transaction repository write implementation scope.

This task does not implement manual cash movement write, does not modify
DataServiceActions, does not wire a runtime DataService write path, does not
modify production QML or startup, does not call AccountingEngine replay, does
not write `audit_log`, and does not call broker, network, credentials, or
endpoint code.
It does not modify DataServiceActions.
It does not write `audit_log`.
It does not write audit_log.
Broker sandbox new capability development remains paused.
broker sandbox new capability development remains paused.

## Scope

TASK-192 adds `ShellAccountingManualTransactionRepository` in DataAccess. The
repository accepts a structured manual transaction command and persists manual
BUY / SELL facts into the existing schema after applying
`migrations/001_initial_schema.sql` and
`migrations/002_shell_accounting_manual_entry_schema.sql`.

Allowed writes are limited to:

- `trade_execution_group`
- `trade_log`

The repository does not write:

- `cash_adjustment`
- `audit_log`
- cash facts / cash ledger
- TradeDraft tables
- broker order status
- any schema or migration file

## Input Policy

The command must provide:

- account id
- portfolio id
- instrument id and security code
- trade side, only `BUY` or `SELL`
- quantity units
- price amount minor
- gross amount minor
- fee amount minor
- tax amount minor
- occurred-at UTC timestamp
- source memo
- request id
- idempotency key

The repository validates required identifiers, positive quantity / price /
gross amount, non-negative fee / tax, side allowlist, occurred-at timestamp, and
sensitive memo policy before writing. Invalid input is rejected before any row is
written.

## Mapping Policy

Manual transaction writes map to:

- `trade_execution_group.action_type`
- `trade_execution_group.trade_source = MANUAL`
- `trade_execution_group.confirm_mode = MANUAL_ENTRY`
- `trade_execution_group.manual_entry = 1`
- `trade_log.action_type`
- `trade_log.trade_source = MANUAL`
- `trade_log.manual_entry = 1`
- `trade_log.request_id`
- `trade_log.idempotency_key`
- `trade_log.occurred_at_utc`
- `trade_log.tax_cents`
- `trade_log.source_memo_sanitized`
- `trade_log.price_1e6`
- `trade_log.quantity_1e6`
- `trade_log.amount_cents`
- `trade_log.fee_cents`
- `trade_log.net_cash_impact_cents`

BUY net cash impact is negative gross plus fee plus tax. SELL net cash impact is
gross minus fee minus tax. `cash_adjustment_uid` remains null for this manual
transaction path.

## Idempotency And Duplicate Policy

A non-empty `idempotencyKey` is matched against
`trade_log.idempotency_key`. If an existing row is found, the repository returns
a stable `IDEMPOTENT_REPLAY` / duplicate result and does not insert another
`trade_log` row.

An empty idempotency key is stored as null and does not use the TASK-190 unique
idempotency index. Request id is used for traceability and not as the unique
dedupe key.

## Transaction / Rollback Policy

Manual transaction persistence writes `trade_execution_group` and `trade_log`
inside one SQLite transaction. Any validation failure, missing referenced row,
unique conflict, or SQLite failure returns an explicit error and leaves no
partial facts. The implementation does not use scattered SQL in DataService
actions.

## Sanitization And Privacy

The repository stores only `source_memo_sanitized`. Newlines and tabs are
normalized, long memo text is bounded, and sensitive memo tokens are rejected.
The repository does not store raw SQL, credentials, tokens, passwords,
endpoints, raw broker payloads, or internal exception stacks.

## Boundaries Kept

- manual cash movement repository write remains disabled / not implemented
- TASK-182 DataService validation wiring remains `writeImplemented=false`
- DataServiceActions.cpp / DataServiceActions.h / DataServiceActionRegistrar.cpp are not changed
- production QML / startup / Presenter / Controller are not changed
- AccountingEngine replay is not changed or called
- StrategyEngine / MarketEngine implementation is not added
- TradeDraft / trade suggestion implementation is not added
- broker SDK, network, credentials, endpoint, real order placement, and automatic trading are not added

## Tests

TASK-192 adds direct repository tests under
`tests/ShellAccountingManualTransactionRepositoryWriteImplementation/`. The
tests apply migrations 001 + 002 to a temporary SQLite database, seed minimal
account / portfolio / instrument rows, call the DataAccess repository directly,
and verify BUY / SELL persistence, TASK-190 fields, idempotency, duplicate
handling, rollback, sanitized memo, no cash movement write, no audit write, and
all forbidden broker / network / trading boundaries.

## Future Scope

Manual cash movement repository write, DataService action write wiring,
AccountingEngine replay / read model integration, production UI integration,
audit write integration, broker integration, and automatic trading require
separate future TASK authorization. Broker sandbox new capability development
remains paused and existing broker gates remain retained.

TASK-194 adds the manual cash movement repository write implementation
authorization gate. It does not modify this TASK-192 manual transaction
repository behavior. Future cash movement repository implementation must remain
a separate TASK and must coordinate with this repository without changing its
manual BUY / SELL semantics.
