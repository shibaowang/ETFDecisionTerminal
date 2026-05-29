# ShellServices Accounting Controller Contract

## Document Purpose

This document defines the future ShellServices accounting read-only controller
boundary.

It does not implement a controller, does not modify QML, does not call
DataServiceClient, and is intended for review before a future ShellServices
implementation task.

## TASK-110 Adapter Port Boundary

The controller can continue to consume `ShellAccountingServiceAdapter` while a
`ShellAccountingDataServiceAdapter` uses an abstract client port underneath in
tests. This still does not wire QML and still does not call the real
DataServiceClient. Controller behavior remains read-only and no-trade.

## Current State

- DataService accounting guard suite exists.
- DataServiceClient wrappers exist.
- Real `position.list` is not complete.
- `cash.summary`, `portfolio.pnl.summary`, `base_position.summary`, and
  `sniper_pool.summary` are guards.
- ShellServices has not connected accounting actions.
- QML has not connected accounting actions.
- AccountingEngine has not been connected to DataService.
- SQLite facts query has not been connected to DataService.

## Architecture Boundary

Future allowed direction:

```text
QML
-> ShellServices ViewModel / Model
-> ShellAccountingReadOnlyController
-> DataServiceClient wrapper
-> DataService action
```

Forbidden directions:

- QML -> DataServiceClient
- QML -> SQLite
- QML -> AccountingEngine
- QML -> DataAccess
- ShellServices -> SQLite
- ShellServices -> AccountingEngine
- ShellServices -> DataAccess
- ShellServices -> write action
- ShellServices -> TradeDraft generation

QML 不得直接调用 DataServiceClient.

## ShellAccountingReadOnlyController Candidate Responsibilities

Future candidate responsibilities:

- `refreshPositionList()`
- `refreshCashSummary()`
- `refreshPortfolioPnlSummary()`
- `refreshBasePositionSummary()`
- `refreshSniperPoolSummary()`
- expose loading / unavailable / warning / error states
- expose lastRefreshTime
- expose issues
- expose readOnly flags
- map DataService payload to Shell ViewModel / Model

The controller must be read-only, must not write the database, must not compute
complex accounting replay, must not generate trade suggestions, must not
generate TradeDraft rows, and must not execute strategies.

## Non-responsibilities

ShellServices accounting controller is not responsible for:

- replay algorithm
- SQLite query
- DataAccess facts loading
- market price fetching
- FX rate fetching
- TradeLog write
- snapshot write
- TradeDraft generation
- broker order
- QML rendering details
- strategy execution

## Future ViewModel / Model Candidates

- `ShellPositionListModel`: read-only, QML-bindable, does not calculate
  accounting, does not call DataServiceClient, and does not access SQLite.
- `ShellPositionSummaryObject` or row model: read-only row data only.
- `ShellCashSummaryObject`: read-only cash summary state only.
- `ShellAccountCashSummaryListModel`: read-only account cash rows only.
- `ShellPortfolioPnlObject`: read-only PnL summary state only.
- `ShellBasePositionObject`: read-only base position display object only.
- `ShellSniperPoolObject`: read-only sniper pool display object only.
- `ShellSniperTierListModel`: read-only tier rows only.
- `ShellAccountingIssueListModel`: read-only issue list.
- `ShellAccountingStatusObject`: read-only loading / unavailable / warning /
  error / stale status object.

Each candidate is QML-bindable but must not compute accounting, must not call
DataServiceClient, and must not access SQLite.

## Common State Contract

Common state fields:

- `loading`
- `loaded`
- `unavailable`
- `warning`
- `error`
- `stale`
- `empty`
- `readOnly`
- `implemented`
- `lastRefreshTime`
- `dataQualityStatus`

`unavailable` must not be displayed as normal empty data. `warning` must not be
swallowed. `error` must be visible. `implemented=false` must be visible.
`readOnly=true` must be visible or inferable.

## Issues Contract

DataService payload issues must flow into `ShellAccountingIssueListModel`.

- blocking issue must be visible
- warning issue must be visible
- issue code / message / level / blocking / sourceId should be preserved
- QML should not interpret complex business meaning directly; it displays the
  ViewModel state and issue fields

## Action-specific Mapping

### position.list

- DataService `PositionListResponse` -> `ShellPositionListModel`
- `implemented=false` maps to unavailable
- `MARKET_PRICE_MISSING` maps to warning
- `MULTI_CURRENCY_UNSUPPORTED` maps to error
- no trade suggestion generation

### cash.summary

- `CashSummaryResponse` -> `ShellCashSummaryObject` /
  `ShellAccountCashSummaryListModel`
- `CASH_SUMMARY_NOT_AVAILABLE` maps to unavailable
- do not derive cash from snapshots

### portfolio.pnl.summary

