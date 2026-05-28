# Position DTO ViewModel Mapping

## 1. Document Purpose

This document lists DTO to ShellServices ViewModel / QML role mappings for
future implementation. It is a lookup table for implementation planning only.
It does not implement C++, QML, DataService actions, accounting replay, or
database writes.

## 2. Field Naming Rules

- QML role names use camelCase.
- Text fields are preferred for display.
- Raw numeric fields may be added later, but QML should not format raw values by
  default.
- Status fields must map to badges.
- QML must not directly consume raw DataService JSON.
- QML must not directly call `DataServiceClient`.
- QML must not calculate money.
- QML must not calculate return ratios.

## 3. PositionSummaryDto -> ShellPositionListModel

| DTO field | ViewModel role | QML display label | Empty display | Severity handling | Notes |
| --- | --- | --- | --- | --- | --- |
| accountId | accountId | Account | `-` | none | Hidden if page scoped. |
| portfolioId | portfolioId | Portfolio | `-` | none | Hidden if page scoped. |
| instrumentCode | instrumentCode | Code | `-` | error if missing | Required identity column. |
| instrumentName | instrumentName | Name | `-` | warning if missing | Display with code. |
| instrumentType | instrumentType | Type | `-` | badge optional | From DTO only. |
| marketCode | marketCode | Market | `-` | none | Empty allowed for CASH. |
| currency | currency | Currency | `-` | warning if missing | Required for money columns. |
| quantityText | quantityText | Quantity | unavailable | ERROR for negative replay | QML does not calculate. |
| availableQuantityText | availableQuantityText | Available | unavailable | WARNING if unsupported | Settlement rules from service. |
| frozenQuantityText | frozenQuantityText | Frozen | unavailable | WARNING if unsupported | From DTO only. |
| costAmountText | costAmountText | Cost | unavailable | COST_BASIS_UNDEFINED | Do not fake. |
| costPriceText | costPriceText | Cost price | unavailable | COST_BASIS_UNDEFINED | Do not fake. |
| marketPriceText | marketPriceText | Market price | unavailable | MARKET_PRICE_MISSING | Do not infer from cost. |
| marketValueText | marketValueText | Market value | unavailable | MARKET_PRICE_MISSING | Service result only. |
| unrealizedPnlText | unrealizedPnlText | Unrealized PnL | unavailable | missing cost/price issues | Service result only. |
| unrealizedPnlRatioText | unrealizedPnlRatioText | Unrealized % | unavailable | denominator issue | Service result only. |
| realizedPnlText | realizedPnlText | Realized PnL | unavailable | cost basis issue | Service result only. |
| positionWeightText | positionWeightText | Weight | unavailable | valuation issue | Service result only. |
| basePositionFlag | basePositionFlag | Base | `No` | badge | QML does not decide. |
| dataQualityStatus | dataQualityStatus | Status | UNKNOWN | badge | OK/WARNING/ERROR/STALE. |
| warningCount | warningCount | Warnings | `0` | warning badge | Count only. |
| errorCount | errorCount | Errors | `0` | error badge | Count only. |

## 4. PositionListResponse -> ShellPositionSummaryObject

| DTO field | ViewModel property | QML display label | Empty display | Severity handling | Notes |
| --- | --- | --- | --- | --- | --- |
| accountId | accountId | Account | `-` | none | Context. |
| portfolioId | portfolioId | Portfolio | `-` | none | Context. |
| positions.length | totalPositionCount | Positions | `0` | none | Derived from response size only if supplied by controller. |
| totalMarketValueText | totalMarketValueText | Total market value | unavailable | market price issue | No local summary unless authorized. |
| totalCostAmountText | totalCostAmountText | Total cost | unavailable | cost basis issue | From response. |
| totalUnrealizedPnlText | totalUnrealizedPnlText | Total unrealized PnL | unavailable | PnL issue | From response. |
| dataQualityStatus | dataQualityStatus | Status | UNKNOWN | badge | Response status. |
| metadata.stale | stale | Stale | `false` | stale badge | From metadata. |
| metadata.generatedAt | generatedAt | Generated at | unavailable | stale if old | ISO text. |
| metadata.sourceFromTime/sourceToTime | sourceRangeText | Source range | unavailable | stale if old | Display only. |

