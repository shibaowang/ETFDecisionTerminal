# ShellAccounting QML Startup Registration Wiring

## Document Purpose

TASK-129 only completes production startup registration wiring for the
ShellAccounting QML type module. This document does not authorize production
QML import, QML page binding, real accounting action implementation,
DataService integration, SQLite access, AccountingEngine access, TradeDraft,
trade suggestion, strategy execution, broker order, or order placement.

## Current State

- `ShellAccountingQmlRegistration` helper already exists.
- `ETFDecisionTerminal.ShellAccounting` 1.0 can be registered and imported in
  tests.
- TASK-127 completed the helper implementation.
- TASK-128 completed the startup registration gate.
- Production app startup adds one centralized registration call in TASK-129.
- Production QML still does not import the module.
- Production QML page binding is still not implemented.
- Real accounting action implementation is still not implemented.

## Startup Wiring Implementation Summary

TASK-129 adds only a centralized call to `registerShellAccountingQmlTypes`.
The registered module remains `ETFDecisionTerminal.ShellAccounting` 1.0 and the
registration remains limited to the read-only allowlist types. Startup wiring
is not QML page binding and is not real accounting action implementation.

The wiring adds no `DataServiceClient`, no SQLite, no `DataAccess`, no
`AccountingEngine`, no write path, no TradeDraft, and no trade suggestion.

## Allowed Registration Call

`registerShellAccountingQmlTypes` is the only allowed ShellAccounting startup
wiring call.

- Do not directly use `qmlRegisterType` or `qmlRegisterSingletonType` as a
  ShellAccounting replacement in startup.
- Do not directly register `DataServiceClient`.
- Do not directly register `AccountingEngine`.
- Do not directly register SQLite or `DataAccess`.
- Do not directly register adapters or concrete ports.
- Do not add a hidden registration path.
- Do not dynamically register from QML.

## Startup Location Policy

Registration location must be centralized and easy to scan. The call must stay
near existing app QML type registration when the project has such an entry
point. There must be no scattered startup registration, no plugin hidden registration,
and no test-only registration being used as production registration.

The current production startup call is in
`apps/ETFDecisionShell/src/main.cpp`, next to the existing `qmlRegisterType`
calls for the shell module.

## Read-only Allowlist Preservation

The TASK-127 allowlist remains unchanged:

- `ShellAccountingPresenter`
- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`

Registration remains `qmlRegisterUncreatableType`. There is no creatable
ShellAccounting type, no forbidden runtime object exposure, and no context
property exposure.

## Rollback / Disable Strategy

Startup wiring must be reversible. If registration breaks startup, revert the
centralized include and one helper call. If module import fails, disable
accounting module registration. Fallback must not call DataServiceClient
directly, must not fake data, must not write database, and must show
Unavailable or no accounting UI.

The rollback scope is limited to the include, one helper call, minimal CMake wiring
if any is needed, and TASK-129 tests/docs.

## Post-wiring Tests

- App startup / build.
- Module registration gate.
- QML static gate.
- QML binding smoke scaffold.
- QML runtime smoke CTest.
- QML type registration scaffold.
- QML type registration implementation gate.
- Startup registration gate.
- Startup registration wiring tests.
- Full CTest.
- `transport_local_socket_echo` 50 repeat.

## Future Production QML Binding Requirements

Future production QML import or page binding must be a separate explicitly
authorized task. That task must re-gate:

- production QML import.
- production QML page/component.
- presenter instance lifecycle.
- context exposure policy.
- Unavailable state behavior.
- no write path.
- no TradeDraft.
- no trading button.
