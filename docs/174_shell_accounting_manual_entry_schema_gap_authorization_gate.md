# ShellAccounting Manual Entry Schema Gap Authorization Gate

## Document Purpose

TASK-188 is a manual entry schema gap authorization gate-only task. It defines
the authorization boundary for future manual transaction and manual cash
movement schema gap resolution after TASK-187 found that the current schema is
only partially adequate.

TASK-188 does not modify schema. TASK-188 does not add a migration. TASK-188 does not add a schema file.
TASK-188 does not implement repository implementation. TASK-188 does not write a database. TASK-188 does not execute SQL. TASK-188 does not modify DataServiceActions. TASK-188 does not modify the TASK-185 repository scaffold. TASK-188 does not modify TASK-178 validation production code. TASK-188 only defines future schema gap resolution boundary.

The source of truth remains `migrations/001_initial_schema.sql`. All secondary
schema descriptions must be reconciled against `migrations/001_initial_schema.sql`.

## Current State

TASK-187 completed a schema adequacy review against
`migrations/001_initial_schema.sql`. That review found partial schema support
for future manual buy / sell records and manual deposit / withdraw records, but
it also found unresolved schema gaps.

Current manual entry production code remains validation-only and disabled for
write persistence:

- TASK-178 validation production code remains pure validation.
- TASK-182 DataService manual entry action wiring remains validation-only with
  `writeImplemented=false`.
- TASK-185 repository scaffold remains disabled and write-not-implemented.
- TASK-186 repository implementation authorization remains gate-only.
- TASK-187 schema adequacy review remains review-only.

No repository implementation, SQL execution, SQLite write, `trade_log` write,
cash facts write, cash ledger write, audit write, ledger write, TradeDraft,
suggestion implementation, broker SDK, network call, credentials, endpoint,
real broker order id, real order placement, or automatic trading is authorized
by TASK-188.

## Future Schema Gap Resolution Boundary

Future schema gap resolution must be separately authorized before manual
transaction repository implementation or manual cash movement repository
implementation can proceed. Future schema implementation must add an independent migration file and must not directly edit
`migrations/001_initial_schema.sql`.

Future schema implementation must declare:

- migration id.
- rollback policy.
- idempotency and duplicate handling policy.
- compatibility with existing read-only demo behavior.
- compatibility with AccountingEngine replay gates.
- compatibility with broker disabled, no-real-order, no-network,
  no-credentials, and no-order-placement gates.
- the split between schema implementation and repository implementation.

Future repository implementation may continue only after schema gaps are resolved or after a separately authorized lossless mapping is approved. Schema implementation, repository implementation, DataService action write implementation, replay integration, and UI integration must remain separate authorization steps.

## Manual Transaction Candidate Schema Design

This section lists future candidate design items only. TASK-188 does not
implement them.

Manual transaction request trace / idempotency candidates:

- `request_id` for request trace.
- `idempotency_key` for idempotent writes.
- `occurred_at_utc` for business event time.
- `tax_cents` or equivalent lossless tax amount field for `taxAmountMinor`.
- `source_memo_sanitized` or equivalent sanitized memo field.
- manual transaction duplicate handling constraint.
- transaction boundary between manual transaction write and audit write.
- privacy boundary for `memo` and `sourceMemo`.

Future schema design must decide whether existing `created_at_utc`,
`trading_date`, `local_time`, `memo`, `manual_reason`, `uid`, or other fields
can be used through a separately authorized lossless mapping. If not, an
independent migration must add the missing support.

## Manual Cash Movement Candidate Schema Design

This section lists future candidate design items only. TASK-188 does not
implement them.

Manual cash movement request trace / idempotency candidates:

- `request_id` for request trace.
- `idempotency_key` for idempotent writes.
- `occurred_at_utc` for business event time.
- `movement_type` stable mapping for deposit, withdraw, and adjustment.
- `cash_adjustment` / `trade_log` transaction linkage.
- a decision on whether all manual cash movements must write both `trade_log`
  and `cash_adjustment`.
- cash movement duplicate handling constraint.
- transaction boundary between cash movement write and audit write.
- `source_memo_sanitized` or equivalent sanitized memo field.

Future schema design must define the relationship between `trade_log` cash facts and `cash_adjustment` before repository write implementation can proceed.

## Audit / Rollback / Privacy Candidate Schema Design

This section lists future candidate design items only. TASK-188 does not
implement them.

Audit / privacy candidates:

- audit request trace.
- audit idempotency.
- payload classification.
- redaction status.
- sanitized payload policy.
- failure atomicity.
- partial write rollback.
- sanitized memo and source reference policy.

Future schema design must ensure logs and persisted payloads do not store raw SQL, complete raw trade payloads, full sensitive memo payloads, credentials,
endpoints, tokens, passwords, internal exception stacks, or broker payloads.

## Forbidden Current-Scope Changes

TASK-188 does not authorize:

- modifying `migrations/001_initial_schema.sql`.
- adding a migration.
- adding a schema file.
- adding tables, columns, indexes, or constraints.
- modifying production QML.
- modifying production startup.
- modifying Presenter / Controller behavior.
- modifying `DataServiceActions.cpp`.
- modifying `DataServiceActions.h`.
- modifying `DataServiceActionRegistrar.cpp`.
- modifying TASK-178 validation header/source.
- modifying TASK-185 repository scaffold header/source.
- adding repository implementation.
- adding SQL.
- executing SQL.
- adding SQLite write.
- adding SQL INSERT / UPDATE / DELETE / REPLACE.
- writing `trade_log`.
- writing cash facts or cash ledger.
- writing audit or ledger.
- adding persistent ids such as `tradeLogId`, `cashFactId`, `auditLogId`, or
  `ledgerId`.
- modifying AccountingEngine production replay.
- modifying StrategyEngine or MarketEngine.
- adding UI.
- adding TradeDraft or suggestion implementation.
- adding broker SDK.
- adding network calls.
- adding credentials or endpoints.
- real order placement.
- generating or storing a real broker order id.
- adding reconciliation, cancellation, or correction.
- adding automatic trading.

## Broker And Trading Policy

The first phase still does not connect a real broker, does not enable automatic
trading, and does not place real orders.

Broker sandbox new capability development remains paused. Pausing broker sandbox work does not remove existing broker gates. Existing broker disabled,
fail-closed, no-real-order, no-network, no-credentials, and no-order-placement
gates must remain present and passing.

## Required Follow-Up

Future schema implementation must be a separately authorized TASK. It must add
an independent migration file and must not directly edit
`migrations/001_initial_schema.sql`.

TASK-189 adds the manual entry schema implementation authorization gate. It is
still gate-only: it does not modify schema, add a migration, add a schema file,
implement repository writes, execute SQL, write SQLite, or change
DataServiceActions. Schema implementation remains unimplemented until a later
separately authorized migration task.

Future repository implementation, DataService action write implementation,
AccountingEngine replay integration, production UI work, TradeDraft work,
broker work, strategy work, automatic trading, and real order placement must
each be separately authorized after this gate.