- `PortfolioPnlSummaryResponse` -> `ShellPortfolioPnlObject`
- `MARKET_PRICE_MISSING` maps to warning
- do not fabricate `totalAssets` or PnL

### base_position.summary

- `BasePositionSummaryResponse` -> `ShellBasePositionObject`
- `sellableAboveBaseAmountText` is not a sell suggestion
- do not generate TradeDraft

### sniper_pool.summary

- `SniperPoolSummaryResponse` -> `ShellSniperPoolObject` /
  `ShellSniperTierListModel`
- `remainingAmountText` is not a buy suggestion
- `completed` is not calculated by QML
- do not generate TradeDraft

## Refresh / Lifecycle Contract

Refresh is a user-triggered or page-triggered read-only action. Refresh must
not write the database, must not generate TradeDraft rows, must not execute
strategy, and must not introduce background automatic trading.

Refresh errors should not clear the last successful data unless a later design
explicitly says so. Stale and unavailable states must be visible. Cancel and
timeout behavior must be controlled.

## Privacy / Display Boundary

Privacy mode may hide amounts, but it must not change the underlying data.
Hiding amounts is not the same as clearing data. Issues and warnings remain
visible.

QML must not bypass privacy state to access raw amount sources directly. Real
amount text comes from ShellServices ViewModel, not QML calculation.

## Testing Expectations

Future implementation must include:

- controller construction test
- no direct QML -> DataServiceClient static check
- `implemented=false` -> unavailable state mapping
- warning issue mapping
- blocking error mapping
- no-write test
- no TradeDraft generation test
- no strategy execution test
- refresh does not write database
- QML smoke test only after controller exists

## TASK-100 Test Scaffolding

TASK-100 adds Shell accounting controller test scaffolding under
`tests/ShellAccountingControllerScaffolding`.

Covered contract tests:

- `shell_accounting_controller_state_contract`
- `shell_accounting_controller_issue_contract`
- `shell_accounting_controller_privacy_contract`
- `shell_accounting_controller_no_trade_action_contract`

These tests use test-only scenario descriptors and fake payload state. They do
not implement `ShellAccountingReadOnlyController`, do not call real
`DataServiceClient`, do not access SQLite, and do not write database tables.

The scaffolding locks the future state, issue, privacy, and no trade action
boundaries before production ShellServices controller code is authorized.

## TASK-101 Production Skeleton

TASK-101 adds a production-side `ShellAccountingReadOnlyController` skeleton in
`libs/ShellServices`.

Current skeleton scope:

- state mapping
- issue storage / exposure
- privacy display text selection
- read-only / no-write flags
- no TradeDraft generation
- no trade suggestion generation
- no strategy execution
- no broker order

The skeleton still does not call real `DataServiceClient`, does not access
SQLite, does not call AccountingEngine, and is not wired to QML. It is a
production boundary object only, not service integration.

## TASK-102 Fake/Client Boundary Tests

TASK-102 adds fake/client boundary integration tests under
`tests/ShellAccountingFakeClientBoundary`.

Current coverage:

- fake guard payloads for `position.list`, `cash.summary`,
  `portfolio.pnl.summary`, `base_position.summary`, and `sniper_pool.summary`
  map to `Unavailable`
- fake loading transitions into `Unavailable`, `Loaded`, `Empty`, `Warning`,
  `Error`, and `Stale`
- fake issue payloads preserve code, level, message, blocking flag, and
  sourceId
- fake privacy payloads preserve raw amount text while selecting masked display
  text in privacy mode
- fake no-trade payloads keep TradeDraft generation, trade suggestion,
  strategy execution, and broker order disabled

These tests only exercise fake payload -> controller state mapping. They do not
start real DataServiceClient integration, do not wire QML, do not access
SQLite, do not call AccountingEngine, and do not write database tables.

## TASK-103 Service Adapter Boundary

TASK-103 adds the service adapter boundary docs:

- [Shell Accounting Service Adapter Boundary](51_shell_accounting_service_adapter_boundary.md)
- [Shell Accounting Service Integration Readiness](52_shell_accounting_service_integration_readiness.md)

Future service integration should flow through a
`ShellAccountingServiceAdapter` abstraction. `ShellAccountingReadOnlyController`
should not directly hard-depend on `DataServiceClient`.

The real adapter remains unimplemented. Fake adapter behavior must stay
test-only. Real DataServiceClient integration, QML integration, SQLite access,
AccountingEngine calls, writes, TradeDraft generation, and trade suggestions
remain out of scope until separately authorized.

## TASK-104 Service Adapter Interface Skeleton

TASK-104 adds a production-side `ShellAccountingServiceAdapter` abstraction and
service request/result types. `ShellAccountingReadOnlyController` may hold an
optional adapter pointer, but it still does not call real services.

The controller remains read-only, write-disabled, and non-QML-facing. Real
DataServiceClient integration remains unimplemented and requires a separate
task.

