# SQLite Read-only Facts Query Boundary

## Document Purpose

This document defines the read-only boundary for future SQLite facts queries
needed by production accounting replay.

This document does not implement SQLite queries. It does not add a DataAccess
repository. It does not add a DataService action. It does not call
AccountingEngine. It exists for review before any implementation task is
authorized.

## Current Status

- AccountingEngine has a production-side DTO-only replay skeleton.
- DataService accounting action contracts are defined.
- DataService accounting no-write test plans are defined.
- SQLite facts query implementation is not present.
- DataAccess accounting facts repository is not present.
- DataService does not call AccountingEngine.
- Snapshot writes are not implemented.
- TradeLog writes are not authorized.

## Core Principles

- `trade_log` is the fact ledger.
- snapshot 是派生数据.
- Snapshots are derived data.
- Snapshots must not be used as facts source.
- Replay first phase is read-only.
- SQLite query first phase is read-only.
- DataAccess only reads and maps facts.
- DataAccess does not implement replay algorithms.
- AccountingEngine receives DTOs and does not directly access SQLite.
- DataServiceApi must not host complex replay algorithms.
- QML does not access SQLite.
- QML does not calculate accounting.
- Every write path requires a separate authorized task.

## Future Fact Types

### A. TradeFactDto

Candidate source: `trade_log`.

Draft field mapping:

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

### B. CashFactDto

Candidate source: initial cash, deposit, withdrawal, and adjustment facts.

If the current schema has no independent cash facts source, future design is
required before production replay can rely on cash facts.

Draft field mapping:

- `factId`
- `time`
- `accountId`
- `portfolioId`
- `action`
- `amountText`
- `currency`
- `note`

### C. MarketPriceFactDto

Candidate source: future market price facts table. It is not implemented today.
Market prices must not be fetched from the network during replay.

Draft field mapping:

- `instrumentCode`
- `priceTime`
- `priceText`
- `currency`
- `source`

### D. FxRateFactDto

Candidate source: future FX rate facts table. It is not implemented today. FX
rates must not be fetched from the network during replay.

Draft field mapping:

- `baseCurrency`
- `quoteCurrency`
- `rateTime`
- `rateText`
- `source`

## trade_log Query Boundary

- Only `SELECT` is allowed.
- Queries must filter by `sourceFromTime` and `sourceToTime` when supplied.
- Queries must support optional `accountId` and `portfolioId` filters.
- Sorting must be stable, preferably by `tradeTime` and stable row id.
- `action` must map explicitly to `BUY` or `SELL`.
- Missing fee must remain missing; it must not default to `0`.
- `currency` source must be explicit.
- Invalid trade facts should become an issue or validation error.
- Do not `UPDATE trade_log`.
- Do not fix `trade_log` based on replay results.
- Do not write adjustments from the query layer.

## Cash Facts Query Boundary

- If no independent cash facts table exists, cash facts must not be fabricated.
- Initial cash source must be explicit.
- Deposit, withdrawal, and adjustment facts must have explicit sources.
- If the schema cannot support the requested cash facts, return
  `REPLAY_NOT_AVAILABLE` or `DATA_SOURCE_MISSING`.
- Do not derive cash facts from `cash_snapshot`.
- Do not write cash facts.

## Market Price Facts Query Boundary

- Production market price facts table is not implemented today.
- Missing market price facts must return `MARKET_PRICE_MISSING`.
- Do not fetch market prices over the network.
- Do not fabricate prices.
- Do not replace historical prices with the latest price.
- Do not write market price tables.
- A valid price fact needs `priceTime`, `source`, and `currency`.

## FX Rate Facts Query Boundary

- Production FX rate facts table is not implemented today.
- Missing FX rate facts must return `FX_RATE_MISSING` or
  `MULTI_CURRENCY_UNSUPPORTED`.
- Do not fetch FX rates over the network.
- Do not fabricate FX rates.
- Do not treat USD as CNY.
- Do not write FX rate tables.
- A valid FX fact needs `rateTime` and `source`.

