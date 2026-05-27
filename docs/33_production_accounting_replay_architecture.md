# Production Accounting Replay Architecture Boundary

## Purpose

This document defines the architecture boundary for production accounting
replay.

This document does not implement code. It exists to guide future decisions about
whether to add `libs/AccountingEngine` and how that module would collaborate
with DataAccess, DataServiceApi, DataServiceClient, ShellServices, and QML.

The test-only `AccountingReplayMinimalEngine` under `tests/AccountingFixtures`
must not be directly migrated to production.

## Current State

- FX001-FX013 test-only coverage complete.
- `accounting.health` is implemented as a read-only action.
- `accounting.replay.preview` is still a guard action.
- Production accounting replay is not implemented.
- DataService `position.list` is not implemented.
- DataService `cash.summary` is not implemented.
- DataService `portfolio.pnl.summary` is not implemented.
- SQLite production replay is not connected.
- Snapshot writes are not implemented.
- TradeLog writes are not authorized.

## Architecture Principles

- `trade_log` is the fact ledger.
- Snapshot data is derived data.
- Production replay is read-only by default.
- Replay results must be explainable, reproducible, and testable.
- QML does not calculate accounting.
- ShellServices does not calculate accounting.
- DataServiceApi does not carry complex replay algorithms.
- DataAccess does not carry replay algorithms.
- Write actions require separate authorization.
- Snapshot writes require separate authorization.
- The test-only engine must not be directly migrated to production.

## Candidate Module Layers

### A. CoreDomain

- Contains stable domain types only.
- Avoids Qt dependencies.
- Does not access SQLite.
- Does not perform replay I/O.

### B. DataAccess

- Wraps SQLite access.
- Provides controlled read-only fact queries when authorized.
- Does not implement replay algorithms.
- Does not write business tables unless a task explicitly authorizes it.

### C. AccountingEngine Candidate

- Candidate future module: `libs/AccountingEngine`.
- Hosts production replay algorithms.
- Accepts facts, market price DTOs, and FX rate DTOs.
- Emits accounting DTOs.
- By default, has no direct SQLite access and receives facts from an application
  layer such as DataServiceApi using DataAccess.
- Does not write tables by default.
- Does not depend on QML.
- Does not depend on ServiceHost.
- Does not depend on Watchdog.

### D. DataServiceApi

- Registers actions.
- Validates requests.
- Calls DataAccess to read facts.
- Calls AccountingEngine when authorized.
- Returns ProtocolResponse.
- Does not carry complex replay algorithms.
- Is read-only by default for production replay.

### E. DataServiceClient

- Calls DataService actions.
- Does not calculate accounting.
- Does not expose arbitrary action passthrough to QML.

### F. ShellServices

- Converts DTOs into ViewModel / Model state.
- Does not calculate accounting.
- Does not access SQLite.
- Does not directly call complex lower-level services beyond approved client
  facades.

### G. QML

- Displays ViewModels only.
- Does not calculate holdings, cost, cash, principal, PnL, base position, or
  sniper pool fields.
- Does not call DataServiceClient.
- Does not access SQLite.

## Dependency Direction

Allowed draft directions:

- DataServiceApi -> DataAccess.
- DataServiceApi -> AccountingEngine.
- AccountingEngine -> CoreDomain.
- AccountingEngine -> Protocol DTO or independent DTO, pending future decision.
- ShellServices -> DataServiceClient.
- QML -> ShellServices.

Forbidden directions:

- QML -> DataServiceClient.
- QML -> DataAccess.
- QML -> AccountingEngine.
- AccountingEngine -> DataServiceApi.
- AccountingEngine -> SQLiteConnection.
- AccountingEngine -> QML / Qt Quick.
- DataAccess -> AccountingEngine.
- CoreDomain -> DataAccess.
- Production libs -> tests/AccountingFixtures.

## Data Source Boundary

Production replay input comes from fact data.

`trade_log` is the transaction fact source. Cash-flow facts need an explicitly
defined source. Market price and FX rate facts are external fact sources, but
they are not implemented yet.

Snapshots are not fact sources. A snapshot may be a cache or display
acceleration artifact, but it must not rewrite facts. Stale snapshots must not
be used as normal replay input.

