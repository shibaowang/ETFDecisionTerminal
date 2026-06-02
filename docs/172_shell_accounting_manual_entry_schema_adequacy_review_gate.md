# TASK-187 ShellAccounting Manual Entry Schema Adequacy Review Gate

## Document Purpose

TASK-187 is a manual entry schema adequacy review gate only. It reviews whether
the existing schema can support a future manual entry repository implementation
for manual buy / sell records and manual deposit / withdraw records.

This task does not modify schema, does not add a migration, does not implement
a repository write path, does not write a database, does not execute SQL, does
not modify DataServiceActions, does not modify the TASK-185 repository scaffold,
does not modify TASK-178 validation production code, and does not modify QML or
startup code.
TASK-187 does not implement a repository write path.
TASK-187 does not modify DataServiceActions.

The source of truth for this review is `migrations/001_initial_schema.sql`.
Any schema descriptions in other documents are secondary and must be reconciled
against `migrations/001_initial_schema.sql`.
All secondary schema descriptions must be reconciled against `migrations/001_initial_schema.sql`.

## Current State

TASK-182 provides payload parsing and validation wiring for
`accounting.manual_transaction.create` and
`accounting.manual_cash_movement.create`. Valid payloads still return
validation accepted plus `writeImplemented=false`.

TASK-185 provides a disabled-only DataAccess repository scaffold. The scaffold
continues to return disabled / write-not-implemented and does not write SQLite,
`trade_log`, cash facts, audit, or ledger records.

TASK-186 adds the future repository implementation authorization gate. It
requires schema adequacy confirmation before any manual entry repository write
implementation can be authorized.

TASK-187 only reviews existing schema adequacy. It does not enable writing.

## Schema Review Source

The review object is `migrations/001_initial_schema.sql`.

The following existing tables are relevant:

- `trade_execution_group`
- `trade_log`
- `cash_adjustment`
- `audit_log`
- `account`
- `portfolio`
- `instrument`

This document does not infer adequacy from names alone. Every adequacy claim
below cites table / field / constraint evidence from
`migrations/001_initial_schema.sql`.

## Manual Transaction Schema Adequacy

Existing schema has partial support for future manual buy / sell facts:

- `trade_execution_group.account_id` and `trade_log.account_id` can represent
  `accountId`.
- `trade_execution_group.portfolio_id` and `trade_log.portfolio_id` can
  represent `portfolioId`.
- `trade_log.actual_code` can represent `securityCode` after repository-level
  mapping to `instrument.code`.
- `trade_log.actual_instrument_id` can link the manual entry to
  `instrument.id`.
- `trade_execution_group.action_type` and `trade_log.action_type` allow `BUY`
  and `SELL`.
- `trade_execution_group.trade_source` and `trade_log.trade_source` allow
  `MANUAL`.
- `trade_execution_group.manual_entry` and `trade_log.manual_entry` can mark
  manual facts.
- `trade_log.quantity_1e6` can represent quantity after conversion from
  validated manual quantity units.
- `trade_log.price_1e6` can represent price after conversion from validated
  minor amount input.
- `trade_log.amount_cents` can represent gross amount.
- `trade_log.fee_cents` can represent fee.
- `trade_log.net_cash_impact_cents` can represent the computed cash effect.
- `trade_log.memo` and `trade_execution_group.manual_reason` can represent a
  sanitized `sourceMemo`.
- `trade_log.local_time`, `trade_log.trading_date`, and
  `trade_log.created_at_utc` can represent timing metadata, but the future
  implementation must define the exact mapping for `occurredAt`.
- `trade_log.uid` and `trade_execution_group.uid` provide unique row identity.

The current schema is not sufficient to authorize manual transaction write
implementation yet because these schema gaps must be resolved:

- There is no explicit `tax_cents` column for the TASK-178 `taxAmountMinor`
  input. A future task must either authorize a schema field or document a
  lossless existing-column mapping before writing.
- There is no explicit `idempotency_key` column or unique index for manual
  transaction requests.
- There is no explicit `request_id` column for manual transaction request
  tracing.
- There is no dedicated unique constraint tying manual transaction idempotency,
  account, portfolio, instrument, side, quantity, price, and occurred time
  together.
- `occurredAt` mapping is not fully explicit because the schema has
  `created_at_utc`, `local_time`, and `trading_date`, but no dedicated
  `occurred_at_utc` field.

Manual transaction schema adequacy outcome: partially adequate but blocked by
schema gaps. A future schema authorization TASK is required before any manual
transaction repository write implementation if these gaps are not resolved by
a separately approved mapping decision.

## Manual Cash Movement Schema Adequacy

Existing schema has partial support for future manual deposit / withdraw facts:

- `trade_execution_group.action_type` and `trade_log.action_type` allow
  `CASH_IN`, `CASH_OUT`, and `CASH_ADJUSTMENT`.
- `trade_execution_group.trade_source` and `trade_log.trade_source` allow
  `MANUAL`.
- `trade_execution_group.account_id`, `trade_log.account_id`, and
  `cash_adjustment.account_id` can represent `accountId`.
