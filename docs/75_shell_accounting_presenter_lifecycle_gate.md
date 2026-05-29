# ShellAccounting Presenter Lifecycle Gate

TASK-132 defines the gate for a future ShellAccountingPresenter lifecycle and
QML context exposure implementation.

## Document Purpose

This document defines the boundary before any future
ShellAccountingPresenter lifecycle / context exposure implementation. This
document does not modify production startup, does not modify production QML,
does not create presenter, does not setContextProperty, and does not implement
real accounting action.

## Current State

- Production startup has registered `ETFDecisionTerminal.ShellAccounting 1.0`.
- Production QML has a read-only shell page.
- `ShellAccountingReadOnlyPage.qml` declares a nullable `accountingPresenter`
  property.
- Current `accountingPresenter` remains null.
- `ShellAccountingPresenter` has not been created by production startup.
- Context property exposure has not happened.
- DataService, SQLite, AccountingEngine, and TradeDraft are not connected.

## Presenter Lifecycle Gate Summary

- Only explicit user authorization can start presenter creation.
- Presenter lifecycle implementation must be a separate task.
- Presenter owner must be explicit.
- Presenter destruction order must be explicit.
- QML context property name must be explicit.
- QML exposure must be read-only.
- Unavailable / null state must remain usable.
- Lifecycle must not introduce a write path.
- Lifecycle must not introduce TradeDraft or trading UI.
- Presenter lifecycle is not real accounting action.

## Required Pre-gates

- TASK-119 QML static gate.
- TASK-121 QML binding smoke scaffold.
- TASK-122 runtime QML smoke CTest.
- TASK-125 QML type registration scaffold.
- TASK-126 QML type registration implementation gate.
- TASK-127 QML type registration helper implementation.
- TASK-128 startup registration gate.
- TASK-129 startup wiring implementation.
- TASK-130 production QML binding gate.
- TASK-131 production QML binding shell.
- Full CTest.
- `transport_local_socket_echo` 50 repeat.
- `git status` clean.

## Future Presenter Creation Policy

- Presenter creation location must be centralized.
- Creation must be easy to scan.
- Ownership must be explicit.
- No hidden singleton.
- No anonymous heap allocation without owner.
- No QML-created presenter.
- No presenter creation in QML.
- No presenter creation in tests being reused as production path.

## Future Context Exposure Policy

- Context property name must be explicitly authorized.
- Recommended name: `accountingPresenter`.
- No anonymous context property.
- No broad service context exposure.
- No DataServiceClient context exposure.
- No concrete adapter context exposure.
- No write-enabled object exposure.
- QML must tolerate null presenter.

## No Real Accounting Action Policy

- Presenter lifecycle does not authorize DataServiceClient.
- Presenter lifecycle does not authorize SQLite.
- Presenter lifecycle does not authorize AccountingEngine.
- Presenter lifecycle does not authorize TradeDraft.
- Presenter lifecycle does not authorize trading UI.
- Real data adapter must be a separate task.
- Context exposure is not DataService / SQLite / AccountingEngine integration.

## Rollback / Disable Strategy

The presenter lifecycle rollback path must be explicit and limited.

- Presenter exposure must be reversible.
- If presenter construction breaks startup, disable construction.
- If context exposure breaks QML, remove context property.
- Fallback must keep `accountingPresenter` null.
- Fallback must not fake data.
- Fallback must not call DataServiceClient.
- Fallback must not write database.
- Fallback must not generate TradeDraft.
- Fallback must not show trading UI.
- Rollback scope is limited to future presenter construction, context property,
  tests, and docs.

## Required Post-lifecycle Tests

- App startup smoke.
- Context property scan.
- QML null-presenter smoke.
- No DataServiceClient / SQLite / AccountingEngine scan.
- No write token scan.
- No TradeDraft / trading UI scan.
- Rollback policy scan.
- Full CTest.
- Transport 50 repeat.

## PR Requirements For Future Lifecycle Implementation

A future presenter lifecycle PR must report:

- Presenter owner.
- Creation location.
- Destruction order.
- Context property name.
- QML binding location.
- Unavailable / null behavior.
- No DataServiceClient.
- No SQLite.
- No AccountingEngine.
- No write path.
- No TradeDraft.
- Rollback plan.
- All gate test results.

## TASK-133 Authorized Lifecycle Implementation

TASK-133 implements the authorized presenter lifecycle and context exposure.
`ShellAccountingPresenter` is created once in `apps/ETFDecisionShell/src/main.cpp`
and exposed once as `accountingPresenter`. This remains read-only and
Unavailable-safe: real accounting action, DataServiceClient, SQLite,
AccountingEngine, database writes, TradeDraft, trade suggestion, and trading UI
are still not implemented.
