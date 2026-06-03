# ShellAccounting Manual Entry Readback Daily-Use Runtime Acceptance Test Plan

## Document Purpose

TASK-209 defines runtime acceptance tests for the manual entry daily-use
baseline authorized by TASK-208. The tests use local synthetic SQLite fixtures
and the existing DataService dispatcher. They do not add production behavior or
modify production source.

## Test Matrix

| Area | Runtime probe |
| --- | --- |
| docs/214 exists | Verify the runtime acceptance document exists. |
| docs/215 exists | Verify this test plan exists. |
| Documentation registration | Verify README, docs/README, and docs/12 reference TASK-209 and docs/214 / docs/215. |
| Temporary SQLite fixture | Apply migrations 001 and 002 to a temporary database and insert synthetic account, portfolio, and instrument rows. |
| BUY write setup | Use `accounting.manual_transaction.create` with a valid manual BUY payload. |
| Deposit write setup | Use `accounting.manual_cash_movement.create` with a valid Deposit payload. |
| position.list acceptance | Verify manual BUY instrument, quantity, account / portfolio / instrument mapping, safe partial status, unavailable market value, unavailable unrealized PnL, and no silent success. |
| cash.summary acceptance | Verify Deposit principal base, cash balance, currency, account / portfolio mapping, no fabricated cash facts, and no silent success. |
| portfolio.pnl.summary acceptance | Verify cash balance, principal base, holding cost, unavailable total assets / market value / PnL / return, safe partial issue, and no fabricated PnL. |
| Readback no-write | Verify readback actions do not change trade_execution_group, trade_log, cash_adjustment, or audit_log row counts. |
| Payload privacy | Verify responses do not expose raw SQL, raw payload, credentials, endpoints, stack traces, broker payload, full trade payload, or real broker order ids. |
| Static production boundary | Verify QML / Presenter / Controller / ShellServices do not bypass DataService and no production source is modified by TASK-209. |
| Broker boundary | Verify no broker SDK, network, credentials, endpoint, real order placement, or automatic trading is added. |
| Regression gates | Retain TASK-208, TASK-207, TASK-206, TASK-205, TASK-204, TASK-202, TASK-200, TASK-198, TASK-196, TASK-192, broker, and real broker gates. |

## Required Probes

- Runtime DataService dispatcher probe.
- Temporary SQLite fixture probe.
- Manual BUY write setup probe.
- Manual Deposit write setup probe.
- `position.list` daily-use readback probe.
- `cash.summary` daily-use readback probe.
- `portfolio.pnl.summary` safe partial readback probe.
- Readback no-write row count probe.
- Response privacy probe.
- No QML calculation probe.
- No direct QML DataServiceClient / SQLite / DataAccess probe.
- No production startup / Presenter / Controller / ShellServices bypass probe.
- No DataServiceActions production modification probe.
- No repository modification probe.
- No migration / schema modification probe.
- No AccountingEngine replay integration probe.
- No audit / ledger write probe.
- No broker / network / credentials / endpoint probe.
- No real order / automatic trading probe.
- Retained regression gate probe.

## Go / No-Go Checklist

Go only if:

- docs/214 and docs/215 exist and are indexed.
- The TASK-209 runtime CTest is registered as
  `shell_accounting_manual_entry_readback_daily_use_runtime_acceptance`.
- The runtime test writes only the setup BUY and Deposit facts through the
  already authorized DataService actions.
- `position.list` returns manual BUY readback with safe unavailable market value
  and PnL fields.
- `cash.summary` returns manual Deposit cash readback with visible principal
  base and no fabricated cash facts.
- `portfolio.pnl.summary` returns safe partial manual readback without
  fabricated PnL or market value.
- Readback actions do not write additional database rows.
- Payloads remain sanitized.
- Production QML, startup, Presenter, Controller, ShellServices,
  DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
  migrations, AccountingEngine, StrategyEngine, MarketEngine, broker, network,
  credentials, endpoint, real order, and automatic trading code remain
  unchanged.

No-Go if TASK-209 modifies production source, changes migrations, adds runtime
implementation, fabricates PnL, writes audit / ledger rows, connects replay,
connects broker / network / credentials / endpoints, places a real order, or
enables automatic trading.
