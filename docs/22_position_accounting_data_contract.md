# Position Accounting Data Contract

## 1. Document Purpose

This is the v0.3 read-only position and accounting-derived data contract design.
It is an implementation-before-contract document and does not mean code has
been implemented.

Future DataService read-only actions, accounting replay, ShellServices
ViewModels, and QML read-only position pages must follow this contract.

This task does not implement a position page, cash page, PnL page, replay code,
DataService action, Shell ViewModel, database schema change, or write capability.

## 2. Design Principles

- trade_log 是唯一事实账本.
- DTOs are read-only display contracts and are not fact sources.
- snapshot is a derived result and is not a fact source.
- `cash_snapshot`, `position_snapshot`, and `portfolio_summary` are derived
  cache/display results only.
- Replay results must be explainable, replayable, and testable.
- QML 不计算账务.
- QML must not calculate position, cost, principal, cash, return, base position,
  or sniper pool values.
- QML must not access SQLite.
- QML must not directly call `DataServiceClient`.
- All errors must be returned explicitly and must not be silently swallowed.
- No automatic trading, broker API, or real order placement is in scope.

## 3. Common Field Rules

Common fields used by future DTOs:

| Field | Required | Nullable | Unit / format | Source | Notes |
| --- | --- | --- | --- | --- | --- |
| accountId | context-dependent | yes | stable id text | request / fact rows | Nullable for portfolio-wide summaries. |
| portfolioId | yes for portfolio data | no | stable id text | request / fact rows | Required for portfolio-scoped outputs. |
| instrumentCode | position rows only | yes | instrument code | fact rows / instrument table | Nullable outside instrument scope. |
| strategyCode | strategy-derived rows only | yes | strategy code | strategy / policy config | Not required for plain accounting summaries. |
| currency | yes for monetary fields | no | ISO-like currency text | account / instrument / fact rows | Multi-currency support must be explicit. |
| generatedAt | yes | no | ISO 8601 | replay / snapshot generator | Time the DTO was produced. |
| sourceFromTime | yes | yes | ISO 8601 | replay source range | Nullable only for empty ledgers. |
| sourceToTime | yes | yes | ISO 8601 | replay source range | Nullable only for empty ledgers. |
| calculationVersion | yes | no | stable version string | replay / snapshot code | Must change when calculation semantics change. |
| stale | yes | no | boolean | snapshot / replay metadata | True when source is stale. |
| dataQualityStatus | yes | no | enum text | derived from issues | See DataQualityStatus. |
| warningCount | yes | no | integer | issues | Count of warning issues. |
| errorCount | yes | no | integer | issues | Count of error issues. |
| warnings | yes | no | AccountingIssueDto array | issues | User-visible warnings. |
| errors | yes | no | AccountingIssueDto array | issues | User-visible errors. |

ID fields may be empty only when the DTO scope is intentionally broader than
the identifier. For example, `accountId` may be empty on a portfolio-wide
summary, but `portfolioId` must be present for portfolio-scoped DTOs.

Time fields should use ISO 8601 strings. Future code may keep raw time values
internally, but the UI contract exposes stable text fields.

Money and ratio fields should separate display text from raw integer drafts:

- `xxxText` for UI display.
- Future raw cents fields may use `xxxCents`.
- Future raw ratio fields may use `xxxPpm`.

QML should prefer text fields and must not format raw money or ratio values by
itself unless a later task explicitly authorizes a display formatter.

## 4. DataQualityStatus

| Status | Meaning | UI display allowed | Calculation input allowed | User-visible hint |
| --- | --- | --- | --- | --- |
| OK | Complete and current result. | yes | yes | Normal state. |
| WARNING | Result is usable but has non-blocking issues. | yes | cautious | Show warning count and details. |
| ERROR | Result is blocked by accounting errors. | yes | no | Show blocking errors. |
| STALE | Result is older than the accepted source range. | yes | no for fresh calculations | Show stale source range. |
| INCOMPLETE | Required inputs are missing. | yes | no | Show missing input category. |
| UNSUPPORTED | Mode, currency, or data shape is unsupported. | yes | no | Show unsupported reason. |
| REPLAY_REQUIRED | A snapshot is unavailable or obsolete and replay is needed. | yes | no until replay | Show replay-required state. |
| SNAPSHOT_MISSING | A requested snapshot does not exist. | yes | no | Show missing snapshot state. |

## 5. AccountingErrorCode

