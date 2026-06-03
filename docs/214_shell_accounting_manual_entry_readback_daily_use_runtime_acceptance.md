# ShellAccounting Manual Entry Readback Daily-Use Runtime Acceptance

## Document Purpose

TASK-209 adds runtime acceptance coverage for the ShellAccounting manual entry
daily-use baseline. It verifies that the already implemented manual entry write
and readback chain can support a local MVP user flow for manual BUY position
readback, manual Deposit cash readback, and safe partial portfolio PnL readback.

TASK-209 is tests-and-docs only. It does not modify production code, production
QML, startup, Presenter, Controller, ShellServices adapter / port code,
DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
migrations, AccountingEngine replay, StrategyEngine, MarketEngine, broker SDKs,
network, credentials, endpoints, real order placement, real broker order ids, or
automatic trading.

## Scope

The runtime acceptance scope is:

- Use a temporary SQLite database.
- Apply `migrations/001_initial_schema.sql`.
- Apply `migrations/002_shell_accounting_manual_entry_schema.sql`.
- Insert synthetic account, portfolio, and instrument fixtures.
- Write a valid manual BUY through `accounting.manual_transaction.create`.
- Write a valid Deposit through `accounting.manual_cash_movement.create`.
- Read back the facts through `position.list`, `cash.summary`, and
  `portfolio.pnl.summary`.
- Verify safe partial / unavailable status where market price, replay, or PnL
  inputs are unavailable.

The acceptance test uses only synthetic local fixtures. It does not use real
user data, network calls, broker credentials, broker endpoints, broker SDKs, or
real order placement.

## Runtime Acceptance Boundary

TASK-209 verifies the existing DataService boundary:

- Manual BUY and Deposit writes continue through the authorized DataService
  action and repository paths from TASK-198, TASK-192, and TASK-196.
- Readback uses `position.list`, `cash.summary`, and `portfolio.pnl.summary`.
- Readback does not write additional database rows.
- Readback does not execute AccountingEngine replay unless separately requested
  and authorized by a future task.
- Readback does not fabricate market value, realized PnL, unrealized PnL, or
  total portfolio PnL.
- Readback returns explicit manual entry readback / safe partial status instead
  of silent success.

## position.list Acceptance

The runtime test verifies that `position.list` exposes the manual BUY baseline:

- The manual BUY instrument is visible.
- The instrument code is visible.
- Account and portfolio mapping are visible.
- Quantity reflects the persisted manual BUY quantity.
- Cost reflects persisted manual trade facts.
- Market value remains `UNAVAILABLE` without market prices.
- Unrealized PnL remains `UNAVAILABLE` without replay and market prices.
- The response contains `MANUAL_ENTRY_READBACK_MAPPING`.
- The response contains `PNL_UNAVAILABLE_WITHOUT_REPLAY`.
- The response has no fabricated values and no silent success.

## cash.summary Acceptance

The runtime test verifies that `cash.summary` exposes the manual Deposit
baseline together with the manual BUY cash impact:

- The account and portfolio mapping are visible.
- The CNY currency is visible.
- The Deposit principal base is visible.
- The cash balance reflects persisted manual facts.
- The response contains `MANUAL_ENTRY_READBACK_MAPPING`.
- The response does not fabricate cash or principal facts.
- The response has no silent success when data is partial or unavailable.

## portfolio.pnl.summary Acceptance

The runtime test verifies safe partial portfolio PnL behavior:

- Cash balance may be visible from persisted manual facts.
- Principal base may be visible from persisted manual Deposit facts.
- Holding cost may be visible from persisted manual BUY facts.
- Total assets remain `UNAVAILABLE` without market prices.
- Total market value remains `UNAVAILABLE` without market prices.
- Total PnL remains `UNAVAILABLE` without replay and market prices.
- Total return remains `UNAVAILABLE` without replay and market prices.
- The response contains `PNL_UNAVAILABLE_WITHOUT_REPLAY`.
- The response has no fabricated market value or PnL and no silent success.

## Static Boundary

TASK-209 acceptance tests also verify static boundaries:

- Production QML does not calculate positions, cash, or PnL.
- QML does not directly call DataServiceClient.
- QML does not directly access SQLite or DataAccess.
- Presenter / Controller / ShellServices do not bypass the DataService boundary.
- DataServiceActions are not modified by TASK-209.
- DataAccess repositories are not modified by TASK-209.
- Migrations are not modified by TASK-209.
- No replay, audit / ledger, broker, network, credentials, endpoint, real order,
  or automatic trading behavior is added.

## MVP Conclusion

The BUY + Deposit daily-use baseline is accepted when the runtime test passes.
SELL, Withdrawal, richer PnL, market price integration, AccountingEngine replay,
audit / ledger expansion, broker sandbox capability, real broker order,
strategy execution, and automatic trading remain separate future tasks unless
already covered by explicit runtime tests.
