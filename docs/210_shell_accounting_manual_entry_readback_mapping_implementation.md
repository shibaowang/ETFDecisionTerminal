# ShellAccounting Manual Entry Readback Mapping Implementation

## TASK-207 Purpose

TASK-207 implements the minimum DataService readback mapping for persisted
ShellAccounting manual entry facts. The implementation lets existing read
actions consume manual BUY / SELL and Deposit / Withdrawal facts after the
authorized write path has persisted them.

This task implements readback mapping only. It does not implement AccountingEngine replay,
snapshot rebuild, derived table refresh, audit / ledger write, backup/export,
packaging, broker SDK integration, network calls, credentials, endpoints, real
order placement, real broker order ids, or automatic trading.
Broker sandbox new capability development remains paused.
TASK-207 does not write audit or ledger rows.
TASK-207 does not connect broker SDK, network, credentials, or endpoints.

## Current State

TASK-206 completed the readback mapping authorization gate. TASK-192 and
TASK-196 provide DataAccess-only manual transaction and manual cash movement
repository writes. TASK-198 wires those repositories through DataService write
actions. TASK-200, TASK-202, and TASK-204 verify the minimum QML / Presenter
manual entry path and post-write readback boundary.

TASK-207 keeps QML, Presenter, Controller, ShellServices, write repositories,
migrations, and DataService action registration unchanged. The new behavior is
inside `DataServiceActions.cpp` read action handling for:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`

## DataService Readback Boundary

Manual entry readback mapping stays inside the DataService boundary. It reads
authorized persisted facts from the existing schema and does not call QML,
Presenter, Controller, ShellServices, DataServiceClient, SQLite from UI code, or
AccountingEngine replay.

The readback mapping uses `SELECT` queries only. It does not execute `INSERT`,
`UPDATE`, `DELETE`, or `REPLACE`, and it does not write `trade_log`,
`trade_execution_group`, `cash_adjustment`, `audit_log`, ledger rows, snapshots,
or derived tables.

## position.list Mapping Policy

`position.list` maps manual BUY / SELL facts from `trade_log` when the existing
read-only snapshot facts query has no rows and replay is not requested.

The mapping policy is:

- Include only `manual_entry = 1`, `trade_source = 'MANUAL'`, non-voided rows.
- Include manual `BUY` and `SELL` actions.
- Aggregate by account, portfolio, and instrument code.
- BUY increases quantity.
- SELL decreases quantity.
- Cost basis uses a simple persisted fact policy: BUY adds gross amount plus
  fee, and SELL subtracts gross amount plus fee.
- Market value and unrealized PnL remain `UNAVAILABLE`.
- Realized and unrealized PnL flags remain false.
- A safe readback issue explains that replay and market prices are not used.

## cash.summary Mapping Policy

`cash.summary` maps manual transaction cash impact and manual cash movement
dual-write facts from `trade_log`, linked to `cash_adjustment` for cash
movements.

The mapping policy is:

- BUY cash outflow is read from `trade_log.net_cash_impact_cents`.
- SELL cash inflow is read from `trade_log.net_cash_impact_cents`.
- Deposit and Withdrawal cash movements are read through the authorized
  `trade_log` to `cash_adjustment` linkage.
- Cash balance aggregates persisted net cash impact by account and portfolio.
- Principal base is the conservative sum of manual cash movement cash impacts.
- The mapping does not fake missing currency data or write any derived cash
  table.

## portfolio.pnl.summary Mapping Policy

`portfolio.pnl.summary` returns a safe manual-entry-aware partial summary when
manual readback rows exist and snapshot summary facts are absent.

The partial summary may expose:

- cash balance
- principal base
- manual position holding cost

The partial summary must not fabricate:

- market value
- realized PnL
- unrealized PnL
- total return
- market price dependent values

The response uses partial / unavailable status and explicit issues when replay,
pricing, or complete PnL inputs are unavailable. It must not silently report
full success for unsupported PnL.

## Response Safety

Readback responses must not expose raw SQL, raw payloads, credentials,
endpoints, tokens, stack traces, broker payloads, full trade logs, or real order
ids. Responses include safe privacy flags and issue codes instead of internal
implementation details.

Duplicate idempotency writes are persisted once by the authorized write
repositories, so readback aggregation must not double count duplicate requests.

## Out Of Scope

TASK-207 does not:

- modify production QML, startup, Presenter, Controller, or ShellServices
- modify DataServiceActionRegistrar
- modify write repositories
- modify `migrations/001_initial_schema.sql`
- modify `migrations/002_shell_accounting_manual_entry_schema.sql`
- add a migration or schema file
- implement AccountingEngine replay
- implement snapshot rebuild or derived table refresh
- write audit or ledger rows
- connect broker SDK, network, credentials, or endpoints
- place real orders or create real broker order ids
- enable automatic trading

Future replay, audit, ledger, backup/export, packaging, broker sandbox, real
broker order, strategy execution, and automatic trading work must remain
separately authorized.

TASK-208 has now added a daily-use acceptance authorization gate for this
readback mapping. TASK-208 does not modify production code or add runtime
daily-use acceptance implementation.

## Test Coverage

TASK-207 adds runtime and static acceptance tests for:

- docs/210 and docs/211 registration
- temporary SQLite 001 + 002 migration fixtures
- manual BUY / SELL / Deposit / Withdrawal write fixtures
- duplicate idempotency readback stability
- `position.list` manual quantity and SELL reduction mapping
- `cash.summary` Deposit, Withdrawal, BUY outflow, and SELL inflow mapping
- `portfolio.pnl.summary` safe partial / unavailable PnL policy
- response sanitization
- no readback writes after setup/action writes
- no QML, Presenter, Controller, ShellServices, repository, migration,
  registrar, replay, broker, network, credentials, endpoint, real order, or
  automatic trading drift
