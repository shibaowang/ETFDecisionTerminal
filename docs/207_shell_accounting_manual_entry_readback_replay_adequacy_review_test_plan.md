# ShellAccounting Manual Entry Readback Replay Adequacy Review Test Plan

## Document Purpose

TASK-205 defines a review-gate-only test plan for ShellAccounting manual entry
readback / replay adequacy. It uses static checks, documentation consistency
checks, and boundary probes only. It does not write a database, does not trigger
network access, and does not depend on real user data.

## Test Matrix

| Area | Probe |
| --- | --- |
| docs/206 exists | Verify the readback / replay adequacy review gate document exists. |
| docs/207 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-205 and docs/206 / docs/207. |
| Review-gate-only scope | Verify docs/206 says no production code changes and no readback / replay implementation. |
| Manual transaction fact adequacy | Verify docs/206 covers `trade_execution_group`, `trade_log`, BUY, SELL, quantity, price, fee, tax, cash impact, instrument mapping, account / portfolio mapping, and idempotency / request trace. |
| Manual cash movement fact adequacy | Verify docs/206 covers `trade_log`, `cash_adjustment`, deposit, withdrawal, `cash_adjustment.trade_log_id`, amount sign, currency, principal base / cash balance, and idempotency / request trace. |
| Existing read actions | Verify docs/206 reviews `position.list`, `cash.summary`, `portfolio.pnl.summary`, `base_position.summary`, and `sniper_pool.summary`. |
| Readback / replay gap matrix | Verify docs/206 lists schema gap, repository mapping gap, DataService readback gap, replay gap, UI refresh gap, fixture/test gap, audit/ledger gap, and backup/export gap. |
| Formal conclusion | Verify docs/206 contains a specific partially-ready / not-ready conclusion and exact next task recommendation. |
| Production drift | Verify TASK-205 does not modify production code, QML, startup, Presenter / Controller, ShellServices adapter / port, DataServiceActions, repositories, migrations, AccountingEngine, StrategyEngine, MarketEngine, broker code, network, credentials, endpoints, real orders, or automatic trading. |
| Regression | Verify TASK-204, TASK-202, TASK-200, TASK-198, TASK-196, TASK-192, broker, and real broker gates remain present and passing. |

## Required Probes

- Static source scan.
- Documentation token scan.
- Changed-path allowlist scan.
- No production code drift scan.
- No QML / startup / Presenter / Controller drift scan.
- No ShellServices adapter / port drift scan.
- No DataServiceActions drift scan.
- No DataAccess repository drift scan.
- No migrations drift scan.
- No new migration or schema file scan.
- No runtime SQL / SQLite write scan.
- No readback / replay implementation scan.
- No audit / ledger write scan.
- No broker / network / credentials / endpoint scan.
- No real order / automatic trading scan.
- Retained TASK-204 / TASK-202 / TASK-200 / TASK-198 / TASK-196 / TASK-192 regression probes.
- Retained broker and real broker gate probes.
- Verify no production code changed.
- Verify no runtime SQL / SQLite write was added.
- Verify no broker, network, credentials, endpoint, real order, or automatic trading behavior was added.

## Go / No-Go Checklist

Go only if:

- docs/206 and docs/207 exist and are indexed.
- TASK-205 gate tests pass.
- TASK-204 runtime E2E acceptance remains passing.
- TASK-202 post-write refresh implementation remains passing.
- TASK-200 QML Presenter implementation remains passing.
- TASK-198 DataService write wiring implementation remains passing.
- TASK-196 / TASK-192 repository tests remain passing.
- Broker and real broker gates remain passing.
- No production code changed.
- No migrations changed.
- No readback / replay implementation was added.
- No runtime SQL / SQLite write was added.
- No audit / ledger write was added.
- No broker, network, credentials, endpoint, real order, or automatic trading
  behavior was added.

No-Go if any production implementation change, database write, replay change,
broker capability, network call, credential handling, endpoint configuration,
real order placement, automatic trading behavior, or schema drift appears.

TASK-206 follow-up note: the readback mapping authorization gate is now tracked
by
`docs/208_shell_accounting_manual_entry_readback_mapping_authorization_gate.md`
and
`docs/209_shell_accounting_manual_entry_readback_mapping_authorization_test_plan.md`.
TASK-206 remains gate-only and does not implement readback mapping, replay,
audit / ledger, broker, real order placement, or automatic trading.
