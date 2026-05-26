# Accounting Replay Fixture Samples

## 1. Document Purpose

This document details accounting replay test fixture samples for future
implementation. It only defines examples and does not implement code.

Future replay implementation must first use these fixtures to build automated
tests. Every fixture should have input facts, expected output, and expected
issues before production replay logic is implemented.

These fixtures are not actual database data, do not write a real database, do
not call DataService actions, do not drive trading, and do not generate
TradeDraft. trade_log 是事实账本 in these samples: trade facts are input
examples for future replay assertions only. Expected output is the assertion
target for future replay, not a fact source.

## 2. Common Conventions

- default accountId: `ACC-DEMO-001`
- default portfolioId: `PF-DEMO-001`
- default currency: `CNY`
- default instruments: `CASH`, `159509`, `518880`
- default source range: `2026-01-01T00:00:00+08:00` to `2026-01-31T23:59:59+08:00`
- amount fields use `xxxText` and future raw `xxxCents` drafts.
- quantity fields use `quantityText` and future raw `quantity1e6` drafts.
- fee fields use `feeText` and future raw `feeCents` drafts.
- market price is treated as missing when not specified.
- all expected DTOs are read-only results and are not fact sources.
- QML 不计算账务.
- no hidden external market dependency is allowed.
- 不写数据库.

Proposed standard for these samples: buy cost includes fee. Sell proceeds are
reduced by fee. This is a proposed standard for fixture consistency; future
implementation must either adopt it or update the contract and all fixtures in
one reviewed task.

## 3. Input Fact Format

These formats are fixture input drafts only. They are not schema changes. The
real schema remains defined by `migrations/001_initial_schema.sql`.

### TradeFact

- factId
- tradeTime
- accountId
- portfolioId
- instrumentCode
- action
- quantityText
- priceText
- amountText
- feeText
- cashFlowText
- currency
- source
- note

### CashFact

- factId
- time
- accountId
- portfolioId
- action
- amountText
- currency
- note

### MarketPriceFact

- instrumentCode
- priceTime
- priceText
- currency
- source

## 4. Expected Output Format

Every fixture should provide:

- Expected PositionSummaryDto
- Expected CashSummaryDto
- Expected PortfolioPnlDto
- Expected issues
- Blocking or non-blocking
- Notes

Expected output is the future replay assertion target. It must not be treated
as source data and must not write `cash_snapshot`, `position_snapshot`, or
`portfolio_summary`.

## 5. Fixture Overview

| fixtureId | name | purpose | expectedStatus | blockingIssue | mainErrorCode |
| --- | --- | --- | --- | --- | --- |
| FX001_EMPTY_LEDGER | Empty ledger | no facts | OK or INCOMPLETE | no | SOURCE_RANGE_EMPTY |
| FX002_SINGLE_BUY | Single buy | one buy with fee | OK | no | none |
| FX003_BUY_SELL_PARTIAL | Partial sell | realized / remaining split | OK | no | none |
| FX004_SELL_EXCEEDS_POSITION | Sell exceeds position | prevent negative holdings | ERROR | yes | SELL_EXCEEDS_POSITION |
| FX005_MISSING_FEE | Missing fee | fee validation | WARNING or ERROR | policy-dependent | MISSING_FEE |
| FX006_NEGATIVE_CASH | Negative cash | cash boundary | ERROR | yes | NEGATIVE_CASH |
| FX007_MULTI_INSTRUMENT | Multi instrument | instrument grouping | WARNING if price missing | no for quantity | MARKET_PRICE_MISSING |
| FX008_MULTI_ACCOUNT | Multi account | dimension isolation | OK | no | none |
| FX009_BASE_POSITION_LOCKED | Base locked | 20% base boundary | OK | no | none |
| FX010_SNIPER_TIER_COMPLETED | Sniper tier completed | tier fact aggregation | OK | no | none |
| FX011_STALE_SNAPSHOT | Stale snapshot | stale derived cache | STALE | yes for fresh calc | SNAPSHOT_STALE |
| FX012_MISSING_MARKET_PRICE | Missing market price | valuation degradation | WARNING | no for quantity | MARKET_PRICE_MISSING |
| FX013_MULTI_CURRENCY_UNSUPPORTED | Multi-currency unsupported | FX boundary | ERROR | yes | MULTI_CURRENCY_UNSUPPORTED |

## FX001_EMPTY_LEDGER

### Purpose

Empty ledger. No trade facts and no cash facts. Verifies empty positions are not
an accounting error.

### Input facts

```text
TradeFact: []
CashFact: []
```

### Market price facts

```text
MarketPriceFact: []
```

### Expected PositionSummaryDto

```text
positions: []
dataQualityStatus: OK
metadata.sourceTradeCount: 0
```

### Expected CashSummaryDto

```text
cashBalanceText: "0.00"
availableCashText: "0.00"
currency: "CNY"
dataQualityStatus: OK or INCOMPLETE
```

The accepted status must be chosen by implementation. `OK` means an empty
ledger is valid. `INCOMPLETE` means an initial cash fact is required.

### Expected PortfolioPnlDto

```text
totalAssetsText: "0.00" or ""
principalText: ""
dataQualityStatus: OK or INCOMPLETE
```

### Expected issues

```text
SOURCE_RANGE_EMPTY, blocking=false, userVisible=true
```

### Blocking?

Non-blocking.

### Notes

Do not create fake cash, fake positions, or snapshot rows.

## FX002_SINGLE_BUY

### Purpose

Single buy with initial cash. This fixture fixes the proposed standard that buy
cost includes fee.

### Input facts

