# Release Notes - v0.5 DataService Accounting Guard Suite

## Date

TBD

## Highlights

- `accounting.health`
- `accounting.replay.preview` guard
- `position.list` guard
- `cash.summary` guard
- `portfolio.pnl.summary` guard
- `base_position.summary` guard
- `sniper_pool.summary` guard
- DataServiceClient guard wrappers
- No-write guard tests
- Readiness review

## Not Included

- Real `position.list`
- Real `cash.summary`
- Real `portfolio.pnl.summary`
- Real `base_position.summary`
- Real `sniper_pool.summary`
- SQLite facts query integration
- AccountingEngine integration
- QML integration
- Snapshot writes
- TradeLog writes
- TradeDraft
- Strategy execution
- Broker API
- Auto trading

## Validation

- Full CTest passed.
- Transport repeat passed.
- No-write guard tests passed.
- DevDocs static checks passed.

## Suggested Tag After Merge

`v0.5.0-dataservice-accounting-guard-suite`

This task does not create a Git tag.
