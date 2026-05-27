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
