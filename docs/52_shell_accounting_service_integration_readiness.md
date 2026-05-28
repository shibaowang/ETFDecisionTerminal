# Shell Accounting Service Integration Readiness

## Document Purpose

This document decides whether work may start on a real
`ShellAccountingReadOnlyController` service adapter implementation.

This document does not implement code, does not call `DataServiceClient`, does
not wire QML, does not access SQLite, and does not authorize real service
integration.

## Go / No-Go Checklist

- [ ] `ShellAccountingReadOnlyController` skeleton has been merged.
- [ ] Fake/client boundary tests have been merged.
- [ ] Service adapter boundary docs have been merged.
- [ ] DataServiceClient guard wrappers exist.
- [ ] DataService guard suite has been merged.
- [ ] QML not wired has been confirmed.
- [ ] No TradeDraft has been confirmed.
- [ ] No trade suggestion has been confirmed.
- [ ] Adapter does not access SQLite has been confirmed.
- [ ] Adapter does not call AccountingEngine has been confirmed.
- [ ] Timeout / protocol error strategy has been defined.
- [ ] `implemented=false -> Unavailable` strategy has been defined.
- [ ] User explicitly authorized real adapter implementation.

## Recommended First-stage Real Adapter Scope

The first real adapter task should support only:

- read-only behavior
- `position.list` guard payload
- `cash.summary` guard payload
- `portfolio.pnl.summary` guard payload
- `base_position.summary` guard payload
- `sniper_pool.summary` guard payload
- protocol success / failure mapping
- domain issue mapping
- timeout mapping
- no QML
- no writes
- no TradeDraft

## Conditions That Should Block Implementation

Do not enter implementation when:

- QML integration has not been separated into its own task.
- The plan calls `DataServiceClient` directly from the controller instead of
  going through an adapter.
- The plan puts business accounting logic inside the controller.
- The plan generates trade suggestions.
- The plan generates TradeDraft.
- The plan accesses SQLite.
- The plan calls AccountingEngine.
- Timeout / error mapping is not defined.
- There are no tests for `implemented=false -> Unavailable`.

## Candidate Next Tasks

Suggested next tasks:

- TASK-104: ShellAccountingServiceAdapter interface skeleton without real
  DataServiceClient
- TASK-104: ShellAccounting real adapter test plan
- TASK-104: ShellAccountingReadOnlyController fake adapter injection tests

Any real DataServiceClient integration requires separate authorization. Any QML
integration requires separate authorization.
