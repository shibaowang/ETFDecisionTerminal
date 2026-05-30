# ShellAccounting Read-Only Replay Implementation

## TASK-139 Purpose

TASK-139 implements ShellAccounting AccountingEngine read-only replay after the TASK-138 replay / snapshot rebuild gate. This is read-only replay only. It is not snapshot rebuild, not snapshot write, not TradeDraft, not trade suggestion, not strategy execution, and not broker order submission.

## Scope

The authorized read-only actions remain:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

Replay is explicit and static-scannable through the DataService action payload `calculationMode: readonlyReplay`. The existing TASK-137 read-only facts query remains the rollback and fallback path.

## DataService Boundary

AccountingEngine replay only occurs inside the DataService boundary. Shell, QML, Presenter, and ShellServices do not directly call `AccountingReplayEngine`, SQLite, DataAccess, `replayFromFacts`, or any replay helper. DataService loads read-only trade facts and read-only initial cash facts, maps them to replay DTOs, and maps replay output back into the existing read-only action payloads.

## Replay Input

Replay input comes from authorized read-only sources only:

- `AccountingTradeFactReader` for `trade_log` facts, SELECT-only.
- `ShellAccountingReadOnlyFactsQuery::loadInitialCashFacts` for account initial cash facts, SELECT-only.
- Existing TASK-137 facts query remains available for base position / sniper pool read-only rows and fallback.

No fake trade facts are generated. Missing or unsupported facts map to explicit empty, warning, unavailable, or error state.

## Replay Output

Replay output is mapped only to read-only accounting state:

- `position.list`: replayed `PositionSummaryDto` rows.
- `cash.summary`: replayed cash summary rows.
- `portfolio.pnl.summary`: replayed PnL summary, with missing market price mapped to warning/unavailable valuation fields.
- `base_position.summary`: DataService-only replay metadata plus TASK-137 read-only base allocation facts.
- `sniper_pool.summary`: DataService-only replay metadata plus TASK-137 read-only tier facts.

## Forbidden Scope

snapshot rebuild still not implemented. Snapshot write still not authorized. TASK-139 does not write `cash_snapshot`, `position_snapshot`, `portfolio_summary`, `audit_log`, `trade_log`, `trade_execution_group`, or `trade_draft`. It does not modify `migrations/001_initial_schema.sql`.

TASK-139 does not generate TradeDraft, trade suggestions, strategy execution, broker orders, or trading UI.

## Error Mapping

Replay maps these cases without silent success:

- missing facts -> empty/unavailable read-only payload.
- missing market price -> `MARKET_PRICE_MISSING` issue and unavailable valuation fields.
- missing FX rate -> `FX_RATE_MISSING` issue.
- unsupported currency -> explicit replay issue.
- negative cash -> `NEGATIVE_CASH` issue.
- sell exceeds position -> `SELL_EXCEEDS_POSITION` issue.
- inconsistent facts -> blocking issue list.
- query error -> query-error payload without raw SQL or raw payload exposure.
- unsupported replay scenario -> explicit replay-not-implemented issue.

## Privacy Policy

QML receives only mapped read-only state and issue summaries. Payloads keep `rawSqlExposed: false`, `rawTradeLogPayloadExposed: false`, and `rawReplayPayloadExposed: false`. The response does not include raw SQL, full `trade_log` payloads, internal exception stacks, or sensitive raw replay payloads.

## Rollback / Fallback

Rollback can disable the explicit `readonlyReplay` calculation path and fallback to TASK-137 read-only facts query. Fallback to TASK-137 read-only facts query includes no fake data, must not write database tables, must not rebuild snapshots, must not generate TradeDraft, and must not bypass DataService.

## Tests

TASK-139 adds `shell_accounting_readonly_replay_*` implementation tests for DataService-only replay boundary, the five read-only actions, missing facts, missing market price, missing FX, inconsistent facts, no snapshot rebuild, no snapshot write, no trade / strategy, privacy, and rollback readiness.

## Unauthorised Future Scope

Snapshot rebuild, snapshot write authorization, AccountingEngine replay expansion beyond the read-only DataService boundary, real trade suggestions, TradeDraft generation, strategy execution, and broker order submission all require separate future tasks.

## TASK-140 Update

TASK-140 adds the snapshot rebuild / snapshot write gate. TASK-139 read-only
replay remains the fallback. Snapshot rebuild/write is still not implemented,
still requires separate explicit authorization, and still must not introduce
TradeDraft, trade suggestion, strategy execution, broker order, or trading UI.
