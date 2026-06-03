# ShellAccounting Manual Entry Readback Daily-Use Acceptance Authorization Test Plan

## Document Purpose

TASK-208 defines static documentation and boundary probes for manual entry
readback daily-use acceptance. The tests do not implement new functionality,
write a database, trigger network calls, call broker code, or depend on real
data.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/212 exists | Verify the daily-use acceptance authorization document exists. |
| docs/213 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-208 and docs/212 / docs/213. |
| Authorization-only scope | Verify docs/212 says TASK-208 is gate-only and no production code changes. |
| position.list acceptance | Verify docs/212 covers BUY visibility, SELL reduction or safe not-ready issue, instrument, account / portfolio, unavailable market value / PnL, no silent success, and no QML calculation. |
| cash.summary acceptance | Verify docs/212 covers Deposit inflow, Withdrawal outflow or safe not-ready issue, BUY outflow, SELL inflow or safe issue, currency, principal policy, no silent success, and no QML calculation. |
| portfolio.pnl.summary acceptance | Verify docs/212 covers safe partial behavior and no fabricated realized PnL, unrealized PnL, or market value. |
| UI-visible status | Verify docs/212 covers write status, readback refresh status, readback result / safe unavailable issue, invalid payload, duplicate idempotency, and sanitized errors. |
| MVP conclusion | Verify docs/212 defines ready / not-ready status and recommends TASK-209. |
| Production code unchanged | Verify TASK-208 changed paths exclude production code. |
| Existing gates retained | Verify TASK-207, TASK-206, TASK-205, TASK-204, TASK-202, TASK-200, TASK-198, TASK-196, TASK-192, broker, and real broker gates remain registered. |
| Forbidden drift | Verify no runtime SQL / SQLite read/write, replay, audit / ledger, broker, network, credentials, endpoint, real order, or automatic trading behavior is added. |

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
- No readback implementation scan.
- No AccountingEngine replay scan.
- No audit / ledger write scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Retained regression gate scan.

## Go / No-Go Checklist

Go only if:

- docs/212 and docs/213 exist and are indexed.
- TASK-208 tests pass.
- docs/212 contains position.list daily-use acceptance.
- docs/212 contains cash.summary daily-use acceptance.
- docs/212 contains portfolio.pnl.summary safe partial acceptance.
- docs/212 contains MVP ready / not-ready conclusion.
- docs/212 recommends TASK-209 runtime daily-use acceptance implementation.
- production code, QML, startup, Presenter, Controller, ShellServices,
  DataServiceActions, repositories, and migrations are unchanged.
- no daily-use acceptance implementation, runtime SQL / SQLite read/write,
  read model refresh, replay, audit / ledger, broker, network, credentials,
  endpoint, real order, or automatic trading behavior is added.

No-Go if TASK-208 modifies production code, adds runtime implementation,
writes a database, weakens broker / real broker gates, fabricates PnL, connects
network or broker capability, places a real order, or enables automatic
trading.

