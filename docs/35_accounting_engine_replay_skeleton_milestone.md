# AccountingEngine Production-side Replay Skeleton Milestone

## Milestone Name

AccountingEngine Production-side Replay Skeleton Milestone

## Milestone Position

This milestone completes a first production-side `AccountingEngine` DTO-only
read-only replay skeleton across a controlled set of basic scenarios.

This is not complete production accounting replay.
This is not a DataService action.
This is not SQLite integration.
This is not a snapshot writer.
This is not a TradeLog writer.
This is not QML page functionality.

The current skeleton accepts DTO inputs and returns deterministic DTO outputs or
controlled issues. It does not query persistent facts, does not write any
business table, and does not move test-only replay implementation into
production.

## Covered Scenarios

Coverage keywords: empty ledger, single BUY, BUY + SELL partial sell,
SELL_EXCEEDS_POSITION, MISSING_FEE, NEGATIVE_CASH, Multi-instrument BUY,
Multi-account BUY, Multi-currency unsupported, Missing market price.

### Empty ledger: OK

The empty ledger scenario is the zero-fact baseline.

Input constraints: no trade facts, no cash facts, no market price facts, and no
FX rate facts.

Output result: `OK`, empty positions, zero CNY cash summary, and zero CNY
portfolio PnL shell.

Not done: no SQLite read, no DataService action, no snapshot write.

### Single BUY: OK

Input constraints: one `INITIAL_CASH` fact, one CNY `BUY` fact, one account,
one portfolio, one instrument, valid amount, quantity, and fee.

Output result: `OK`, one position with quantity / cost, cash balance after
amount plus fee, and unavailable valuation fields.

Not done: no market value, no unrealized PnL, no real market price query.

### BUY + SELL partial sell: OK

Input constraints: one CNY `BUY`, one CNY `SELL`, one `INITIAL_CASH`, same
account, same portfolio, same instrument, and `SELL` quantity below `BUY`
quantity.

Output result: `OK`, remaining position quantity / cost, cash balance, and
realized PnL for the skeleton scenario.

Not done: no full multi-transaction replay, no market value, no unrealized PnL.

### SELL_EXCEEDS_POSITION: ERROR

Input constraints: one `BUY`, one `SELL`, same account / portfolio /
instrument, and `SELL` quantity greater than `BUY` quantity.

Output result: `ERROR` with blocking `SELL_EXCEEDS_POSITION`.

Not done: no negative position output, no successful cash summary, no
portfolio PnL success output.

### MISSING_FEE: WARNING

Input constraints: one `BUY` fact whose `feeText` is missing, empty, or an
explicit unavailable value.

Output result: `WARNING` with non-blocking `MISSING_FEE`.

Not done: missing fee is not defaulted to zero and does not continue to a
normal cost / position / cash / PnL output.

### NEGATIVE_CASH: ERROR

Input constraints: one `INITIAL_CASH`, one CNY `BUY`, valid fee, and
`amount + fee > initialCash`.

Output result: `ERROR` with blocking `NEGATIVE_CASH`.

Not done: no implicit overdraft, no successful position / cash / PnL output.

### Multi-instrument BUY: OK

Input constraints: one account, one portfolio, one CNY cash fact, two or more
CNY `BUY` facts, and at least two different instrument codes.

Output result: `OK`, positions grouped by instrument code, and cash reduced by
the sum of all BUY amount plus fee values.

Not done: no multi-instrument SELL, no real market value, no unrealized PnL.

### Multi-account BUY: OK

Input constraints: two or more accounts, same portfolio, same instrument,
separate CNY `INITIAL_CASH` facts, and CNY `BUY` facts with valid fee.

Output result: `OK`, positions grouped by `accountId + portfolioId +
instrumentCode`, and account cash summaries kept separate by account.

Not done: no multi-account SELL, no cross-portfolio aggregation, no merged
account cash.

### Multi-currency unsupported: ERROR

Input constraints: CNY plus non-CNY facts, for example USD, without an FX rate
policy.

Output result: `ERROR` with blocking `MULTI_CURRENCY_UNSUPPORTED` and
`FX_RATE_MISSING`.

Not done: no FX conversion, no fake exchange rate, no successful multi-currency
valuation.

### Missing market price: WARNING

Input constraints: a CNY `BUY` that can form a position, requested valuation /
`pnl` output, and missing or non-matching market price facts.

Output result: `WARNING` with non-blocking `MARKET_PRICE_MISSING`; quantity,
cost, and cash may be returned while valuation fields stay `UNAVAILABLE`.

Not done: no market feed, no market value calculation, no unrealized PnL
calculation.

## Currently Not Covered

- Multi-step complex transaction replay is not covered.
- Multi-instrument SELL is not covered.
- Multi-account SELL is not covered.
- Successful multi-currency valuation is not covered.
- Real `marketValue` is not covered.
- `unrealizedPnl` is not covered.
- Production `basePosition` replay is not covered.
- Production `sniperPool` replay is not covered.
- Snapshot stale production integration is not covered.
- DataService action integration is not covered.
- SQLite facts query integration is not covered.

## Current Capability Statement

- `AccountingEngineInfo.replayImplemented=false`.
- `AccountingEngineInfo.productionReady=false`.
- `AccountingEngineCapabilities.writeEnabled=false`.
- `snapshotWriteEnabled=false`.
- `tradeLogWriteEnabled=false`.
- `dataServiceActionImplemented=false`.
- `supportsMarketPrice=false`.
- `supportsFxRate=false`.
- `supportsMultiCurrency=false`.

These values are correct. Individual skeleton scenarios being available does
not mean complete production replay is implemented. `productionReady=false`
remains the correct state because there is no DataService integration, no
SQLite facts query boundary, no production write policy, and no complete replay
coverage.

## Current Forbidden Boundaries

- no SQLite
- no DataAccess dependency
- no DataService action
- no QML accounting calculation
- no snapshot writes
- no TradeLog writes
- no TradeDraft generation
- no broker order
- no auto trading
- no real market feed
- no FX rate service
- no test-only implementation migration

## Current Test Coverage

- `accounting_engine_boundary`
- `accounting_replay_dto_parser_boundary`
- `accounting_replay_engine_empty_ledger`
- `accounting_replay_engine_single_buy`
- `accounting_replay_engine_buy_sell_partial`
- `accounting_replay_engine_sell_exceeds_position`
- `accounting_replay_engine_missing_fee`
- `accounting_replay_engine_negative_cash`
- `accounting_replay_engine_multi_instrument_buy`
- `accounting_replay_engine_multi_account_buy`
- `accounting_replay_engine_multi_currency_unsupported`
- `accounting_replay_engine_missing_market_price`

## Validation Commands

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R transport_local_socket_echo --repeat until-fail:50 --output-on-failure
```

## Suggested Tag

Suggested tag after the milestone PR is merged:

```powershell
git tag -a v0.4.0-accounting-engine-replay-skeleton -m "v0.4 AccountingEngine Replay Skeleton"
git push origin v0.4.0-accounting-engine-replay-skeleton
```

The tag must only be created manually by the user after the PR is merged. This
task does not create a Git tag.

## Next Phase Recommendations

- DataService read-only action contract review.
- SQLite read-only facts query boundary.
- Production replay integration test plan.
- No-write table count test plan.
- Decide whether production `basePosition` / `sniperPool` scenarios continue
  as DTO-only skeleton work.
- Market price and FX rate support require separate explicit authorization.
