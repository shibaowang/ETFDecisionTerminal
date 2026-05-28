# Shell Accounting Service Adapter Boundary

## Document Purpose

This document defines how `ShellAccountingReadOnlyController` should consume
future accounting actions through a service adapter boundary.

This document does not implement an adapter, does not modify the controller,
does not call `DataServiceClient`, does not wire QML, and does not authorize
service integration. It is a review document for a later ShellServices service
integration task.

## Current State

- `ShellAccountingReadOnlyController` skeleton exists.
- Shell accounting fake/client boundary tests exist.
- DataServiceClient guard wrappers exist.
- DataService accounting guard suite exists.
- Real `position.list` is not implemented.
- Shell controller does not call real `DataServiceClient`.
- QML is not wired to the accounting controller.
- SQLite, AccountingEngine, and DataAccess are not wired into ShellServices.

## Design Principles

- The controller must not directly hard-depend on `DataServiceClient`.
- The controller should depend on a `ShellAccountingServiceAdapter` abstract
  interface.
- Fake adapter implementations are test-only.
- The real adapter must be implemented in a separate authorized task.
- The adapter can call read-only accounting actions only.
- The adapter must not access SQLite.
- The adapter must not call AccountingEngine.
- The adapter must not generate TradeDraft.
- The adapter must not generate trade suggestions.
- The adapter must not execute strategy.
- The adapter is not responsible for QML rendering.

## Future Layering Draft

Intended read-only flow:

```text
QML
-> ShellAccounting ViewModel / Model
-> ShellAccountingReadOnlyController
-> ShellAccountingServiceAdapter interface
-> ShellAccountingDataServiceAdapter
-> DataServiceClient wrappers
-> DataService actions
```

Forbidden directions:

- QML -> DataServiceClient
- QML -> ShellAccountingDataServiceAdapter
- QML -> SQLite
- QML -> AccountingEngine
- ShellAccountingReadOnlyController -> DataServiceClient direct hard dependency
- ShellAccountingReadOnlyController -> SQLite
- ShellAccountingReadOnlyController -> AccountingEngine
- ShellAccountingReadOnlyController -> DataAccess
- ShellAccountingServiceAdapter -> write action
- ShellAccountingServiceAdapter -> TradeDraft generation

## ShellAccountingServiceAdapter Interface Draft

Future interface draft, not code:

`ShellAccountingServiceAdapter`:

- `fetchPositionList(request)`
- `fetchCashSummary(request)`
- `fetchPortfolioPnlSummary(request)`
- `fetchBasePositionSummary(request)`
- `fetchSniperPoolSummary(request)`

Common result draft:

`ShellAccountingServiceResult`:

- `actionName`
- `protocolSuccess`
- `implemented`
- `readOnly`
- `writeEnabled`
- `payloadStatus`
- `dataQualityStatus`
- `issues`
- `rawPayload`, optional
- `timeout`
- `transportError`
- `protocolError`
- `domainError`

This is a contract sketch only. This task does not create interface code.
Creating the interface requires a separate authorized task.

## Fake Adapter Boundary

- Fake adapter code is only for tests.
- Fake adapter code must not live in production libs.
- Fake adapter code must not call `DataServiceClient`.
- Fake adapter code only provides in-memory payloads.
- Fake adapter code may simulate guard, warning, error, stale, timeout, and
  transport error responses.
- Fake adapter code must not access SQLite.
- Fake adapter code must not write database tables.

## Real Adapter Boundary

- The real adapter may wrap DataServiceClient guard wrappers in a future task.
- The first real adapter stage can call read-only actions only.
- The real adapter must not interpret complex accounting.
- The real adapter must not generate trade suggestions.
- The real adapter must not generate TradeDraft.
- The real adapter must map protocol errors and domain issues distinctly.
- The real adapter must preserve `implemented=false`.
- The real adapter must preserve `readOnly` and `writeEnabled`.
- The real adapter needs explicit timeout, cancellation, and transport error
  policy.
- Implementing the real adapter requires a separate task.

## Error / Issue Mapping

The adapter boundary must handle:

- `protocolSuccess=false`
- malformed response
- timeout
- transport error
- `implemented=false`
- `*_NOT_AVAILABLE`
- `MARKET_PRICE_MISSING`
- `MULTI_CURRENCY_UNSUPPORTED`
- `FX_RATE_MISSING`
- `REPLAY_NOT_AVAILABLE`
- unknown issue

Protocol error is not the same as a domain issue. Domain issues must become
`ShellAccountingIssue` entries. Warnings must not be swallowed. Blocking errors
must remain visible.

## Refresh Lifecycle

- Controller refresh calls the adapter in a future task.
- Begin refresh sets `Loading`.
- Adapter success maps to `Loaded`, `Empty`, `Warning`, `Error`, or
  `Unavailable`.
- Adapter timeout maps to `Error` or `Unavailable`; a future implementation
  task must fix the exact policy.
- Refresh must not write database tables.
- Refresh must not generate TradeDraft.
- Refresh must not execute strategy.
- Refresh should not clear the last successful data unless a future design
  explicitly says so.

## Privacy Boundary

- The adapter returns raw text.
- ShellServices owns `displayText` and `privacyMode`.
- QML binds only `displayText`.
- The adapter does not perform QML masking.
- Privacy mode does not mutate raw payload data.
- Issues and warnings remain visible in privacy mode.

## Testing Expectations

Future implementation must include:

- fake adapter construction test
- real adapter construction test
- controller consumes fake adapter test
- controller consumes real guard payload test
- protocol error mapping test
- timeout mapping test
- `implemented=false` mapping test
- issue mapping test
- no TradeDraft test
- no trade suggestion test
- no direct QML -> DataServiceClient static check