```text
CashFact CF-001:
  time: 2026-01-01T09:00:00+08:00
  accountId: ACC-DEMO-001
  portfolioId: PF-DEMO-001
  action: INITIAL_CASH
  amountText: "100000.00"
  currency: CNY

TradeFact TF-001:
  tradeTime: 2026-01-02T10:00:00+08:00
  instrumentCode: 159509
  action: BUY
  quantityText: "1000"
  quantity1e6: 1000000000
  priceText: "1.000"
  amountText: "1000.00"
  feeText: "1.00"
  feeCents: 100
  cashFlowText: "-1001.00"
  currency: CNY
```

### Market price facts

```text
MarketPriceFact MP-001:
  instrumentCode: 159509
  priceTime: 2026-01-31T15:00:00+08:00
  priceText: "1.000"
  currency: CNY
```

### Expected PositionSummaryDto

```text
instrumentCode: 159509
quantityText: "1000"
costAmountText: "1001.00"
costPriceText: "1.001"
marketPriceText: "1.000"
marketValueText: "1000.00"
unrealizedPnlText: "-1.00"
dataQualityStatus: OK
```

### Expected CashSummaryDto

```text
cashBalanceText: "98999.00"
availableCashText: "98999.00"
buyCashOutflowText: "1001.00"
feeTotalText: "1.00"
dataQualityStatus: OK
```

### Expected PortfolioPnlDto

```text
cashText: "98999.00"
marketValueText: "1000.00"
totalAssetsText: "99999.00"
unrealizedPnlText: "-1.00"
realizedPnlText: "0.00"
dataQualityStatus: OK
```

### Expected issues

```text
[]
```

### Blocking?

Non-blocking.

### Notes

This is the baseline buy fixture for cost-with-fee behavior.

## FX003_BUY_SELL_PARTIAL

### Purpose

Partial sell after a buy. Verifies remaining quantity, cash balance, and
realized / unrealized separation.

### Input facts

```text
CashFact CF-001: INITIAL_CASH 100000.00 CNY

TradeFact TF-001:
  action: BUY
  instrumentCode: 159509
  quantityText: "1000"
  priceText: "1.000"
  amountText: "1000.00"
  feeText: "1.00"
  cashFlowText: "-1001.00"

TradeFact TF-002:
  action: SELL
  instrumentCode: 159509
  quantityText: "400"
  priceText: "1.200"
  amountText: "480.00"
  feeText: "1.00"
  cashFlowText: "479.00"
```

### Market price facts

```text
MarketPriceFact MP-001: 159509 @ 1.100 CNY on 2026-01-31
```

### Expected PositionSummaryDto

```text
instrumentCode: 159509
quantityText: "600"
costAmountText: "600.60"
costPriceText: "1.001"
marketPriceText: "1.100"
marketValueText: "660.00"
realizedPnlText: "78.60"
unrealizedPnlText: "59.40"
dataQualityStatus: OK
```

Realized PnL draft: sell proceeds 479.00 minus sold cost 400.40.

### Expected CashSummaryDto

```text
cashBalanceText: "99478.00"
buyCashOutflowText: "1001.00"
sellCashInflowText: "479.00"
feeTotalText: "2.00"
dataQualityStatus: OK
```

### Expected PortfolioPnlDto

```text
cashText: "99478.00"
marketValueText: "660.00"
totalAssetsText: "100138.00"
realizedPnlText: "78.60"
unrealizedPnlText: "59.40"
totalPnlText: "138.00"
dataQualityStatus: OK
```

### Expected issues

```text
[]
```

### Blocking?

Non-blocking.

### Notes

The fixture assumes average cost with fee included. If the project later chooses
another cost convention, this fixture must be updated with the contract.

## FX004_SELL_EXCEEDS_POSITION

### Purpose

Selling more than replayed holdings must not silently produce negative
holdings.

### Input facts

```text
CashFact CF-001: INITIAL_CASH 100000.00 CNY
TradeFact TF-001: BUY 159509 quantity 100 @ 1.000, fee 1.00
TradeFact TF-002: SELL 159509 quantity 200 @ 1.100, fee 1.00
```

### Market price facts

```text
MarketPriceFact MP-001: 159509 @ 1.100 CNY
```

### Expected PositionSummaryDto

```text
instrumentCode: 159509
quantityText: ""
dataQualityStatus: ERROR
```

### Expected CashSummaryDto

```text
cashBalanceText: ""
dataQualityStatus: ERROR
```

### Expected PortfolioPnlDto

```text
totalAssetsText: ""
dataQualityStatus: ERROR
```

### Expected issues

```text
SELL_EXCEEDS_POSITION, blocking=true, sourceId=TF-002
NEGATIVE_POSITION, blocking=true, instrumentCode=159509
```

### Blocking?

Blocking.

### Notes

Future replay must stop or degrade the result instead of presenting negative
position as normal.

## FX005_MISSING_FEE

### Purpose

Missing fee must be visible. This fixture defines both accepted interpretations
until fee strictness is finalized.

### Input facts

```text
CashFact CF-001: INITIAL_CASH 100000.00 CNY
TradeFact TF-001: BUY 159509 quantity 1000 @ 1.000, fee missing
```

### Market price facts

```text
MarketPriceFact MP-001: 159509 @ 1.000 CNY
```

### Expected PositionSummaryDto

```text
quantityText: "1000"
costAmountText: "1000.00" or ""
dataQualityStatus: WARNING or ERROR
```

### Expected CashSummaryDto

```text
cashBalanceText: "99000.00" or ""
feeTotalText: ""
dataQualityStatus: WARNING or ERROR
```

### Expected PortfolioPnlDto

