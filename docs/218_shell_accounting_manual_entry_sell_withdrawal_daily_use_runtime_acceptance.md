# ShellAccounting Manual Entry SELL Withdrawal Daily-Use Runtime Acceptance

## Document Purpose

TASK-211 adds tests-and-docs-only runtime acceptance coverage for the manual
entry SELL / Withdrawal daily-use expansion authorized by TASK-210. The new
runtime acceptance CTest uses a temporary SQLite database, applies migrations
001 and 002, writes synthetic manual facts through existing DataService actions,
and verifies readback behavior for `position.list`, `cash.summary`, and
`portfolio.pnl.summary`.

TASK-211 does not modify production code, production QML, startup, Presenter,
Controller, ShellServices adapter / port code, DataServiceActions,
DataServiceActionRegistrar, DataAccess repositories, migrations,
AccountingEngine replay, StrategyEngine, MarketEngine, broker SDKs, network,
credentials, endpoints, real order placement, real broker order ids, or
automatic trading.

## Runtime Acceptance Scope

The TASK-211 runtime acceptance test keeps the first phase local and synthetic:

- Temporary SQLite database only.
- `migrations/001_initial_schema.sql` and
  `migrations/002_shell_accounting_manual_entry_schema.sql` are applied without
  modification.
- Synthetic account, portfolio, and instrument fixtures are inserted by the
  test.
- Manual facts are written only through existing DataService action dispatch:
  `accounting.manual_transaction.create` and
  `accounting.manual_cash_movement.create`.
- Readback is verified through `position.list`, `cash.summary`, and
  `portfolio.pnl.summary`.

## SELL Acceptance

The SELL runtime acceptance scenario writes a manual BUY fixture before SELL,
then writes a manual SELL through the authorized DataService boundary. The test
verifies:

- BUY fixture before SELL.
- SELL quantity reduction is visible in `position.list`.
- SELL cash inflow is visible in `cash.summary`.
- Cost amount reflects reduced manual position cost.
- Market value remains `UNAVAILABLE` when no prices exist.
- Unrealized PnL remains `UNAVAILABLE` when no prices exist.
- No fabricated realized PnL.
- No fabricated unrealized PnL.
- No AccountingEngine replay.
- Readback does not write additional database rows.
- No QML calculation of SELL position or cash effects.

The sell-exceeds-position scenario must not silently succeed. If the repository
rejects the write, the test requires a safe non-write response. If the
repository accepts the write, `position.list` must make the negative position
visible and continue to expose safe partial / unavailable PnL status rather
than fabricating market value or PnL.

## Withdrawal Acceptance

The Withdrawal runtime acceptance scenario writes a Deposit fixture before
Withdrawal, then writes a Withdrawal through the authorized DataService
boundary. The test verifies:

- Deposit fixture before Withdrawal.
- Withdrawal cash outflow is visible in `cash.summary`.
- Currency is visible and stable.
- Principal base reflects the synthetic Deposit / Withdrawal flow.
- `portfolio.pnl.summary` keeps PnL unavailable when replay or prices are
  unavailable.
- No fabricated PnL.
- No AccountingEngine replay.
- Readback does not write additional database rows.
- No QML calculation of Withdrawal cash effects.

The insufficient-cash / negative-cash scenario must not silently succeed. If
the repository rejects the write, the test requires a safe non-write response.
If the repository accepts the write, `cash.summary` must make the negative cash
state visible and `portfolio.pnl.summary` must keep PnL unavailable instead of
fabricating values.

## DataService Boundary

TASK-211 uses existing DataService action dispatch only. The runtime acceptance
test does not add a new action, does not change action registration, does not
change validation logic, and does not change repository logic.

The write setup remains limited to already authorized manual entry writes:

- manual transaction write for BUY / SELL;
- manual cash movement write for Deposit / Withdrawal.

The readback phase must remain no-write:

- `position.list` must not write rows;
- `cash.summary` must not write rows;
- `portfolio.pnl.summary` must not write rows.

## Privacy And Safety

Responses must remain sanitized:

- no raw SQL;
- no full raw payload;
- no credentials;
- no endpoint;
- no broker payload;
- no real broker order id;
- no internal exception stack trace.

Readback responses must keep the established safe flags:

- no write-enabled readback;
- no replay execution;
- no snapshot rebuild;
- no AccountingEngine call;
- no TradeDraft generation;
- no trade suggestion;
- no strategy execution;
- no broker order submission.

## Forbidden Behavior

TASK-211 does not authorize:

- production QML changes;
- startup changes;
- Presenter / Controller / ShellServices changes;
- DataServiceActions changes;
- repository changes;
- migration or schema changes;
- AccountingEngine replay;
- audit or ledger expansion;
- broker SDKs;
- network calls;
- credentials;
- endpoints;
- real order placement;
- real broker order ids;
- automatic trading.

This scope remains no broker SDK, no real order, and no automatic trading.

## Rollback / Disable Strategy

If TASK-211 acceptance creates a regression, removing the TASK-211 CTest and
docs restores the prior TASK-210 authorization-only state. Existing BUY +
Deposit daily-use acceptance, manual entry write wiring, repository writes, and
broker-disabled gates remain unaffected.

## Test Checklist

- `shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance`
  passes.
- TASK-210 SELL / Withdrawal authorization gate passes.
- TASK-209 BUY + Deposit runtime acceptance passes.
- TASK-208 daily-use authorization gate passes.
- TASK-207 readback mapping implementation and authorization gates pass.
- TASK-198 / TASK-196 / TASK-192 manual write regressions pass.
- Broker disabled, broker order dry-run, real broker authorization, and real
  broker implementation gates pass.

Future SELL / Withdrawal production UI changes, replay integration, market
price valuation, realized PnL computation, broker order placement, strategy
execution, or automatic trading must be separately authorized.

TASK-212 has now added a replay / audit / ledger / backup-export adequacy
review gate in
`docs/220_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_gate.md`
and
`docs/221_shell_accounting_manual_entry_replay_audit_ledger_adequacy_review_test_plan.md`.
TASK-212 is review-gate-only and does not implement replay, audit / ledger
writes, backup/export/restore, broker, network, credentials, endpoints, real
order placement, or automatic trading.
