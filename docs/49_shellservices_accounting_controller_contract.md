# ShellServices Accounting Controller Contract

## Document Purpose

This document defines the future ShellServices accounting read-only controller
boundary.

It does not implement a controller, does not modify QML, does not call
DataServiceClient, and is intended for review before a future ShellServices
implementation task.

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