## Snapshot Boundary

- `position_snapshot`, `cash_snapshot`, and `portfolio_summary` are derived
  data.
- snapshot 是派生数据, not the fact source.
- Replay query must not treat snapshots as facts source.
- Stale snapshots must not be used for normal replay.
- Snapshots can only be future display caches or validation references.
- Snapshot writes require a separate task.
- Snapshots must not rewrite `trade_log`.

## Read-only SQLite Connection Boundary

- Future implementation should prefer SQLite read-only open mode.
- Do not use `BEGIN IMMEDIATE`.
- Do not use `BEGIN EXCLUSIVE`.
- Do not execute `INSERT`.
- Do not execute `UPDATE`.
- Do not execute `DELETE`.
- Do not execute `CREATE`.
- Do not execute `DROP`.
- Do not execute `ALTER`.
- Do not execute `REPLACE`.
- Do not execute `VACUUM`.
- Do not use `PRAGMA writable_schema`.
- Do not run migrations.
- Do not mutate schema.
- If the SQLite driver cannot enforce read-only handles, no-write table count
  tests must compensate.
- Queries must be verifiable against temporary databases and test databases.

## Query API Boundary Draft

Draft future interface name:

`AccountingFactsReadOnlyRepository`

Draft methods:

- `loadTradeFacts(request)`
- `loadCashFacts(request)`
- `loadMarketPriceFacts(request)`
- `loadFxRateFacts(request)`
- `loadReplayFactsBundle(request)`

Draft `ReplayFactsBundle` fields:

- `tradeFacts`
- `cashFacts`
- `marketPriceFacts`
- `fxRateFacts`
- `issues`
- `metadata`

This is only a draft. This task does not create interface code. If a future
task creates the repository, it must be separately authorized.

## Failure Behavior

Future query boundary failures include:

- `DATA_SOURCE_MISSING`
- `INVALID_TIME_RANGE`
- `INVALID_REQUEST`
- `DATA_VERSION_MISMATCH`
- `MARKET_PRICE_MISSING`
- `FX_RATE_MISSING`
- `SNAPSHOT_STALE`
- `REPLAY_NOT_AVAILABLE`
- `SQLITE_READONLY_OPEN_FAILED`
- `SQLITE_QUERY_FAILED`

Failures must not trigger writes. Invalid requests must not write. Query
failures must not write. Replay unavailable results must not write.

## Required Tests Before Implementation

- read-only open test
- `SELECT`-only SQL scanner
- forbidden SQL keyword scanner
- no-write table count before / after
- invalid request no-write test
- query failure no-write test
- empty ledger query test
- single buy facts query test
- missing fee facts query test
- missing market price facts query test
- multi-currency no FX facts query test
- transport repeat test

## Forbidden Items

- Do not calculate replay in the query layer.
- Do not calculate PnL in the query layer.
- Do not calculate basePosition or sniperPool in the query layer.
- Do not generate TradeDraft in the query layer.
- Do not write snapshots in the query layer.
- Do not write TradeLog rows in the query layer.
- Do not call external market or FX services in the query layer.
- Do not access SQLite from QML.
- Do not use the test-only fixture loader as production DataAccess.

## TASK-085 position.list Guard Boundary

The `position.list` guard does not use SQLite facts query. It returns
`POSITION_LIST_NOT_AVAILABLE` without reading `trade_log`, cash facts, market
price facts, FX rate facts, or snapshots.

Future SQLite query integration for real `position.list` requires a separate
task and must keep the read-only and no-write boundaries in this document.

## TASK-086 Read-only / No-write Harness Skeleton

TASK-086 adds a test-only read-only / no-write harness skeleton. It validates
the testing boundary needed before a future SQLite facts query can be
implemented.

The harness provides protected table row-count checks and a forbidden SQL
scanner, but it does not implement a SQLite facts query. The DataAccess
accounting facts repository remains unimplemented, and `position.list` remains a
guard.

