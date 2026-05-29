# ShellAccounting Presenter Lifecycle Implementation

TASK-133 wires the minimum production-owned ShellAccountingPresenter lifecycle
and QML context exposure needed by the read-only ShellAccounting QML shell.

## Document Purpose

This document records the TASK-133 implementation boundary. It does not
authorize real accounting action, DataServiceClient access, SQLite access,
AccountingEngine access, database writes, TradeDraft generation, trade
suggestions, strategy execution, broker submission, or trading UI.

## TASK-133 Scope

TASK-133 only adds:

- centralized `ShellAccountingPresenter` creation.
- explicit presenter ownership and lifetime.
- explicit `accountingPresenter` QML context property exposure.
- minimal QML binding from the context property to the existing
  `ShellAccountingReadOnlyPage.qml` nullable presenter property.

## Presenter Creation Location

The authorized production creation location is
`apps/ETFDecisionShell/src/main.cpp`.

The startup function creates:

- `std::shared_ptr<ShellAccountingReadOnlyController> shellAccountingController`.
- stack-owned `ShellAccountingPresenter shellAccountingPresenter`.

No QML file creates a presenter, no hidden singleton is introduced, and no raw
`new ShellAccountingPresenter` allocation is used.

## Presenter Owner

`shellAccountingPresenter` is stack-owned by the production startup scope in
`main.cpp`. The controller is owned through `std::shared_ptr` and is assigned to
the presenter through `setController`.

No controller, adapter, concrete port, DataServiceClient, SQLite, DataAccess,
or AccountingEngine object is exposed through QML context.

## Destruction / Lifetime Policy

`ShellAccountingPresenter shellAccountingPresenter` is declared before
`QQmlApplicationEngine engine`. `QQmlApplicationEngine is created after the presenter`,
so C++ destruction order means `engine is destroyed before the presenter`.
This makes the presenter lifetime cover QML object usage and avoids a dangling
context pointer during engine teardown.

## Context Property

The only authorized context property is:

```cpp
engine.rootContext()->setContextProperty("accountingPresenter", &shellAccountingPresenter);
```

The context property name is `accountingPresenter`. It appears exactly once in
production startup. It exposes only `ShellAccountingPresenter`; it does not
expose `ShellAccountingReadOnlyController`, `ShellAccountingDataServiceAdapter`,
`ShellAccountingDataServiceClientPortAdapter`, DataServiceClient, SQLite,
DataAccess, or AccountingEngine.

## QML Binding Location

The binding path is:

- `Main.qml` reads the `accountingPresenter` context property.
- `AppShell.qml` carries it as a required property.
- `ContentHost.qml` passes it to `ShellAccountingReadOnlyPage.qml`.
- `ShellAccountingReadOnlyPage.qml` keeps
  `property ShellAccountingPresenter accountingPresenter: null`.

The page still tolerates `accountingPresenter` being null.

## Unavailable / Null Behavior

The ShellAccounting page remains read-only and Unavailable-safe. When the
presenter is present, the page still displays `UNAVAILABLE / READ ONLY` because
real accounting data is not connected. Fallback must not fake data, must not
call DataServiceClient, must not access SQLite, must not access a DB, must not
generate TradeDraft, and must not show trading UI.

## Production-safe Unavailable Adapter

TASK-133 does not add or wire a live adapter. The presenter uses a
`ShellAccountingReadOnlyController` with no service adapter configured. This is
the production-safe unavailable path: refresh attempts remain controlled
Unavailable and do not call DataServiceClient, SQLite, DataAccess, or
AccountingEngine.

No new adapter file is added.

## Explicit Non-goals

TASK-133 does not:

- call DataServiceClient.
- access SQLite.
- call AccountingEngine.
- connect DataAccess.
- read or write `trade_log`.
- write any database table.
- generate snapshot data.
- generate TradeDraft.
- generate trade suggestions.
- execute strategy.
- submit broker orders.
- show trading buttons.
- modify DataServiceApi or DataServiceClient behavior.
- modify `migrations/001_initial_schema.sql`.

presenter lifecycle is not real accounting action. context exposure is not DataService / SQLite / AccountingEngine integration.

## Rollback

The rollback scope is intentionally small:

- remove the `ShellAccountingPresenter` and controller construction from
  `main.cpp`.
- remove the single `accountingPresenter` context property.
- remove the QML property plumbing through `Main.qml`, `AppShell.qml`, and
  `ContentHost.qml`.
- remove TASK-133 tests and docs.

Rollback can disable context exposure while keeping the read-only QML page in
its null-presenter state. Fallback must not call DataServiceClient, must not
fake data, must not write database tables, must not generate TradeDraft, and
must not show trading UI.

## Next Stage

Connecting a real data adapter, DataService action, SQLite-backed facts query,
AccountingEngine replay, snapshot write path, TradeDraft generation, trade
suggestion, strategy execution, or broker order flow requires a separate
explicit TASK with its own gates.