```text
dataQualityStatus: WARNING or ERROR
```

### Expected issues

```text
MISSING_FEE, blocking=false if zero-fee fallback is allowed
MISSING_FEE, blocking=true if fee is mandatory
```

### Blocking?

Policy-dependent. The fixture must be resolved before implementation.

### Notes

The implementation must not silently assume zero fee without the accounting
contract explicitly allowing it.

## FX006_NEGATIVE_CASH

### Purpose

Cash replay must detect negative cash.

### Input facts

```text
CashFact CF-001: INITIAL_CASH 100.00 CNY
TradeFact TF-001: BUY 159509 quantity 1000 @ 1.000, fee 1.00, cashFlow -1001.00
```

### Market price facts

```text
MarketPriceFact MP-001: 159509 @ 1.000 CNY
```

### Expected PositionSummaryDto

```text
quantityText: "1000"
dataQualityStatus: ERROR
```

### Expected CashSummaryDto

```text
cashBalanceText: "-901.00"
dataQualityStatus: ERROR
```

### Expected PortfolioPnlDto

```text
dataQualityStatus: ERROR
```

### Expected issues

```text
NEGATIVE_CASH, blocking=true, sourceId=TF-001
```

### Blocking?

Blocking.

### Notes

Whether margin is later supported must be a separate accounting mode. Default
mode treats this as an error.

## FX007_MULTI_INSTRUMENT

### Purpose

Replay groups positions by instrument and does not confuse `159509` with
`518880`.

### Input facts

```text
CashFact CF-001: INITIAL_CASH 100000.00 CNY
TradeFact TF-001: BUY 159509 quantity 1000 @ 1.000, fee 1.00
TradeFact TF-002: BUY 518880 quantity 500 @ 4.000, fee 2.00
```

### Market price facts

```text
MarketPriceFact: []
```

### Expected PositionSummaryDto

```text
positions:
  - instrumentCode: 159509, quantityText: "1000", costAmountText: "1001.00"
  - instrumentCode: 518880, quantityText: "500", costAmountText: "2002.00"
dataQualityStatus: WARNING
```

### Expected CashSummaryDto

```text
cashBalanceText: "96997.00"
buyCashOutflowText: "3003.00"
feeTotalText: "3.00"
dataQualityStatus: OK
```

### Expected PortfolioPnlDto

```text
marketValueText: ""
totalAssetsText: ""
dataQualityStatus: WARNING
```

### Expected issues

```text
MARKET_PRICE_MISSING, blocking=false for quantity, blocking=true for valuation
```

### Blocking?

Non-blocking for position quantity; blocking for valuation totals.

### Notes

This fixture has no hidden external market dependency.

## FX008_MULTI_ACCOUNT

### Purpose

Facts from different accounts must not be merged accidentally.

### Input facts

```text
CashFact CF-001: ACC-DEMO-001 / PF-DEMO-001 INITIAL_CASH 100000.00 CNY
CashFact CF-002: ACC-DEMO-002 / PF-DEMO-001 INITIAL_CASH 50000.00 CNY
TradeFact TF-001: ACC-DEMO-001 BUY 159509 quantity 1000 @ 1.000, fee 1.00
TradeFact TF-002: ACC-DEMO-002 BUY 159509 quantity 200 @ 1.000, fee 1.00
```

### Market price facts

```text
MarketPriceFact MP-001: 159509 @ 1.000 CNY
```

### Expected PositionSummaryDto

```text
ACC-DEMO-001 position: 159509 quantityText "1000"
ACC-DEMO-002 position: 159509 quantityText "200"
dataQualityStatus: OK
```

### Expected CashSummaryDto

```text
ACC-DEMO-001 cashBalanceText: "98999.00"
ACC-DEMO-002 cashBalanceText: "49799.00"
```

### Expected PortfolioPnlDto

```text
portfolio-scoped output must state whether it aggregates both accounts or is account-filtered
dataQualityStatus: OK
```

### Expected issues

```text
[]
```

### Blocking?

Non-blocking.

### Notes

Account and portfolio dimensions must be explicit in requests and responses.

## FX009_BASE_POSITION_LOCKED

### Purpose

Verify the 20% locked base-position boundary. UI must display the derived
result and must not compute it.

### Input facts

```text
CashFact CF-001: INITIAL_CASH 100000.00 CNY
TradeFact TF-001: BUY 159509 quantity 20000 @ 1.000, fee 10.00
Base policy draft: targetBaseRatioText "20%"
Portfolio total asset draft: 100000.00 CNY
```

### Market price facts

```text
MarketPriceFact MP-001: 159509 @ 1.000 CNY
```

### Expected PositionSummaryDto

```text
instrumentCode: 159509
quantityText: "20000"
basePositionFlag: true
dataQualityStatus: OK
```

### Expected CashSummaryDto

```text
cashBalanceText: "79990.00"
dataQualityStatus: OK
```

### Expected PortfolioPnlDto

```text
totalAssetsText: "99990.00"
dataQualityStatus: OK
```

### Expected issues

```text
[]
```

### Blocking?

Non-blocking.

### Notes

Expected BasePositionDto:

```text
targetBaseRatioText: "20%"
currentBaseRatioText: "20%"
lockedBaseAmountText: "20000.00"
sellableAboveBaseAmountText: "0.00"
```

This fixture does not implement sell logic. It only asserts the derived
boundary: sellable amount excludes locked base.

## FX010_SNIPER_TIER_COMPLETED

### Purpose

Verify sniper tier completion comes from buy fact aggregation, not from current
market value.

### Input facts

