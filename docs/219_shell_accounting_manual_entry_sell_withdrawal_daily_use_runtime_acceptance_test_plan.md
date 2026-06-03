# ShellAccounting Manual Entry SELL Withdrawal Daily-Use Runtime Acceptance Test Plan

## Document Purpose

TASK-211 defines runtime acceptance tests for manual entry SELL / Withdrawal
daily-use behavior after the TASK-210 authorization gate. The tests are
tests-and-docs only and do not implement production runtime behavior.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/218 exists | Verify the TASK-211 runtime acceptance document exists. |
| docs/219 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-211 and docs/218 / docs/219. |
| CTest registration | Verify `shell_accounting_manual_entry_sell_withdrawal_daily_use_runtime_acceptance` is registered. |
| Temporary SQLite fixture | Apply migrations 001 and 002 in a temporary database and insert synthetic account / portfolio / instrument fixtures. |
| SELL setup | Write manual BUY before SELL through `accounting.manual_transaction.create`. |
| SELL reduction | Verify `position.list` exposes reduced quantity and reduced cost. |
| SELL cash inflow | Verify `cash.summary` exposes SELL cash inflow effects. |
| Sell exceeds position | Verify rejected write stays no-write, or accepted write exposes negative position and safe unavailable PnL state. |
| Withdrawal setup | Write Deposit before Withdrawal through `accounting.manual_cash_movement.create`. |
| Withdrawal cash outflow | Verify `cash.summary` exposes Withdrawal cash outflow. |
| Insufficient cash / negative cash | Verify rejected write stays no-write, or accepted write exposes negative cash and safe unavailable PnL state. |
| Portfolio partial PnL | Verify `portfolio.pnl.summary` keeps market value, PnL, and return unavailable when replay/prices are unavailable. |
| Readback no-write | Verify `position.list`, `cash.summary`, and `portfolio.pnl.summary` do not write additional rows. |
| Payload privacy | Verify readback and write responses do not expose raw SQL, raw payload, credentials, endpoint, broker payload, real order ids, or stack traces. |
| Static production boundary | Verify production QML, startup, Presenter, Controller, ShellServices, DataServiceActions, repositories, and migrations are not modified by TASK-211. |
| Broker boundary | Verify no broker SDK, broker order, real order placement, real broker order id, network, credentials, endpoint, or automatic trading behavior is added. |
| Regression gates | Verify TASK-210, TASK-209, TASK-208, TASK-207, TASK-198, TASK-196, TASK-192, broker, and real broker gates remain registered and pass. |

## Required Probes

- Runtime temporary SQLite fixture probe.
- Migration 001 / 002 application probe.
- Synthetic account / portfolio / instrument fixture probe.
- DataService write dispatch probe.
- SELL quantity reduction probe.
- SELL cash inflow probe.
- Sell-exceeds-position safe behavior probe.
- Withdrawal cash outflow probe.
- Insufficient-cash / negative-cash safe behavior probe.
- `position.list` readback probe.
- `cash.summary` readback probe.
- `portfolio.pnl.summary` safe partial probe.
- Readback no-write row count probe.
- Response sanitization probe.
- No production code scan.
- No QML / startup / Presenter / Controller scan.
- No ShellServices adapter / port scan.
- No DataServiceActions scan.
- No repository scan.
- No migration / schema file scan.
- No AccountingEngine replay scan.
- No audit / ledger write scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Retained regression gate scan.

## Go / No-Go Checklist

Go only if:

- docs/218 and docs/219 exist and are indexed.
- TASK-211 CTest is registered.
- SELL reduction readback is visible or unsafe states are explicit.
- SELL cash inflow is visible or unsafe states are explicit.
- sell-exceeds-position does not silently succeed.
- Withdrawal cash outflow is visible or unsafe states are explicit.
- insufficient cash / negative cash does not silently succeed.
- market value, realized PnL, unrealized PnL, total PnL, and return are not
  fabricated when replay/prices are unavailable.
- readback actions do not write additional database rows.
- responses remain sanitized.
- production QML, startup, Presenter, Controller, ShellServices,
  DataServiceActions, repositories, migrations, AccountingEngine replay,
  audit / ledger, broker, network, credentials, endpoint, real order, and
  automatic trading behavior are unchanged.
- existing TASK-210, TASK-209, TASK-208, TASK-207, TASK-198, TASK-196,
  TASK-192, broker, and real broker gates pass.

No-Go if TASK-211 modifies production code, changes DataServiceActions,
changes repositories, changes migrations, adds runtime replay, writes audit or
ledger rows, fabricates PnL, calls broker/network code, places a real order, or
enables automatic trading.