## 5. CashSummaryDto -> ShellCashSummaryObject

| DTO field | ViewModel property | QML display label | Empty display | Severity handling | Notes |
| --- | --- | --- | --- | --- | --- |
| accountId | accountId | Account | `-` | none | Context. |
| portfolioId | portfolioId | Portfolio | `-` | none | Context. |
| currency | currency | Currency | `-` | warning | Required. |
| cashBalanceText | cashBalanceText | Cash balance | unavailable | NEGATIVE_CASH | Service result only. |
| availableCashText | availableCashText | Available cash | unavailable | warning if unsupported | Service result only. |
| frozenCashText | frozenCashText | Frozen cash | unavailable | warning if unsupported | Service result only. |
| netContributionText | netContributionText | Net contribution | unavailable | PRINCIPAL_UNDEFINED | Service result only. |
| buyCashOutflowText | buyCashOutflowText | Buy outflow | unavailable | cash mismatch | Service result only. |
| sellCashInflowText | sellCashInflowText | Sell inflow | unavailable | cash mismatch | Service result only. |
| feeTotalText | feeTotalText | Fees | unavailable | MISSING_FEE | Service result only. |
| dataQualityStatus | dataQualityStatus | Status | UNKNOWN | badge | Response status. |

## 6. PortfolioPnlDto -> ShellPortfolioPnlObject

| DTO field | ViewModel property | QML display label | Empty display | Severity handling | Notes |
| --- | --- | --- | --- | --- | --- |
| portfolioId | portfolioId | Portfolio | `-` | none | Context. |
| currency | currency | Currency | `-` | warning | Required. |
| totalAssetsText | totalAssetsText | Total assets | unavailable | valuation issue | From DTO. |
| principalText | principalText | Principal | unavailable | PRINCIPAL_UNDEFINED | Do not fake. |
| netContributionText | netContributionText | Net contribution | unavailable | principal issue | From DTO. |
| cashText | cashText | Cash | unavailable | cash issue | From DTO. |
| marketValueText | marketValueText | Market value | unavailable | MARKET_PRICE_MISSING | From DTO. |
| realizedPnlText | realizedPnlText | Realized PnL | unavailable | cost basis issue | From DTO. |
| unrealizedPnlText | unrealizedPnlText | Unrealized PnL | unavailable | valuation issue | From DTO. |
| totalPnlText | totalPnlText | Total PnL | unavailable | PnL issue | From DTO. |
| totalReturnRatioText | totalReturnRatioText | Total return | unavailable | denominator issue | Denominator from service. |
| dailyPnlText | dailyPnlText | Daily PnL | unavailable | price issue | From DTO. |
| dailyReturnRatioText | dailyReturnRatioText | Daily return | unavailable | denominator issue | From DTO. |
| dataQualityStatus | dataQualityStatus | Status | UNKNOWN | badge | Response status. |

## 7. BasePositionDto -> ShellBasePositionObject

| DTO field | ViewModel property | QML display label | Empty display | Severity handling | Notes |
| --- | --- | --- | --- | --- | --- |
| portfolioId | portfolioId | Portfolio | `-` | none | Context. |
| targetBaseRatioText | targetBaseRatioText | Target base ratio | unavailable | config issue | 20% rule from DTO. |
| targetBaseAmountText | targetBaseAmountText | Target base amount | unavailable | valuation issue | From DTO. |
| currentBaseRatioText | currentBaseRatioText | Current base ratio | unavailable | valuation issue | From DTO. |
| currentBaseAmountText | currentBaseAmountText | Current base amount | unavailable | valuation issue | From DTO. |
| lockedBaseAmountText | lockedBaseAmountText | Locked base | unavailable | config issue | From DTO. |
| basePositionStatus | basePositionStatus | Base status | UNKNOWN | badge | From DTO. |
| damagedBaseAmountText | damagedBaseAmountText | Damaged base | unavailable | warning/error | From DTO. |
| sellableAboveBaseAmountText | sellableAboveBaseAmountText | Sellable above base | unavailable | warning/error | Display only, no trade action. |
| dataQualityStatus | dataQualityStatus | Status | UNKNOWN | badge | Response status. |