```text
CashFact CF-001: INITIAL_CASH 100000.00 CNY
TradeFact TF-BASE: BUY 159509 base position amount 20000.00, fee 10.00
TradeFact TF-T1: BUY 159509 sniper T1 amount 1000.00, fee 1.00
Sniper policy draft:
  poolAmountText: "63000.00"
  tiers: T1..T6 weights 1/2/4/8/16/32
  T1 targetAmountText: "1000.00"
```

### Market price facts

```text
MarketPriceFact MP-001: 159509 @ 1.050 CNY
```

### Expected PositionSummaryDto

```text
instrumentCode: 159509
dataQualityStatus: OK
```

### Expected CashSummaryDto

```text
cashBalanceText: "78989.00"
dataQualityStatus: OK
```

### Expected PortfolioPnlDto

```text
dataQualityStatus: OK
```

### Expected issues

```text
[]
```

### Blocking?

Non-blocking.

### Notes

Expected SniperPoolDto tier summary:

```text
tierName: T1
weight: 1
targetAmountText: "1000.00"
executedAmountText: "1000.00"
remainingAmountText: "0.00"
completed: true
```

The 80% sniper pool must not resize from floating PnL.

## FX011_STALE_SNAPSHOT

### Purpose

Stale snapshots must be visible and must not become fact sources.

### Input facts

```text
Snapshot draft:
  snapshotId: SNAP-OLD-001
  generatedAt: 2026-01-10T15:00:00+08:00
  sourceToTime: 2026-01-10T15:00:00+08:00
TradeFact TF-NEW:
  tradeTime: 2026-01-20T10:00:00+08:00
  action: BUY
  instrumentCode: 159509
```

### Market price facts

```text
MarketPriceFact MP-001: 159509 @ 1.000 CNY on 2026-01-31
```

### Expected PositionSummaryDto

```text
dataQualityStatus: STALE
metadata.stale: true
metadata.staleReason: "snapshot older than source trade range"
```

### Expected CashSummaryDto

```text
dataQualityStatus: STALE
metadata.stale: true
```

### Expected PortfolioPnlDto

```text
dataQualityStatus: STALE
metadata.stale: true
```

### Expected issues

```text
SNAPSHOT_STALE, blocking=true for fresh calculation, userVisible=true
```

### Blocking?

Blocking for fresh calculation.

### Notes

Do not use the stale snapshot to overwrite or correct trade facts.

## FX012_MISSING_MARKET_PRICE

### Purpose

Missing market price should not hide quantity or cost, but it blocks valuation
and unrealized PnL.

### Input facts

```text
CashFact CF-001: INITIAL_CASH 100000.00 CNY
TradeFact TF-001: BUY 159509 quantity 1000 @ 1.000, fee 1.00
```

### Market price facts

```text
MarketPriceFact: []
```

### Expected PositionSummaryDto

```text
quantityText: "1000"
costAmountText: "1001.00"
marketPriceText: ""
marketValueText: ""
unrealizedPnlText: ""
dataQualityStatus: WARNING
```

### Expected CashSummaryDto

```text
cashBalanceText: "98999.00"
dataQualityStatus: OK
```

### Expected PortfolioPnlDto

```text
marketValueText: ""
unrealizedPnlText: ""
totalAssetsText: ""
dataQualityStatus: WARNING
```

### Expected issues

```text
MARKET_PRICE_MISSING, blocking=false for quantity and cost, blocking=true for valuation
```

### Blocking?

Non-blocking for position quantity; blocking for valuation output.

### Notes

Do not infer market value from cost.

## FX013_MULTI_CURRENCY_UNSUPPORTED

### Purpose

Unsupported multi-currency aggregation must be explicit when FX rates are
missing.

### Input facts

```text
CashFact CF-CNY: INITIAL_CASH 100000.00 CNY
CashFact CF-USD: INITIAL_CASH 1000.00 USD
TradeFact TF-CNY: BUY 159509 quantity 1000 @ 1.000 CNY, fee 1.00 CNY
TradeFact TF-USD: BUY USDETF quantity 10 @ 50.000 USD, fee 1.00 USD
```

### Market price facts

```text
MarketPriceFact MP-CNY: 159509 @ 1.000 CNY
MarketPriceFact MP-USD: USDETF @ 50.000 USD
FX rate: missing
```

### Expected PositionSummaryDto

```text
CNY position row may display in CNY.
USD position row may display in USD.
portfolio aggregate dataQualityStatus: ERROR
```

### Expected CashSummaryDto

```text
CNY cashBalanceText: "98999.00"
USD cashBalanceText: "499.00"
aggregate cashBalanceText: ""
dataQualityStatus: ERROR
```

### Expected PortfolioPnlDto

```text
totalAssetsText: ""
totalReturnRatioText: ""
dataQualityStatus: ERROR
```

### Expected issues

```text
MULTI_CURRENCY_UNSUPPORTED, blocking=true
FX_RATE_MISSING, blocking=true
```

### Blocking?

Blocking for aggregate portfolio output.

### Notes

No hidden FX fallback is allowed. Future implementation must show unsupported
currency or missing FX status in UI-visible issues.

## 6. Implementation Guardrails

- fixture samples are documentation only.
- fixture samples must not write a real database.
- fixture samples must not depend on external market data.
- fixture samples must not perform automatic trading.
- fixture samples must not generate TradeDraft.
- fixture samples must not call write actions.
- future implementation must create tests from FX001-FX013 before replay logic
  is accepted.
- Future ViewModel tests should use these fixtures through
  [Position Shell ViewModel Design](25_position_shell_viewmodel_design.md) and
  [Position DTO ViewModel Mapping](26_position_dto_viewmodel_mapping.md).

