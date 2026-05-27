# Release Notes - v0.4 AccountingEngine Replay Skeleton

## Date

TBD

## Highlights

- AccountingEngine production-side DTO-only replay skeleton.
- Empty ledger.
- Single BUY.
- BUY + SELL partial sell.
- SELL_EXCEEDS_POSITION.
- MISSING_FEE.
- NEGATIVE_CASH.
- Multi-instrument BUY.
- Multi-account BUY.
- Multi-currency unsupported.
- Missing market price.
- Boundary tests and no-write/no-dependency guard.

## Not Included

- DataService action.
- SQLite integration.
- `productionReady=true`.
- Full `replayImplemented=true`.
- snapshot writes.
- TradeLog writes.
- TradeDraft.
- QML accounting page.
- real market data.
- real FX rate.
- broker API.
- auto trading.

## Validation

- Full CTest passed.
- Transport repeat passed.
- Boundary tests passed.
- DevDocs static checks passed.

## Suggested Tag After Merge

`v0.4.0-accounting-engine-replay-skeleton`

This task does not create a Git tag. The tag should only be created manually by
the user after the milestone PR is merged.
