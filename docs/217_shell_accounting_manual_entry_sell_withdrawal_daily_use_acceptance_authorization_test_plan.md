# ShellAccounting Manual Entry SELL Withdrawal Daily-Use Acceptance Authorization Test Plan

## Document Purpose

TASK-210 defines static documentation and boundary probes for future SELL /
Withdrawal daily-use acceptance expansion. TASK-211 has now added
tests-and-docs-only runtime acceptance coverage for that expansion. The TASK-210
gate still does not implement production runtime behavior, trigger replay, call
broker code, or depend on real data.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/216 exists | Verify the SELL / Withdrawal daily-use acceptance authorization document exists. |
| docs/217 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-210 and docs/216 / docs/217. |
| Authorization-only scope | Verify docs/216 says TASK-210 is gate-only, no runtime acceptance implementation, and no production code changes. |
| BUY + Deposit baseline | Verify docs/216 records that TASK-209 proved the BUY + Deposit runtime acceptance baseline. |
| SELL boundary | Verify docs/216 covers BUY fixture before SELL, SELL quantity reduction, sell exceeds position safe issue, SELL cash inflow, fee / tax treatment policy, no fabricated realized PnL, no fabricated unrealized PnL, no replay, no silent success, and no QML calculation. |
| Withdrawal boundary | Verify docs/216 covers Deposit fixture before Withdrawal, Withdrawal cash outflow, insufficient cash / negative cash policy, currency mapping, principal flow safe status, no fabricated PnL, no replay, no silent success, and no QML calculation. |
| TASK-211 runtime acceptance | Verify docs/216 records that TASK-211 added runtime acceptance tests only and keeps production code, QML, DataServiceActions, repositories, migrations, replay, audit / ledger, broker, real order, and automatic trading out of scope. |
| Production code unchanged | Verify TASK-210 changed paths exclude production code. |
| Existing gates retained | Verify TASK-211, TASK-209, TASK-208, TASK-207, TASK-204, TASK-202, TASK-200, TASK-198, TASK-196, TASK-192, broker, and real broker gates remain registered. |
| Forbidden drift | Verify no runtime SQL / SQLite read/write, runtime acceptance implementation, read model refresh, replay, audit / ledger, broker, network, credentials, endpoint, real order, or automatic trading behavior is added. |

## Required Probes

- Static source scan.
- Documentation token scan.
- Changed-path allowlist scan.
- No production code scan.
- No QML / startup / Presenter / Controller scan.
- No ShellServices adapter / port scan.
- No DataServiceActions scan.
- No repository scan.
- No migration / schema file scan.
- No runtime SQL / SQLite read/write scan.
- No production runtime implementation scan.
- No readback implementation scan.
- No AccountingEngine replay scan.
- No audit / ledger write scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Retained regression gate scan.

## Go / No-Go Checklist

Go only if:

- docs/216 and docs/217 exist and are indexed.
- TASK-210 tests pass.
- docs/216 contains the TASK-209 BUY + Deposit baseline.
- docs/216 contains SELL daily-use acceptance boundaries.
- docs/216 contains Withdrawal daily-use acceptance boundaries.
- docs/216 records TASK-211 runtime acceptance tests only.
- production code, QML, startup, Presenter, Controller, ShellServices,
  DataServiceActions, repositories, and migrations are unchanged.
- no SELL / Withdrawal production runtime implementation, production SQL /
  SQLite read/write, read model refresh, replay, audit / ledger, broker,
  network, credentials, endpoint, real order, or automatic trading behavior is
  added.
- existing TASK-211, TASK-209, TASK-208, TASK-207, TASK-204, TASK-202, TASK-200,
  TASK-198, TASK-196, TASK-192, broker, and real broker gates remain registered.

No-Go if TASK-210 modifies production code, adds production runtime
implementation, writes a database outside the authorized TASK-211 synthetic
test fixture, weakens broker / real broker gates, fabricates PnL, connects
network or broker capability, places a real order, or enables automatic
trading.