## 7. TASK-049 Replay Preview Guard Reference

`accounting.replay.preview` now references FX001-FX013 as required fixtures,
but it remains a non-implementation guard.

- The guard must return `implemented=false` and `replayExecuted=false`.
- The guard must not return calculated position, cash, PnL, base-position, or
  sniper-pool results.
- Fixture-backed replay implementation must cover all FX001-FX013 samples
  before returning real preview output.
- The fixture samples still do not write a real database, do not rely on
  external market data, do not drive trading, and do not generate TradeDraft.

## 8. TASK-050 Static JSON Fixture Files

The FX001-FX013 samples are now mirrored as static JSON fixture files under
`tests/fixtures/accounting_replay/`.

- `tests/fixtures/accounting_replay/fixtures_index.json` lists the complete
  fixture set.
- `tests/AccountingFixtures/validate_accounting_replay_fixtures.py` validates
  required structure and forbidden tokens.
- `accounting_replay_fixture_static_validation` runs the validator through
  CTest.

Current validation is static only. It does not calculate cash, position, PnL,
base-position, or sniper-pool values; it does not access SQLite; it does not
call DataService; it does not depend on external market data; and it does not
implement accounting replay.

## 9. TASK-051 Static Fixture Loader Boundary

`tests/AccountingFixtures/AccountingFixtureLoader` provides a test-support-only
C++ loader for the static fixture files.

- It reads `tests/fixtures/accounting_replay/fixtures_index.json`.
- It loads FX001-FX013 JSON fixture files.
- It parses `tradeFacts`, `cashFacts`, `marketPriceFacts`, `expectedOutputs`,
  `expectedIssues`, `blocking`, and `notes` into in-memory structures.
- It validates fixture IDs, missing files, JSON shape, empty
  `externalDependencies`, required `expectedOutputs`, and required issue codes
  for error fixtures.
- It exposes read-only access through `fixtureIds()` and `fixtureById()`.

The loader is not a replay implementation. It does not calculate cash balances,
position quantities, cost basis, PnL, base-position status, or sniper-pool
tiers. It does not access SQLite, does not call DataService, does not use
external market data, and does not write files. Future replay implementation
must use this loader or a compatible fixture contract instead of bypassing the
fixture set.

## 10. TASK-052 Fixture-Backed Harness Skeleton

FX001-FX013 are now connected to
`tests/AccountingFixtures/AccountingReplayTestHarness`.

- The harness loads fixtures through `AccountingFixtureLoader`.
- The harness iterates every fixture and calls `AccountingReplayStubEngine`.
- The current stub result is `implemented=false`, `replayExecuted=false`, and
  `status=NOT_IMPLEMENTED`.
- `NOT_IMPLEMENTED` is the expected result for this stage and must not be
  treated as a failed replay calculation.
- The harness verifies that all FX001-FX013 fixtures are covered, including
  error fixtures.

The harness still does not implement replay. It does not calculate cash,
position, PnL, cost basis, base-position status, or sniper-pool tiers. It does
not access SQLite, does not call DataService, does not write output files, and
does not write database tables.

Future replay implementation must replace the stub only after fixture-backed
tests are explicitly defined. It must not skip fixtures, remove error fixtures,
or change expected fixture outputs without a separate contract justification.

## 11. TASK-053 AccountingReplayResult Skeleton

The fixture-backed harness now returns a test-only `AccountingReplayResult`
skeleton.

- `AccountingReplayResult` carries `fixtureId`, `implemented`,
  `replayExecuted`, `status`, `message`, `metadata`, `issues`, and raw output
  placeholders.
- `AccountingReplayResultMapper` maps valid fixtures to `NOT_IMPLEMENTED` and
  invalid fixtures to `INVALID_FIXTURE`.
- `AccountingReplayStubEngine` returns `implemented=false`,
  `replayExecuted=false`, `status=NOT_IMPLEMENTED`, and an issue with
  `code=REPLAY_NOT_IMPLEMENTED`.
- `positionListResponseRaw`, `cashSummaryRaw`, `portfolioPnlRaw`,
  `basePositionRaw`, and `sniperPoolRaw` remain empty.

This is still not a replay implementation. The skeleton does not calculate
cash, position, PnL, cost basis, base-position, or sniper-pool output. Future
replay implementation must fill result fields gradually while preserving
FX001-FX013 coverage and must not hide fixture failures by changing the result
contract or fixture expectations without separate justification.

## 12. TASK-054 Expected-Output Assertion Skeleton

FX001-FX013 are now connected to a test-only expected-output assertion skeleton.

- `AccountingExpectedOutputInspector` reads fixture `expectedOutputs` and checks
  that `positionSummaries`, `cashSummary`, and `portfolioPnl` are present.
- The inspector reads expected issue codes such as `SELL_EXCEEDS_POSITION`,
  `MISSING_FEE`, `NEGATIVE_CASH`, `SNAPSHOT_STALE`,
  `MARKET_PRICE_MISSING`, and `MULTI_CURRENCY_UNSUPPORTED`.
- `AccountingReplayAssertionSkeleton` currently asserts only the
  `NOT_IMPLEMENTED` guard: `implemented=false`, `replayExecuted=false`,
  `status=NOT_IMPLEMENTED`, `REPLAY_NOT_IMPLEMENTED` issue present, and all raw
  result outputs empty.
- Real position, cash, and PnL value assertions return `SKIPPED_BY_DESIGN`
  until fixture-backed replay is authorized and implemented.
