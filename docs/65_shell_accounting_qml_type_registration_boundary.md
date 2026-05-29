# ShellAccounting QML Type Registration Boundary

## Document Purpose

This document defines the boundary for a future ShellAccounting QML type
registration task. It does not register QML types, modify production QML, or
implement a production QML page. It is a review document that must be referenced
before any ShellAccounting type is exposed to QML.

## Current State

- `ShellAccountingPresenter` exists.
- `ShellAccountingStatusObject` exists.
- `ShellAccountingIssueListModel` exists.
- `ShellPositionListModel` exists.
- TASK-119 QML static gate is complete.
- TASK-121 QML binding smoke scaffold is complete.
- TASK-122 runtime smoke CTest is complete.
- TASK-123 production QML binding boundary is complete.
- ShellAccounting QML types are not registered.
- Production accounting QML pages are not implemented.
- Real accounting actions are still not implemented.

## First-Stage Allowed Registration Candidates

The first registration stage may only consider these ShellServices read-only
objects:

- `ShellAccountingPresenter`
- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`

Registration must be read-only. The types may expose read-only properties,
read-only model roles, and read-only refresh affordances that already exist on
the presenter boundary. They must not expose write methods, trade methods,
`DataServiceClient`, `ShellAccountingDataServiceAdapter`, or
`ShellAccountingDataServiceClientPortAdapter`.

## Future But Not First-Stage Objects

These objects are future candidates only and are not part of the first-stage
registration unless a separate task explicitly authorizes them:

- `ShellCashSummaryObject`
- `ShellPortfolioPnlObject`
- `ShellBasePositionObject`
- `ShellSniperPoolObject`
- `ShellSniperTierListModel`

When these objects are eventually considered, they must remain read-only and
must not expose `TradeDraft`, trade suggestion, strategy execution, broker
order, or write behavior.

## Forbidden Registered Or Exposed Objects

Production QML type registration must not register or expose:

- `DataServiceClient`
- `DataServiceApi`
- `ShellAccountingDataServiceAdapter`
- `ShellAccountingDataServiceClientPortAdapter`
- `DataAccess`
- `SQLite` / `SQLiteConnection`
- `AccountingEngine`
- `AccountingReplayEngine`
- `AccountingTradeFactReader`
- `StrategyEngine`
- `MarketEngine`
- Broker / order API
- any write service

## Forbidden Methods And Properties

Registered ShellAccounting QML types must not expose:

- `buy`
- `sell`
- `createTradeDraft`
- `brokerOrder`
- `submitOrder`
- `strategyExecute`
- `executeStrategy`
- `data.audit.append`
- `writeEnabled: true`
- `writeAction`
- `snapshotWrite`
- `tradeLogWrite`
- `positionSnapshotWrite`
- `cashSnapshotWrite`
- `portfolioSummaryWrite`

## Registration Location Boundary

QML type registration can only be added by a task that explicitly authorizes
registration. Registration code must be centralized, easy to static scan, and
not scattered across unrelated startup paths. It must not expose
`DataServiceClient`, SQLite, `AccountingEngine`, `DataAccess`, adapter, or
concrete port objects to QML.

Any registration task must update the TASK-119 static gate allowlist and
denylist at the same time, so future production QML can be scanned without
weakening the no-direct-service and no-trade boundaries.

## QML Import And Module Boundary

The suggested future module name is `ETFDecisionTerminal.ShellAccounting`, unless
the project chooses a different existing module style in the registration task.

The module may provide only read-only ShellAccounting ViewModel / Model types.
It must not provide trade actions, write actions, broker access, strategy
execution, or direct DataService access.

## Required Gates After Registration

After any registration task, the following gates must pass:

- TASK-119 QML static gate.
- TASK-121 QML binding smoke scaffold.
- TASK-122 QML runtime smoke CTest.
- Production QML type registration smoke test.
- No direct `DataServiceClient` scan.
- No SQLite / `AccountingEngine` scan.
- No trade button scan.
- No write action scan.
- Full CTest.
- `transport_local_socket_echo` repeat 50.

## Rollback And Disable Strategy

QML type registration must be reversible. If registration fails, app startup must
not be broken. If the presenter is unavailable or unconfigured, QML must show
Unavailable through ShellServices state. It must not fallback to direct
`DataServiceClient`, fake data, SQLite queries, write actions, `TradeDraft`,
trade suggestions, strategy execution, or broker orders.

## TASK-125 Registration Scaffold

TASK-125 adds QML type registration scaffold CTests under
`tests/ShellAccountingQmlTypeRegistrationScaffold`. The scaffold automates the
first-stage allowlist, forbidden type denylist, forbidden method / property
denylist, module import boundary, TASK-119 / TASK-121 / TASK-122 gate
dependency, and no-production-change checks.

Production registration is still not implemented. No QML type is registered, no
production QML file is modified, and this scaffold does not authorize real
accounting actions, writes, TradeDraft, trade suggestions, broker orders, or
strategy execution.

## TASK-126 Implementation Gate

TASK-126 adds the implementation gate in
`docs/67_shell_accounting_qml_type_registration_implementation_gate.md` and the
rollback strategy in
`docs/68_shell_accounting_qml_type_registration_rollback_strategy.md`.
Registration remains not implemented. Future registration must pass the
implementation gate, location gate, allowlist gate, forbidden exposure gate,
rollback gate, and no-production-registration gate before any production type is
registered.

## TASK-127 Type Registration Implementation

TASK-127 implements the first-stage registration boundary. Only
`ShellAccountingPresenter`, `ShellAccountingStatusObject`,
`ShellAccountingIssueListModel`, and `ShellPositionListModel` are registered,
and they are registered as uncreatable read-only QML types under
`ETFDecisionTerminal.ShellAccounting` 1.0. Production QML binding is still not
implemented and forbidden service / engine / adapter / write / trade exposure
remains denied.
