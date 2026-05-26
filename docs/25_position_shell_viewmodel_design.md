# Position Shell ViewModel Design

## 1. Document Purpose

This document defines the future v0.3 read-only position page ShellServices
ViewModel / Model design. It is a design document only and does not mean C++,
QML, DataService actions, replay logic, or database writes have been
implemented.

Future QML position pages must get data through ShellServices ViewModels. QML
must not parse DataService responses directly, must not call `DataServiceClient`
directly, and must not access SQLite.

## 2. Design Principles

- DataService read-only action returns DTOs.
- ShellServices converts DTOs into QObject / QAbstractListModel surfaces.
- QML only binds ShellServices.
- Shell ViewModel is not a fact source.
- ViewModel does not write databases.
- ViewModel does not execute strategies.
- ViewModel does not generate TradeDraft.
- ViewModel does not calculate accounting; it only displays DataService
  results.
- Errors and warnings must be exposed explicitly.
- trade_log is the only fact ledger.
- snapshot is a derived result.
- No automatic trading, broker API, or real order placement is in scope.

## 3. Layering

```text
DataService read-only action
  -> DataServiceClient
  -> ShellAccountingReadOnlyFacade (future)
  -> ShellAccountingReadOnlyController
  -> ShellPositionListModel / ShellCashSummaryObject / ShellPortfolioPnlObject
     / ShellBasePositionObject / ShellSniperPoolObject
     / ShellAccountingIssueListModel
  -> QML readonly pages
```

Current scope does not implement these classes. It only defines the design.
QML must not bypass the Controller.

## 4. ShellAccountingReadOnlyController Draft

Suggested properties:

- connectionState
- refreshState
- isBusy
- canRefresh
- selectedAccountId
- selectedPortfolioId
- selectedInstrumentCode
- lastError
- lastSuccessAtText
- positionListModel
- positionSummaryObject
- cashSummaryObject
- portfolioPnlObject
- basePositionObject
- sniperPoolObject
- issueListModel

Suggested methods:

- connectToDataService(socketName)
- disconnect()
- refreshAccountingHealth()
- refreshPositions(accountId, portfolioId)
- refreshCashSummary(accountId, portfolioId)
- refreshPortfolioPnl(portfolioId)
- refreshBasePosition(portfolioId)
- refreshSniperPool(portfolioId)
- refreshAllAccountingReadonly()
- setSelectedAccount(accountId)
- setSelectedPortfolio(portfolioId)
- setSelectedInstrument(instrumentCode)
- clearAccountingData()

Rules:

- All methods are read-only.
- Do not expose `sendRaw`.
- Do not expose arbitrary actions.
- Do not call write actions.
- Do not call `data.audit.append`.
- Do not access SQLite.
- Do not perform accounting calculation.

## 5. ShellPositionListModel Design

Source: `PositionListResponse.positions`.

Required roles:

- accountId
- portfolioId
- instrumentCode
- instrumentName
- instrumentType
- marketCode
- currency
- quantityText
- availableQuantityText
- frozenQuantityText
- costAmountText
- costPriceText
- marketPriceText
- marketValueText
- unrealizedPnlText
- unrealizedPnlRatioText
- realizedPnlText
- positionWeightText
- basePositionFlag
- dataQualityStatus
- warningCount
- errorCount

These fields come from DTOs and are not calculated in the model. Missing
market price should display as unavailable. `dataQualityStatus=ERROR` should
render as an error badge in UI. Empty `positions` is not an error.

## 6. ShellPositionSummaryObject Design

Source: aggregate fields from `PositionListResponse`.

Suggested properties:

- accountId
- portfolioId
- totalPositionCount
- totalMarketValueText
- totalCostAmountText
- totalUnrealizedPnlText
- dataQualityStatus
- warningCount
- errorCount
- stale
- generatedAt
- sourceRangeText

This object only displays DataService results. It must not summarize the
position model locally. If local summary is ever required, it needs a separate
authorized task.

## 7. ShellCashSummaryObject Design

Source: `CashSummaryDto`.

Required properties:

- accountId
- portfolioId
- currency
- cashBalanceText
- availableCashText
- frozenCashText
- netContributionText
- buyCashOutflowText
- sellCashInflowText
- feeTotalText
- dataQualityStatus
- warningCount
- errorCount
- generatedAt

QML does not calculate cash. Negative cash must show a `NEGATIVE_CASH` issue.
`cash_snapshot` is not a fact source.

## 8. ShellPortfolioPnlObject Design

Source: `PortfolioPnlDto`.

Required properties:

- portfolioId
- currency
- totalAssetsText
- principalText
- netContributionText
- cashText
- marketValueText
- realizedPnlText
- unrealizedPnlText
- totalPnlText
- totalReturnRatioText
- dailyPnlText
- dailyReturnRatioText
- dataQualityStatus
- warningCount
- errorCount

Realized and unrealized PnL must be displayed separately. The denominator for
`totalReturnRatioText` comes from DataService and is not decided by QML. If
principal is undefined, UI must show unavailable or an error state.

## 9. ShellBasePositionObject Design

Source: `BasePositionDto`.

Required properties:

- portfolioId
- targetBaseRatioText
- targetBaseAmountText
- currentBaseRatioText
- currentBaseAmountText
- lockedBaseAmountText
- basePositionStatus
- damagedBaseAmountText
- sellableAboveBaseAmountText
- dataQualityStatus
- warningCount
- errorCount

20% 底仓 is locked. Sell logic must not touch base position. UI only displays
the result and must not decide whether a position can be sold. Base damage
state comes from DTO, not QML calculation.