## Read-only Replay Boundary

The first production replay stage must be read-only.

- Do not write `audit_log` unless a task explicitly authorizes diagnostic audit.
- Do not write `trade_log`.
- Do not write snapshots.
- Do not write `portfolio_summary`.
- Do not generate TradeDraft.
- Do not generate trade recommendations.
- Do not execute strategies.
- Do not place orders.

## Future DataService Action Boundary

This task does not implement any of these actions.

### `accounting.health`

- Status: implemented.
- Scope: read-only.
- Purpose: accounting boundary health.

### `accounting.replay.preview`

- Status: guard implemented.
- Scope: read-only guard.
- Failure behavior: returns replay unavailable state instead of calculated
  results.

### `position.list`

- Status: not implemented.
- Scope: should be read-only.
- Input: account, portfolio, as-of scope, and optional filters.
- Output: position list DTO.
- Depends on facts: trade facts, cash facts, market price facts where needed.
- Forbidden writes: all database writes, snapshots, TradeDraft, audit unless
  separately authorized.
- Failure behavior: explicit issues for missing price, stale data, unsupported
  accounting mode, and replay unavailable.
- Tests required: fixture-backed replay tests, action contract tests, no-write
  table-count tests, and failure behavior tests.

### `cash.summary`

- Status: not implemented.
- Scope: should be read-only.
- Input: account, portfolio, currency, and as-of scope.
- Output: cash summary DTO.
- Depends on facts: cash-flow facts and trade cash effects.
- Forbidden writes: all database writes and snapshots.
- Failure behavior: explicit issues for missing cash source, unsupported
  currency, and replay unavailable.
- Tests required: fixture-backed cash tests, action contract tests, and
  no-write table-count tests.

### `portfolio.pnl.summary`

- Status: not implemented.
- Scope: should be read-only.
- Input: account, portfolio, valuation scope, and currency policy.
- Output: portfolio PnL summary DTO.
- Depends on facts: trade facts, cash facts, market price facts, and FX facts
  when authorized.
- Forbidden writes: all database writes and snapshots.
- Failure behavior: explicit issues for missing market price, missing FX rate,
  stale snapshot, and replay unavailable.
- Tests required: fixture-backed PnL tests, missing data tests, and no-write
  table-count tests.

### `base_position.summary`

- Status: not implemented.
- Scope: should be read-only.
- Input: account, portfolio, policy scope, and as-of scope.
- Output: base position DTO.
- Depends on facts: position and policy inputs.
- Forbidden writes: all database writes, TradeDraft, and trade action output.
- Failure behavior: explicit issues for missing policy, missing price, and
  replay unavailable.
- Tests required: fixture-backed base-position tests and no-action tests.

### `sniper_pool.summary`

- Status: not implemented.
- Scope: should be read-only.
- Input: account, portfolio, pool policy, and as-of scope.
- Output: sniper pool DTO.
- Depends on facts: trade facts and pool policy inputs.
- Forbidden writes: all database writes, TradeDraft, buy action, and sell
  action output.
- Failure behavior: explicit issues for unsupported tier policy, missing facts,
  and replay unavailable.
- Tests required: fixture-backed sniper-pool tests and no-action tests.

## Replay Algorithm Boundary

Complex replay algorithms must not be written directly in DataServiceApi, QML,
ShellServices, or DataAccess.

The production replay algorithm should live in AccountingEngine or an explicitly
equivalent module. It needs unit tests covering FX001-FX013 and the error,
warning, stale, and unsupported cases.

## Snapshot Boundary

Production replay read-only stage does not write snapshots.

A snapshot writer must be a separate task. Any snapshot must include
`calculationVersion`, source range, and `generatedAt`. Stale snapshot detection
has test-only fixture coverage, but production snapshot writes are not
implemented.

Snapshots must not rewrite `trade_log`.

## TradeLog Boundary

TradeLog writes are not part of the production replay read-only stage.

Execution confirmation, manual accounting entry, Excel import, and automatic
trading are not part of this stage. Historical fact correction requires a
separate reversal or adjustment design.

Replay may read facts. It must not silently correct facts.

## Market Price / FX Boundary

