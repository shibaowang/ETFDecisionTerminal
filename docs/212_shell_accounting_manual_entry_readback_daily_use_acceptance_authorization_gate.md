# ShellAccounting Manual Entry Readback Daily-Use Acceptance Authorization Gate

## Document Purpose

TASK-208 is daily-use acceptance authorization gate-only. It defines the
acceptance standard for using ShellAccounting manual entry readback in the
local assistant decision MVP.

This task does not implement new functionality. It does not modify production
code, production QML, startup, Presenter, Controller, ShellServices,
DataServiceActions, repositories, migrations, AccountingEngine replay, runtime
SQL / SQLite read/write behavior, audit / ledger writes, broker SDKs, network
calls, credentials, endpoints, real order placement, real broker order ids, or
automatic trading.
TASK-208 does not modify production code.
TASK-208 does not modify production QML.
TASK-208 does not add runtime SQL / SQLite read/write behavior.
No new readback implementation is authorized by TASK-208.
No replay implementation is authorized by TASK-208.
TASK-208 does not implement replay.
TASK-208 does not write audit / ledger rows.
TASK-208 does not connect broker SDK, network, credentials, or endpoint.
TASK-208 does not place real orders or enable automatic trading.

Broker sandbox new capability development remains paused.

## Current Implemented Chain

The current chain is:

- TASK-192 implemented manual transaction DataAccess repository write.
- TASK-196 implemented manual cash movement DataAccess repository dual-write.
- TASK-198 implemented DataService runtime write wiring.
- TASK-200 implemented minimal QML / Presenter manual entry UI wiring.
- TASK-202 implemented post-write DataService read-boundary refresh.
- TASK-204 implemented manual entry MVP runtime E2E acceptance tests.
- TASK-205 completed readback / replay adequacy review.
- TASK-206 completed readback mapping authorization.
- TASK-207 implemented DataService readback mapping for `position.list`,
  `cash.summary`, and safe partial `portfolio.pnl.summary`.

TASK-208 only defines daily-use acceptance. It does not add readback mapping,
readback implementation, replay implementation, read model refresh, UI behavior, database write,
audit / ledger behavior, broker capability, network capability, credentials,
endpoint handling, real order placement, or automatic trading.

## position.list Daily-Use Acceptance

Daily-use acceptance for `position.list` requires:

- Manual BUY position visible after a successful manual transaction write.
- Instrument code visible for the manual entry position (`instrument code visible`).
- Account / portfolio mapping visible for the manual entry position.
- Manual SELL reduction visible after a successful manual SELL, or an explicit
  safe not-ready issue if a scenario is not yet supported.
- Market value unavailable if there are no prices.
- Unrealized PnL unavailable if there are no prices.
- No fabricated market value or unrealized PnL.
- No silent success when data is partial or unavailable.
- Response marks manual entry readback / safe partial status.
- No QML calculation of positions.

## cash.summary Daily-Use Acceptance

Daily-use acceptance for `cash.summary` requires:

- Deposit cash inflow visible after a successful manual cash movement write.
- Withdrawal cash outflow visible after a successful manual cash movement
  write, or an explicit safe not-ready issue if a scenario is not yet
  supported.
- BUY cash outflow visible after a successful manual transaction write.
- SELL cash inflow visible after a successful manual transaction write, or an
  explicit safe not-ready issue if a scenario is not yet supported.
- Currency visible.
- Principal base policy visible or a safe not-ready issue when unsupported.
- No fabricated principal or cash facts.
- No silent success when data is partial or unavailable.
- No QML calculation of cash.

## portfolio.pnl.summary Daily-Use Acceptance

Daily-use acceptance for `portfolio.pnl.summary` requires:

- Cash balance may be visible from manual entry readback.
- Holding cost may be visible from manual entry readback.
- Realized PnL not fabricated.
- Unrealized PnL not fabricated.
- Market value not fabricated.
- Missing market price, missing replay, or incomplete PnL inputs produce a safe
  partial / unavailable issue.
- portfolio.pnl.summary must show a safe partial / unavailable issue when full
  PnL is not supported.
- No silent success when PnL is partial or unavailable.
- No QML calculation of PnL.

## UI-Visible Status And Issue Acceptance

Daily-use acceptance requires the user to see:

- manual entry write status
- readback refresh status
- position / cash / PnL readback result or safe unavailable issue
- invalid payload status
- duplicate idempotency status
- sanitized errors only

Responses and UI-visible issues must not expose raw SQL, raw payload,
credentials, endpoint, token, stack trace, broker payload, full trade payload,
or real broker order id.

## MVP Ready / Not Ready Conclusion

Manual entry write + readback is partially daily-use ready.

Position and cash readback can be accepted for the BUY + Deposit baseline.
SELL, Withdrawal, PnL, market price, and AccountingEngine replay remain separate future work unless already covered by explicit runtime tests.

The recommended next task is TASK-209 runtime daily-use acceptance
implementation. Future replay, audit, ledger, backup/export, packaging,
broker sandbox, real broker order, strategy execution, and automatic trading
must remain separate TASKs.

TASK-209 has now added tests-and-docs-only runtime daily-use acceptance coverage
for the BUY + Deposit baseline. The acceptance boundary remains no production
code changes, no production QML calculation, no replay implementation, no audit
/ ledger write expansion, no broker / network / credentials / endpoint, no real
order placement, and no automatic trading.
