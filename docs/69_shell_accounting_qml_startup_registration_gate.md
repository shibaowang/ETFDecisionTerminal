# ShellAccounting QML Startup Registration Gate

TASK-128 owns this startup registration gate. It is a documentation and
test-only gate task, not production startup wiring.

## Document Purpose

This document defines the gate for a future task that wires the
`ShellAccountingQmlRegistration` helper into production app startup. This
document does not modify startup code, does not call the registration helper,
does not modify QML, and does not implement a QML page.

## Current Status

- `ShellAccountingQmlRegistration` exists.
- `ETFDecisionTerminal.ShellAccounting` 1.0 can be registered and imported in
  tests.
- TASK-127 completed the helper implementation.
- Production app startup does not call `registerShellAccountingQmlTypes`.
- Production QML does not import `ETFDecisionTerminal.ShellAccounting`.
- Production QML pages are not implemented for ShellAccounting.
- Real accounting actions are not implemented.

## Startup Wiring Implementation Gate Summary

Production startup wiring may begin only after explicit user authorization and
must be a separate task. Wiring must be centralized and easy to scan. Startup
wiring is not production QML page binding, is not real accounting action
implementation, does not expose `DataServiceClient`, SQLite, `DataAccess`, or
`AccountingEngine`, does not add write ability, and does not display trading
buttons.

## Required Pre-Gates

- TASK-119 QML static gate.
- TASK-121 QML binding smoke scaffold.
- TASK-122 runtime QML smoke CTest.
- TASK-125 QML type registration scaffold.
- TASK-126 QML type registration implementation gate.
- TASK-127 QML type registration helper implementation.
- Full CTest.
- `transport_local_socket_echo` 50 repeat.
- `git status` clean.

## Allowed Startup Call

The future task may only add a centralized call to
`registerShellAccountingQmlTypes` for module
`ETFDecisionTerminal.ShellAccounting` 1.0. The registration must remain limited
to read-only allowlist types.

Forbidden startup wiring includes direct registration of `DataServiceClient`,
`AccountingEngine`, SQLite, `DataAccess`, adapters, concrete ports, hidden
registration paths, or registration from QML.

## Startup Location Policy

- Registration location must be centralized.
- Registration must be easy to static scan.
- Registration should live near existing app QML type registration if the
  project has such an entry point.
- No scattered startup registration.
- No hidden plugin registration.
- No test-only registration used as production registration.
- The future location must be documented.

## Rollback / Disable Strategy

Startup wiring must be reversible. If registration breaks startup, revert the
centralized call. If module import fails, disable accounting module
registration. Fallback must not call `DataServiceClient`, must not fake data,
must not write any database table, and must show Unavailable or no accounting
UI.

## Required Post-Wiring Tests

- App startup smoke.
- Module import smoke.
- QML static gate.
- QML binding smoke scaffold.
- QML runtime smoke CTest.
- QML type registration scaffold.
- QML type registration implementation gate.
- No production QML page binding unless separately authorized.
- Full CTest.
- transport 50 repeat.

## PR Requirements For Future Startup Wiring

A future startup wiring PR must report:

- startup file changed.
- exact registration call.
- registered module name and version.
- allowlist types.
- confirmation that no forbidden type is registered.
- confirmation that no production QML page binding was added.
- rollback plan.
- all gate test results.
