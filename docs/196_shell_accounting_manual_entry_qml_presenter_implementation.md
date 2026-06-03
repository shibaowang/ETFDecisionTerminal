# ShellAccounting Manual Entry QML Presenter Implementation

## Document Purpose

TASK-200 implements the ShellAccounting manual entry QML / Presenter wiring
authorized by TASK-199. The implementation exposes minimal manual transaction
and manual cash movement forms in the production ShellAccounting page and routes
submissions only through ShellServices into DataService actions.

This task does not modify migrations, does not modify DataServiceActions or
DataServiceActionRegistrar, does not modify DataAccess repositories, does not
trigger AccountingEngine replay, does not write audit or ledger rows directly,
does not add broker SDKs, network calls, credentials, endpoints, real order
placement, or automatic trading.
TASK-200 does not trigger AccountingEngine replay.

## Current State

TASK-198 already implemented validation-first DataService write wiring for:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

TASK-200 connects production QML to those actions through the existing
Presenter / Controller / adapter / DataService boundary. QML does not call
DataServiceClient directly and does not access SQLite or DataAccess directly.

## Production UI Scope

The production ShellAccounting page now includes:

- a manual BUY / SELL transaction form.
- a Deposit / Withdrawal cash movement form.
- safe status, issue, and result text for the last manual entry submission.

The UI remains a controlled ShellAccounting entry point. It does not expose
broker order placement, strategy execution, automatic trading, or direct
database controls.

## Presenter And Controller Boundary

`ShellAccountingPresenter` exposes invokable methods for structured
submissions:

- `submitManualTransaction(...)`
- `submitManualCashMovement(...)`

The Presenter performs minimal UI-level shape checks, then passes structured
requests to `ShellAccountingReadOnlyController`. The Controller delegates to the
configured `ShellAccountingServiceAdapter`. The DataService adapter builds
DataService payloads and calls only:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

## Response Mapping

The UI exposes:

- `manualEntryBusy`
- `lastManualEntryStatus`
- `lastManualEntryIssue`
- `lastManualEntryResult`

The response text may show sanitized write status, validation issues,
repository issues, duplicate / idempotent state, `databaseWritten`,
`tradeLogWritten`, and `cashAdjustmentWritten` flags when the DataService
payload provides them.

Failure states are explicit and must not be silent success.

## Privacy And Safety Policy

TASK-200 UI and ShellServices code must not expose raw SQL, raw payloads,
credentials, endpoints, tokens, secrets, internal stack traces, broker payloads,
real broker order ids, or real order ids.

The implementation must not call DataAccess repositories from QML, Presenter,
or Controller. It must not write SQLite directly from QML, Presenter, or
Controller.

## Forbidden Behavior

TASK-200 does not:

- modify `migrations/001_initial_schema.sql`.
- modify `migrations/002_shell_accounting_manual_entry_schema.sql`.
- add a migration or schema file.
- modify DataServiceActions, DataServiceActionRegistrar, or repositories.
- scatter SQL in ShellServices.
- trigger AccountingEngine replay or read model refresh.
- write `audit_log` or ledger rows directly.
- generate TradeDraft or trade suggestions.
- connect broker SDKs, network calls, credentials, or endpoints.
- place real orders or store real broker order ids.
- execute StrategyEngine or MarketEngine trading behavior.
- enable automatic trading.

## Rollback / Disable Strategy

If manual entry UI wiring must be disabled, the ShellAccounting page can retain
the read-only accounting and existing TradeDraft sections while hiding or
disabling the manual entry section. Disabling manual entry UI must not affect
TASK-198 DataService action behavior or DataAccess repository behavior.

## Test Checklist

TASK-200 is covered by:

- `shell_accounting_manual_entry_qml_presenter_implementation`
- TASK-199 authorization gate regression.
- TASK-198 DataService write wiring implementation regression.
- TASK-196 manual cash movement repository regression.
- TASK-192 manual transaction repository regression.
- broker disabled / broker order / real broker authorization / real broker
  implementation gates.
- full CTest and transport local socket echo 50 repeat.

Future readback, refresh, replay integration, audit UI, ledger UI, broker
capability, real order placement, or automatic trading must be separately
authorized.

## TASK-201 Authorization Gate Update

TASK-201 adds the post-write readback / refresh authorization gate for the
manual entry flow. TASK-200 manual entry QML / Presenter wiring remains
unchanged.

Post-write readback, refresh, AccountingEngine replay integration, read model
refresh, snapshot refresh, UI auto-refresh, refresh buttons, audit writes,
ledger writes, broker access, network calls, credentials, endpoints, real
order placement, and automatic trading remain unimplemented unless separately
authorized by a later TASK.
