# ShellAccounting Manual Entry Readback Mapping Authorization Gate

## Document Purpose

TASK-206 is a readback mapping authorization gate-only task for the
ShellAccounting manual entry MVP. It does not implement readback mapping,
does not modify production code, does not modify DataServiceActions,
does not modify repositories, does not modify migrations, and
does not add runtime SQL / SQLite read/write behavior.

TASK-206 authorizes only the future implementation boundary for mapping manual
entry facts into existing DataService readback actions. Future implementation
must be a separate TASK, must stay inside the DataService readback boundary,
and must not rely on QML calculations or UI raw payloads.

This task does not implement AccountingEngine replay, derived snapshot rebuild,
derived table refresh, audit / ledger write, backup/export, broker SDK,
network calls, credentials, endpoint configuration, real order placement, real
broker order ids, or automatic trading.
Broker sandbox new capability development remains paused.

## Current Prerequisites

The current authorized chain is:

- TASK-192 implemented the DataAccess-only manual transaction repository write
  for BUY / SELL facts into `trade_execution_group` and `trade_log`.
- TASK-196 implemented the DataAccess-only manual cash movement repository
  dual-write for Deposit / Withdrawal facts into `trade_log` and
  `cash_adjustment`.
- TASK-198 implemented DataService runtime write wiring through validation and
  authorized repositories.
- TASK-200 implemented minimum QML / Presenter manual entry UI wiring through
  the existing boundary.
- TASK-202 implemented the post-write readback refresh boundary.
- TASK-204 implemented manual entry MVP runtime E2E acceptance tests.
- TASK-205 completed the readback / replay adequacy review and concluded that
  manual entry MVP facts are partially ready for future readback mapping, while
  direct replay / daily-use PnL implementation is not ready.

## position.list Readback Mapping Policy

Future `position.list` readback mapping may consume persisted manual
transaction facts from `trade_log` and related `trade_execution_group` facts.
The future implementation must address:

- BUY aggregation from persisted manual BUY facts.
- SELL reduction from persisted manual SELL facts.
- instrument mapping from persisted fact fields to safe readback response
  fields.
- account / portfolio mapping without relying on UI raw payloads.
- cost basis policy for manual entry positions.
- fee / tax treatment in position cost and realized effects.
- partial sell policy.
- sell exceeds position detection.
- stale / invalid facts handling.
- idempotency trace visibility policy in sanitized responses.
- no QML calculation; QML must not compute positions, cost basis, or sell
  reduction.

## cash.summary Readback Mapping Policy

Future `cash.summary` readback mapping may consume manual transaction facts and
manual cash movement facts from `trade_log` and `cash_adjustment`. The future
implementation must address:

- BUY cash outflow.
- SELL cash inflow.
- Deposit cash inflow.
- Withdrawal cash outflow.
- fee / tax cash treatment.
- `cash_adjustment.trade_log_id` linkage.
- currency aggregation policy.
- principal base policy.
- principal base / capital flow policy.
- negative cash / insufficient cash policy.
- stale / invalid facts handling.
- no QML calculation; QML must not compute cash balance or capital flow.

## portfolio.pnl.summary Readback Mapping Policy

Future `portfolio.pnl.summary` readback mapping may consume manual entries only
after explicit policy is documented and tested in a separate implementation
TASK. The future implementation must address:

- realized PnL.
- unrealized PnL.
- fee / tax treatment.
- cash movement treatment.
- principal flow.
- market price dependency.
- stale price / missing price policy.
- multi-instrument / multi-account handling.
- stale / invalid facts handling.
- no QML calculation; QML must not compute PnL.

## Future Implementation Acceptance

Future readback mapping implementation acceptance must include:

- temporary SQLite fixtures.
- BUY / SELL fixtures.
- Deposit / Withdrawal fixtures.
- duplicate idempotency.
- invalid no-write.
- response sanitization.
- no direct UI DB access.
- no replay unless separately authorized.
- no broker.
- no network / credentials / endpoint access.
- no real order placement.
- no automatic trading.

The future implementation must prove that `position.list`, `cash.summary`, and
`portfolio.pnl.summary` responses are derived from persisted manual entry facts
through authorized DataService readback boundaries. It must not add generic SQL
escape hatches, runtime UI calculations, or direct QML / Presenter / Controller
database access.

## Formal Authorization Conclusion

TASK-206 authorizes future readback mapping implementation only after a separate implementation TASK.
TASK-207 has now implemented the authorized DataService readback mapping for
`position.list`, `cash.summary`, and safe partial `portfolio.pnl.summary`.
TASK-206 does not authorize readback implementation in this task,
does not authorize replay, does not authorize audit / ledger, does not
authorize backup/export, and does not authorize broker or real order
capability.

TASK-207 does not implement AccountingEngine replay, audit / ledger,
backup/export, packaging, broker sandbox, real broker order, strategy
execution, or automatic trading.

Future replay, audit, ledger, backup/export, packaging, broker sandbox, real
broker order, strategy execution, and automatic trading work must remain
separately authorized.