## TASK-105 Fake Adapter Injection Behavior

TASK-105 adds controller refresh behavior through the abstract
`ShellAccountingServiceAdapter` only. Tests inject a test-only fake adapter and
drive:

- `refreshPositionList`
- `refreshCashSummary`
- `refreshPortfolioPnlSummary`
- `refreshBasePositionSummary`
- `refreshSniperPoolSummary`

The controller maps fake adapter results into loading, unavailable, loaded,
empty, warning, error, and stale states. It preserves visible issues, keeps
`readOnly=true`, keeps `writeEnabled=false`, and keeps TradeDraft generation,
trade suggestion generation, strategy execution, and broker order submission
disabled.

Real DataServiceClient integration has not started. QML integration has not
started. The fake adapter remains test-only.

## TASK-106 DataService Adapter Consumer Boundary

TASK-106 defines the future `ShellAccountingDataServiceAdapter` boundary in
[docs/53_shell_accounting_dataservice_adapter_boundary.md](53_shell_accounting_dataservice_adapter_boundary.md)
and its test plan in
[docs/54_shell_accounting_dataservice_adapter_test_plan.md](54_shell_accounting_dataservice_adapter_test_plan.md).

The controller may consume DataServiceClient only indirectly through
`ShellAccountingServiceAdapter` and a separately authorized real adapter. The
controller must not directly hard-depend on DataServiceClient, must not access
SQLite, and must not call AccountingEngine.

## TASK-107 Adapter Spy Scaffolding Boundary

TASK-107 adds test-only spy/wrapper scaffolding for the future real adapter.
The controller contract remains unchanged: it may consume service results only
through `ShellAccountingServiceAdapter`, never through a direct DataServiceClient
hard dependency, and QML remains disconnected.

## TASK-108 DataService Adapter Skeleton Consumer Boundary

TASK-108 adds a production `ShellAccountingDataServiceAdapter` skeleton. The
controller can inject it through the existing `ShellAccountingServiceAdapter`
abstraction, but the skeleton only returns adapter-not-connected /
`UNAVAILABLE` results.

This does not start real DataServiceClient integration. The controller still
does not directly hard-depend on DataServiceClient, QML is still disconnected,
and no write, TradeDraft, trade suggestion, strategy, or broker order path is
enabled.

## TASK-109 Live-call Controller Gate

TASK-109 keeps the controller boundary unchanged for future live-call work:

- controller must continue to consume `ShellAccountingServiceAdapter`
- controller must not directly depend on DataServiceClient
- live-call adapter behavior does not imply QML integration
- `implemented=false` must still map to `Unavailable`
- warning, error, and stale states must remain visible
- controller must not generate TradeDraft or trade suggestions

## TASK-111 Concrete Port Controller Boundary

TASK-111 adds the concrete DataServiceClient port below the adapter boundary.
The controller boundary remains unchanged:

- `ShellAccountingReadOnlyController` still does not include DataServiceClient;
- the controller still consumes `ShellAccountingServiceAdapter`;
- real DataServiceClient access is isolated to
  `ShellAccountingDataServiceClientPortAdapter`;
- QML remains disconnected from accounting actions;
- no TradeDraft, trade suggestion, strategy execution, broker order, SQLite
  access, AccountingEngine call, or database write is added.

## TASK-112 Concrete Port Integration Hardening

TASK-112 adds controller-level integration tests for the full guard-wrapper
consumer path:

`ShellAccountingReadOnlyController -> ShellAccountingDataServiceAdapter ->
ShellAccountingDataServiceClientPortAdapter -> DataServiceClient guard wrapper`.

The tests cover all five read-only accounting actions and assert
`implemented=false -> Unavailable`, visible issues, `readOnly=true`,
`writeEnabled=false`, and no TradeDraft / no trade suggestion / no strategy /
no broker order.

The controller still does not directly depend on DataServiceClient, DataServiceApi,
DataAccess, AccountingEngine, SQLite, QtQuick, or QML. QML remains unwired.

## TASK-113 ViewModel / Model Read-only Boundary

TASK-113 adds ShellServices-side read-only accounting ViewModel / Model
skeletons for future QML binding:

- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`

The controller remains the service refresh boundary. Future QML must bind these
ShellServices objects / models instead of calling DataServiceClient, SQLite,
DataAccess, AccountingEngine, or adapter classes directly.

The new ViewModel / Model skeletons consume only in-memory state, issue, and
display-text data. They do not call services, write tables, generate TradeDraft,
generate trade suggestions, execute strategies, submit broker orders, or
register QML types. QML integration remains a separate task.

## TASK-114 Presenter / QML Binding Plan

TASK-114 defines the future presenter and QML binding plan in
[docs/57_shell_accounting_presenter_contract.md](57_shell_accounting_presenter_contract.md)
and
[docs/58_shell_accounting_qml_binding_readiness_plan.md](58_shell_accounting_qml_binding_readiness_plan.md).

The controller is consumed by the presenter / facade boundary, not by QML
calling DataServiceClient directly. QML integration remains separately
authorized and is not implemented in this task.

## TASK-115 Presenter Skeleton

TASK-115 adds the production `ShellAccountingPresenter` skeleton between the
controller and any future QML binding. The presenter can hold and expose
`ShellAccountingStatusObject`, `ShellAccountingIssueListModel`, and
`ShellPositionListModel`, and it can hold an optional
`ShellAccountingReadOnlyController`.

The presenter still routes refresh only through the controller boundary. QML
still does not directly consume the controller, adapters, concrete port, or
DataServiceClient.

## TASK-116 Presenter Concrete Port Integration

TASK-116 verifies that the controller concrete-port integration can be consumed
from `ShellAccountingPresenter`. The presenter path remains:

`Presenter -> ShellAccountingReadOnlyController -> ShellAccountingDataServiceAdapter
-> ShellAccountingDataServiceClientPortAdapter -> DataServiceClient guard wrapper`.

The controller still does not directly depend on DataServiceClient, and the
presenter still does not bypass the controller.

## TASK-117 Presenter All Guard Refresh

TASK-117 extends presenter refresh wiring to all five controller read-only
guard actions:

- `refreshPositionList`
- `refreshCashSummary`
- `refreshPortfolioPnlSummary`
- `refreshBasePositionSummary`
- `refreshSniperPoolSummary`

`refreshAllReadOnly` invokes the five controller refreshes in the fixed guard
order: `position.list`, `cash.summary`, `portfolio.pnl.summary`,
`base_position.summary`, and `sniper_pool.summary`. The controller remains the
only presenter refresh boundary; Presenter still does not directly depend on
DataServiceClient, adapters, concrete ports, SQLite, DataAccess,
AccountingEngine, or QML.

All current guard payloads map to `Unavailable`, keep their visible
`*_NOT_AVAILABLE` issues, preserve `readOnly=true` and `writeEnabled=false`,
and keep Empty distinct from Unavailable.

## TASK-118 QML Smoke Readiness

The future QML binding smoke plan is
[docs/59_shell_accounting_qml_binding_smoke_plan.md](59_shell_accounting_qml_binding_smoke_plan.md).
The QML no-trade / no-direct-service static gate is
[docs/60_shell_accounting_qml_static_gate.md](60_shell_accounting_qml_static_gate.md).

QML is still not wired to the controller, presenter, adapter, or
DataServiceClient. Future QML must bind only ShellServices read-only
presentation objects and must not call DataServiceClient, SQLite, DataAccess,
AccountingEngine, write actions, TradeDraft, trade suggestions, strategies, or
broker orders.

## Explicitly Forbidden

- QML 不直接调用 DataServiceClient。
- QML 不访问 SQLite。
- QML 不计算持仓。
- QML 不计算现金。
- QML 不计算收益。
- QML 不计算底仓。
- QML 不计算狙击池。
- ShellServices 不写库。
- ShellServices 不生成 TradeDraft。
- ShellServices 不生成交易建议。
- ShellServices 不执行策略。
## TASK-119 QML Static Gate CTest

The ShellAccounting QML static gate CTest now protects the future controller /
presenter UI boundary. QML remains unwired, but any later accounting QML task
must pass the no direct DataServiceClient, no SQLite / DataAccess, no
AccountingEngine, no write-token, no premature binding, and accounting-scoped
no-trade scans.

Controller integration still flows through ShellServices abstractions; QML must
not bypass the presenter / model boundary or call controller internals directly
to reach services.

## TASK-120 ShellAccounting Read-only UI Readiness

ShellAccounting read-only UI readiness milestone is
`docs/61_shell_accounting_readonly_ui_readiness_milestone.md`. Next phase review
is `docs/62_shell_accounting_next_phase_review.md`.

The milestone confirms the controller can participate in the read-only
Presenter -> Controller -> Adapter -> Concrete Port -> DataServiceClient guard
wrapper path while QML remains unwired and real accounting actions remain
unimplemented.

## TASK-121 QML Binding Smoke Scaffold

The QML binding smoke scaffold confirms future QML must stay above the
Presenter / ViewModel / Model boundary. It does not change controller behavior
and does not authorize QML to call controller, adapter, concrete port, or
DataServiceClient directly.

## TASK-122 QML Smoke CTest

The test-only QML smoke CTests verify that future QML can read fake read-only
presenter/status/issue/position properties without touching controller internals
or service clients.

Controller behavior is unchanged. Production QML remains unwired and must not
call the controller directly to reach DataServiceClient, SQLite, DataAccess, or
AccountingEngine.
