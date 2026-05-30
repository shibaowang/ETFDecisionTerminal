# ShellAccounting Production Trading UI Implementation

## TASK-152 Purpose

TASK-152 implements the first production ShellAccounting trading UI surface for
Draft creation, Draft review, and Draft confirmation. TASK-151 authorization
gate is complete and remains the governing boundary.

This task exposes UI entry points only through the existing
presenter/controller/adapter/DataService boundary. It does not call broker
systems, does not execute strategies, does not enable automatic trading, does
not place orders, and does not modify schema or migrations.

## Production Trading UI Scope

The production UI is intentionally narrow:

- create draft
- review draft
- confirm draft
- ledger status

The UI distinguishes Draft-only state from confirmed-ledger state. Draft
creation is separate from confirmation, and confirmation requires a distinct
user step.

## DataService-only UI Boundary

QML calls only `ShellAccountingPresenter` methods. The presenter routes through
`ShellAccountingReadOnlyController`, `ShellAccountingDataServiceAdapter`, and
the DataService client port. QML does not directly call `DataServiceClient`,
SQLite, DataAccess, or database write paths.

Draft creation uses the authorized DataService action
`accounting.tradedraft.create`. Draft confirmation uses
`accounting.tradedraft.confirm`. The UI does not expose a generic write escape
hatch.

## Authorization Token Continuity

The existing backend authorization tokens remain required:

- `TASK-148_TRADEDRAFT_WRITE` for Draft creation.
- `TASK-150_TRADEDRAFT_CONFIRM` for Draft confirmation.

Authorization missing, disabled state, transport unavailable, Draft creation
failed, confirmation failed, stale draft, duplicate confirmation, audit
unavailable, and ledger write failed must surface as visible UI state. There is
no silent success.

## User Confirmation Policy

Draft creation and Draft confirmation are different actions. Confirmation is
not automatic after creation. The production UI requires an explicit review
checkbox before confirmation and displays both Draft-only and confirmed-ledger
state.

## Error Mapping Policy

The UI maps these cases to visible status / issue text:

- authorization missing
- trading UI disabled
- draft creation failed
- confirmation failed
- stale draft
- duplicate confirmation
- audit unavailable
- ledger write failed
- transport unavailable
- no silent success

## Privacy Policy

The UI must not display raw SQL. The privacy boundary is no raw SQL, no full trade_log payload, no full snapshot or draft payload, and no internal stack trace. Issue text must stay sanitized before reaching QML.

## Forbidden Behavior

TASK-152 does not authorize:

- broker order placement
- strategy execution
- automatic trading
- order placement
- direct QML database write
- direct QML DataServiceClient access
- raw UI payload becoming a ledger fact
- schema changes

## Rollback / Disable Strategy

Disable production trading UI and fallback to the read-only accounting page.
The disable production trading UI path must be reversible without backend data
changes.
Rollback must preserve existing read-only accounting, snapshot, audit,
TradeDraft, and confirmation backend behavior. Fallback must provide no fake draft, no fake confirmation, no broker fallback, no strategy fallback, and no silent success.

## Test Checklist

- production trading UI implementation CTest
- production trading UI authorization gate
- QML static gate
- production QML binding tests
- TradeDraft creation tests
- TradeDraft confirmation tests
- full CTest
- transport_local_socket_echo 50 repeat

## Future Scope

Broker order placement, StrategyEngine execution, automatic trading, and
additional trading workflow UI require separate TASK authorization.