## 10. ShellSniperPoolObject Design

Source: `SniperPoolDto`.

Required properties:

- portfolioId
- poolAmountText
- usedAmountText
- remainingAmountText
- tierCount
- dataQualityStatus
- warningCount
- errorCount
- calculationVersion

The 80% sniper pool and related tier state must come from DTOs. UI must not
calculate pool sizing.

## 11. ShellSniperTierListModel Design

Source: `SniperPoolDto.tierSummary`.

Required roles:

- tierName
- weight
- targetAmountText
- executedAmountText
- remainingAmountText
- completed
- dataQualityStatus

T1..T6 1/2/4/8/16/32 weights come from DTO only. `completed` comes from replay
aggregation of buy facts, not current market value. Tier state must not expand
or shrink with floating PnL.

## 12. ShellAccountingIssueListModel Design

Source: `AccountingIssueDto[]`.

Required roles:

- level
- code
- message
- sourceTable
- sourceId
- accountId
- portfolioId
- instrumentCode
- blocking
- userVisible

Blocking issues must be prominent in UI. `userVisible=false` issues may appear
in a developer detail area rather than the primary page, but QML must not
silently swallow them.

## 13. DTO To ViewModel Mapping

| DTO field group | ShellServices property / role | QML display area | Nullable | Empty display | Error handling |
| --- | --- | --- | --- | --- | --- |
| PositionSummaryDto identity | ShellPositionListModel identity roles | position table | no | `-` | error badge when status ERROR |
| PositionSummaryDto quantity | quantityText / availableQuantityText | position table quantity columns | maybe | unavailable | issue list if missing |
| PositionSummaryDto cost | costAmountText / costPriceText | position table cost columns | yes | unavailable | COST_BASIS_UNDEFINED |
| PositionSummaryDto market | marketPriceText / marketValueText | valuation columns | yes | unavailable | MARKET_PRICE_MISSING |
| CashSummaryDto cash | ShellCashSummaryObject cash fields | cash summary panel | maybe | unavailable | NEGATIVE_CASH |
| PortfolioPnlDto PnL | ShellPortfolioPnlObject PnL fields | PnL panel | maybe | unavailable | PRINCIPAL_UNDEFINED / MARKET_PRICE_MISSING |
| BasePositionDto base | ShellBasePositionObject fields | base-position panel | maybe | unavailable | dataQualityStatus badge |
| SniperPoolDto pool | ShellSniperPoolObject / ShellSniperTierListModel | sniper-pool panel | maybe | unavailable | dataQualityStatus badge |
| AccountingIssueDto issue | ShellAccountingIssueListModel roles | warning / error list | maybe | hidden only when userVisible=false | blocking issue shown prominently |

See [Position DTO ViewModel Mapping](26_position_dto_viewmodel_mapping.md) for
the full mapping table.

## 14. QML Page Draft

Future pages, not implemented now:

- `PositionReadOnlyPage.qml`
  - binds `ShellPositionListModel`, `ShellPositionSummaryObject`,
    `ShellAccountingIssueListModel`
  - no edit, trade, accounting, or TradeDraft buttons
  - empty state when no positions
  - visible errors and stale state
  - does not calculate quantity, cost, market value, or PnL
- `CashPnlReadOnlyPage.qml`
  - binds `ShellCashSummaryObject` and `ShellPortfolioPnlObject`
  - no deposit, withdrawal, cash calibration, or accounting entry buttons
  - displays unavailable principal or PnL as controller-provided status
  - does not calculate returns
- `BaseSniperReadOnlyPage.qml`
  - binds `ShellBasePositionObject`, `ShellSniperPoolObject`,
    `ShellSniperTierListModel`
  - no strategy execution, TradeDraft generation, sell suggestion, or trade
    button
  - displays derived base/sniper state only
  - does not calculate base damage or tier completion

## 15. Refresh / State Flow Design

The future accounting controller should reuse the existing read-only data state
flow ideas:

- states: IDLE / CONNECTING / REFRESHING / SUCCESS / FAILED
- reject duplicate refresh while busy
- failed refresh should preserve prior successful data unless a later design
  explicitly says otherwise
- stale data must show stale state
- lastError and lastSuccessAtText must be exposed

## 16. Filter / Sort Design

`ShellPositionListModel` may later use `ShellReadOnlyTableProxyModel` or a
dedicated proxy.

Rules:

- filtering / sorting does not call services
- filtering / sorting does not mutate DTOs
- filtering / sorting does not write databases

Suggested filters:

- account
- portfolio
- instrument
- dataQualityStatus
- basePositionFlag
- positive / zero quantity

Suggested sorting:

- instrumentCode
- marketValue
- unrealizedPnl
- positionWeight
- dataQualityStatus

## 17. Test Fixture Mapping

FX001-FX013 are the future ViewModel test input source. Each fixture should at
least verify:

- DTO parser
- ViewModel population
- issue mapping
- QML role availability

Error fixtures must not be skipped. This includes negative cash, sell exceeds
position, stale snapshot, missing market price, and unsupported currency.

## 18. Prohibited Scope

- 不在 QML 中计算持仓.
- 不在 QML 中计算成本.
- 不在 QML 中计算收益.
- 不在 QML 中判断底仓破损.
- 不在 QML 中判断 sniper tier 完成.
- 不生成 TradeDraft.
- 不执行策略.
- 不写 trade_log.
- 不写 snapshot.
- 不下单.
- 不接券商接口.
- Do not implement automatic trading.
