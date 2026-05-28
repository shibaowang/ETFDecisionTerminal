# DataService Accounting Guard No-write Readiness Review

## Document Purpose

This document reviews whether the DataService accounting guard suite is ready
to enter future real read-only action implementation work.

This document does not implement real actions, does not connect SQLite, and
does not call AccountingEngine.

## Readiness Summary

- Guard suite exists.
- No-write harness exists.
- No-write guard tests exist.
- ForbiddenSqlScanner exists.
- DataAccess trade facts reader skeleton exists.
- Cash facts source remains unconfirmed.
- Market price source remains unconfirmed.
- FX rate source remains unconfirmed.
- DataService and AccountingEngine are not integrated.
- QML is not integrated.

This is enough for contract readiness review. It is not enough to replace any
guard with a real implementation.

## Go / No-Go Checklist

- [ ] All guard actions have been merged to `main`.
- [ ] All guard actions have `implemented=false`.
- [ ] All guard actions have `readOnly=true`.
- [ ] All guard actions have `writeEnabled=false`.
- [ ] All guard action no-write tests pass.
- [ ] AccountingEngine has not been called by DataService.
- [ ] SQLite facts query has not been called by DataService.
- [ ] QML does not directly call DataServiceClient.
- [ ] Snapshot has not been written.
- [ ] TradeLog has not been written.
- [ ] TradeDraft has not been generated.
- [ ] The next task boundary is explicit.

## Preconditions For Real position.list

- Trade facts reader exists and is still read-only.
- Cash facts source needs confirmation.
- AccountingEngine DTO mapping needs confirmation.
- No-write tests must be extended for the real action path.
- Changing the DataService payload from `implemented=false` to a real result
  must be a separate task.
- Failure behavior must be fixed before implementation.
- QML must remain disconnected unless separately authorized.

## Preconditions For Real cash.summary

- CashFactDto source must be confirmed.
- `cash_snapshot` and `portfolio_summary` must not be used to reverse-engineer
  facts.
- If schema is insufficient, schema / cash ledger design must happen first.
- No-write tests must be extended.

## Preconditions For Real portfolio.pnl.summary

- Trade facts, cash facts, and market price facts sources must be confirmed.
- Missing market price must not fabricate valuation.
- Multi-currency input must not fabricate FX rates.
- No-write tests must be extended.

## Preconditions For Real base_position.summary

- Base position rules must be frozen.
- `sellableAboveBaseAmountText` is not a trade suggestion.
- TradeDraft generation is forbidden.
- No-write tests must be extended.

## Preconditions For Real sniper_pool.summary

- Sniper pool rules must be frozen.
- `completed` is not derived from current market value.
- `remainingAmountText` is not a trade suggestion.
- TradeDraft generation is forbidden.
- Strategy execution is forbidden.
- No-write tests must be extended.

## Risk List

- The guard suite is mistaken for a real implementation.
- QML directly calls DataServiceClient.
- A DataService action writes to the database.
- Snapshot data is used to reverse-engineer facts.
- AccountingEngine is connected to DataService too early.
- Cash facts source remains unclear.
- Market price source remains unclear.
- FX rate source remains unclear.
- basePosition or sniperPool output is mistaken for a trade suggestion.
- TradeDraft generation is connected too early.
- Strategy execution is connected by mistake.

## Recommended Next Task

Recommended candidates:

- TASK-094: DataService accounting guard suite release notes / tag readiness.
- TASK-094: `position.list` real implementation boundary design.
- TASK-094: ShellServices accounting controller contract docs.

Do not directly change a guard into a real implementation without a dedicated
task. Do not directly connect QML. Do not write snapshots. Do not directly call
AccountingEngine unless that boundary is explicitly authorized.
