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

## TASK-104 Interface Skeleton Status

TASK-104 completes the `ShellAccountingServiceAdapter` interface skeleton and
service request/result types.

Updated readiness:

- `ShellAccountingServiceAdapter` interface skeleton exists.
- `ShellAccountingServiceResult` expresses protocol error and domain issue
  separately.
- Test-only fake adapter contract tests exist.
- Controller adapter injection skeleton exists, but it does not call services.
- Real adapter still requires separate authorization.
- DataServiceClient integration has not started.
- QML integration has not started.

## TASK-106 DataService Adapter Boundary Status

TASK-106 defines the `ShellAccountingDataServiceAdapter` boundary and test plan:

- [Shell Accounting DataService Adapter Boundary](53_shell_accounting_dataservice_adapter_boundary.md)
- [Shell Accounting DataService Adapter Test Plan](54_shell_accounting_dataservice_adapter_test_plan.md)

Updated readiness:

- Real adapter boundary and test plan are documented.
- Real adapter implementation still requires separate authorization.
- DataServiceClient integration has not started.
- QML integration has not started.
- Adapter implementation must preserve protocol error vs domain issue mapping,
  no-write boundaries, no TradeDraft, and no trade suggestion behavior.

## TASK-107 Spy / Wrapper Readiness Gate

TASK-107 adds spy/wrapper scaffolding as a Go / No-Go prerequisite for the real
adapter.

Updated readiness:

- read-only wrapper allowlist tests exist
- forbidden action denylist tests exist
- method mapping descriptor tests exist
- request / response / error mapping scaffolding exists
- no-write / no-trade scaffolding exists
- real DataServiceClient integration has not started
- QML integration has not started

## TASK-105 Fake Adapter Injection Status

TASK-105 completes fake adapter injection behavior for controller refresh
paths.

Updated readiness:

- Controller refresh can consume the abstract adapter contract.
- Fake adapter refresh tests cover all five accounting actions.
- Guard unavailable payloads still map to `Unavailable`.
- Protocol error, timeout, transport error, warning, blocking issue, stale,
  loaded, and empty mappings are covered.
- Privacy and no-trade boundaries remain covered after refresh.
- Real adapter still requires separate authorization.
- DataServiceClient integration has not started.
- QML integration has not started.

## TASK-108 Skeleton Readiness Status

TASK-108 adds the `ShellAccountingDataServiceAdapter` production skeleton.

Updated readiness:

- Skeleton implements `ShellAccountingServiceAdapter`.
- Skeleton has no live client and returns adapter-not-connected /
  `UNAVAILABLE`.
- Skeleton tests cover interface, not-connected results, no live calls,
  no-write / no-trade, and controller integration.
- Real DataServiceClient integration has not started.
- QML integration has not started.
- Replacing not-connected behavior with real wrapper calls still requires
  explicit authorization and must pass the TASK-107 spy/wrapper gate.
