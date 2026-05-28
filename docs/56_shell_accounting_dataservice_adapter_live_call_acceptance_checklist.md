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