## Explicitly Forbidden

- QML must not call `DataServiceClient` directly.
- The controller must not directly depend on `DataServiceClient`.
- The adapter must not access SQLite.
- The adapter must not call AccountingEngine.
- The adapter must not write database tables.
- The adapter must not generate TradeDraft.
- The adapter must not generate trade suggestions.
- The adapter must not execute strategy.
- The adapter must not place broker orders.

## TASK-104 Interface Skeleton

TASK-104 adds the production-side `ShellAccountingServiceAdapter` interface
skeleton and `ShellAccountingServiceRequest` / `ShellAccountingServiceResult`
types in `libs/ShellServices`.

Current scope:

- interface skeleton only
- request / result data contract only
- optional controller adapter pointer injection only
- test-only fake adapter under `tests/ShellAccountingServiceAdapter`

Still not implemented:

- real `ShellAccountingDataServiceAdapter`
- real `DataServiceClient` calls
- QML integration
- SQLite access
- AccountingEngine calls
- database writes
- TradeDraft generation
- trade suggestions

The interface skeleton does not depend on DataServiceClient, DataServiceApi,
DataAccess, AccountingEngine, SQLite, QtQuick, or QML. The fake adapter is only
for tests and must not move into production libs.

## TASK-105 Fake Adapter Consumption

TASK-105 validates that `ShellAccountingReadOnlyController` can consume the
abstract `ShellAccountingServiceAdapter` using a test-only fake adapter.

Covered behavior:

- action-specific refresh methods call the corresponding adapter method
- refresh enters `Loading` before the adapter returns
- fake guard payloads map to `Unavailable`
- fake protocol / timeout / transport errors map to visible error state
- fake domain issues remain visible
- fake stale, warning, loaded, and empty states map through the controller
- privacy display and no-trade boundaries remain intact

The real `ShellAccountingDataServiceAdapter` is still not implemented. The fake
adapter remains under tests. The controller still does not include or call real
DataServiceClient.

## TASK-106 DataService Adapter Boundary

TASK-106 adds the next boundary docs:

- [Shell Accounting DataService Adapter Boundary](53_shell_accounting_dataservice_adapter_boundary.md)
- [Shell Accounting DataService Adapter Test Plan](54_shell_accounting_dataservice_adapter_test_plan.md)

The future `ShellAccountingDataServiceAdapter` remains a real adapter candidate,
not an implementation in this task. `ShellAccountingServiceAdapter` remains the
abstract layer. A real adapter must only wrap read-only DataServiceClient guard
wrappers, must not access SQLite, must not call AccountingEngine, must not
generate TradeDraft or trade suggestions, and must not be consumed directly by
QML.

## TASK-107 DataService Adapter Spy Scaffolding

TASK-107 adds test-only spy/wrapper scaffolding for the future
`ShellAccountingDataServiceAdapter`.

The scaffolding proves the future real adapter must stay inside the read-only
accounting wrapper allowlist and must reject write, draft, snapshot, strategy,
and broker action paths. The real adapter still requires separate
authorization.

## TASK-108 DataService Adapter Skeleton

TASK-108 adds a production skeleton named `ShellAccountingDataServiceAdapter`.
It implements `ShellAccountingServiceAdapter`, but it is not a real live
DataServiceClient adapter.

Current boundary:

- no DataServiceClient include or call
- no DataServiceApi include
- no DataAccess include
- no AccountingEngine include
- no SQLite include or access
- no QtQuick / QML include
- no live client
- no socket / IPC / network call
- no writes
- no TradeDraft
- no trade suggestion

All fetch methods currently return a controlled adapter-not-connected /
`UNAVAILABLE` result. Real DataServiceClient integration still requires a
separate task and explicit authorization.

## TASK-109 Live-call Gate Protection

TASK-109 adds a live-call gate for future real adapter implementation. The gate
protects `ShellAccountingServiceAdapter` consumers by requiring the real adapter
to stay behind the abstraction, use only read-only accounting wrappers, preserve
protocol/domain issue mapping, keep no-write / no-trade behavior, and provide a
rollback path to not-connected / unavailable.

The controller still consumes only `ShellAccountingServiceAdapter`. QML remains
unwired.

## TASK-110 Abstract Client Port

`ShellAccountingDataServiceAdapter` now has a live-call skeleton that consumes a
ShellServices-owned `ShellAccountingDataServiceClientPort` abstraction. The
adapter can call wrapper-like methods on a test-only spy port, but it still does
not depend on or call the real DataServiceClient. A concrete DataServiceClient
port remains a separate future task.

## TASK-111 Concrete Client Port

TASK-111 adds `ShellAccountingDataServiceClientPortAdapter` as the concrete
implementation of `ShellAccountingDataServiceClientPort`.

The service-adapter boundary remains unchanged: `ShellAccountingReadOnlyController`
continues to consume `ShellAccountingServiceAdapter`, and
`ShellAccountingDataServiceAdapter` continues to consume the abstract client
port. Only the concrete port includes and holds the real `DataServiceClient`.

The concrete port is limited to the five read-only accounting guard wrappers
and does not add QML integration, SQLite access, AccountingEngine calls,
database writes, TradeDraft generation, trade suggestions, strategy execution,
or broker orders.

## TASK-112 Controller Concrete Port Integration

TASK-112 verifies the service adapter boundary from the controller side. The
controller consumes `ShellAccountingServiceAdapter`; the DataService adapter
consumes `ShellAccountingDataServiceClientPort`; and the concrete port consumes
only read-only DataServiceClient guard wrappers.

The tests keep the layering rule intact: controller and QML do not call
DataServiceClient directly, and the concrete port remains the only class that
may wrap the real client.
