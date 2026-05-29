# ShellAccounting Production QML Binding Gate

TASK-130 owns this production QML binding gate.

## Document Purpose

This document defines the gate before any future ShellAccounting production QML
import or page binding task. This document does not modify production QML, does
not add a QML import, does not create a QML page/component, does not bind
`accountingPresenter`, does not expose `ShellAccountingPresenter`, and does not
implement real accounting action.

## Current Status

- `ShellAccountingQmlRegistration` helper exists.
- `ETFDecisionTerminal.ShellAccounting` 1.0 is registered in production
  startup.
- TASK-129 completed centralized startup wiring.
- Production QML has not imported the module.
- Production QML page binding is not implemented.
- `accountingPresenter` is not bound.
- Real accounting action implementation is not implemented.
- DataService, SQLite, AccountingEngine, and TradeDraft are not integrated.

## Production QML Binding Gate Summary

Production QML may be modified only after explicit user authorization.
Production QML binding must be a separate implementation task. Production QML
import must be centralized and easy to static scan, and any page/component must
remain read-only. Production QML binding is not real accounting action and is
not trading UI. In short: production QML binding 不等于 real accounting action.

Production QML binding must not introduce write ability, must not show trading
buttons, and must not bypass Unavailable or disabled state behavior.

## Required Pre-gates

- TASK-119 QML static gate.
- TASK-121 QML binding smoke scaffold.
- TASK-122 runtime QML smoke CTest.
- TASK-125 QML type registration scaffold.
- TASK-126 QML type registration implementation gate.
- TASK-127 QML type registration helper implementation.
- TASK-128 startup registration gate.
- TASK-129 startup registration wiring implementation.
- Full CTest.
- `transport_local_socket_echo` 50 repeat.
- `git status` clean.

## Allowed Future QML Import Policy

The only future allowed import is:

```qml
import ETFDecisionTerminal.ShellAccounting 1.0
```

That import may appear only in explicitly authorized production QML files. It
must not be broadly added across unrelated QML files, must not live in hidden
plugin paths, must not be dynamic, and must not be paired with trading-related
QML controls or write-enabled QML actions.

## Allowed Future Page / Component Policy

A future ShellAccounting page/component must be read-only. It must show
Unavailable if the presenter is unavailable, must not expose trade action, must
not generate TradeDraft, must not call services directly, must not access the
database, and must not bypass the C++ presenter/controller boundary.

## Future Presenter / Context Policy

`ShellAccountingPresenter` lifecycle must be explicitly authorized in a future
task. Any QML context exposure must be explicitly named and test-covered. There
must be no anonymous context properties, no `DataServiceClient` exposure, no
concrete adapter exposure, no write-enabled object exposure, and no ownership
ambiguity.

## Rollback / Disable Strategy

Production QML binding must be reversible. If QML import breaks startup, remove
the import and page route. If the presenter is unavailable, show Unavailable.
Fallback must not call DataServiceClient directly, must not fake data, must not write
database, must not generate TradeDraft, and must not show trade UI. It must not write database.

The rollback scope for a future binding task must be limited to the QML import,
page route, context binding, and that task's tests/docs.

## Required Post-binding Tests

- QML static gate.
- Production QML import scan.
- Production page smoke.
- No trade UI scan.
- No write token scan.
- No DataServiceClient / SQLite / AccountingEngine scan.
- Unavailable state smoke.
- Context exposure scan.
- Rollback policy scan.
- Full CTest.
- transport 50 repeat.

## PR Requirements For Future Binding Implementation

A future production QML binding PR must report:

- changed QML files.
- exact import location.
- exact page/component route.
- presenter/context lifecycle.
- Unavailable state behavior.
- confirmation that there is no write path.
- confirmation that there is no TradeDraft.
- confirmation that there is no trading UI.
- rollback plan.
- all gate test results.

## TASK-131 Authorized Binding Shell

TASK-131 has implemented the first authorized production QML binding shell.
The gate now permits exactly one production QML import:

```qml
import ETFDecisionTerminal.ShellAccounting 1.0
```

That import is authorized only in
`apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml`. The page may
declare only `property ShellAccountingPresenter accountingPresenter: null`.
It remains read-only, disabled, and Unavailable-safe. It does not create a
presenter, does not expose a context property, does not call DataServiceClient,
does not access SQLite, does not call AccountingEngine, does not write a
database table, does not generate TradeDraft or trade suggestions, and does not
show trading UI.
