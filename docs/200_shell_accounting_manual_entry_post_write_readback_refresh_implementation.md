# ShellAccounting Manual Entry Post-Write Readback Refresh Implementation

## Document Purpose

TASK-202 implements the minimal ShellAccounting manual entry
post-write readback refresh path authorized by TASK-201.

This task refreshes only through existing DataService read boundaries after a
successful manual transaction or manual cash movement write. It does not
implement AccountingEngine replay integration, read model refresh, snapshot
refresh, snapshot rebuild after write, derived table refresh after write, audit
writes, ledger writes, broker access, network calls, credentials, endpoints,
real order placement, or automatic trading.

## Current State

TASK-192 implemented manual transaction DataAccess repository writes. TASK-196
implemented manual cash movement DataAccess dual-write repository behavior.
TASK-198 wired DataService write actions:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

TASK-200 wired the production QML / Presenter manual entry UI. TASK-201 added
the post-write readback / refresh authorization gate.

## Implementation Scope

TASK-202 adds Presenter-visible post-write refresh state:

- `postWriteRefreshEnabled`
- `postWriteRefreshBusy`
- `lastPostWriteRefreshStatus`
- `lastPostWriteRefreshIssue`
- `lastPostWriteRefreshSummary`

QML displays the status through stable object names:

- `shellAccountingPostWriteRefreshStatus`
- `shellAccountingPostWriteRefreshIssue`
- `shellAccountingPostWriteRefreshSummary`

The write result remains separate from the refresh result. A refresh failure
must not convert a successful write into silent failure.

## Manual Transaction Refresh

After successful `accounting.manual_transaction.create`, the Presenter triggers
readback through the Controller using existing DataService read actions:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`

Validation failures and write failures do not trigger post-write readback.

## Manual Cash Movement Refresh

After successful `accounting.manual_cash_movement.create`, the Presenter
triggers readback through the Controller using existing DataService read
actions:

- `cash.summary`
- `portfolio.pnl.summary`

Validation failures and write failures do not trigger post-write readback.

## DataService Read Boundary

TASK-202 does not add DataService actions. It does not modify
DataServiceActions, DataServiceActionRegistrar, DataAccess repositories,
migrations, or validation production code. TASK-202 does not modify DataServiceActions.
TASK-202 does not modify DataServiceActionRegistrar. TASK-202 does not modify
DataAccess repositories.

QML, Presenter, and Controller must not directly read SQLite, must not directly
write SQLite, must not call DataAccess repositories, and must not scatter raw
SQL. QML must not calculate position, cash, PnL, realized gain, unrealized
gain, or derived accounting facts.

## Error Mapping

Refresh status is explicit:

- `READY`
- `REFRESHING`
- `OK`
- `EMPTY`
- `WARNING`
- `STALE`
- `ERROR`
- `UNAVAILABLE`
- `DISABLED`

Refresh issues are safe UI-visible messages. Raw SQL, raw manual entry
payloads, credentials, endpoints, tokens, secrets, internal exception stacks,
raw broker payloads, raw trade-log payloads, and sensitive memo content must
not be exposed.

## Forbidden Behavior

TASK-202 does not:

- TASK-202 does not trigger AccountingEngine replay.
- implement read model refresh.
- implement snapshot refresh.
- implement snapshot rebuild after write.
- implement derived table refresh after write.
- write `audit_log` or ledger rows.
- add TradeDraft or suggestion behavior.
- modify DataServiceActions or DataServiceActionRegistrar.
- modify DataAccess repositories.
- modify migrations or add schema files.
- connect broker SDKs, network calls, credentials, or endpoints.
- place real orders or store real broker order ids.
- execute StrategyEngine or MarketEngine trading behavior.
- enable automatic trading.

## Rollback / Disable Strategy

Post-write refresh remains isolated from manual entry writes. Disabling refresh
must leave TASK-198 DataService write wiring and TASK-200 manual entry UI write
status intact. Disabled refresh must not fabricate readback data, must not
trigger replay, must not read SQLite directly, and must not call broker or
strategy code.

## Test Checklist

TASK-202 is covered by:

- `shell_accounting_manual_entry_post_write_readback_refresh_implementation`
- TASK-201 authorization gate regression.
- TASK-200 QML Presenter implementation regression.
- TASK-198 DataService write wiring implementation regression.
- TASK-196 manual cash movement repository regression.
- TASK-192 manual transaction repository regression.
- broker disabled / broker order / real broker authorization / real broker
  implementation gates.
- full CTest and transport local socket echo 50 repeat.

## TASK-203 Acceptance Gate Update

TASK-203 adds a gate-only manual entry MVP E2E acceptance authorization layer
on top of this implementation. It does not add new TASK-202 behavior and does
not modify QML, Presenter, Controller, ShellServices, DataServiceActions,
DataAccess repositories, migrations, replay, audit, ledger, broker, network,
credentials, endpoints, real order placement, or automatic trading.

Future AccountingEngine replay, read model refresh, snapshot refresh,
snapshot rebuild, audit write, ledger policy, broker capability, real order
placement, or automatic trading must be separately authorized.