- Future replay implementation must replace the skipped assertions with real
  checks instead of deleting the skeleton, skipping fixtures, clearing
  `expectedOutputs`, or placing fake outputs in a `NOT_IMPLEMENTED` result.

The assertion skeleton does not calculate cash, position, PnL, cost basis,
base-position, or sniper-pool output. It does not access SQLite, does not call
DataService, does not write output files, and does not write database tables.

## 13. TASK-055 Minimal FX001 Empty-Ledger Replay

`FX001_EMPTY_LEDGER` now has a test-only minimal replay implementation through
`AccountingReplayMinimalEngine`.

- `FX001_EMPTY_LEDGER` is the only fixture with `implemented=true`.
- `FX001_EMPTY_LEDGER` is the only fixture with `replayExecuted=true`.
- `FX001_EMPTY_LEDGER` returns `status=OK`.
- The result contains `positionListResponseRaw.positions=[]`.
- The result contains zero-style `cashSummaryRaw` and `portfolioPnlRaw` objects
  generated from the empty input scope.
- `FX002_SINGLE_BUY` through `FX013_MULTI_CURRENCY_UNSUPPORTED` still return
  `NOT_IMPLEMENTED`.

This implementation is intentionally limited to empty input facts. It does not
implement buy replay, sell replay, fee handling, cash-flow calculation, cost
basis, PnL, base-position, sniper-pool, market price valuation, SQLite access,
DataService calls, file writes, database writes, or DataService actions. Future
replay work must add one fixture at a time and must keep earlier fixture tests
passing without changing fixture expected outputs to fit an incorrect algorithm.

## 14. TASK-056 Minimal FX002 Single-Buy Replay

`FX002_SINGLE_BUY` now has a test-only minimal replay implementation through
`AccountingReplayMinimalEngine`.

- `FX001_EMPTY_LEDGER` and `FX002_SINGLE_BUY` are the only fixtures with
  `implemented=true`.
- `FX001_EMPTY_LEDGER` and `FX002_SINGLE_BUY` are the only fixtures with
  `replayExecuted=true`.
- `FX002_SINGLE_BUY` returns `status=OK`.
- `FX002_SINGLE_BUY` returns one `159509` position with `quantityText=1000`.
- `FX002_SINGLE_BUY` uses the fixture proposed standard that buy cost includes
  fee, so `costAmountText=1001.00 CNY`.
- `FX002_SINGLE_BUY` returns `cashBalanceText=98999.00 CNY`.
- `FX003_BUY_SELL_PARTIAL` through `FX013_MULTI_CURRENCY_UNSUPPORTED` still
  return `NOT_IMPLEMENTED`.

This implementation is intentionally limited to a single BUY fact and one
initial cash fact. It does not implement sell replay, partial sells, realized
PnL, unrealized PnL, market valuation, multi-instrument replay, multi-account
replay, missing-fee errors, negative-cash errors, oversell errors,
base-position calculation, sniper-pool calculation, SQLite access, DataService
calls, file writes, database writes, or DataService actions. Future replay work
must continue one fixture at a time and keep FX001/FX002 tests passing without
changing fixture expected outputs to fit an incorrect algorithm.

## 15. TASK-057 Minimal FX003 Partial-Sell Replay

`FX003_BUY_SELL_PARTIAL` now has a test-only minimal replay implementation
through `AccountingReplayMinimalEngine`.

- `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, and `FX003_BUY_SELL_PARTIAL` are the
  only fixtures with `implemented=true`.
- `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, and `FX003_BUY_SELL_PARTIAL` are the
  only fixtures with `replayExecuted=true`.
- `FX003_BUY_SELL_PARTIAL` returns `status=OK`.
- `FX003_BUY_SELL_PARTIAL` returns one remaining `159509` position with
  `quantityText=600`.
- `FX003_BUY_SELL_PARTIAL` returns `costAmountText=600.60 CNY`.
- `FX003_BUY_SELL_PARTIAL` returns `cashBalanceText=99478.00 CNY`.
- `FX003_BUY_SELL_PARTIAL` returns `realizedPnlText=78.60 CNY`.
- `FX004_SELL_EXCEEDS_POSITION` through `FX013_MULTI_CURRENCY_UNSUPPORTED` still
  return `NOT_IMPLEMENTED`.

The realized PnL口径 for this fixture is:

- buyGrossAmount = `1000.00 CNY`
- buyFee = `1.00 CNY`
- totalBuyCost = `1001.00 CNY`
- buyQuantity = `1000`
- averageCostPerUnit = `1.001 CNY`
- sellQuantity = `400`
- allocatedSoldCost = `400.40 CNY`
- sellGrossAmount = `480.00 CNY`
- sellFee = `1.00 CNY`
- netSellInflow = `479.00 CNY`
- `realizedPnl = netSellInflow - allocatedSoldCost = 78.60 CNY`
- remainingQuantity = `600`
- remainingCostAmount = `600.60 CNY`
- cashBalance = `99478.00 CNY`

This implementation is intentionally limited to one initial cash fact, one BUY
fact, and one later SELL fact for the same instrument. It does not implement
FX004 oversell handling, missing-fee validation, negative-cash errors,
multi-account replay, multi-instrument replay, market valuation, unrealized PnL,
base-position, sniper-pool, SQLite access, DataService calls, file writes,
database writes, or DataService actions. Future implementation must add one
fixture at a time without changing fixture expected outputs to fit an incorrect
algorithm.

## 16. TASK-058 Minimal FX004 Sell-Exceeds-Position Detection

`FX004_SELL_EXCEEDS_POSITION` now has a test-only minimal replay error
detection path through `AccountingReplayMinimalEngine`.