| Code | Trigger condition | Impact range | UI display suggestion | Blocking |
| --- | --- | --- | --- | --- |
| NEGATIVE_POSITION | Replay produces a quantity below zero. | instrument / account / portfolio | Show affected instrument and stop derived position result. | yes |
| NEGATIVE_CASH | Replay produces cash below zero under the selected convention. | account / portfolio / currency | Show affected cash account and amount. | yes |
| MISSING_FEE | A trade requires fee data but the fee is missing. | trade / position / PnL | Show the source trade if available. | maybe |
| CASH_FLOW_MISMATCH | Cash inflow/outflow does not balance with facts. | account / portfolio | Show mismatch range. | yes |
| SELL_EXCEEDS_POSITION | Sell quantity exceeds replayed holdings. | instrument / trade | Show source trade and current quantity. | yes |
| COST_BASIS_UNDEFINED | Cost basis convention is not defined. | position / PnL | Show convention-missing message. | yes |
| PRINCIPAL_UNDEFINED | Principal convention is not defined. | portfolio PnL | Show principal-definition required. | yes |
| FX_RATE_MISSING | Multi-currency conversion requires missing FX rate. | portfolio / currency | Show currency pair and date range. | yes |
| MARKET_PRICE_MISSING | Market value requires missing price data. | position / portfolio | Show missing price and keep cost fields visible. | no for cost, yes for valuation |
| SNAPSHOT_STALE | Snapshot source range or version is outdated. | snapshot-backed DTO | Show stale timestamp/version. | maybe |
| SOURCE_RANGE_EMPTY | Requested replay range has no source facts. | request scope | Show empty ledger state. | no |
| MULTI_CURRENCY_UNSUPPORTED | Requested mode spans unsupported currencies. | portfolio | Show unsupported currency mode. | yes |
| DATA_VERSION_MISMATCH | Snapshot/replay version does not match contract. | DTO / service response | Show version mismatch. | yes |
| REPLAY_NOT_AVAILABLE | Replay engine or fixture is not available. | service action | Show replay unavailable. | yes |
| UNSUPPORTED_ACCOUNTING_MODE | Requested accounting mode is not supported. | request scope | Show supported modes. | yes |

## 6. CalculationMetadataDto

Fields:

- calculationVersion
- replayVersion
- sourceFromTime
- sourceToTime
- generatedAt
- sourceTradeCount
- sourceCashFlowCount
- snapshotId
- stale
- staleReason
- dataQualityStatus
- warningCount
- errorCount

Every position, cash, PnL, base-position, and sniper-pool DTO should include
metadata. Metadata is read-only diagnostic context. It must not be used by QML
or UI code to modify fact data, rewrite `trade_log`, or generate corrections.

## 7. AccountingIssueDto

Fields:

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

Rules:

- Issues must be displayable.
- `blocking=true` means UI should show an error state and avoid presenting the
  result as complete.
- `sourceId` may be empty to avoid exposing internal details.
- Issues do not authorize writes, corrections, or automatic repairs.

## 8. PositionSummaryDto

Fields:

| Field | Meaning | Source | Required | Nullable | UI display suggestion |
| --- | --- | --- | --- | --- | --- |
| accountId | Account scope. | request / facts | context-dependent | yes | Show in detail or filter chips. |
| portfolioId | Portfolio scope. | request / facts | yes | no | Usually hidden if page scoped. |
| instrumentCode | Stable instrument code. | facts / instrument table | yes | no | Required identity column. |
| instrumentName | Display name. | instrument table | yes | no | Show with code. |
| instrumentType | ETF / CASH / other type. | instrument table | yes | no | Badge or text. |
| marketCode | Market code. | instrument table | yes | yes | Empty for cash-like rows. |
| currency | Currency. | instrument / facts | yes | no | Show in money columns. |
| quantityText | Current quantity. | replay | yes | no | Text display only. |
| availableQuantityText | Available quantity. | replay / future settlement rules | yes | yes | Nullable until settlement rules exist. |
| frozenQuantityText | Frozen quantity. | replay / future settlement rules | yes | yes | Nullable if unsupported. |
| costAmountText | Cost amount. | replay cost basis | yes | yes | Must be empty if cost basis undefined. |
| costPriceText | Unit cost. | replay cost basis | yes | yes | Must be empty if cost basis undefined. |
| marketPriceText | Market price. | future market data / snapshot | yes | yes | Empty with MARKET_PRICE_MISSING. |
| marketValueText | Market value. | market price * quantity | yes | yes | Empty with MARKET_PRICE_MISSING. |
| unrealizedPnlText | Unrealized PnL. | valuation - cost | yes | yes | Empty if cost or price missing. |
| unrealizedPnlRatioText | Unrealized PnL ratio. | PnL convention | yes | yes | Empty if denominator undefined. |
| realizedPnlText | Realized PnL. | replay sell facts | yes | yes | Empty until realized convention exists. |
| positionWeightText | Weight in portfolio. | valuation summary | yes | yes | Empty if valuation missing. |
| basePositionFlag | Whether part of base position. | future base policy derivation | yes | yes | Badge only. |
| dataQualityStatus | Data quality. | issues / metadata | yes | no | Badge. |
| metadata | Calculation metadata. | replay / snapshot | yes | no | Detail panel. |
| issues | Accounting issues. | replay / validation | yes | no | Error/warning list. |

