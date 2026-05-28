# ShellAccountingDataServiceAdapter Live-call Acceptance Checklist

## Document Purpose

This checklist decides whether `ShellAccountingDataServiceAdapter` may move from
the not-connected skeleton to real DataServiceClient wrapper calls.

This document does not implement live calls, does not call DataServiceClient,
does not wire QML, does not access SQLite, does not call AccountingEngine, and
does not write database tables.

## Go / No-Go Checklist

- [ ] `ShellAccountingDataServiceAdapter` skeleton has been merged.
- [ ] Spy/wrapper scaffolding has been merged.
- [ ] Live-call gate docs have been merged.
- [ ] DataServiceClient accounting guard wrappers exist.
- [ ] Read-only allowlist has been confirmed.
- [ ] Forbidden denylist has been confirmed.
- [ ] Request mapping tests are ready.
- [ ] Response mapping tests are ready.
- [ ] Protocol error tests are ready.
- [ ] Domain issue tests are ready.
- [ ] Timeout / transport tests are ready.
- [ ] No-write / no-trade tests are ready.
- [ ] Rollback to not-connected strategy has been confirmed.
- [ ] QML not wired has been confirmed.
- [ ] SQLite not accessed has been confirmed.
- [ ] AccountingEngine not called has been confirmed.
- [ ] 用户明确授权 live-call implementation.

## First Live-call Allowed Scope

The first live-call implementation should only allow:

- `position.list` guard wrapper
- `cash.summary` guard wrapper
- `portfolio.pnl.summary` guard wrapper
- `base_position.summary` guard wrapper
- `sniper_pool.summary` guard wrapper
- protocol / domain mapping
- timeout / transport mapping
- no-write / no-trade
- no QML

## Explicitly Excluded

- real `position.list` implementation
- real `cash.summary` implementation
- real `portfolio.pnl.summary` implementation
- real `base_position.summary` implementation
- real `sniper_pool.summary` implementation
- SQLite facts query access from the adapter
- AccountingEngine calls from the adapter
- QML integration
- TradeDraft
- trade suggestion
- strategy execution
- broker order
- write-enabled actions

## Required Tests Before Merge

- allowlist tests
- method mapping tests
- request mapping tests
- response mapping tests
- error mapping tests
- no-write / no-trade tests
- no QML direct dependency tests
- no SQLite dependency tests
- no AccountingEngine dependency tests
- controller integration tests
- transport repeat tests

## TASK-111 Acceptance Update

TASK-111 completes the concrete port for guard wrappers:

- [x] `ShellAccountingDataServiceClientPortAdapter` exists.
- [x] Only the concrete port includes / holds `DataServiceClient`.
- [x] The concrete port calls only the five read-only accounting guard wrappers.
- [x] `ShellAccountingDataServiceAdapter` still uses the abstract port.
- [x] `ShellAccountingReadOnlyController` still has no direct DataServiceClient
  dependency.
- [x] QML remains unwired.
- [x] SQLite access remains forbidden.
- [x] AccountingEngine calls remain forbidden.
- [x] no-write / no-trade behavior remains required.

Future work that wires QML or implements real accounting action payloads still
requires a separate authorization.

## TASK-112 Acceptance Update

Controller concrete port integration hardening is complete:

- [x] Controller refresh is tested through the concrete port for all five guard
  wrappers.
- [x] Guard payloads map to `Unavailable`.
- [x] Guard issues remain visible.
- [x] `readOnly=true` and `writeEnabled=false` are preserved.
- [x] No TradeDraft / no trade suggestion / no strategy / no broker order is
  preserved.
- [x] Controller and adapter still do not directly include DataServiceClient.
- [x] QML remains unwired.

QML integration, SQLite facts access, AccountingEngine integration, and real
accounting action implementation remain separate No-Go items until explicitly
authorized.

## TASK-114 Presenter / QML Binding Update

- [x] Presenter / QML binding plan docs are defined.
- [x] QML integration remains not implemented.
- [ ] QML integration requires explicit separate authorization.
- [ ] Future QML tasks must reference docs/57 and docs/58.
- [ ] Read-only UI smoke tests must be defined before QML binding.

## TASK-110 Status

- [x] Live-call skeleton with abstract client port has been added.
- [x] Test-only spy port covers method mapping.
- [x] Request mapping is covered by tests.
- [x] Response mapping is covered by tests.
- [x] Protocol / timeout / transport / domain issue mapping is covered by tests.
- [x] No-write / no-trade behavior is covered by tests.
- [x] No-real-dependency checks are covered by tests.
- [ ] Concrete DataServiceClient port implementation is still not authorized.
- [ ] Real DataServiceClient integration is still not started.
- [ ] QML integration is still not started.

## TASK-116 Presenter Integration Checkpoint

TASK-116 completes a presenter-level checkpoint for the concrete port guard
wrapper path. The presenter can consume `position.list` through the controller
and adapter boundaries, but QML integration and real accounting action
implementation still require separate authorization.

## TASK-117 Presenter All Guard Refresh Checkpoint

- [x] Presenter refresh methods exist for all five read-only accounting guard
  actions.
- [x] `refreshAllReadOnly` invokes `position.list`, `cash.summary`,
  `portfolio.pnl.summary`, `base_position.summary`, and
  `sniper_pool.summary` in fixed order.
- [x] Current guard payloads map to `Unavailable`.
- [x] All five `*_NOT_AVAILABLE` issues remain visible in the all-refresh
  aggregate result.
- [x] Presenter still does not directly call DataServiceClient, adapters, or
  concrete ports.
- [x] No QML / SQLite / AccountingEngine / write / TradeDraft / trade
  suggestion behavior is enabled.

QML integration and real accounting action implementation remain separate
No-Go items until explicitly authorized.
