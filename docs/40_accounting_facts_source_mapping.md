# Accounting Facts Source Mapping

## Document Purpose

This document maps future AccountingEngine DTOs to SQLite and DataAccess fact
sources. It does not implement code and does not modify schema.

## DTO and Source Overview

- `TradeFactDto` -> `trade_log`
- `CashFactDto` -> future cash facts source or initial cash source; current
  source is pending design
- `MarketPriceFactDto` -> future market price facts source; current production
  source is not implemented
- `FxRateFactDto` -> future FX rate facts source; current production source is
  not implemented
- `ReplayRequestDto` -> DataService request payload
- `AccountingIssueDto` -> validation and replay issue output

## TradeFactDto Mapping Draft

Draft fields:

- `factId`
- `tradeTime`
- `accountId`
- `portfolioId`
- `instrumentCode`
- `action`
- `quantityText`
- `priceText`
- `amountText`
- `feeText`
- `cashFlowText`
- `currency`
- `source`
- `note`

`feeText` missing must not default to `0`. SELL quantity exceeding available
position is not a query-layer error; it is a replay-layer issue. The query layer
only preserves facts.

## CashFactDto Mapping Draft

The current cash facts source requires review.

Initial cash, deposit, withdrawal, and adjustment facts must have auditable
sources. Cash facts cannot be inferred from `cash_snapshot`. Cash facts cannot
be inferred from `portfolio_summary`. cash_snapshot 是派生数据, not a facts
source.

## MarketPriceFactDto Mapping Draft

Current production market price facts are not implemented.

When market price facts are missing, the future query path must return
`MARKET_PRICE_MISSING`. Price facts must include time and source. Market prices
must not be fetched from the network during replay.

## FxRateFactDto Mapping Draft

Current production FX rate facts are not implemented.

When FX rate facts are missing, the future query path must return
`FX_RATE_MISSING` or `MULTI_CURRENCY_UNSUPPORTED`. FX facts must include time
and source. FX rates must not be fetched from the network during replay.

## Metadata Mapping

Draft metadata fields:

- `sourceFromTime`
- `sourceToTime`
- `generatedAt`
- `calculationVersion`
- `replayEngineVersion`
- `queryVersion`
- `rowCounts`
- `dataQualityStatus`

## Open Questions

- How should the cash facts source be designed?
- Should a market price facts table be added?
- Should an FX rate facts table be added?
- Is `trade_log` sufficient to express fees and currency?
- What filtering rules should apply to multiple accounts and portfolios?
- What pagination and performance boundaries are needed?
- How should timezone and timestamp normalization work?

## TASK-085 position.list Guard Boundary

The `position.list` guard does not use this facts mapping. It returns a
`PositionListResponse` future output shape with an empty `positions` array only
to describe the contract.

Future real `position.list` implementation is the first point where this
mapping may be used, and that work requires a separate task.

## TASK-086 No-write Harness Boundary

The no-write harness skeleton does not read facts and does not implement this
mapping. It only verifies that future callbacks and SQL snippets can be tested
against protected table row counts and forbidden write keywords.

Future mapping implementation must use the read-only boundary in
`docs/39_sqlite_readonly_facts_query_boundary.md`, must not use snapshots as
facts, and must pass no-write harness checks before DataService integration.

## TASK-087 trade_log to TradeFactRow Skeleton Mapping

TASK-087 adds a DataAccess-side trade facts reader that maps current
`trade_log` rows to `TradeFactRow`. This is a skeleton mapping and not an
AccountingEngine DTO mapping.

Current mapping:

- `factId` <- `trade_log.uid`
- `tradeTime` <- `COALESCE(local_time, created_at_utc)`
- `accountId` <- `CAST(account_id AS TEXT)`
- `portfolioId` <- `CAST(portfolio_id AS TEXT)`
- `instrumentCode` <- `actual_code`
- `action` <- `action_type`
- `quantityText` <- `quantity_1e6` formatted as decimal quantity
- `priceText` <- `price_1e6` formatted as decimal price plus currency when
  present
- `amountText` <- `amount_cents` formatted as money text
- `feeText` <- `fee_cents` formatted as money text
- `cashFlowText` <- `net_cash_impact_cents` formatted as money text
- `currency` <- `instrument.currency`
- `source` <- `trade_source`
- `note` <- `memo`

Mapping gaps:

- current `trade_log.fee_cents` is `NOT NULL DEFAULT 0`, so a missing fee cannot
  be distinguished from an explicit zero fee at this layer;
- account and portfolio are currently exposed as numeric id text, not external
  account or portfolio DTO ids;
- cash facts, market price facts, and FX facts remain unmapped;
- DataServiceApi or a future mapper must decide how to convert this
  DataAccess-side row into AccountingEngine DTOs.

