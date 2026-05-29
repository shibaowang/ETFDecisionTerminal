# ShellAccounting QML Type Registration Implementation Gate

## Document Purpose

This document defines the final gate before a future ShellAccounting QML type
registration implementation task. It does not register QML types, modify
production QML, or implement a production QML page. It exists for review before
real production registration starts. This TASK-126 document is a gate definition,
not production registration.

## Current State

- `docs/65_shell_accounting_qml_type_registration_boundary.md` is complete.
- `docs/66_shell_accounting_qml_type_registration_test_plan.md` is complete.
- TASK-125 registration scaffold is complete.
- TASK-119 static gate is complete.
- TASK-121 binding smoke scaffold is complete.
- TASK-122 runtime smoke CTest is complete.
- QML types are still not registered.
- Production QML is still not modified.
- Real accounting actions are still not implemented.

## Implementation Gate Summary

- QML type registration implementation can start only after 用户明确授权.
- Registration implementation must be a separate task.
- The first stage may only perform read-only registration.
- Registration is not a production QML page.
- Registration is not real accounting data.
- Registration is not write ability.
- Registration is not `TradeDraft`, strategy, or broker integration.

## Required Pre-Gates

- TASK-119 QML static gate.
- TASK-121 QML binding smoke scaffold.
- TASK-122 runtime QML smoke CTest.
- TASK-125 QML type registration scaffold.
- `docs/65_shell_accounting_qml_type_registration_boundary.md`.
- `docs/66_shell_accounting_qml_type_registration_test_plan.md`.
- Full CTest.
- `transport_local_socket_echo` repeat 50.
- `git status` clean.

## Registration Location Gate

- Registration location must be centralized.
- Registration must not be scattered across multiple modules.
- Registration must be easy to static scan.
- Registration location must be documented.
- Registration must have a no-forbidden-registration test.
- Registration must not be performed dynamically from QML files.
- Test-only helpers must not pretend to be production registration.
- Hidden plugin or service registration paths are not allowed.

## Read-Only Type Allowlist Gate

The first stage may only register:

- `ShellAccountingPresenter`.
- `ShellAccountingStatusObject`.
- `ShellAccountingIssueListModel`.
- `ShellPositionListModel`.

Allowed types may expose only read-only properties and read-only model roles.
They must not expose mutation methods, service methods, trade methods, adapters,
ports, or `DataServiceClient`.

## Forbidden Type Gate

Registration must not expose:

- `DataServiceClient`.
- `DataServiceApi`.
- `ShellAccountingDataServiceAdapter`.
- `ShellAccountingDataServiceClientPortAdapter`.
- `DataAccess`.
- `SQLite` / `SQLiteConnection`.
- `AccountingEngine`.
- `AccountingReplayEngine`.
- `AccountingTradeFactReader`.
- `StrategyEngine`.
- `MarketEngine`.
- Broker / order API.
- write service.

## Forbidden Method / Property Gate

Registration must not expose:

- `buy`.
- `sell`.
- `createTradeDraft`.
- `brokerOrder`.
- `submitOrder`.
- `strategyExecute`.
- `executeStrategy`.
- `data.audit.append`.
- `writeEnabled: true`.
- `writeAction`.
- `snapshotWrite`.
- `tradeLogWrite`.
- `positionSnapshotWrite`.
- `cashSnapshotWrite`.
- `portfolioSummaryWrite`.
- `setWriteEnabled`.
- `enableTrading`.
- `submitTrade`.
- `placeOrder`.
- `generateSuggestion`.

## Module / Import Gate

The suggested future module name is `ETFDecisionTerminal.ShellAccounting`. The
module may only contain read-only ShellAccounting objects. It must not provide
DataService access, SQLite access, AccountingEngine access, write actions, or
trade actions. Module import smoke must be tested by the registration task.

## Post-Registration Required Tests

- QML type registration smoke.
- Module import smoke.
- Type availability smoke.
- Property denylist smoke.
- Method denylist smoke.
- TASK-119 static gate.
- TASK-121 scaffold.
- TASK-122 runtime smoke.
- TASK-125 registration scaffold.
- Full CTest.
- transport 50 repeat.

## Rollback / Disable Gate

Registration must be reversible. The future task must provide a disable,
feature-flag, compile-time switch, or clearly documented revert path. After
disable, the app must still start. QML must not fallback to `DataServiceClient`,
fake data, SQLite, write actions, `TradeDraft`, trade suggestions, strategy
execution, or broker order. Disabled accounting UI must show Unavailable or not
show accounting UI. Rollback must not write to any database table.

## PR Requirements For Future Registration Implementation

A future registration implementation PR must report:

- registration location.
- registered type list.
- confirmation that no forbidden type is registered.
- confirmation that no forbidden method is exposed.
- QML static gate result.
- QML smoke result.
- no-write / no-trade result.
- rollback strategy.
- full CTest result.
- transport 50 repeat result.
