# Position Read-Only Data Contract Draft

## Document Purpose

This document defines a future read-only position data contract draft. It is a
design note only and does not implement DTOs, DataService actions, ViewModels,
QML pages, replay logic, database writes, or schema changes.

All DTOs in this document are read-only display contracts. They are not fact
sources, they do not write to the database, and they must not drive trading or
TradeDraft generation.

## Shared Contract Rules

- Every DTO is read-only.
- Every DTO is not a fact source.
- DTO values may come from accounting replay or from future derived snapshots.
- 当前未实现.
- QML must not calculate any DTO field by itself.
- QML must bind future ShellServices ViewModels or models only.
- DTOs must expose data quality and version fields when the source can be
  stale, incomplete, or unsupported.

## PositionSummaryDto

Fields:

- accountId
- portfolioId
- instrumentCode
- instrumentName
- quantityText
- availableQuantityText
- costAmountText
- marketValueText
- unrealizedPnlText
- unrealizedPnlRatioText
- currency
- dataQualityStatus
- warningCount

Contract:

- Read-only.
- Not a fact source.
- May be sourced from replay or position_snapshot.
- 当前未实现.
- QML must not calculate quantity, cost, market value, or PnL.

## CashSummaryDto

Fields:

- accountId
- portfolioId
- cashBalanceText
- availableCashText
- frozenCashText
- netContributionText
- currency
- dataQualityStatus

Contract:

- Read-only.
- Not a fact source.
- May be sourced from replay or cash_snapshot.
- 当前未实现.
- QML must not calculate cash balance, available cash, frozen cash, or net
  contribution.

## PortfolioPnlDto

Fields:

- portfolioId
- totalAssetsText
- principalText
- cashText
- marketValueText
- realizedPnlText
- unrealizedPnlText
- totalReturnRatioText
- dataQualityStatus

Contract:

- Read-only.
- Not a fact source.
- May be sourced from replay, valuation inputs, or portfolio_summary.
- 当前未实现.
- QML must not calculate total assets, principal, PnL, or return ratio.

## BasePositionDto

Fields:

- portfolioId
- targetBaseRatioText
- currentBaseRatioText
- lockedBaseAmountText
- basePositionStatus
- warningText

Contract:

- Read-only.
- Not a fact source.
- May be sourced from replay, policy configuration, or future snapshots.
- 当前未实现.
- QML must not calculate base-position completion, locked amount, or warning
  state.

## SniperPoolDto

Fields:

- portfolioId
- poolAmountText
- usedAmountText
- remainingAmountText
- tierSummary
- calculationVersion

Contract:

- Read-only.
- Not a fact source.
- May be sourced from replay, cash facts, policy configuration, or future
  snapshots.
- 当前未实现.
- QML must not calculate sniper-pool size, tier allocation, or tier completion.

## Error Status Enum Draft

- OK
- WARNING
- ERROR
- STALE
- INCOMPLETE
- UNSUPPORTED

## Data Version Fields Draft

- calculationVersion
- sourceFromTime
- sourceToTime
- generatedAt
- stale

## Prohibited Use

- DTOs must not write to the database.
- DTOs must not drive trading.
- DTOs must not generate TradeDraft.
- DTOs must not become fact sources.
- DTOs must not hide accounting errors.
- DTOs must not be calculated directly in QML.

## Superseded By Stable Contract

This draft has been refined into [Position Accounting Data Contract](22_position_accounting_data_contract.md).
It remains as an early draft record for TASK-044. Future implementation planning
should use the stable contract and [Position Accounting Test Fixture Design](23_position_accounting_test_fixture_design.md).
