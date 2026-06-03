# ShellAccounting Manual Entry Readback Mapping Authorization Test Plan

## Document Purpose

TASK-206 defines a gate-only test plan for authorizing the future
ShellAccounting manual entry readback mapping implementation boundary. The
tests are static source scans, documentation consistency checks, and boundary
checks only. They do not write a database, do not add runtime SQL / SQLite
read/write behavior, do not trigger network access, and do not depend on real
user data.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/208 exists | Verify the readback mapping authorization gate document exists. |
| docs/209 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-206 and docs/208 / docs/209. |
| Authorization-only scope | Verify docs/208 says TASK-206 is readback mapping authorization gate-only and no production code changes are authorized. |
| position.list mapping policy | Verify docs/208 covers BUY aggregation, SELL reduction, instrument mapping, account / portfolio mapping, cost basis, fee / tax, partial sell, sell exceeds position, stale / invalid facts, and no QML calculation. |
| cash.summary mapping policy | Verify docs/208 covers BUY cash outflow, SELL cash inflow, Deposit cash inflow, Withdrawal cash outflow, fee / tax cash treatment, `cash_adjustment.trade_log_id`, currency aggregation, principal base, and negative cash / insufficient cash policy. |
| portfolio.pnl.summary mapping policy | Verify docs/208 covers realized PnL, unrealized PnL, fee / tax treatment, cash movement treatment, principal flow, market price dependency, stale price / missing price, and multi-instrument / multi-account handling. |
| Formal authorization conclusion | Verify docs/208 states TASK-206 only authorizes future readback mapping implementation after a separate implementation TASK and recommends TASK-207. |
| TASK-207 implementation evolution | Verify the gate permits only the TASK-207 DataService readback mapping implementation in `DataServiceActions.cpp` plus TASK-207 docs and tests. |
| Production drift | Verify QML / Presenter / Controller unchanged, ShellServices adapter / port unchanged, DataServiceActionRegistrar unchanged, repositories unchanged, and migrations unchanged. |
| Forbidden capability drift | Verify no runtime SQLite write in readback, no replay implementation, no audit / ledger write, no broker / network / credentials / endpoint, no real order placement, and no automatic trading. |
| Regression | Verify existing TASK-205 / TASK-204 / TASK-202 / TASK-200 / TASK-198 / TASK-196 / TASK-192 gates and broker / real broker gates remain retained. |

## Required Probes

- Static source scan.
- Documentation token scan.
- Changed-path allowlist scan.
- TASK-207 DataService readback mapping implementation scan.
- No QML / startup / Presenter / Controller drift scan.
- No ShellServices adapter / port drift scan.
- No DataServiceActionRegistrar drift scan.
- No DataAccess repository drift scan.
- No migrations drift scan.
- No new migration / schema file scan.
- No runtime SQL / SQLite write outside readback scan.
- Authorized readback implementation scan.
- No AccountingEngine replay implementation scan.
- No snapshot rebuild / derived table refresh scan.
- No audit / ledger write scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Retained TASK-205 / TASK-204 / TASK-202 / TASK-200 / TASK-198 / TASK-196 /
  TASK-192 regression probes.
- Retained broker and real broker gate probes.
- Verify no production code changed.
- Verify no readback implementation was added.
- Verify no AccountingEngine replay implementation was added.
- Verify no broker, network, credentials, endpoint, real order, or automatic trading behavior was added.

## Go / No-Go Checklist

Go only if:

- docs/208 and docs/209 exist and are indexed.
- TASK-206 gate tests pass.
- TASK-205 adequacy review remains passing.
- TASK-204 runtime E2E acceptance remains passing.
- TASK-202 post-write refresh implementation remains passing.
- TASK-200 QML Presenter implementation remains passing.
- TASK-198 DataService write wiring implementation remains passing.
- TASK-196 / TASK-192 repository tests remain passing.
- Broker and real broker gates remain passing.
- Only authorized TASK-207 DataService readback mapping production code changed.
- No migrations changed.
- No unauthorized production code changed.
- No runtime SQL / SQLite write was added by readback.
- No replay implementation was added.
- No audit / ledger write was added.
- No broker, network, credentials, endpoint, real order, or automatic trading
  behavior was added.

No-Go if any production implementation change, database read/write
implementation, replay change, broker capability, network call, credential
handling, endpoint configuration, real order placement, automatic trading
behavior, or schema drift appears.
