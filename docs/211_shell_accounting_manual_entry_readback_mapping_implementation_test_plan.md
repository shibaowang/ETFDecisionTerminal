# ShellAccounting Manual Entry Readback Mapping Implementation Test Plan

## Document Purpose

TASK-207 verifies the implemented DataService readback mapping for persisted
manual entry facts. The tests use temporary SQLite fixtures and synthetic data.
They do not use real user data, network calls, broker credentials, endpoints,
or real broker order placement.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/210 exists | Verify the implementation document exists. |
| docs/211 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-207 and docs/210 / docs/211. |
| Temporary SQLite fixture | Apply migrations 001 and 002 to an isolated database. |
| Manual BUY fixture | Write a valid manual BUY through the authorized DataService write action. |
| Manual SELL fixture | Write a valid manual SELL through the authorized DataService write action. |
| Manual Deposit fixture | Write a valid manual Deposit through the authorized DataService write action. |
| Manual Withdrawal fixture | Write a valid manual Withdrawal through the authorized DataService write action. |
| Duplicate idempotency | Verify duplicate writes do not double count readback. |
| position.list mapping | Verify manual BUY quantity and SELL reduction are reflected in readback. |
| cash.summary mapping | Verify Deposit, Withdrawal, BUY outflow, and SELL inflow are reflected in readback. |
| portfolio.pnl.summary mapping | Verify safe partial status and unavailable PnL instead of fabricated values. |
| Response sanitization | Verify responses do not expose raw SQL, raw payload, credentials, endpoint, token, stack trace, broker payload, or real order id. |
| No readback write | Verify readback dispatches do not change `trade_log`, `trade_execution_group`, `cash_adjustment`, or `audit_log` row counts. |
| Static DataService boundary | Verify readback mapping stays in DataService read actions and does not scatter write SQL in read mapping. |
| Production boundary | Verify QML, Presenter, Controller, ShellServices, DataServiceActionRegistrar, write repositories, and migrations are unchanged. |
| Forbidden capability drift | Verify no replay, snapshot rebuild, derived table refresh, audit / ledger write, broker SDK, network, credentials, endpoint, real order, or automatic trading behavior is added. |
| Regression | Verify TASK-206, TASK-205, TASK-204, TASK-202, TASK-200, TASK-198, TASK-196, TASK-192, broker, and real broker gates remain retained. |

## Required Probes

- Static source scan.
- Documentation token scan.
- Temporary SQLite runtime probe.
- DataService write fixture probe.
- DataService read action probe.
- Duplicate idempotency probe.
- Forbidden write token scan for readback mapping functions.
- No QML / startup / Presenter / Controller / ShellServices drift scan.
- No DataServiceActionRegistrar drift scan.
- No write repository drift scan.
- No migrations drift scan.
- No AccountingEngine replay scan.
- No snapshot rebuild / derived table refresh scan.
- No audit / ledger write scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Response privacy scan.
- Retained regression probe.

## Go / No-Go Checklist

Go only if:

- docs/210 and docs/211 exist and are indexed.
- TASK-207 runtime readback mapping tests pass.
- `position.list` maps manual entry BUY / SELL facts without replay.
- `cash.summary` maps manual transaction cash impact and cash movement facts.
- `portfolio.pnl.summary` returns safe partial / unavailable PnL when pricing
  or replay inputs are missing.
- duplicate idempotency writes do not double count readback.
- readback dispatches do not write the database.
- production QML, startup, Presenter, Controller, ShellServices,
  DataServiceActionRegistrar, write repositories, and migrations are unchanged.
- no AccountingEngine replay, snapshot rebuild, audit / ledger, broker,
  network, credentials, endpoint, real order placement, or automatic trading
  behavior is added.

TASK-208 has now added a daily-use acceptance authorization gate. Runtime
daily-use acceptance implementation remains a separate future TASK.

No-Go if any readback path writes the database, fabricates PnL, calls replay,
changes UI or write repositories, changes migrations, exposes unsafe payload
details, connects broker/network/credentials/endpoints, places real orders, or
enables automatic trading.
