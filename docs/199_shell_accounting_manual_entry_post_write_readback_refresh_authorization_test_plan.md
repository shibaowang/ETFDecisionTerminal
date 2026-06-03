# ShellAccounting Manual Entry Post-Write Readback Refresh Authorization Test Plan

## Document Purpose

This document defines the TASK-201 test plan for the ShellAccounting manual
entry post-write readback / refresh authorization gate.

TASK-201 is gate-only. The tests are static documentation, source, and changed
path checks. They must not write a database, trigger network access, call
broker code, run replay, or depend on real data.

## Test Matrix

### Documentation And Registration

- docs/198 exists.
- docs/199 exists.
- README, docs/README, and docs/12 register TASK-201.
- docs/196 and docs/197 mention the TASK-201 authorization gate.
- tests/CMake registers `ShellAccountingManualEntryPostWriteReadbackRefreshAuthorizationGate`.

### Authorization-Only Policy

- docs/198 states this task is authorization-only.
- docs/198 states the current task does not implement readback.
- docs/198 states the current task does not implement refresh.
- docs/198 states the current task does not implement replay.
- docs/198 states future readback / refresh implementation must be a separate
  TASK.
- docs/198 states future replay / read model refresh must be a separate TASK.
- docs/198 states future audit / ledger policy must be a separate TASK.

### Manual Transaction Boundary

- docs/198 references `accounting.manual_transaction.create`.
- Future manual transaction success handling may refresh position, cash, and
  PnL only through DataService read boundaries or separately authorized replay
  boundaries.
- QML / Presenter / Controller must not directly read SQLite or DataAccess.
- QML must not calculate position, cash, or PnL facts.
- Manual transaction post-write handling must not generate broker orders,
  strategy execution, real orders, TradeDrafts, suggestions, or automatic
  trading.

### Manual Cash Movement Boundary

- docs/198 references `accounting.manual_cash_movement.create`.
- Future manual cash movement success handling may refresh cash and PnL only
  through DataService read boundaries or separately authorized replay
  boundaries.
- If position or trade-log readback is needed, it must still use the
  DataService read boundary.
- QML / Presenter / Controller must not directly read SQLite or DataAccess.
- QML must not calculate cash or PnL facts.
- Manual cash movement post-write handling must not generate broker orders,
  strategy execution, real orders, TradeDrafts, suggestions, or automatic
  trading.

### No Production Implementation Drift

- Production QML is not modified by TASK-201.
- Startup files are not modified by TASK-201.
- Presenter and Controller files are not modified by TASK-201.
- ShellServices adapter / port files are not modified by TASK-201.
- DataServiceActions.cpp, DataServiceActions.h, and
  DataServiceActionRegistrar.cpp are not modified by TASK-201.
- DataAccess repositories are not modified by TASK-201.
- migrations/001_initial_schema.sql and
  migrations/002_shell_accounting_manual_entry_schema.sql are not modified.
- No migration or schema file is added.

### Forbidden Capability Probes

- No direct SQLite read/write is introduced.
- No direct DataAccess readback is introduced.
- No AccountingEngine replay integration is introduced.
- No read model refresh implementation is introduced.
- No snapshot refresh or snapshot rebuild after write is introduced.
- No UI auto-refresh is introduced.
- No refresh button is introduced.
- No audit_log or ledger write is introduced.
- No broker SDK, network call, credentials, endpoint, real broker order id,
  real order placement, strategy execution, or automatic trading is introduced.
- Broker disabled, broker order, real broker authorization, real broker
  implementation, no-network, no-credentials, and no-order-placement gates
  remain retained.

### Regression

- TASK-200 QML Presenter implementation tests pass.
- TASK-199 authorization gate tests pass.
- TASK-198 DataService write wiring implementation tests pass.
- TASK-196 manual cash movement repository tests pass.
- TASK-192 manual transaction repository tests pass.
- Full CTest passes.
- transport local socket echo 50 repeat passes.

## Required Probes

- Static source scan.
- Changed-path allowlist scan.
- Documentation keyword scan.
- README / docs index / prompt template registration scan.
- Production QML no-drift scan.
- Startup no-drift scan.
- Presenter / Controller no-drift scan.
- ShellServices adapter / port no-drift scan.
- DataServiceActions no-drift scan.
- DataServiceActionRegistrar no-drift scan.
- DataAccess repository no-drift scan.
- Migration and schema no-drift scan.
- Direct SQLite / DataAccess scan.
- Replay and read model refresh scan.
- Refresh button and UI auto-refresh scan.
- Audit / ledger write scan.
- Broker / network / credentials / endpoint scan.
- Real order and automatic trading scan.
- Existing broker gate retention scan.

## Go / No-Go Checklist

- Go only if TASK-201 modifies docs and tests inside the authorized scope.
- Go only if docs/198 and docs/199 exist and are indexed.
- Go only if TASK-201 gate tests pass.
- Go only if full CTest passes.
- Go only if production QML, startup, Presenter, Controller, ShellServices,
  DataServiceActions, repositories, and migrations are unchanged.
- Go only if no readback / refresh / replay implementation was added.
- Go only if no direct SQLite / DataAccess access was added.
- Go only if no audit / ledger write was added.
- Go only if no broker, network, credentials, endpoint, real order placement,
  or automatic trading was added.
- No-go if any production implementation file changes outside the explicit
  TASK-201 authorization scope.

## TASK-202 Implementation Update

TASK-202 evolves this authorization gate to allow only the authorized minimal
post-write readback refresh implementation. The allowed production changes are
limited to ShellAccounting QML and ShellServices Presenter / Controller refresh
wiring. The gate must continue to block DataServiceActions changes,
DataServiceActionRegistrar changes, DataAccess repository changes, migration
changes, AccountingEngine replay, snapshot rebuild after write, derived table
refresh after write, audit or ledger writes, broker SDKs, network, credentials,
endpoints, real order placement, and automatic trading.
