# ShellAccounting Manual Entry Readback Replay Adequacy Review Gate

## Document Purpose

TASK-205 is a readback / replay adequacy review gate-only task for the
ShellAccounting manual entry MVP. It does not implement readback / replay,
does not modify production code, does not write any database table, and does
not add runtime SQL / SQLite write behavior.

This task establishes a review conclusion and gap matrix for whether manual
entry facts written by the local assistant decision MVP are adequate for
readback, future AccountingEngine replay, and daily-use accounting pages. It
does not modify production QML, startup, Presenter, Controller, ShellServices
adapter / port code, DataServiceActions, DataServiceActionRegistrar, DataAccess
repositories, migrations, AccountingEngine production code, StrategyEngine,
MarketEngine, broker code, broker SDK, network, credentials, endpoints,
real order placement, real broker order ids, or automatic trading.

Broker sandbox new capability development remains paused. Existing broker,
real broker, no-network, no-credentials, and no-order-placement gates must
remain present and passing.

## Current Implemented Chain

The current implemented chain is:

- TASK-192 implemented the DataAccess-only manual transaction repository write
  for manual BUY / SELL facts into `trade_execution_group` and `trade_log`.
- TASK-196 implemented the DataAccess-only manual cash movement repository
  dual-write for Deposit / Withdrawal facts into `trade_log` and
  `cash_adjustment`.
- TASK-198 implemented DataService runtime write wiring through validation and
  the authorized repositories.
- TASK-200 implemented the minimum production QML / Presenter / Controller /
  ShellServices manual entry UI wiring through DataService action boundaries.
- TASK-202 implemented the post-write readback refresh boundary through existing
  read actions after successful manual entry writes.
- TASK-204 implemented runtime E2E acceptance tests with temporary SQLite
  fixtures, synthetic manual BUY and Deposit payloads, duplicate idempotency
  checks, invalid no-write checks, sanitized response checks, and TASK-202
  refresh boundary checks.

## Manual Transaction Fact Adequacy

Manual transaction facts currently write to `trade_execution_group` and
`trade_log`. The write side records manual BUY and SELL concepts, quantity,
price, fee, tax, net cash impact, instrument mapping, account / portfolio
mapping, and idempotency / request trace fields.

Adequacy review:

- BUY facts are adequate for a future `position.list` readback implementation
  if the read query maps `trade_log` quantity, price, fee, tax, side, account,
  portfolio, and instrument fields consistently.
- SELL facts are structurally present, but daily-use replay adequacy remains
  partial until future tests cover realized PnL, partial sell, sell exceeds
  position detection, fee treatment, tax treatment, and cash impact semantics.
- `trade_execution_group` can support grouping and request trace, but future
  readback implementation must prove how group ids are exposed or hidden in
  safe responses.
- `trade_log` is the factual ledger source for manual transaction facts. Future
  AccountingEngine replay must explicitly consume the manual trade source,
  manual flags, request id, idempotency key, occurred time, source memo, fee,
  tax, quantity, and net cash impact without relying on UI raw payloads.
- Existing TASK-204 acceptance proves runtime persistence and duplicate
  idempotency for one BUY fixture, but does not prove full daily-use readiness
  for SELL, realized PnL, unrealized PnL, multi-instrument, multi-account,
  multi-currency, stale instrument, or failed-reference scenarios.

## Manual Cash Movement Fact Adequacy

Manual cash movement facts currently write to `trade_log` and `cash_adjustment`.
The write side records Deposit and Withdrawal concepts,
`cash_adjustment.trade_log_id`, amount sign, currency,
account / portfolio mapping, principal base / cash balance intent, and
idempotency / request trace fields.
For static review, these same concepts are also recorded as deposit and
withdrawal fact categories.

Adequacy review:

- Deposit facts are adequate for a future `cash.summary` readback
  implementation if the read query joins or reconciles `trade_log` and
  `cash_adjustment` consistently.
- Withdrawal facts are structurally present, but daily-use replay adequacy
  remains partial until future tests cover negative amount treatment, cash
  balance effects, principal base treatment, withdrawal insufficient-cash
  policy, and duplicate idempotency behavior.
- `cash_adjustment.trade_log_id` provides traceability from the cash movement
  detail row back to the factual `trade_log` row. Future readback must prove the
  link is required, stable, and not exposed as sensitive raw internals.
- Existing TASK-204 acceptance proves runtime persistence and duplicate
  idempotency for one Deposit fixture, but does not prove full daily-use
  readiness for Withdrawal, multi-currency cash, principal flow reporting,
  portfolio cash aggregation, or export / backup consistency.

## Existing DataService Read Actions Review

The current read actions are:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

Review status:

- `position.list` has an existing read boundary and is a candidate consumer of
  manual transaction facts, but TASK-205 does not prove it already consumes all
  manual BUY / SELL fields correctly.
- `cash.summary` has an existing read boundary and is a candidate consumer of
  manual cash movement facts, but TASK-205 does not prove it already consumes
  `cash_adjustment` and `trade_log` dual-write facts correctly.
- `portfolio.pnl.summary` has an existing read boundary, but manual entry PnL
  adequacy is not ready until fee / tax treatment, realized / unrealized PnL,
  principal movement treatment, and stale fact behavior are reviewed with
  fixture coverage.
- `base_position.summary` and `sniper_pool.summary` may show stale, derived, or
  unavailable state for manual entries until future readback mapping proves
  their dependency on manual ledger facts.
- Future work may require new DataAccess read query support, AccountingEngine
  replay updates, derived snapshot refresh, and fixture expansion. Each must be
  separately authorized.

## Readback / Replay Gap Matrix

| Area | Review Status | Gap |
| --- | --- | --- |
| position.list readiness | Partially ready | Manual BUY persistence is proven, but SELL, partial sell, fee / tax, and position replay mapping need future tests. |
| cash.summary readiness | Partially ready | Deposit persistence is proven, but Withdrawal, amount sign, principal base, and cash balance aggregation need future tests. |
| portfolio.pnl.summary readiness | Not ready | Realized / unrealized PnL, fee / tax treatment, capital flow treatment, and stale fact handling are not formally proven. |
| AccountingEngine replay readiness | Not ready | Manual transaction and manual cash movement facts need explicit replay mapping and fixture coverage. |
| read query readiness | Partially ready | Existing actions exist, but manual entry consumption by read queries is not fully proven. |
| UI refresh readiness | Partially ready | TASK-202 refresh boundary exists, but daily-use content correctness after manual facts is not fully proven. |
| fixture/test readiness | Partially ready | TASK-204 covers BUY and Deposit runtime acceptance; SELL, Withdrawal, PnL, and replay fixtures remain gaps. |
| audit/ledger readiness | Not ready for new work | TASK-205 does not authorize audit or ledger writes; future audit / ledger policy must be a separate TASK. |
| backup/export readiness | Not ready | Packaging, backup, export, and restore acceptance for manual entry facts need a separate TASK. |
| schema gap | No new schema change authorized | Existing schema may be adequate for persistence, but readback/replay field sufficiency still needs implementation proof. |
| repository mapping gap | Partially ready | Write mapping exists, but readback mapping from persisted facts remains unproven. |
| DataService readback gap | Partially ready | Read boundaries exist; manual fact consumption and safe response mapping remain future work. |
| replay gap | Not ready | No new AccountingEngine replay behavior is implemented or authorized by TASK-205. |
| UI refresh gap | Partially ready | UI can trigger existing refresh boundary; it must not calculate accounting facts. |
| fixture/test gap | Partially ready | More fixtures are needed for daily-use confidence. |
| audit/ledger gap | Not ready | No audit / ledger expansion is authorized. |
| backup/export gap | Not ready | No packaging, backup, or export acceptance exists for manual entries yet. |

## Formal Review Conclusion

The manual entry MVP is partially ready for a future readback implementation
authorization task, but it is not ready for a direct replay implementation or
daily-use PnL implementation without more focused work.

Ready:

- DataService write wiring can persist manual BUY and Deposit facts through
  authorized repository boundaries.
- TASK-202 can request existing read action refreshes after successful writes.
- TASK-204 proves the minimum runtime E2E write path, idempotency, invalid
  no-write behavior, and sanitized responses with temporary SQLite fixtures.

Not ready:

- Full `position.list`, `cash.summary`, and `portfolio.pnl.summary` correctness
  over manual entries is not proven.
- AccountingEngine replay readiness is not proven.
- SELL, Withdrawal, realized / unrealized PnL, fee / tax treatment, principal
  flow, stale facts, and fixture coverage are incomplete.
- Backup / export / restore acceptance is not reviewed.

## Next Task Recommendation

Recommended next task: TASK-206 should be a ShellAccounting manual entry
readback mapping authorization gate. It should authorize only the design and
tests required to prove how manual transaction and manual cash movement facts
flow into `position.list`, `cash.summary`, and `portfolio.pnl.summary`.

TASK-206 should not implement AccountingEngine replay until a later dedicated
replay implementation task confirms fixtures, realized / unrealized PnL
semantics, fee / tax treatment, and principal flow policy.

Future tasks must separately authorize:

- readback implementation
- AccountingEngine replay implementation
- audit write / ledger policy
- backup / export / packaging acceptance
- broker sandbox capability
- real broker order placement
- automatic trading
