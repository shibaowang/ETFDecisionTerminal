# Cash Facts Source Boundary

## Document Purpose

This document defines the future facts-source boundary for `CashFactDto`.

This document does not implement a cash facts query, does not modify schema,
does not add a DataAccess repository, and does not add a DataService action. It
is a review boundary for later `cash.summary` and replay cash input work.

## Current Status

- The `trade_log` read-only trade facts query skeleton exists.
- The cash facts read-only query is not implemented.
- The `cash.summary` DataService action is not implemented.
- AccountingEngine has a DTO-only replay skeleton, but it is not connected to
  SQLite.
- DataService does not call AccountingEngine for accounting replay.
- Snapshot writes are not implemented.
- `cash_snapshot` and `portfolio_summary` are derived data.
- Known schema gap: current `trade_log.fee_cents` is `NOT NULL DEFAULT 0`, so a
  missing fee cannot be distinguished from an explicit zero fee at that layer;
  this is recorded in `docs/40_accounting_facts_source_mapping.md`.

## CashFactDto Target Fields

Target fields:

- `factId`
- `time`
- `accountId`
- `portfolioId`
- `action`
- `amountText`
- `currency`
- `note`

Supported actions:

- `INITIAL_CASH`
- `DEPOSIT`
- `WITHDRAW`
- `ADJUSTMENT`

`CashFactDto` is not `cash_snapshot`. `CashFactDto` is not
`portfolio_summary`. It should come from auditable facts.

## Allowed Facts Source Candidates

### A. Initial Cash Facts

Initial cash must come from an auditable source. Candidate sources include an
account initialization record, a future cash ledger, or explicit initial cash
facts.

If the current schema is insufficient, the future query path must return
`DATA_SOURCE_MISSING` or `REPLAY_NOT_AVAILABLE` instead of fabricating initial
cash.

### B. Deposit Facts

`DEPOSIT` should come from an explicit cash ledger or deposit event. It must not
be inferred from cash balance deltas.

### C. Withdraw Facts

`WITHDRAW` should come from an explicit cash ledger or withdraw event. It must
not be inferred from cash balance deltas.

### D. Adjustment Facts

`ADJUSTMENT` should come from an explicit adjustment or correction record. It
must be auditable and must not silently mutate historical facts.

### E. Trade Cash Impact

`trade_log.net_cash_impact_cents` can express cash impact caused by a trade.
This is part of the trade fact.

It is not equivalent to `INITIAL_CASH`, `DEPOSIT`, or `WITHDRAW` CashFactDto
inputs. Replay can consume both `TradeFactDto` and `CashFactDto`, but the query
layer must not aggregate them into cash balances.

## Forbidden Cash Facts Sources

These sources are forbidden as cash facts sources:

- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- UI display values
- dashboard derived values
- current market value reverse inference
- PnL reverse inference
- historical total assets delta reverse inference
- manual Excel display values, unless they have been imported into a formal
  fact table by a separate authorized task
- external temporary files, unless a later import task authorizes them

`cash_snapshot` is derived data; cash_snapshot is derived data. `portfolio_summary`
is derived data; portfolio_summary is derived data. Snapshot data is not a
facts source. Future implementation must not reverse-engineer cash facts from
snapshot data. It must not reverse-engineer cash facts from snapshot. Stale
snapshot data must not be used as cash facts.

## Current Schema Review Requirements

A later implementation task must first review:

- whether an account initial cash facts source exists;
- whether deposit, withdraw, and adjustment fact tables exist;
- whether a cash ledger exists;
- whether `trade_log` is sufficient for transaction cash impact;
- whether fee, currency, account, and portfolio fields are sufficient;
- whether a new schema or migration is required.

If a new schema is required, it must be a separate migration task. If the schema
is insufficient, the query layer must report the gap instead of fabricating
facts.

## Future Cash Facts Read-only Query Contract

Draft request:

`CashFactQueryRequest`

- `accountId`, optional
- `portfolioId`, optional
- `sourceFromTime`, optional
- `sourceToTime`, optional
- `action`, optional
- `limit`, optional
- `offset`, optional

Draft row:

`CashFactRow`

- `factId`
- `time`
- `accountId`
- `portfolioId`
- `action`
- `amountText`
- `currency`
- `note`
- `source`

Future query requirements:

- only `SELECT` SQL;
- support time filtering;
- support optional `accountId` and `portfolioId` filtering;
- stable ordering;
- parameter binding for untrusted input;
- no database writes;
- no cash balance calculation;
- no replay calculation.

## Failure Behavior

Expected domain failures:

- `DATA_SOURCE_MISSING`
- `CASH_FACTS_SOURCE_UNAVAILABLE`
- `INVALID_TIME_RANGE`
- `INVALID_REQUEST`
- `DATA_VERSION_MISMATCH`
- `REPLAY_NOT_AVAILABLE`
- `SQLITE_READONLY_OPEN_FAILED`
- `SQLITE_QUERY_FAILED`

If the current schema is insufficient, future code should return
`DATA_SOURCE_MISSING` or `CASH_FACTS_SOURCE_UNAVAILABLE`. Failure paths must not
write. Invalid request paths must not write.

## Required Tests Before Implementation

A later implementation must add or extend:

- read-only open test;
- `SELECT`-only SQL scanner;
- forbidden SQL keyword scanner;
- protected table row-count before / after checks;
- missing source test;
- invalid request no-write test;
- empty cash facts test;
- initial cash fact mapping test;
- deposit / withdraw / adjustment mapping tests, if schema supports them;
- schema gap test;
- transport repeat test.

## Forbidden Work

The query layer must not:

- calculate cash balance;
- calculate position;
- calculate PnL;
- generate TradeDraft;
- write snapshots;
- write TradeLog rows;
- reverse-engineer facts from snapshots;
- expose cash facts directly to QML;
- use the test-only fixture loader as production DataAccess.
