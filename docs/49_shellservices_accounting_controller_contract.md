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
