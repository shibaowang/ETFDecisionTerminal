# ShellAccounting Manual Entry SELL Withdrawal Daily-Use Acceptance Authorization Gate

## Document Purpose

TASK-210 is SELL / Withdrawal daily-use acceptance authorization gate-only. It
defines the boundary for future runtime acceptance coverage after TASK-209
proved the BUY + Deposit daily-use baseline. TASK-211 has now added
tests-and-docs-only SELL / Withdrawal daily-use runtime acceptance coverage
within this boundary.

This task does not implement runtime acceptance. It does not modify production
code, production QML, startup, Presenter, Controller, ShellServices adapter /
port code, DataServiceActions, repositories, migrations, AccountingEngine
replay, snapshot rebuild, derived table refresh, runtime SQL / SQLite
read/write behavior, audit / ledger writes, broker SDKs, network calls,
credentials, endpoints, real order placement, real broker order ids, or
automatic trading.
TASK-210 does not modify production code.
TASK-210 does not modify production QML.
TASK-210 does not modify startup, Presenter, Controller, or ShellServices.
TASK-210 does not modify ShellServices adapter / port code.
TASK-210 does not modify DataServiceActions, repositories, or migrations.
TASK-210 does not add runtime SQL / SQLite read/write behavior.
TASK-210 does not add replay, audit / ledger, broker, network, credentials,
endpoint, real order placement, real broker order ids, or automatic trading.

Broker sandbox new capability development remains paused.

## Current BUY + Deposit Baseline

TASK-209 proved the BUY + Deposit daily-use runtime acceptance baseline with
synthetic local fixtures:

- A valid manual BUY is written through the existing DataService action and
  repository path.
- A valid Deposit is written through the existing DataService action and
  repository path.
- `position.list` exposes the manual BUY position with safe unavailable market
  value and PnL fields.
- `cash.summary` exposes the Deposit and BUY cash effects without fabricated
  cash facts.
- `portfolio.pnl.summary` returns safe partial / unavailable PnL when replay or
  market prices are unavailable.

SELL / Withdrawal remain separate acceptance expansion from production behavior.
TASK-211 now adds runtime acceptance tests for that expansion without adding
production behavior.

## SELL Daily-Use Acceptance Boundary

Future SELL daily-use acceptance must use a BUY fixture before SELL so the
position reduction has an explicit local source. The runtime probe should
verify:

- BUY fixture before SELL.
- SELL quantity reduction in `position.list`.
- `sell exceeds position` safe issue when SELL quantity is larger than the
  available manual position.
- SELL cash inflow in `cash.summary`.
- Fee / tax treatment policy is explicit and does not fabricate unavailable
  values.
- No fabricated realized PnL.
- No fabricated unrealized PnL.
- No replay.
- No silent success when SELL data is partial or unavailable.
- No QML calculation of SELL position or cash effects.

Future SELL acceptance must not treat UI raw payload as ledger facts, must not
fabricate positions or PnL, and must keep AccountingEngine replay disabled
unless a separate TASK authorizes replay.

## Withdrawal Daily-Use Acceptance Boundary

Future Withdrawal daily-use acceptance must use a Deposit fixture before
Withdrawal so the cash outflow has an explicit local source. The runtime probe
should verify:

- Deposit fixture before Withdrawal.
- Withdrawal cash outflow in `cash.summary`.
- Insufficient cash / negative cash policy produces a safe issue or explicitly
  documented safe status.
- Currency mapping is visible and stable.
- Principal flow safe status is visible when principal base support is partial
  or unsupported.
- No fabricated PnL.
- No replay.
- No silent success when Withdrawal data is partial or unavailable.
- No QML calculation of Withdrawal cash effects.

Future Withdrawal acceptance must not fabricate cash balances, principal flows,
or PnL, and must not add replay, audit / ledger expansion, broker, network,
credentials, endpoint, real order placement, or automatic trading behavior.

## TASK-211 Runtime Acceptance Scope

Future TASK-211 may implement SELL / Withdrawal runtime acceptance tests if
separately authorized. TASK-211 has now implemented those runtime acceptance
tests only:

- Use temporary SQLite DB and synthetic fixtures.
- Reuse existing authorized DataService manual entry write and readback
  boundaries.
- Prove SELL quantity reduction, SELL cash inflow, Withdrawal cash outflow, and
  safe partial status without production drift.
- Prove sell-exceeds-position and insufficient-cash / negative-cash scenarios
  do not silently succeed.
- Avoid production feature additions.
- Do not modify QML / Presenter / Controller / ShellServices.
- Do not modify DataServiceActions unless a blocking bug is found and separately authorized.
- Do not modify repositories / migrations.
- Do not add replay / audit / ledger / broker / real order / automatic
  trading.

TASK-211 keeps the first phase local and synthetic: no broker SDK, no
network, no credentials, no endpoint, no real broker order id, no real order
placement, and no automatic trading.