SELL quantity exceeding position is still a replay-layer issue, not a query
layer issue. The query layer preserves facts only.

## TASK-088 CashFactDto Source Boundary

`CashFactDto` mapping remains in boundary and decision status. The source
boundary is defined in `docs/41_cash_facts_source_boundary.md`, and the
readiness decision is defined in `docs/42_cash_facts_query_decision.md`.

Future code must not reverse-engineer cash facts from `cash_snapshot` or
`portfolio_summary`. `cash_snapshot` is derived data. `portfolio_summary` is
derived data. Snapshot values are not facts sources.

`trade_log.net_cash_impact_cents` is a transaction cash impact field. It is not
an initial cash source, not a deposit source, and not a withdraw source. Schema
gaps around initial cash, deposit, withdraw, adjustment, fee distinction,
currency, account, and portfolio sources need later review before any cash facts
query implementation.

## TASK-089 cash.summary Guard Mapping Boundary

The `cash.summary` guard does not use this facts mapping. It returns a
`CashSummaryResponse` future output shape with `cashSummary=null` and an empty
`accountCashSummaries` array only to describe the contract.

Future real `cash.summary` implementation is the point where `CashFactDto`
mapping may be used, after `docs/41_cash_facts_source_boundary.md` and
`docs/42_cash_facts_query_decision.md` confirm the facts source.

## TASK-090 portfolio.pnl.summary Guard Mapping Boundary

The `portfolio.pnl.summary` guard does not use this facts mapping. It returns a
`PortfolioPnlSummaryResponse` future output shape with `portfolioPnl=null` only
to describe the contract.

Future real `portfolio.pnl.summary` implementation is the point where
`TradeFactDto`, `CashFactDto`, and `MarketPriceFactDto` mapping may be composed
for PnL calculation. That work still requires separate read-only query,
AccountingEngine integration, and no-write test authorization.

## TASK-091 base_position.summary Guard Mapping Boundary

The `base_position.summary` guard does not use this facts mapping. It returns a
`BasePositionSummaryResponse` future output shape with `basePosition=null` only
to describe the contract.

Future real `base_position.summary` implementation is the point where
`TradeFactDto`, position derivation, and portfolio-derived DTO mapping may be
composed for base position calculation. That work still requires separate
read-only query, AccountingEngine integration, and no-write test authorization.
`sellableAboveBaseAmountText` is not a trade suggestion.

## TASK-092 sniper_pool.summary Guard Mapping Boundary

The `sniper_pool.summary` guard does not use this facts mapping. It returns a
`SniperPoolSummaryResponse` future output shape with `sniperPool=null` and an
empty `tierSummary` array only to describe the contract.

Future real `sniper_pool.summary` implementation is the point where
`TradeFactDto`, cash facts, tier derivation, and sniper-pool-derived DTO mapping
may be composed for sniper pool calculation. That work still requires separate
read-only query, AccountingEngine integration, and no-write test authorization.
`remainingAmountText` is not a trade suggestion, and `completed` is not derived
from current market value by the guard.

## TASK-093 Guard Suite Mapping Boundary

TASK-093 records the DataService accounting guard suite milestone in
`docs/43_dataservice_accounting_guard_suite_milestone.md` and the no-write
readiness review in
`docs/44_dataservice_accounting_guard_no_write_readiness_review.md`.

The guard suite does not use this facts mapping to produce real accounting
outputs. `position.list`, `cash.summary`, `portfolio.pnl.summary`,
`base_position.summary`, and `sniper_pool.summary` still return guard payloads.
Future real actions must explicitly authorize DTO mapping from read-only facts
to AccountingEngine request / response structures.

## TASK-094 position.list Mapping Boundary

Future real `position.list` requires `TradeFactRow` -> `TradeFactDto` mapping,
as documented in `docs/45_position_list_real_implementation_boundary.md`.
Readiness is tracked in
`docs/46_position_list_real_implementation_readiness_checklist.md`.

Cash facts, market price, and FX gaps affect `position.list`
`dataQualityStatus` and issues. Missing market price must not fabricate
valuation, and multi-currency input without FX facts must not fabricate rates.

## TASK-095 position.list Mapping Contract Tests

TASK-095 adds test-only `TradeFactRow` -> `TradeFactDto` mapping contract
tests. The mapping preserves `feeText` as-is and does not correct the known
`trade_log.fee_cents NOT NULL DEFAULT 0` schema gap. That gap still requires a
future design decision before real missing-fee semantics can be trusted from
production query data.

The tests are contract tests only; they do not connect DataAccess to
DataService and do not implement real `position.list`.
