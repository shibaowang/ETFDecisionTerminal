# ShellAccounting Manual Entry MVP E2E Acceptance Authorization Gate

## Document Purpose

TASK-203 is the ShellAccounting manual entry MVP E2E acceptance authorization
gate. This is gate-only documentation and static test coverage for the local
assistant decision MVP manual entry chain.

The current task does not implement new functionality. TASK-203 does not modify
production code, does not write a database, does not connect replay, does not
write audit or ledger rows, does not connect broker, network, credentials, or
endpoint capability, does not place real orders, and does not enable automatic
trading.

TASK-203 authorization-only non-goals:

- does not modify production code.
- does not write audit or ledger rows.
- does not connect broker, network, credentials, or endpoint.
- does not enable automatic trading.

TASK-203 does not modify production QML, startup, Presenter, Controller,
ShellServices adapter / port code, DataServiceActions, DataServiceActionRegistrar,
DataAccess repositories, validation production code, migrations, schema files,
AccountingEngine replay, StrategyEngine, MarketEngine, broker code, or any
runtime E2E implementation.

## Current State

TASK-192 implemented manual transaction DataAccess repository writes. TASK-196
implemented manual cash movement DataAccess repository dual-write behavior.
TASK-198 implemented validation-first DataService runtime write wiring for:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

TASK-200 implemented the minimal ShellAccounting QML / Presenter manual entry
UI wiring. TASK-202 implemented post-write readback refresh through existing
DataService read boundaries after successful manual entry writes.

The current implemented chain is:

`QML -> Presenter -> Controller -> ShellServices adapter -> DataService action -> DataAccess repository`

Acceptance must prove no layer bypasses its boundary.

## Manual Transaction E2E Acceptance Boundary

Manual transaction MVP E2E acceptance must prove:

- QML form calls only Presenter.
- Presenter and Controller call only ShellServices adapter boundaries.
- ShellServices adapter calls only DataService action boundaries.
- DataService action calls only DataAccess repository boundary.
- Valid BUY / SELL payload can write `trade_execution_group` and `trade_log`.
- Invalid payload does not write database rows.
- Duplicate idempotencyKey does not duplicate writes.
- Sensitive memo handling is safe and sanitized.
- Write success can trigger DataService read boundary refresh.
- QML / Presenter / Controller do not directly access SQLite or DataAccess.
- No broker, network, credentials, endpoint, real order, or automatic trading
  side effects are allowed.

## Manual Cash Movement E2E Acceptance Boundary

Manual cash movement MVP E2E acceptance must prove:

- QML form calls only Presenter.
- Presenter and Controller call only ShellServices adapter boundaries.
- ShellServices adapter calls only DataService action boundaries.
- DataService action calls only DataAccess repository boundary.
- Valid Deposit / Withdrawal payload can dual-write `trade_log + cash_adjustment`.
- `cash_adjustment.trade_log_id` points to `trade_log.id`.
- Invalid payload does not write database rows.
- Duplicate idempotencyKey does not duplicate writes.
- Sensitive memo handling is safe and sanitized.
- Write success can trigger DataService read boundary refresh.
- QML / Presenter / Controller do not directly access SQLite or DataAccess.
- No broker, network, credentials, endpoint, real order, or automatic trading
  side effects are allowed.

## Daily-Use MVP Acceptance Boundary

Daily-use MVP acceptance must define and verify:

- Manual buy / sell records can be submitted.
- Deposit / Withdrawal cash movement records can be submitted.
- Success and failure status are visible.
- Validation issue messages are visible and safe.
- Duplicate and idempotent status are visible.
- Post-write readback refresh state is visible.
- Position, cash, and PnL read views refresh through DataService read boundary
  or remain safely unavailable.
- No silent success is allowed.
- Raw SQL, raw payload, credentials, endpoint values, and internal stack trace
  exposure are forbidden.
- Broker, order placement, and automatic trading side effects are forbidden.

## No Bypass Policy

TASK-203 acceptance must retain these boundaries:

- QML must not call DataServiceClient directly.
- QML must not read or write SQLite.
- QML must not import or call DataAccess.
- Presenter / Controller must not call DataAccess repositories.
- Presenter / Controller must not execute SQL.
- Presenter / Controller must not trigger AccountingEngine replay.
- DataServiceActions must remain the only DataService write action boundary for
  the existing manual entry actions.
- Runtime SQL must remain inside the authorized DataAccess repositories from
  TASK-192 and TASK-196.

## Forbidden Behavior

TASK-203 does not authorize:

- production QML changes.
- production startup changes.
- Presenter or Controller behavior changes.
- ShellServices adapter / port changes.
- DataServiceActions or DataServiceActionRegistrar changes.
- DataAccess repository changes.
- changes to `migrations/001_initial_schema.sql` or
  `migrations/002_shell_accounting_manual_entry_schema.sql`.
- new migrations or schema files.
- new runtime E2E implementation.
- new runtime SQL / SQLite write paths.
- new readback, refresh, read model refresh, or replay behavior.
- AccountingEngine replay integration.
- audit_log or ledger writes.
- TradeDraft or suggestion behavior.
- broker SDK, network, credentials, or endpoint integration.
- real order placement or storage of real broker order ids.
- automatic trading.
- deleting, weakening, or bypassing broker / real broker / no-network /
  no-credentials / no-order-placement gates.

## Future Task Split

Future full E2E implementation tests that need runtime DB fixtures beyond this
static gate must be a separate TASK. Future runtime E2E fixture tests,
acceptance implementation, audit write policy, ledger policy, real broker
capability, strategy execution, or automatic trading must be separately
authorized. Broker sandbox new capability development remains paused in this
phase, and existing broker gates remain required.

## TASK-204 Runtime E2E Acceptance Update

TASK-204 adds the separately authorized runtime E2E acceptance implementation
for the manual entry MVP chain. TASK-204 uses temporary SQLite databases,
applies migrations 001 / 002, and verifies synthetic manual BUY and Deposit
payloads through existing DataService action boundaries.

TASK-204 does not modify production code, does not add product functionality,
does not modify QML / Presenter / Controller / ShellServices, does not modify
DataServiceActions, repositories, or migrations, does not connect replay, does
not write audit or ledger rows, does not connect broker, network, credentials,
or endpoints, does not place real orders, and does not enable automatic trading.
