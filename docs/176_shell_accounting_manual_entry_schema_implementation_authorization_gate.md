# ShellAccounting Manual Entry Schema Implementation Authorization Gate

## Document Purpose

TASK-189 is a manual entry schema implementation authorization gate-only task.
It defines the boundary for a future schema implementation task after TASK-187
identified manual entry schema gaps and TASK-188 documented the schema gap
authorization boundary.

TASK-189 does not modify schema. TASK-189 does not add a migration. TASK-189 does not add a schema file.
TASK-189 does not implement repository implementation. TASK-189 does not write a database. TASK-189 does not execute SQL. TASK-189 does not modify DataServiceActions. TASK-189 does not modify the TASK-185 repository scaffold. TASK-189 does not modify TASK-178 validation production code. TASK-189 only defines future schema implementation authorization boundary.

The source of truth remains `migrations/001_initial_schema.sql`. A future
schema implementation must add an independent migration file and must not
directly edit `migrations/001_initial_schema.sql`.

Future schema implementation must not directly edit `migrations/001_initial_schema.sql`.

## Current State

TASK-187 completed schema adequacy review. TASK-188 established the manual
entry schema gap authorization boundary. Current manual entry write behavior
remains disabled:

- TASK-178 validation production code remains pure validation.
- TASK-182 DataService manual entry action wiring remains validation-only with
  `writeImplemented=false`.
- TASK-185 repository scaffold remains disabled and write-not-implemented.
- TASK-186 repository implementation authorization remains gate-only.
- TASK-187 schema adequacy review remains review-only.
- TASK-188 schema gap authorization remains gate-only.

No schema implementation, migration, schema file, repository implementation,
SQL execution, SQLite write, `trade_log` write, cash facts write, cash ledger
write, audit write, ledger write, TradeDraft, suggestion implementation, broker
SDK, network call, credentials, endpoint, real broker order id, real order
placement, or automatic trading is authorized by TASK-189.

## Future Schema Implementation Boundary

Future schema implementation must be a separately authorized TASK. The future
schema task must use an independent migration file and must not directly edit
`migrations/001_initial_schema.sql`.

Future schema implementation must declare:

- migration filename and migration id policy.
- forward migration policy.
- rollback / forward-fix policy, even if the current migration runner does not
  support automatic rollback.
- compatibility with existing data.
- compatibility with existing full CTest and the read-only demo.
- compatibility with AccountingEngine replay gates.
- compatibility with broker disabled, fail-closed, no-real-order, no-network,
  no-credentials, and no-order-placement gates.
- idempotency and duplicate handling policy.
- the split from repository implementation and DataService write
  implementation.

The future schema task must not include repository implementation. The future
schema task must not include DataService write implementation. The future
schema task must not include QML / UI work. The future schema task must not
include broker, network, credentials, endpoint, real order, or automatic
trading work.

Future repository implementation may continue only after the schema gaps are
resolved or after a separately authorized lossless mapping is approved.

Future repository implementation may continue only after the schema gaps are resolved.
Future schema implementation must keep a split from repository implementation and DataService write implementation.

## Future Migration Implementation Readiness Checklist

This checklist is readiness-only and is not executed by TASK-189:

- migration filename / id policy.
- forward migration policy.
- rollback / forward-fix policy.
- compatibility with existing data.
- compatibility with existing CTest and read-only demo.
- no direct edit to `001_initial_schema.sql`.
- no repository implementation in schema task.
- no DataService write implementation in schema task.
- no QML / UI in schema task.
- no broker / network / credentials / endpoint in schema task.
- no real order / automatic trading in schema task.

## Future Migration Candidate Categories

The future schema implementation must resolve or explicitly approve lossless
mapping for these candidate categories:

- manual transaction trace / idempotency fields.
- manual transaction `request_id`.
- manual transaction `idempotency_key`.
- manual transaction occurredAt field or approved lossless mapping, such as
  `occurred_at_utc`.
- manual transaction tax amount support or approved lossless mapping, such as
  `tax_cents`.
- manual transaction duplicate handling constraints.
- manual cash movement trace / idempotency fields.
- manual cash movement `request_id`.
- manual cash movement `idempotency_key`.
- manual cash movement occurredAt field or approved lossless mapping, such as
  `occurred_at_utc`.
- manual cash movement type mapping support.
- cash adjustment / trade_log linkage support.
- audit trace / payload classification / redaction support.
- audit request trace and audit idempotency.
- failure atomicity / rollback policy support.
- sanitized memo / source reference support.

## Privacy And Payload Policy

Future schema implementation must not store raw SQL, raw manual entry payload,
raw trade payload, credentials, tokens, keys, passwords, endpoints, broker
payloads, or internal exception stacks. Any memo, source memo, request trace,
or audit-related schema design must include sanitized payload policy and
redaction policy.

Future schema implementation must not store broker payloads.

## Forbidden Current-Scope Changes

TASK-189 does not authorize:

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
an independent migration file, declare a migration id, include rollback or
forward-fix policy, and must not directly edit
`migrations/001_initial_schema.sql`.

Future repository implementation, DataService action write implementation,
AccountingEngine replay integration, production UI work, TradeDraft work,
broker work, strategy work, automatic trading, and real order placement must
each be separately authorized after this gate.