## 8. SniperPoolDto -> ShellSniperPoolObject / ShellSniperTierListModel

| DTO field | ViewModel role/property | QML display label | Empty display | Severity handling | Notes |
| --- | --- | --- | --- | --- | --- |
| portfolioId | portfolioId | Portfolio | `-` | none | Context. |
| poolAmountText | poolAmountText | Pool amount | unavailable | config issue | From DTO. |
| usedAmountText | usedAmountText | Used | unavailable | warning | From DTO. |
| remainingAmountText | remainingAmountText | Remaining | unavailable | warning | From DTO. |
| tierCount | tierCount | Tiers | `0` | warning if mismatch | From DTO. |
| calculationVersion | calculationVersion | Version | unavailable | version issue | From DTO. |
| tierSummary.tierName | tierName | Tier | `-` | none | T1..T6. |
| tierSummary.weight | weight | Weight | `-` | warning if missing | 1/2/4/8/16/32 from DTO. |
| tierSummary.targetAmountText | targetAmountText | Target | unavailable | config issue | From DTO. |
| tierSummary.executedAmountText | executedAmountText | Executed | unavailable | replay issue | From DTO. |
| tierSummary.remainingAmountText | remainingAmountText | Remaining | unavailable | replay issue | From DTO. |
| tierSummary.completed | completed | Completed | `No` | badge | From replay result, not market value. |
| tierSummary.dataQualityStatus | dataQualityStatus | Status | UNKNOWN | badge | Response status. |

## 9. AccountingIssueDto -> ShellAccountingIssueListModel

| DTO field | ViewModel role | QML display label | Empty display | Severity handling | Notes |
| --- | --- | --- | --- | --- | --- |
| level | level | Level | INFO | badge | INFO/WARNING/ERROR. |
| code | code | Code | UNKNOWN | badge / text | AccountingErrorCode. |
| message | message | Message | unavailable | visible text | User-safe message. |
| sourceTable | sourceTable | Source table | hidden | detail only | May be hidden. |
| sourceId | sourceId | Source id | hidden | detail only | May be empty. |
| accountId | accountId | Account | `-` | none | Context. |
| portfolioId | portfolioId | Portfolio | `-` | none | Context. |
| instrumentCode | instrumentCode | Instrument | `-` | none | Context. |
| blocking | blocking | Blocking | `No` | prominent if true | Blocking issues cannot be hidden. |
| userVisible | userVisible | Visible | `No` | developer details if false | False can be hidden from primary UI. |

## 10. Prohibited Use

- QML does not directly consume raw DataService JSON.
- QML does not directly call `DataServiceClient`.
- QML does not calculate money.
- QML does not calculate return ratios.
- QML does not calculate position, cost, base damage, or sniper tier completion.
- ViewModels are not fact sources.
- ViewModels must not write `trade_log`, snapshots, or audit rows.

## TASK-099 Shell Accounting State Alignment

This mapping aligns with
`docs/49_shellservices_accounting_controller_contract.md` and
`docs/50_shell_accounting_viewmodel_state_contract.md`.

Existing DTO -> ViewModel mapping remains conceptual until a controller
implementation task. Guard payloads with `implemented=false` must map to
Unavailable rather than Empty, and QML must not directly consume raw
DataService JSON or calculate accounting values.
