# ShellAccounting Production QML Binding Implementation

TASK-131 adds the first production QML binding shell for ShellAccounting.
This document records the implementation boundary and rollback scope.

## Document Purpose

This document records the TASK-131 production QML binding implementation.
It does not authorize real accounting action implementation, DataService access,
SQLite access, AccountingEngine access, database writes, TradeDraft generation,
trade suggestions, or order placement.

## TASK-131 Scope

TASK-131 implements only a read-only, unavailable-safe production QML shell:

- A single production QML import of `ETFDecisionTerminal.ShellAccounting 1.0`.
- A minimal `ShellAccountingReadOnlyPage` component.
- Existing navigation/page loading integration for that component.
- A nullable presenter property only.
- Disabled and Unavailable UI behavior when no presenter lifecycle is wired.

## Modified Production QML Files

TASK-131 modifies these production QML files:

- `apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml`
- `apps/ETFDecisionShell/qml/layout/ContentHost.qml`

The page is listed in `apps/ETFDecisionShell/CMakeLists.txt` so the existing
QML module packaging sees it.

## Authorized Import Location

The only authorized production QML import is in:

```qml
apps/ETFDecisionShell/qml/pages/ShellAccountingReadOnlyPage.qml
import ETFDecisionTerminal.ShellAccounting 1.0
```

No other production QML file may import this module in TASK-131.

## Read-only Unavailable-safe Behavior

`ShellAccountingReadOnlyPage.qml` is a shell. It shows:

- `UNAVAILABLE` when no presenter is provided.
- Read-only status text.
- Disabled state text.
- No amount rows and no fake data.
- No runtime action controls.

The page does not call services, does not access a database, and does not
perform refresh or mutation.

## Presenter Property

The page declares only:

```qml
property ShellAccountingPresenter accountingPresenter: null
```

This nullable property is for future lifecycle wiring. TASK-131 does not
create a presenter and does not bind a real presenter instance.

## Explicit Non-goals

TASK-131 does not:

- create `ShellAccountingPresenter`.
- call `setContextProperty("accountingPresenter", ...)`.
- expose `ShellAccountingPresenter` through QML context.
- expose `ShellAccountingReadOnlyController`.
- expose `ShellAccountingDataServiceAdapter`.
- expose `ShellAccountingDataServiceClientPortAdapter`.
- call DataServiceClient.
- access SQLite.
- call AccountingEngine.
- write any database table.
- generate TradeDraft.
- generate trade suggestions.
- show trading buttons.
- implement real `position.list`, `cash.summary`, `portfolio.pnl.summary`,
  `base_position.summary`, or `sniper_pool.summary`.

## Navigation Integration

The existing navigation pattern is used:

- `ShellPageRegistry.cpp` adds the `shell_accounting` page metadata.
- `ContentHost.qml` maps `ShellAccountingReadOnlyPage` to the new component.

No new navigation architecture is introduced.

## Tests

TASK-131 adds production QML binding implementation CTests covering:

- authorized import.
- authorized page/component.
- unavailable state.
- no context exposure.
- no forbidden runtime access.
- no trading UI.
- rollback readiness.

TASK-130 gate tests are evolved from "no import/page yet" to "only the
authorized TASK-131 import/page is allowed".

## Rollback

The rollback scope is intentionally small:

- remove ShellAccountingReadOnlyPage.qml.
- remove the ContentHost mapping.
- remove the shell_accounting registry entry.
- remove the QML file list entry.
- remove TASK-131 docs/tests.

Fallback must not call DataServiceClient, must not fake data, must not write,
must not generate TradeDraft, and must not show trading UI.

## Next Stages

Future tasks must be separate and explicitly authorized for:

- presenter lifecycle creation.
- presenter context exposure.
- real data adapter connection.
- real accounting actions.
- any QML display backed by real accounting payloads.

Those tasks must keep no-write, no-TradeDraft, no trading UI, and unavailable
fallback gates in place.

## TASK-132 Presenter Lifecycle Gate

TASK-132 adds the gate for future presenter lifecycle and context exposure. The
TASK-131 shell still does not create `ShellAccountingPresenter`, does not call
`setContextProperty`, and does not bind a real presenter. Its nullable
`accountingPresenter` property must continue to render an Unavailable / disabled
state until a separate authorized lifecycle task changes that boundary.