- `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, and `FX003_BUY_SELL_PARTIAL` still
  return `status=OK`.
- `FX004_SELL_EXCEEDS_POSITION` returns `implemented=true`.
- `FX004_SELL_EXCEEDS_POSITION` returns `replayExecuted=true`.
- `FX004_SELL_EXCEEDS_POSITION` returns `status=ERROR`.
- `FX004_SELL_EXCEEDS_POSITION` returns a blocking `SELL_EXCEEDS_POSITION`
  issue when SELL quantity exceeds available BUY quantity.
- `FX004_SELL_EXCEEDS_POSITION` does not generate negative holdings.
- `FX004_SELL_EXCEEDS_POSITION` does not generate normal position, cash, PnL,
  base-position, or sniper-pool success outputs.
- `FX005_MISSING_FEE` through `FX013_MULTI_CURRENCY_UNSUPPORTED` still return
  `NOT_IMPLEMENTED`.

This implementation is intentionally limited to detecting one BUY fact followed
by one SELL fact for the same instrument where SELL quantity is greater than BUY
quantity. It does not implement missing-fee validation, negative-cash errors,
multi-account replay, multi-instrument replay, market valuation, realized PnL,
unrealized PnL, base-position, sniper-pool, SQLite access, DataService calls,
file writes, database writes, or DataService actions. Future implementation
must add one fixture at a time without changing fixture expected outputs to fit
an incorrect algorithm.

## 17. TASK-059 Minimal FX005 Missing-Fee Detection

`FX005_MISSING_FEE` now has a test-only minimal replay warning path through
`AccountingReplayMinimalEngine`.

- `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, and `FX003_BUY_SELL_PARTIAL` still
  return `status=OK`.
- `FX004_SELL_EXCEEDS_POSITION` still returns `status=ERROR`.
- `FX005_MISSING_FEE` returns `implemented=true`.
- `FX005_MISSING_FEE` returns `replayExecuted=true`.
- `FX005_MISSING_FEE` returns `status=WARNING`, matching the fixture expected
  output and expected issue level.
- `FX005_MISSING_FEE` returns a non-blocking `MISSING_FEE` issue.
- Missing fee is not silently treated as `fee=0`.
- `FX005_MISSING_FEE` does not generate normal cost, position, cash, PnL,
  base-position, or sniper-pool success outputs.
- `FX006_NEGATIVE_CASH` through `FX013_MULTI_CURRENCY_UNSUPPORTED` still return
  `NOT_IMPLEMENTED`.

This implementation is intentionally limited to detecting one BUY fact where
`feeText` is missing, null, or empty. It does not implement negative-cash errors,
multi-account replay, multi-instrument replay, market valuation, realized PnL,
unrealized PnL, base-position, sniper-pool, SQLite access, DataService calls,
file writes, database writes, or DataService actions. Future implementation
must add one fixture at a time without changing fixture expected outputs to fit
an incorrect algorithm.

## 18. TASK-060 Minimal FX006 Negative-Cash Detection

`FX006_NEGATIVE_CASH` now has a test-only minimal replay error path through
`AccountingReplayMinimalEngine`.

- `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, and `FX003_BUY_SELL_PARTIAL` still
  return `status=OK`.
- `FX004_SELL_EXCEEDS_POSITION` still returns `status=ERROR` with a blocking
  `SELL_EXCEEDS_POSITION` issue.
- `FX005_MISSING_FEE` still returns `status=WARNING` with a non-blocking
  `MISSING_FEE` issue.
- `FX006_NEGATIVE_CASH` returns `implemented=true`.
- `FX006_NEGATIVE_CASH` returns `replayExecuted=true`.
- `FX006_NEGATIVE_CASH` returns `status=ERROR`.
- `FX006_NEGATIVE_CASH` returns a blocking `NEGATIVE_CASH` issue when buy cash requirement exceeds available initial cash.
- Negative cash is not silently accepted as an implicit overdraft.
- `FX006_NEGATIVE_CASH` does not generate normal position, cash, PnL,
  base-position, or sniper-pool success outputs.
- `FX007_MULTI_INSTRUMENT` through `FX013_MULTI_CURRENCY_UNSUPPORTED` still
  return `NOT_IMPLEMENTED`.

This implementation is intentionally limited to detecting one initial cash fact
and one BUY fact where `amountText + feeText` is greater than initial cash. It
does not implement multi-account replay, multi-instrument replay, market
valuation, realized PnL, unrealized PnL, base-position, sniper-pool, SQLite
access, DataService calls, file writes, database writes, or DataService actions.
Future implementation must add one fixture at a time without changing fixture
expected outputs to fit an incorrect algorithm.

## 19. TASK-061 Minimal FX007 Multi-Instrument Replay

`FX007_MULTI_INSTRUMENT` now has a test-only minimal replay path through
`AccountingReplayMinimalEngine`.

- `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, and `FX003_BUY_SELL_PARTIAL` still
  return `status=OK`.
- `FX004_SELL_EXCEEDS_POSITION` still returns `status=ERROR` with a blocking
  `SELL_EXCEEDS_POSITION` issue.
- `FX005_MISSING_FEE` still returns `status=WARNING` with a non-blocking
  `MISSING_FEE` issue.
- `FX006_NEGATIVE_CASH` still returns `status=ERROR` with a blocking
  `NEGATIVE_CASH` issue.
- `FX007_MULTI_INSTRUMENT` returns `implemented=true`.
- `FX007_MULTI_INSTRUMENT` returns `replayExecuted=true`.
- `FX007_MULTI_INSTRUMENT` returns `status=WARNING`, matching the fixture
  expected `MARKET_PRICE_MISSING` warning.