Any future SQLite facts query implementation must pass the no-write harness,
must keep SQL `SELECT`-only, and must remain separately authorized.

## TASK-087 trade_log Read-only Facts Query Skeleton

TASK-087 adds the first SQLite facts query skeleton: a DataAccess-side
`trade_log` read-only facts query skeleton and trade facts reader.
In plain terms, this is the trade_log read-only facts query skeleton.

Current scope:

- only trade facts from `trade_log`;
- only `SELECT` SQL;
- optional `accountId`, `portfolioId`, `sourceFromTime`, `sourceToTime`,
  `limit`, and `offset` filters;
- stable ordering by `created_at_utc` and `id`;
- DataAccess-side `TradeFactRow` output, not AccountingEngine DTO output;
- no replay calculation;
- no snapshot reads as facts;
- no writes.

Still not implemented:

- cash facts query;
- market price facts query;
- FX rate facts query;
- DataService action integration;
- AccountingEngine integration;
- real `position.list`.

The reader must remain covered by the TASK-086 no-write harness and forbidden
SQL scanner before any future DataService integration.

## TASK-088 Cash Facts Source Boundary

TASK-088 defines the cash facts source boundary in
`docs/41_cash_facts_source_boundary.md` and the implementation decision in
`docs/42_cash_facts_query_decision.md`.

The cash facts query remains unimplemented. `cash_snapshot`,
`portfolio_summary`, and `position_snapshot` are derived data and must not be
used as cash facts sources. `trade_log.net_cash_impact_cents` can describe
transaction cash impact, but it is not an initial cash, deposit, or withdraw
facts source.

Any future cash facts query must reference `docs/41_cash_facts_source_boundary.md`
and `docs/42_cash_facts_query_decision.md`, must stay read-only, must use
`SELECT`-only SQL, and must pass the no-write harness before DataService
integration.

## TASK-089 cash.summary Guard Boundary

The `cash.summary` guard does not use SQLite facts query. It returns
`CASH_SUMMARY_NOT_AVAILABLE` without reading cash facts, `cash_snapshot`,
`portfolio_summary`, `trade_log`, market price facts, FX rate facts, or
snapshots.

Future SQLite cash facts query integration for real `cash.summary` requires a
separate task and must keep the read-only and no-write boundaries in this
document.

## TASK-090 portfolio.pnl.summary Guard Boundary

The `portfolio.pnl.summary` guard does not use SQLite facts query. It returns
`PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE` without reading `trade_log`, cash facts,
market price facts, `cash_snapshot`, `position_snapshot`, `portfolio_summary`,
FX rate facts, or snapshots.

Future SQLite trade, cash, and market price facts query integration for real
`portfolio.pnl.summary` requires separate tasks and must keep the read-only and
no-write boundaries in this document.

## TASK-091 base_position.summary Guard Boundary

The `base_position.summary` guard does not use SQLite facts query. It returns
`BASE_POSITION_SUMMARY_NOT_AVAILABLE` without reading `trade_log`,
`position_snapshot`, `cash_snapshot`, `portfolio_summary`, FX rate facts,
market price facts, or snapshots.

The guard does not generate trade suggestions and does not generate TradeDraft
rows. Future SQLite trade facts or position derivation integration for real
`base_position.summary` requires separate tasks and must keep the read-only and
no-write boundaries in this document.

## TASK-092 sniper_pool.summary Guard Boundary

The `sniper_pool.summary` guard does not use SQLite facts query. It returns
`SNIPER_POOL_SUMMARY_NOT_AVAILABLE` without reading `trade_log`,
`position_snapshot`, `cash_snapshot`, `portfolio_summary`, FX rate facts,
market price facts, or snapshots.

The guard does not calculate T1-T6 tiers, does not generate trade suggestions,
and does not generate TradeDraft rows. Future SQLite trade facts or tier
derivation integration for real `sniper_pool.summary` requires separate tasks
and must keep the read-only and no-write boundaries in this document.
