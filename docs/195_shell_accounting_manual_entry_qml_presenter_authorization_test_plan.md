# ShellAccounting Manual Entry QML Presenter Authorization Test Plan

## Document Purpose

This document defines the TASK-199 test plan for the ShellAccounting manual
entry QML / Presenter authorization gate.

The plan is static and gate-only. It does not implement QML, Presenter,
Controller, startup wiring, repository changes, database writes, broker
integration, network access, credentials, endpoint configuration, real order
placement, or automatic trading.

## Test Matrix

### Documentation And Registration

- docs/194 exists.
- docs/195 exists.
- README, docs/README, and docs/12 register TASK-199.
- tests/CMake registers `ShellAccountingManualEntryQmlPresenterAuthorizationGate`.
- docs/192 and docs/193 record that TASK-199 adds a UI / Presenter
  authorization gate while TASK-198 DataService write wiring remains unchanged.

### Authorization-Only Boundary

- docs/194 states QML / Presenter authorization gate-only.
- docs/194 states the current task does not implement UI.
- docs/194 states the current task does not modify production QML, startup,
  Presenter, Controller, DataServiceActions, repositories, or migrations.
- docs/194 states the current task does not write the database, trigger replay,
  write audit or ledger rows, add broker SDKs, add network calls, add
  credentials, add endpoints, place real orders, or enable automatic trading.

### Future Manual Entry UI Boundary

- Future manual transaction UI may collect only structured manual BUY / SELL
  payload fields.
- Future manual transaction UI may only call
  `accounting.manual_transaction.create`.
- Future manual cash movement UI may collect only structured Deposit /
  Withdrawal payload fields.
- Future manual cash movement UI may only call
  `accounting.manual_cash_movement.create`.
- Future UI must call DataService actions only through the application
  Presenter / Controller / adapter boundary.
- Future UI must not call DataAccess repositories directly.
- Future UI must not write SQLite directly.
- Future UI must not generate broker orders, execute strategy logic, or enable
  automatic trading.

### Privacy And Response Boundary

- Future UI may display write status, validation issues, sanitized repository
  issues, duplicate / idempotent result state, and safe DataService errors.
- Future UI must not expose raw SQL, raw payloads, credentials, endpoints,
  tokens, internal stack traces, broker payloads, real broker order ids, or real
  order ids.
- Failure states must not be silent success.

### No Production Drift

- Production QML is not modified.
- Production startup is not modified.
- Presenter and Controller code are not modified.
- DataServiceActions.cpp, DataServiceActions.h, and
  DataServiceActionRegistrar.cpp are not modified.
- TASK-178 validation production code is not modified.
- TASK-192 and TASK-196 repositories are not modified.
- Migrations 001 and 002 are not modified.
- No migration or schema file is added.
- No QML type or UI form is added.
- No direct SQLite write, broker SDK, network endpoint, credential, secret,
  token, password, real broker order id, real order placement, or automatic
  trading token is added.
- No real order placement is added.

### Regression

- TASK-198 DataService write wiring implementation tests pass.
- TASK-196 manual cash movement repository tests pass.
- TASK-192 manual transaction repository tests pass.
- Broker disabled, broker order, real broker authorization, and real broker
  implementation gates pass.
- transport local socket echo 50 repeat passes.
- Full CTest passes.

## Required Probes

- Static source scan.
- Changed-path allowlist scan.
- Production QML no-drift scan.
- Startup no-drift scan.
- Presenter / Controller no-drift scan.
- DataServiceActions no-drift scan.
- DataAccess repository no-drift scan.
- Migration / schema no-drift scan.
- No new QML type / UI form scan.
- Direct SQLite write scan.
- Broker / network / credentials / endpoint scan.
- Real order id / order placement scan.
- Automatic trading scan.
- Documentation policy probe.
- DevDocs acceptance probe.

## Go / No-Go Checklist

- [ ] docs/194 exists.
- [ ] docs/195 exists.
- [ ] README, docs/README, and docs/12 register TASK-199.
- [ ] TASK-199 authorization gate passes.
- [ ] TASK-198 DataService write wiring tests pass.
- [ ] TASK-196 repository tests pass.
- [ ] TASK-192 repository tests pass.
- [ ] Broker gates pass.
- [ ] transport 50 passes.
- [ ] Full CTest passes.
- [ ] git diff --check passes.
- [ ] git status is clean.

## TASK-200 Implementation Update

After TASK-200, this authorization gate allows the specific production
ShellAccounting QML page and ShellServices Presenter / Controller / adapter
changes needed to call `accounting.manual_transaction.create` and
`accounting.manual_cash_movement.create` through the DataService boundary.

The gate continues to forbid production startup changes, DataServiceActions
changes, DataServiceActionRegistrar changes, repository changes, migration
changes, direct SQLite or DataAccess access from QML / Presenter / Controller,
AccountingEngine replay, read model refresh, audit or ledger writes from
ShellServices, broker SDKs, network calls, credentials, endpoints, real order
placement, and automatic trading.
