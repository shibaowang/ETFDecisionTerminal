# ShellAccounting Manual Entry QML Presenter Implementation Test Plan

## Document Purpose

This document defines the TASK-200 test plan for ShellAccounting manual entry
QML / Presenter implementation.

## Test Matrix

### Documentation And Registration

- docs/196 exists.
- docs/197 exists.
- README, docs/README, and docs/12 register TASK-200.
- tests/CMake registers `ShellAccountingManualEntryQmlPresenterImplementation`.
- docs/194 and docs/195 state TASK-200 implementation has been added.

### Production QML

- Production ShellAccounting QML contains a manual entry section.
- The page has manual transaction inputs and a submit button.
- The page has manual cash movement inputs and a submit button.
- QML calls only `accountingPresenter.submitManualTransaction(...)` and
  `accountingPresenter.submitManualCashMovement(...)` for manual entry.
- QML does not call DataServiceClient, SQLite, DataAccess, repositories,
  broker code, strategy code, real order placement, or automatic trading.

### Presenter / Controller Boundary

- `ShellAccountingPresenter` exposes manual entry status properties.
- `ShellAccountingPresenter` exposes structured manual entry invokables.
- `ShellAccountingReadOnlyController` routes manual entry requests to the
  service adapter.
- The service adapter calls the existing DataService actions only.
- DataServiceActions, DataServiceActionRegistrar, repositories, migrations,
  validation production code, AccountingEngine replay, StrategyEngine,
  MarketEngine, broker, network, credentials, and endpoints are not modified.

### Response Mapping

- UI status includes busy, last status, last issue, and last result.
- The Presenter maps safe DataService fields such as `databaseWritten`,
  `tradeLogWritten`, `cashAdjustmentWritten`, duplicate, and idempotent state.
- Invalid UI input returns explicit input error.
- DataService failures remain safe errors and must not be silent success.

### Privacy And Safety

- QML / Presenter / Controller do not expose raw SQL, raw payloads,
  credentials, endpoints, tokens, secrets, internal stack traces, broker
  payloads, real broker order ids, or real order ids.
- No direct SQLite write is introduced.
- No replay/read model refresh is triggered.
- No audit or ledger row is written directly by ShellServices.
- No TradeDraft or suggestion implementation is added.
- No real order placement or automatic trading is added.

### Regression

- TASK-199 authorization gate passes after allowing TASK-200 implementation.
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
- QML manual entry binding scan.
- Presenter invokable and property scan.
- Controller / adapter boundary scan.
- DataServiceActions no-drift scan.
- DataAccess repository no-drift scan.
- Migration / schema no-drift scan.
- Direct SQLite write scan.
- Broker / network / credentials / endpoint scan.
- Real order id / order placement scan.
- Automatic trading scan.
- Documentation policy probe.
- DevDocs acceptance probe.

## Go / No-Go Checklist

- [ ] docs/196 exists.
- [ ] docs/197 exists.
- [ ] README, docs/README, and docs/12 register TASK-200.
- [ ] TASK-200 implementation gate passes.
- [ ] TASK-199 authorization gate passes.
- [ ] TASK-198 DataService write wiring tests pass.
- [ ] TASK-196 repository tests pass.
- [ ] TASK-192 repository tests pass.
- [ ] Broker gates pass.
- [ ] transport 50 passes.
- [ ] Full CTest passes.
- [ ] git diff --check passes.
- [ ] git status is clean.