- `FX007_MULTI_INSTRUMENT` creates separate positions for `159509` and `518880`
  and does not mix their `instrumentCode` values.
- `FX007_MULTI_INSTRUMENT` computes cash from initial cash minus each BUY
  `amountText + feeText`, producing `cashBalanceText=96998.00 CNY`.
- Missing market prices are reported through non-blocking `MARKET_PRICE_MISSING`.
- Market price, market value, and unrealized PnL remain unavailable; this path
  does not fabricate valuation or unrealized PnL.
- `FX008_MULTI_ACCOUNT` through `FX013_MULTI_CURRENCY_UNSUPPORTED` still return
  `NOT_IMPLEMENTED`.

## 20. TASK-062 Minimal FX008 Multi-Account Replay

`FX008_MULTI_ACCOUNT` now has a test-only minimal replay path through
`AccountingReplayMinimalEngine`.

- `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, and `FX003_BUY_SELL_PARTIAL` still
  return `status=OK`.
- `FX004_SELL_EXCEEDS_POSITION` still returns `status=ERROR` with a blocking
  `SELL_EXCEEDS_POSITION` issue.
- `FX005_MISSING_FEE` still returns `status=WARNING` with a non-blocking
  `MISSING_FEE` issue.
- `FX006_NEGATIVE_CASH` still returns `status=ERROR` with a blocking
  `NEGATIVE_CASH` issue.
- `FX007_MULTI_INSTRUMENT` still returns `status=WARNING` and keeps different
  `instrumentCode` values separated.
- `FX008_MULTI_ACCOUNT` returns `implemented=true`.
- `FX008_MULTI_ACCOUNT` returns `replayExecuted=true`.
- `FX008_MULTI_ACCOUNT` returns `status=OK`, matching the fixture contract.
- `FX008_MULTI_ACCOUNT` creates separate `159509` positions for
  `ACC-DEMO-001` and `ACC-DEMO-002` and does not merge or mix their `accountId`
  values.
- `FX008_MULTI_ACCOUNT` preserves `portfolioId=PF-DEMO-001` on account-scoped
  positions and account cash summaries.
- Account cash is exposed through `accountCashSummaries`, producing
  `cashBalanceText=48999.00 CNY` for `ACC-DEMO-001` and
  `cashBalanceText=47999.00 CNY` for `ACC-DEMO-002`.
- Market value and unrealized PnL remain unavailable; this path does not
  fabricate valuation or unrealized PnL.
- `FX009_BASE_POSITION_LOCKED` through `FX013_MULTI_CURRENCY_UNSUPPORTED` still
  return `NOT_IMPLEMENTED`.

This implementation is intentionally limited to grouping CNY BUY facts by
`accountId + portfolioId + instrumentCode`. It does not implement base-position,
sniper-pool, multi-currency, real market valuation, SQLite access, DataService
calls, output file writes, database writes, or DataService actions.

## 21. TASK-063 Minimal FX009 Base-Position Replay

`FX009_BASE_POSITION_LOCKED` now has a test-only minimal replay path through
`AccountingReplayMinimalEngine`.

- `FX001_EMPTY_LEDGER`, `FX002_SINGLE_BUY`, and `FX003_BUY_SELL_PARTIAL` still
  return `status=OK`.
- `FX004_SELL_EXCEEDS_POSITION` still returns `status=ERROR` with a blocking
  `SELL_EXCEEDS_POSITION` issue.
- `FX005_MISSING_FEE` still returns `status=WARNING` with a non-blocking
  `MISSING_FEE` issue.
- `FX006_NEGATIVE_CASH` still returns `status=ERROR` with a blocking
  `NEGATIVE_CASH` issue.
- `FX007_MULTI_INSTRUMENT` and `FX008_MULTI_ACCOUNT` keep their prior minimal
  replay behavior.
- `FX009_BASE_POSITION_LOCKED` returns `implemented=true`.
- `FX009_BASE_POSITION_LOCKED` returns `replayExecuted=true`.
- `FX009_BASE_POSITION_LOCKED` returns `status=OK`, matching the fixture
  contract.
- `FX009_BASE_POSITION_LOCKED` only outputs readonly `basePositionRaw`.
- The 20% locked base position is represented by `targetBaseRatioText=20%`,
  `targetBaseAmountText=20000.00 CNY`, `currentBaseAmountText=20000.00 CNY`,
  `lockedBaseAmountText=20000.00 CNY`, and
  `sellableAboveBaseAmountText=0.00 CNY`.
- `sellableAboveBaseAmountText` is not a sell suggestion and must not be treated
  as a TradeDraft, sell action, strategy command, or broker order.
- `FX009_BASE_POSITION_LOCKED` does not produce `sniperPoolRaw`.
- `FX010_SNIPER_TIER_COMPLETED` through `FX013_MULTI_CURRENCY_UNSUPPORTED` still
  return `NOT_IMPLEMENTED`.

This implementation is intentionally limited to deriving display-only
base-position fields from the FX009 input facts. It does not implement
sniper-pool, multi-currency, real market valuation, SQLite access, DataService
calls, output file writes, database writes, TradeDraft generation, sell actions,
strategy execution, or DataService actions.

This implementation is intentionally limited to same-account, same-portfolio,
CNY-only BUY facts grouped by instrument. It does not implement multi-account
replay, multi-currency replay, market valuation, unrealized PnL, base-position,
sniper-pool, SQLite access, DataService calls, file writes, database writes, or
DataService actions. Future implementation must add one fixture at a time
without changing fixture expected outputs to fit an incorrect algorithm.