Missing market price must not fabricate market value.

Missing FX rate must not fabricate multi-currency totals.

Real market data and FX services require separate tasks. AccountingEngine needs
injectable market price and FX facts. Replay must not directly perform network
queries for market data or FX rates.

## Error Handling Boundary

Production replay must explicitly return these issues where applicable:

- `SELL_EXCEEDS_POSITION`.
- `MISSING_FEE`.
- `NEGATIVE_CASH`.
- `MARKET_PRICE_MISSING`.
- `SNAPSHOT_STALE`.
- `MULTI_CURRENCY_UNSUPPORTED`.
- `FX_RATE_MISSING`.
- `DATA_VERSION_MISMATCH`.
- `REPLAY_NOT_AVAILABLE`.
- `UNSUPPORTED_ACCOUNTING_MODE`.

Errors must not be silently swallowed. Blocking issues must be able to reach the
UI. Warnings must not be disguised as OK.

## Production Readiness Gates

- Architecture document reviewed.
- Module boundary confirmed.
- DTO contract confirmed.
- DataAccess fact query design confirmed.
- No-write tests designed.
- FX001-FX013 tests confirmed as production replay tests.
- QML boundary confirmed.
- DataService action failure behavior confirmed.

## Explicit Prohibitions

- Do not directly migrate `AccountingReplayMinimalEngine` to `libs`.
- Do not write complex replay in DataServiceApi.
- Do not calculate accounting in QML.
- Do not use snapshots as fact sources.
- Do not write `trade_log`.
- Do not write snapshots.
- Do not generate TradeDraft.
- Do not execute strategies.
- Do not place orders.
- Do not connect broker APIs.
- Do not perform network market data or FX rate queries.

## TASK-070 AccountingEngine Skeleton Status

TASK-070 adds the `libs/AccountingEngine` production module skeleton.

The module currently contains only:

- `AccountingEngineInfo`.
- `AccountingEngineCapabilities`.
- `AccountingEngineBoundary`.
- `accounting_engine_boundary` tests.

Current declared state remains:

- No replay algorithm.
- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No write capability.
- No copied code from `tests/AccountingFixtures`.

Future replay implementation requires separate authorization and must preserve
the dependency boundary tests.

## TASK-071 DTO Parser Boundary Status

TASK-071 adds production replay DTO parser and validation boundaries to
`libs/AccountingEngine`.

The boundary currently provides:

- Request and fact DTO skeletons.
- Accounting issue DTO skeletons.
- Required-field validation.
- Enum validation.
- Source time range validation.
- Money text and quantity text validation.
- Currency code validation.

DTO validation is not replay. It does not calculate positions, cash, cost, PnL,
base position, sniper pool, market value, or FX conversion.

Current state remains:

- No replay algorithm.
- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No write capability.

## TASK-072 Empty Ledger Replay Skeleton Status

TASK-072 adds the first production-side read-only replay skeleton entry in
`libs/AccountingEngine`.

Scope:

- Only empty ledger input is supported.
- Empty ledger means no trade facts, no cash facts, no market price facts, and
  no FX rate facts.
- Empty ledger returns an OK result with empty positions and zero CNY cash / PnL
  summary fields.
- Non-empty facts return `UNSUPPORTED_SCENARIO` / `REPLAY_NOT_IMPLEMENTED`.

This is not full production replay. It does not implement FX002 or any buy,
sell, cash-flow, cost, PnL, base-position, sniper-pool, market-price, or FX
calculation.

Current boundaries remain:

- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No snapshot writes.
- No TradeLog writes.

## TASK-078 Multi-instrument BUY Scenario Status

TASK-078 adds a production-side read-only multi-instrument BUY scenario to
`libs/AccountingEngine`.

Scope:

- One `INITIAL_CASH` cash fact.
- Two or more `BUY` trade facts.
- Single account and portfolio.
- At least two distinct `instrumentCode` values.
- CNY only.
- Valid, present `feeText` on every BUY.
- Positions are grouped by `instrumentCode`.
- Different instruments are not merged or confused.
- Cash balance deducts the sum of all BUY amount plus fee.
- Returns `OK` with successful position and cash DTO shells.
- Does not calculate real market value or unrealized PnL.
- Does not output base-position or sniper-pool data.

