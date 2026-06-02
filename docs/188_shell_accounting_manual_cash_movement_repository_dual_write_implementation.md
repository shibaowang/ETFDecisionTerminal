# ShellAccounting Manual Cash Movement Repository Dual-Write Implementation

## Document Purpose

TASK-196 implements ShellAccounting manual cash movement repository dual-write
for Deposit / Withdrawal records. This is the manual cash movement repository dual-write implementation.
The implementation is DataAccess-only and is covered by direct repository tests using a temporary SQLite database initialized with
`migrations/001_initial_schema.sql` and
`migrations/002_shell_accounting_manual_entry_schema.sql`.

## Scope

TASK-196 implements manual cash movement repository dual-write:

- write `trade_log` cash movement fact
- write `cash_adjustment`
- set `cash_adjustment.trade_log_id` to the inserted `trade_log.id`
- set `trade_log.cash_adjustment_uid` to the inserted cash adjustment uid
- set `cash_adjustment.trade_log_uid` to the inserted trade log uid
- complete both writes in one transaction
- rollback on any failure
- return direct-test repository ids / uids only from the repository result

The current schema does not support cash_adjustment-only writes because
`cash_adjustment.trade_log_id` is `NOT NULL` and references `trade_log(id)`.
TASK-196 therefore implements `trade_log + cash_adjustment` dual-write and does
not implement cash_adjustment-only write.

## DataAccess-Only Boundary

The repository is `ShellAccountingManualCashMovementRepository` in DataAccess.
It is not wired into `DataServiceActions`, `DataServiceActions.h`, or
`DataServiceActionRegistrar.cpp`. It is only called by direct repository tests.

TASK-196 does not modify the TASK-192 manual transaction repository. Manual
transaction BUY / SELL repository behavior remains owned by
`ShellAccountingManualTransactionRepository`.

## Write Policy

Deposit maps to `CASH_IN`; Withdrawal maps to `CASH_OUT`.

`trade_log.amount_cents` stores the absolute movement amount. The signed cash
impact is represented by `trade_log.net_cash_impact_cents`: Deposit is positive
and Withdrawal is negative.

`cash_adjustment.amount_cents` stores the signed cash movement amount: Deposit
is positive and Withdrawal is negative. `trade_log.trade_source` is `MANUAL`,
`trade_log.trade_intent_type` is `CASH_EVENT`, and `trade_log.manual_entry` is
set to `1`.

The repository writes request tracing, idempotency, occurred-at, and sanitized
memo fields on both `trade_log` and `cash_adjustment`.

## Idempotency / Duplicate Policy

Non-empty `idempotencyKey` is used for idempotent replay. A duplicate
idempotency key returns an idempotent duplicate result and does not insert a
second `trade_log` or `cash_adjustment` row.

Empty idempotency keys are not treated as duplicate keys and can create
independent rows without breaking the partial unique indexes added by TASK-190.

## Sanitization Policy

`sourceMemo` is sanitized before persistence. Newlines, carriage returns, and
tabs are normalized to spaces, and long memo text is capped. The repository
rejects obvious sensitive payload markers such as raw SQL, credentials, token,
password, secret, endpoint, and URL-like content.

The repository does not store raw SQL, raw credential payloads, broker payloads,
internal exception stacks, endpoint values, or secrets.

## Transaction / Rollback Policy

Both rows are written inside a single `TransactionRunner` transaction. A
reference failure, validation failure, idempotency lookup failure, trade_log
insert failure, cash_adjustment insert failure, or uniqueness conflict fails
closed and leaves no partial facts.

## Explicit Non-Goals

TASK-196 does not modify migrations, does not add a migration or schema file,
does not modify production QML or startup, does not modify Presenter /
Controller, does not modify DataServiceActions, does not write `audit_log`, does
not write ledger rows, does not call AccountingEngine replay, does not create
TradeDraft or trade suggestions, does not call broker SDK, does not use network,
credentials, or endpoints, does not place real orders, and does not add
automatic trading.
TASK-196 does not write audit_log, does not write ledger, does not call AccountingEngine replay, and does not call broker.

Broker sandbox new capability development remains paused. Existing broker,
real-broker, no-network, no-credentials, no-order-placement, and
no-automatic-trading gates remain retained and required.

## Tests

TASK-196 adds
`tests/ShellAccountingManualCashMovementRepositoryDualWriteImplementation/`.
The test suite covers docs/index registration, schema immutability, DataAccess
boundary, direct SQLite 001+002 setup, Deposit / Withdrawal dual-write,
linkage, request/idempotency fields, amount sign policy, duplicate handling,
rollback atomicity, memo sanitization, and no broker / no network / no
credentials / no real order / no automatic trading boundaries.

## Future Work

Future DataService action write wiring must be separately authorized. Future
replay, read-model, UI, audit write, ledger policy, broker, reconciliation,
cancellation, correction, and automatic trading work must also remain separate
TASKs.