Current scope does not implement calculation. If market price is missing,
`marketValueText` may be empty and the response must include
`MARKET_PRICE_MISSING`. If cost convention is undefined, `costAmountText` must
not be faked and the response must include `COST_BASIS_UNDEFINED`.

## 9. PositionListResponse

Fields:

- accountId
- portfolioId
- positions
- totalMarketValueText
- totalCostAmountText
- totalUnrealizedPnlText
- dataQualityStatus
- metadata
- issues

Rules:

- `positions` is an array of PositionSummaryDto.
- Empty positions are not an error.
- Negative positions must be errors.
- Totals must be empty or degraded when required source rows have blocking
  issues.

## 10. CashSummaryDto

Fields:

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
- metadata
- issues

Rules:

- cash_snapshot is not a fact source.
- Cash must be replayed from trade and cash-flow facts.
- Negative cash must return `NEGATIVE_CASH`.
- Missing fee or cash-flow imbalance must be returned as explicit issues.
- Current scope does not implement cash replay.

## 11. PortfolioPnlDto

Fields:

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
- metadata
- issues

Rules:

- Principal must not be faked when the convention is undefined.
- realized PnL and unrealized PnL must be separated.
- `totalReturnRatioText` must document its denominator.
- Current scope does not implement return calculation.

## 12. BasePositionDto

Fields:

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
- metadata
- issues

Rules:

- 20% 底仓 is locked.
- Sell logic must not touch base position unless a future task explicitly
  authorizes it.
- UI displays only and must not calculate base position state.
- Current scope does not implement base position calculation.

## 13. SniperPoolDto

Fields:

- portfolioId
- poolAmountText
- usedAmountText
- remainingAmountText
- tierCount
- tierSummary
- calculationVersion
- dataQualityStatus
- metadata
- issues

Suggested `tierSummary` item fields:

- tierName
- weight
- targetAmountText
- executedAmountText
- remainingAmountText
- completed
- dataQualityStatus

Rules:

- 80% 狙击资金池 must not automatically resize with floating PnL.
- Future T1..T6 design uses 1/2/4/8/16/32 weights as an implementation
  constraint.
- Tier completion comes from buy fact aggregation, not current market value.
- Current scope does not implement sniper-pool calculation.

## 14. Read-Only DataService Action Contract

All actions in this section are future read-only drafts. Current scope does not
implement any action. Future implementation requires separate task
authorization. All actions must be read-only.

### A. accounting.health

- request: optional accountId / portfolioId.
- response: readiness, supported accounting modes, calculationVersion,
  dataQualityStatus, issues.
- source: schema metadata, future replay configuration, future contract
  versions.
- errors: UNSUPPORTED_ACCOUNTING_MODE, DATA_VERSION_MISMATCH.
- forbidden writes: no trade_log writes, no snapshot writes, no audit_log
  writes unless a future diagnostic audit task explicitly authorizes them.

### B. accounting.replay.preview

- request: accountId, portfolioId, sourceFromTime, sourceToTime, accountingMode.
- response: CalculationMetadataDto, issue list, summary counts, preview quality.
- source: trade_log, trade_execution_group, future authorized cash-flow facts.
- errors: REPLAY_NOT_AVAILABLE, SOURCE_RANGE_EMPTY, NEGATIVE_CASH,
  NEGATIVE_POSITION.
- forbidden writes: no trade_log writes, no snapshot writes, no audit_log
  writes.

### C. position.list

- request: accountId, portfolioId, optional instrumentCode, includeZeroQuantity.
- response: PositionListResponse.
- source tables: trade_log, trade_execution_group, instrument metadata, future
  position_snapshot as derived cache only.
- errors: NEGATIVE_POSITION, SELL_EXCEEDS_POSITION, COST_BASIS_UNDEFINED,
  MARKET_PRICE_MISSING.
- forbidden writes: no trade_log writes, no snapshot writes, no audit_log
  writes.

