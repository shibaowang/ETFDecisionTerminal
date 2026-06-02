# ShellAccounting Manual Entry Post-Write Readback Refresh Authorization Gate

## Document Purpose

TASK-201 is a post-write readback / refresh authorization gate for the
ShellAccounting manual entry flow. It defines the boundary for future work that
may refresh read-only accounting views after successful manual transaction or
manual cash movement writes.

This task is authorization-only. It does not implement readback, refresh,
AccountingEngine replay integration, read model refresh, snapshot refresh,
UI auto-refresh, refresh buttons, audit writes, ledger writes, broker access,
network calls, credentials, endpoints, real order placement, or automatic
trading.

TASK-201 does not modify production QML, startup, Presenter, Controller,
ShellServices adapter / port code, DataServiceActions, DataServiceActionRegistrar,
DataAccess repositories, validation production code, migrations, schema files,
AccountingEngine replay, StrategyEngine, MarketEngine, or broker code.

## Current State

TASK-192 implemented DataAccess-only manual transaction repository writes.
TASK-196 implemented DataAccess-only manual cash movement dual-write repository
behavior. TASK-198 implemented DataService runtime write wiring for:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

TASK-200 implemented minimal production QML / Presenter manual entry wiring.
The UI can submit structured manual entries through Presenter / Controller /
ShellServices adapter / DataService action boundaries.

The current UI can show write success or failure status, but it does not
perform post-write readback. It does not refresh `position.list`,
`cash.summary`, or `portfolio.pnl.summary` after write success. It does not
trigger AccountingEngine replay, read model refresh, snapshot rebuild, derived
table updates, audit writes, ledger writes, broker order placement, strategy
execution, or automatic trading.

## Manual Transaction Post-Write Boundary

Future manual transaction success handling may trigger position, cash, and PnL
readback only after a separate implementation TASK authorizes it.

Future readback must go through a DataService read boundary or a separately
authorized replay/read-model boundary. QML, Presenter, and Controller must not
directly read SQLite, must not directly call DataAccess, and must not compute
accounting facts such as position, cash, or PnL in QML.

Manual transaction post-write refresh must not generate broker orders, strategy
orders, TradeDrafts, suggestions, real broker order ids, real order placement,
or automatic trading.

## Manual Cash Movement Post-Write Boundary

Future manual cash movement success handling may trigger cash and PnL readback
only after a separate implementation TASK authorizes it.

If future cash movement readback needs `position.list` or trade-log readback, it
must still use a DataService read boundary or a separately authorized replay/read
model boundary. QML, Presenter, and Controller must not directly read SQLite,
must not directly call DataAccess, and must not compute cash or PnL facts in QML.

Manual cash movement post-write refresh must not generate broker orders,
strategy orders, TradeDrafts, suggestions, real broker order ids, real order
placement, or automatic trading.

## Replay / Read Model Boundary

Future AccountingEngine replay after write must be separately authorized.
Future snapshot or read model refresh after write must be separately authorized.
Future UI auto-refresh and user-triggered refresh button behavior must be
separately authorized.

The current task has no silent refresh side effects. A successful manual entry
write remains a write result only; any later refresh must be explicit,
testable, fail-closed, and bounded by DataService read or separately authorized
replay boundaries.

## DataService Read Boundary Policy

Future post-write readback may use existing read actions such as
`position.list`, `cash.summary`, and `portfolio.pnl.summary` only through the
DataService boundary.

Future work must not expose a generic read/write escape hatch to QML,
Presenter, Controller, or ShellServices. Any new readback or refresh action
must be allowlisted, statically scannable, and covered by tests before it can be
used by production UI.

## No Direct UI Data Access Policy

QML must not directly access SQLite, DataAccess, repositories, AccountingEngine,
or raw SQL. Presenter and Controller must not directly access SQLite,
DataAccess, repositories, AccountingEngine, or raw SQL.

QML must not calculate accounting facts, including positions, cash balances,
PnL, realized gain, unrealized gain, portfolio summary, or derived facts. Those
facts must come from DataService read responses or separately authorized replay
read models.

## Audit / Ledger Policy

Future audit write or ledger policy changes after post-write readback must be
separately authorized. TASK-201 does not write `audit_log`, ledger rows, cash
facts, snapshot tables, `trade_log`, `trade_execution_group`, or
`cash_adjustment`.

## Broker And Trading Policy

First phase remains no real broker, no automatic trading, and no real order
placement. The no real order placement boundary remains unchanged.
Broker sandbox new capability development remains paused.

Existing broker disabled, fail-closed, no-real-order, no-network,
no-credentials, and no-order-placement gates must remain retained. TASK-201
must not introduce broker SDKs, network calls, credentials, endpoints, real
broker order ids, real order placement, StrategyEngine execution, MarketEngine
trading behavior, or automatic trading.

## Privacy Policy

Future readback diagnostics must not expose raw SQL, raw manual entry payloads,
credentials, endpoints, secrets, internal exception stacks, raw broker payloads,
raw trade-log payloads, or sensitive memo content.

UI-visible readback errors must be sanitized and must not imply successful
refresh when refresh failed or was not authorized.

## Rollback / Disable Policy

Future post-write readback and refresh must be disableable. Disabling refresh
must leave TASK-198 DataService write wiring and TASK-200 manual entry UI
status behavior intact. Disabled refresh must not fabricate readback data, must
not perform direct DB reads, must not trigger replay, and must not call broker
or strategy code.

## Future Implementation Requirements

Any future implementation PR for post-write readback / refresh must:

- reference this document and docs/199.
- start from a clean latest main branch.
- keep QML / Presenter / Controller away from direct SQLite and DataAccess.
- prove all readback uses DataService read boundary or separately authorized
  replay boundary.
- prove no QML accounting calculation is introduced.
- prove no audit / ledger write is introduced unless separately authorized.
- prove broker, network, credentials, endpoints, real order placement, and
  automatic trading remain absent.
- pass full CTest and the TASK-201 gate tests.
