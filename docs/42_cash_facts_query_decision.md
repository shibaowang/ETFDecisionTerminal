# Cash Facts Query Decision

## Document Purpose

This document decides whether the project is ready to implement a cash facts
query. It is a decision and readiness document only. It does not implement
code, does not access SQLite, and does not modify schema.

## Decision Summary

The current recommendation is not to directly implement the cash facts query
until schema review confirms an auditable facts source.

Cash facts must not be inferred from `cash_snapshot`, `portfolio_summary`, or
other derived summaries. If the current schema lacks a cash facts source, the
next step should be a schema or cash ledger design task, not a fake query.

## Optional Routes

### Route A: Existing Cash Facts Source

Use this route only if the current schema already has auditable initial cash,
deposit, withdraw, and adjustment facts.

The next task may implement a read-only cash facts query skeleton, but it must
use the no-write harness and forbidden SQL scanner.

### Route B: Schema Is Insufficient

If schema review finds no reliable cash facts source, add cash facts source
design docs first. Any future migration must be a separate task. Replay and
query code must not fabricate facts to fill the gap.

### Route C: Use Only trade_log Cash Impact

`trade_log.net_cash_impact_cents` can support transaction cash impact review.
It cannot replace initial cash, deposit, or withdraw facts and cannot support a
complete `cash.summary` contract by itself.

### Route D: Continue DataService Guard

Keep `cash.summary` as a guard returning `CASH_SUMMARY_NOT_AVAILABLE` until the
facts source is explicit and auditable.

## Go / No-Go Checklist

- [ ] CashFactDto source has been confirmed.
- [ ] Initial cash source has been confirmed.
- [ ] Deposit / withdraw source has been confirmed.
- [ ] Adjustment source has been confirmed.
- [ ] `cash_snapshot` is not used as a facts source.
- [ ] `portfolio_summary` is not used as a facts source.
- [ ] No-write harness is available.
- [ ] `SELECT`-only scanner is available.
- [ ] Schema is not modified by the query task.
- [ ] Next task boundary has been approved.

## Recommendation

If the current schema is insufficient, TASK-089 should continue with cash facts
source schema review docs or a `cash.summary` guard.

If the schema is sufficient, TASK-089 may implement a read-only cash facts query
skeleton.

Any implementation must reference `docs/41_cash_facts_source_boundary.md` and
`docs/42_cash_facts_query_decision.md`.