The scenario keeps valuation fields `UNAVAILABLE` when market price facts are
absent. It does not connect to real market data and does not fabricate
portfolio total assets, market value, or unrealized PnL.

The implementation does not calculate multi-account replay, multi-currency
replay, multi-instrument SELL, mixed multi-instrument BUY/SELL, base position,
sniper pool, or production DataService actions.

Current boundaries remain:

- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No snapshot writes.
- No TradeLog writes.

## TASK-079 Multi-account BUY Scenario Status

TASK-079 adds a production-side read-only multi-account BUY scenario to
`libs/AccountingEngine`.

Scope:

- Two or more `INITIAL_CASH` cash facts.
- Two or more `BUY` trade facts.
- At least two distinct `accountId` values.
- One portfolio for the scenario.
- One CNY instrument, currently exercised with `159509`.
- CNY only.
- Valid, present `feeText` on every BUY.
- Positions are grouped by `accountId + portfolioId + instrumentCode`.
- Different accounts are not merged or confused.
- Cash summaries are emitted per account and are not combined into one total.
- Cash balances deduct only that account's BUY amount plus fee.
- Returns `OK` with successful position DTO shells and per-account cash DTO
  shells.
- Does not calculate real market value or unrealized PnL.
- Does not output base-position or sniper-pool data.

The scenario keeps valuation fields `UNAVAILABLE` when market price facts are
absent. It does not connect to real market data and does not fabricate
portfolio total assets, market value, or unrealized PnL.

The implementation does not calculate multi-currency replay, multi-account SELL,
cross-portfolio aggregation, base position, sniper pool, or production
DataService actions.

Current boundaries remain:

- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No snapshot writes.
- No TradeLog writes.

## TASK-080 Multi-currency Unsupported Detection Status

TASK-080 adds production-side read-only multi-currency unsupported detection to
`libs/AccountingEngine`.

Scope:

- Two or more `BUY` trade facts.
- At least one CNY fact.
- At least one non-CNY fact, currently exercised with USD.
- Missing FX rate facts trigger a controlled rejection.
- Returns `ERROR` with blocking `MULTI_CURRENCY_UNSUPPORTED` and
  `FX_RATE_MISSING` issues.
- Emits no successful positions, cash summaries, portfolio PnL, base-position,
  or sniper-pool data.

This is detection and rejection only. AccountingEngine still does not support
FX rate conversion, does not support successful multi-currency replay, and does
not fabricate FX rates, portfolio total assets, market value, or unrealized PnL.

If FX rate facts are present, this skeleton still does not perform FX
conversion or return a successful multi-currency valuation. Real FX rate
support and multi-currency valuation require separate authorization.

Current boundaries remain:

- `supportsFxRate=false`.
- `supportsMultiCurrency=false`.
- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No network access.
- No snapshot writes.
- No TradeLog writes.

## TASK-081 Missing Market Price Detection Status

TASK-081 adds production-side read-only missing market price detection to
`libs/AccountingEngine`.

Scope:

- One `INITIAL_CASH` cash fact.
- One CNY `BUY` trade fact.
- Requested outputs include `pnl`.
- Missing or non-matching `MarketPriceFactDto`.
- Returns `WARNING` with a non-blocking `MARKET_PRICE_MISSING` issue.
- May emit quantity, cost, and cash output.
- Keeps market value, unrealized PnL, and portfolio total assets unavailable.

This is detection and warning only. AccountingEngine still does not support real
market price lookup, network market queries, market value calculation, or
unrealized PnL calculation. A supplied market price fact does not unlock
successful valuation in this phase.

Current boundaries remain:

- `supportsMarketPrice=false`.
- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No network access.
- No market service calls.
- No snapshot writes.
- No TradeLog writes.

## TASK-076 Missing Fee Detection Status

TASK-076 adds a production-side read-only `MISSING_FEE` controlled warning
scenario to `libs/AccountingEngine`.

Scope:

- One `INITIAL_CASH` cash fact.
- One `BUY` trade fact.
- Single account and portfolio.
- Single instrument.
- CNY only.
- Missing, empty, or explicitly unavailable `feeText`.
- Returns `WARNING` with a non-blocking `MISSING_FEE` issue.
- Does not default the fee to zero.
- Does not output successful positions, cash summary, portfolio PnL,
  base-position, or sniper-pool data.

The implementation does not calculate the full negative-cash fixture scenario,
multi-transaction replay, multi-instrument replay, multi-account replay, market
value, unrealized PnL, base position, sniper pool, multi-currency replay, or
production DataService actions.

Current boundaries remain:

- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No snapshot writes.
- No TradeLog writes.

## TASK-077 Negative Cash Detection Status

TASK-077 adds a production-side read-only `NEGATIVE_CASH` controlled error
scenario to `libs/AccountingEngine`.

Scope:

- One `INITIAL_CASH` cash fact.
- One `BUY` trade fact.
- Single account and portfolio.
- Single instrument.
- CNY only.
- Valid, present `feeText`.
- `BUY amount + fee` greater than initial cash.
- Returns `ERROR` with a blocking `NEGATIVE_CASH` issue.
- Does not allow an implicit overdraft.
- Does not output successful positions, cash summary, portfolio PnL,
  base-position, or sniper-pool data.

`BUY amount + fee == initialCash` remains a successful single BUY with zero
cash balance. Missing `feeText` keeps returning `MISSING_FEE` and is not
overwritten by `NEGATIVE_CASH`.

The implementation does not calculate multi-transaction replay,
multi-instrument replay, multi-account replay, market value, unrealized PnL,
base position, sniper pool, multi-currency replay, or production DataService
actions.

Current boundaries remain:

- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No snapshot writes.
- No TradeLog writes.

## TASK-075 Sell Exceeds Position Detection Status

TASK-075 adds a production-side read-only `SELL_EXCEEDS_POSITION` controlled
error scenario to `libs/AccountingEngine`.

Scope:

- One `INITIAL_CASH` cash fact.
- One `BUY` trade fact and one `SELL` trade fact.
- Single account and portfolio.
- Single instrument.
- CNY only.
- `SELL` quantity greater than bought quantity.
- Returns `ERROR` with a blocking `SELL_EXCEEDS_POSITION` issue.
- Does not output successful positions, cash summary, portfolio PnL,
  base-position, or sniper-pool data.

The implementation does not calculate full missing-fee handling, full
negative-cash fixture handling, multi-transaction replay, multi-instrument
replay, multi-account replay, market value, unrealized PnL, base position,
sniper pool, multi-currency replay, or production DataService actions.

Current boundaries remain:

- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No snapshot writes.
- No TradeLog writes.

## TASK-074 Buy-Sell Partial Replay Skeleton Status

TASK-074 adds a production-side read-only BUY + SELL partial sell replay
skeleton to `libs/AccountingEngine`.

Scope:

- One `INITIAL_CASH` cash fact.
- One `BUY` trade fact and one `SELL` trade fact.
- Single account and portfolio.
- Single instrument.
- CNY only.
- Remaining position cost derived from allocated average cost.
- Cash balance derived from initial cash, buy cost, and net sell inflow.
- Realized PnL derived from net sell inflow minus allocated sold cost.

The implementation does not calculate multi-transaction replay,
multi-instrument replay, multi-account replay, market value, unrealized PnL,
base position, sniper pool, multi-currency replay, or production DataService
actions.

Current boundaries remain:

- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No snapshot writes.
- No TradeLog writes.

## TASK-073 Single Buy Replay Skeleton Status

TASK-073 adds a production-side read-only single BUY replay skeleton to
`libs/AccountingEngine`.

Scope:

- One `INITIAL_CASH` cash fact.
- One `BUY` trade fact.
- Single account and portfolio.
- Single instrument.
- CNY only.
- Cost amount from `amount + fee`.
- Cash balance from `initialCash - amount - fee`.

The implementation does not calculate SELL, realized PnL, unrealized PnL,
market value, base position, sniper pool, multi-account, multi-instrument, or
multi-currency replay.

Current boundaries remain:

- No DataAccess dependency.
- No DataService action.
- No SQLite access.
- No snapshot writes.
- No TradeLog writes.