- `trade_execution_group.portfolio_id`, `trade_log.portfolio_id`, and
  `cash_adjustment.portfolio_id` can represent `portfolioId`.
- `trade_log.amount_cents`, `trade_log.net_cash_impact_cents`, and
  `cash_adjustment.amount_cents` can represent amount.
- `cash_adjustment.adjustment_type` allows `CASH_IN`, `CASH_OUT`, and
  `CASH_ADJUSTMENT`.
- `cash_adjustment.reason`, `cash_adjustment.external_reference`,
  `trade_log.memo`, and `trade_execution_group.manual_reason` can represent a
  sanitized `sourceMemo` or source reference.
- `trade_log.created_at_utc` and `cash_adjustment.created_at_utc` can
  represent timing metadata, but the future implementation must define the
  exact mapping for `occurredAt`.
- `trade_log.uid`, `trade_execution_group.uid`, and `cash_adjustment.uid`
  provide unique row identity.

The current schema is not sufficient to authorize manual cash movement write
implementation yet because these manual cash movement requests schema gaps
must be resolved:

- There is no explicit `idempotency_key` column or unique index for manual cash
  movement requests.
- There is no explicit `request_id` column for manual cash movement request tracing.
- There is no dedicated `occurred_at_utc` field for the business event time.
- The relationship between `trade_log` cash events and `cash_adjustment` must
  be defined transactionally before writing.
- A future task must define whether all manual cash movements require both a
  `trade_log` ledger fact and a `cash_adjustment` row.

Manual cash movement schema adequacy outcome: partially adequate but blocked
by schema gaps. A future schema authorization TASK is required before any
manual cash movement repository write implementation if these gaps are not
resolved by a separately approved mapping decision.

## Audit, Idempotency, Privacy, And Rollback Review

Existing schema has `audit_log`, including `entity_type`, `entity_id`,
`action`, `old_value_json`, `new_value_json`, `reason`, `operator`, and
`created_at_utc`. That table can support sanitized audit events in principle.

Audit policy schema review gaps remain:

- There is no explicit audit idempotency key.
- There is no explicit request id or trace id on `audit_log`.
- There is no schema-level redaction flag or payload classification.
- A future implementation must define sanitized payload shape before writing
  `old_value_json` or `new_value_json`.

Idempotency / duplicate handling schema review gaps remain:

- The manual entry payload fields include `idempotencyKey`, but
  `migrations/001_initial_schema.sql` does not contain an explicit
  `idempotency_key` column for manual transaction or manual cash movement
  writes.
- Existing `uid` uniqueness is row identity, not request idempotency.
- Existing indexes on `trade_log`, `trade_execution_group`, and `audit_log`
  do not provide manual entry duplicate request handling.

Privacy / memo sanitization schema review:

- Future writes must sanitize `sourceMemo` before it enters `memo`,
  `manual_reason`, `reason`, `external_reference`, or audit JSON fields.
- Future writes must not store raw SQL, raw manual entry payloads, full broker
  payloads, credentials, endpoints, tokens, keys, passwords, or internal stack
  traces.

Rollback review:

- Future writes must be atomic across every row they create.
- Future manual transaction writes must define transaction, rollback,
  idempotency, duplicate handling, audit, privacy, and failure atomicity before
  implementation.
- Future manual cash movement writes must define transaction, rollback,
  idempotency, duplicate handling, audit, privacy, and failure atomicity before
  implementation.

## Required Follow-Up

Because this review found schema gaps, a future schema authorization TASK is
required before enabling repository write implementation unless a later
authorized review documents a complete lossless mapping using existing fields.

That future schema authorization TASK must not directly edit `migrations/001_initial_schema.sql`;
it must add an explicit migration or otherwise follow the repository migration
policy in a separately authorized task.

Future repository implementation must also be separately authorized after
schema adequacy is resolved.

Future DataService action write implementation, replay integration, UI changes,
or manual entry read-model changes must be separately authorized.

## Boundary Policy

Future manual transaction write must remain DataService-only and
repository-boundary only. Action handlers must not grow scattered SQL.

Future manual cash movement write must remain DataService-only and
repository-boundary only. Action handlers must not grow scattered SQL.

TASK-187 does not change `DataServiceActions.cpp`, `DataServiceActions.h`, or
`DataServiceActionRegistrar.cpp`.

TASK-187 does not change TASK-178 validation production code.

TASK-187 does not change the TASK-185 repository scaffold.

TASK-187 does not modify AccountingEngine replay, StrategyEngine, MarketEngine,
TradeDraft, suggestion, broker, network, credentials, endpoint, real order, or
automatic trading code.

## Broker And Trading Policy

The first phase still does not connect a real broker, does not enable automatic
trading, and does not place real orders.

Broker sandbox new capability development remains paused. Pausing broker
sandbox work does not remove existing broker gates; the broker sandbox new capability development remains paused policy remains unchanged.

Existing broker disabled, fail-closed, no-real-order, no-network,
no-credentials, and no-order-placement gates must remain present and passing.
