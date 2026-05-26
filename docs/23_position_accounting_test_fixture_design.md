# Position Accounting Test Fixture Design

## 1. Document Purpose

This document defines the future fixture design for position accounting replay
tests. It does not implement code, does not create databases, does not write
fixtures into SQLite, and does not add DataService actions.

## 2. Fixture Principles

- deterministic
- small
- readable
- every fixture has expected output
- every error fixture has expected issue
- no hidden external market dependency
- no automatic trading
- no broker API
- no hidden correction logic

Each fixture should define input facts, expected position output, expected cash
output, expected PnL output, expected issues, blocking status, and notes.

## 3. Fixture List

### FX001_EMPTY_LEDGER

- Purpose: Verify empty fact ranges produce empty read-only outputs.
- Input facts: no trade_log rows, no cash-flow facts.
- Expected position output: empty positions array.
- Expected cash output: zero or empty cash summary according to convention.
- Expected PnL output: empty or zero summary with SOURCE_RANGE_EMPTY.
- Expected issues: SOURCE_RANGE_EMPTY as non-blocking if empty ledger is valid.
- Blocking or non-blocking: non-blocking.
- Notes: Must not invent positions or cash facts.

### FX002_SINGLE_BUY

- Purpose: Verify one buy fact creates one position and cash outflow.
- Input facts: one buy trade with fee and currency.
- Expected position output: one position with positive quantity.
- Expected cash output: buyCashOutflowText includes trade amount and fee.
- Expected PnL output: unrealized fields depend on supplied market price.
- Expected issues: none when fee, cost basis, and price are present.
- Blocking or non-blocking: non-blocking.
- Notes: Cost convention must be explicit before implementation.

### FX003_BUY_SELL_PARTIAL

- Purpose: Verify partial sell keeps remaining position and realized PnL bucket.
- Input facts: one buy, one smaller sell, fees present.
- Expected position output: remaining quantity.
- Expected cash output: sellCashInflowText includes sell amount minus fee.
- Expected PnL output: realized PnL populated only when convention exists.
- Expected issues: COST_BASIS_UNDEFINED if convention is missing.
- Blocking or non-blocking: blocking when cost basis is undefined.
- Notes: Sell must not delete the original buy fact.

### FX004_SELL_EXCEEDS_POSITION

- Purpose: Verify selling more than replayed holdings is blocked.
- Input facts: one buy and one sell larger than holdings.
- Expected position output: blocked or error result.
- Expected cash output: degraded or blocked result.
- Expected PnL output: blocked.
- Expected issues: SELL_EXCEEDS_POSITION and NEGATIVE_POSITION.
- Blocking or non-blocking: blocking.
- Notes: Must be visible to UI.

### FX005_MISSING_FEE

- Purpose: Verify missing fee is reported.
- Input facts: buy or sell with required fee missing.
- Expected position output: may be incomplete.
- Expected cash output: incomplete feeTotalText.
- Expected PnL output: incomplete.
- Expected issues: MISSING_FEE.
- Blocking or non-blocking: depends on future accounting convention.
- Notes: The fixture must not silently assume zero fee unless convention allows.

### FX006_NEGATIVE_CASH

- Purpose: Verify cash replay detects negative cash.
- Input facts: buy outflow greater than available cash under the selected
  starting cash convention.
- Expected position output: position may exist but marked degraded.
- Expected cash output: NEGATIVE_CASH issue.
- Expected PnL output: degraded.
- Expected issues: NEGATIVE_CASH.
- Blocking or non-blocking: blocking.
- Notes: UI must not hide this error.

### FX007_MULTI_INSTRUMENT

- Purpose: Verify replay groups positions by instrument.
- Input facts: buys for CASH and one ETF or two instruments.
- Expected position output: separate rows by instrumentCode.
- Expected cash output: aggregate cash summary.
- Expected PnL output: aggregate summary by portfolio.
- Expected issues: none if conventions are complete.
- Blocking or non-blocking: non-blocking.
- Notes: Instrument identity fields must remain stable.

### FX008_MULTI_ACCOUNT

- Purpose: Verify account and portfolio dimensions do not mix.
- Input facts: two accounts or portfolios with separate trades.
- Expected position output: account/portfolio-scoped rows only.
- Expected cash output: account/portfolio-scoped cash only.
- Expected PnL output: portfolio scoped.
- Expected issues: none if dimensions are valid.
- Blocking or non-blocking: non-blocking.
- Notes: Cross-account aggregation must be explicit.

### FX009_BASE_POSITION_LOCKED

- Purpose: Verify 20% base position lock is represented as derived state.
- Input facts: buys sufficient to satisfy base target.
- Expected position output: basePositionFlag or BasePositionDto state.
- Expected cash output: no special write.
- Expected PnL output: unchanged by UI.
- Expected issues: none when policy is defined.
- Blocking or non-blocking: non-blocking.
- Notes: UI must not compute base lock status.

### FX010_SNIPER_TIER_COMPLETED

- Purpose: Verify a future sniper tier can be marked complete from buy facts.
- Input facts: buy facts matching one T1..T6 tier target.
- Expected position output: normal position rows.
- Expected cash output: pool used / remaining draft values.
- Expected PnL output: unchanged by tier state.
- Expected issues: none when policy is defined.
- Blocking or non-blocking: non-blocking.
- Notes: Tier completion comes from buy fact aggregation, not current market value.

### FX011_STALE_SNAPSHOT

- Purpose: Verify stale snapshot is visible and does not become fact source.
- Input facts: snapshot metadata older than accepted source range.
- Expected position output: stale or replay-required status.
- Expected cash output: stale or replay-required status.
- Expected PnL output: stale or replay-required status.
- Expected issues: SNAPSHOT_STALE.
- Blocking or non-blocking: blocking for fresh calculations.
- Notes: Snapshot must not rewrite trade_log.

### FX012_MISSING_MARKET_PRICE

- Purpose: Verify missing price blocks valuation without blocking quantity.
- Input facts: valid buy facts but no market price.
- Expected position output: quantity present, marketValueText empty.
- Expected cash output: normal cash replay.
- Expected PnL output: valuation degraded.
- Expected issues: MARKET_PRICE_MISSING.
- Blocking or non-blocking: non-blocking for quantity, blocking for valuation.
- Notes: Cost fields must not be faked from market value.

### FX013_MULTI_CURRENCY_UNSUPPORTED

- Purpose: Verify unsupported currency aggregation is explicit.
- Input facts: positions or cash flows in multiple currencies without FX rates.
- Expected position output: per-currency rows may be displayable.
- Expected cash output: aggregate blocked.
- Expected PnL output: aggregate blocked.
- Expected issues: MULTI_CURRENCY_UNSUPPORTED or FX_RATE_MISSING.
- Blocking or non-blocking: blocking for aggregate portfolio result.
- Notes: No hidden FX fallback is allowed.

## 4. Prohibited Fixture Behavior

- fixture must not write a real database.
- fixture must not depend on external market data.
- fixture must not perform automatic trading.
- fixture must not hide correction logic.
- fixture must not create TradeDraft.
- fixture must not call write actions.
- fixture must not mutate trade_log, snapshots, or audit_log.

## 5. Detailed Samples

This document is the fixture overview. Detailed implementation-before samples
for FX001-FX013 are defined in [Accounting Replay Fixture Samples](24_accounting_replay_fixture_samples.md).
Future replay implementation must use `docs/24_accounting_replay_fixture_samples.md`
as the primary sample source for input facts, expected DTO outputs, expected
issues, and blocking expectations.