### D. cash.summary

- request: accountId, portfolioId, currency.
- response: CashSummaryDto.
- source tables: trade_log, future cash-flow facts, future cash_snapshot as
  derived cache only.
- errors: NEGATIVE_CASH, MISSING_FEE, CASH_FLOW_MISMATCH,
  MULTI_CURRENCY_UNSUPPORTED.
- forbidden writes: no trade_log writes, no snapshot writes, no audit_log
  writes.

### E. portfolio.pnl.summary

- request: portfolioId, currency, source range, return convention.
- response: PortfolioPnlDto.
- source tables: trade_log, position-derived data, future market data, future
  portfolio_summary as derived cache only.
- errors: PRINCIPAL_UNDEFINED, COST_BASIS_UNDEFINED, MARKET_PRICE_MISSING,
  FX_RATE_MISSING.
- forbidden writes: no trade_log writes, no snapshot writes, no audit_log
  writes.

### F. base_position.summary

- request: portfolioId, base policy version.
- response: BasePositionDto.
- source tables: trade_log, future policy config, position-derived data.
- errors: COST_BASIS_UNDEFINED, MARKET_PRICE_MISSING, DATA_VERSION_MISMATCH.
- forbidden writes: no trade_log writes, no snapshot writes, no audit_log
  writes.

### G. sniper_pool.summary

- request: portfolioId, tier policy version.
- response: SniperPoolDto.
- source tables: trade_log, cash-derived data, future policy config.
- errors: NEGATIVE_CASH, PRINCIPAL_UNDEFINED, DATA_VERSION_MISMATCH.
- forbidden writes: no trade_log writes, no snapshot writes, no audit_log
  writes.

## 15. ShellServices ViewModel Contract

Future ViewModel drafts:

| ViewModel / Model | Source DTO | QML binding fields | Rules |
| --- | --- | --- | --- |
| ShellPositionListModel | PositionListResponse.positions | instrumentCode, instrumentName, quantityText, costAmountText, marketValueText, dataQualityStatus | No SQLite, no direct DataServiceClient, no accounting calculation. |
| ShellPositionSummaryObject | PositionListResponse totals | totalMarketValueText, totalCostAmountText, totalUnrealizedPnlText, dataQualityStatus | Display DataService result only. |
| ShellCashSummaryObject | CashSummaryDto | cashBalanceText, availableCashText, frozenCashText, netContributionText, dataQualityStatus | Display DataService result only. |
| ShellPortfolioPnlObject | PortfolioPnlDto | totalAssetsText, principalText, realizedPnlText, unrealizedPnlText, totalReturnRatioText | Display DataService result only. |
| ShellBasePositionObject | BasePositionDto | targetBaseRatioText, currentBaseRatioText, lockedBaseAmountText, basePositionStatus | Display DataService result only. |
| ShellSniperPoolObject | SniperPoolDto | poolAmountText, usedAmountText, remainingAmountText, tierSummary | Display DataService result only. |
| ShellAccountingIssueListModel | AccountingIssueDto array | level, code, message, blocking, userVisible | Display errors and warnings. |

ShellServices must not access SQLite, must not directly expose arbitrary
DataService actions, and must not perform accounting calculation in UI-facing
objects unless a future task explicitly defines that boundary.

## 16. QML UI Contract

- QML only binds ShellServices Model / Object instances.
- QML does not calculate quantity, cost,收益, principal, cash, or market value.
- QML does not decide whether base position is damaged.
- QML does not decide sniper-pool tier completion.
- QML does not trigger writes.
- QML does not generate TradeDraft.
- QML does not execute strategy.
- Errors and warnings must be visible.
- Blocking issues must not be displayed as successful results.

## 17. Test Fixture Contract

Future implementation must first define test fixtures. Each fixture must include
input facts, expected DTOs, expected issues, and whether the issue is blocking.

Required fixture categories:

- empty ledger
- buy only
- buy and sell
- sell exceeds position
- fee missing
- negative cash
- multi instrument
- multi account
- base position locked
- sniper tier completed
- stale snapshot
- missing market price
- unsupported currency

The detailed fixture plan is in
[Position Accounting Test Fixture Design](23_position_accounting_test_fixture_design.md).

## 18. Prohibited Scope

- 不写 trade_log.
- 不写 snapshot.
- Do not modify schema.
- Do not implement write actions.
- Do not calculate accounting in QML.
- Do not generate TradeDraft.
- Do not execute strategy.
- Do not place orders.
- Do not integrate broker API.
- Do not implement automatic trading.
