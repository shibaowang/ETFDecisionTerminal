# Accounting Replay Fixtures

These JSON files are static accounting replay fixture samples for future tests.
They are not database rows, do not write SQLite, do not call services, do not
drive trading, and do not generate TradeDraft.

`fixtures_index.json` lists FX001-FX013. Each fixture contains input facts,
expected read-only DTO-shaped outputs, expected issues, blocking status, and
notes. The validator checks structure only; it does not calculate cash,
positions, PnL, or accounting correctness.

The Python static validator is
`tests/AccountingFixtures/validate_accounting_replay_fixtures.py`.

The test-only C++ loader is `tests/AccountingFixtures/AccountingFixtureLoader`.
It only parses and validates the JSON structure. It does not implement replay,
calculate cash, position, or PnL, access SQLite, call DataService, or write
output files.

The fixture-backed test harness is
`tests/AccountingFixtures/AccountingReplayTestHarness`. It currently uses
`AccountingReplayStubEngine`, which returns `NOT_IMPLEMENTED` with
`implemented=false` and `replayExecuted=false` for valid fixtures. This is a
coverage skeleton only and does not implement accounting replay.

The test-only replay result skeleton is
`tests/AccountingFixtures/AccountingReplayResult`. Current stub results keep
position, cash, PnL, base-position, and sniper-pool raw outputs empty and add a
`REPLAY_NOT_IMPLEMENTED` issue.

The test-only expected-output assertion skeleton is
`tests/AccountingFixtures/AccountingReplayAssertionSkeleton`, with shape
inspection in `AccountingExpectedOutputInspector`. It currently validates
fixture expected-output structure and the `NOT_IMPLEMENTED` guard only. Real
position, cash, and PnL value assertions remain `SKIPPED_BY_DESIGN` until replay
is implemented.

The test-only minimal replay engine is
`tests/AccountingFixtures/AccountingReplayMinimalEngine`. It supports
`FX001_EMPTY_LEDGER`, returning `status=OK` with empty positions and zero cash /
PnL raw summaries.

`AccountingReplayMinimalEngine` now also supports `FX002_SINGLE_BUY`, returning
one 159509 position, `quantityText=1000`, `costAmountText=1001.00 CNY`, and
`cashBalanceText=98999.00 CNY`.

`AccountingReplayMinimalEngine` now also supports
`FX003_BUY_SELL_PARTIAL`, returning one remaining 159509 position,
`quantityText=600`, `costAmountText=600.60 CNY`,
`cashBalanceText=99478.00 CNY`, and `realizedPnlText=78.60 CNY`.

`AccountingReplayMinimalEngine` now also supports
`FX004_SELL_EXCEEDS_POSITION`, returning `status=ERROR` with a blocking
`SELL_EXCEEDS_POSITION` issue and no normal position, cash, or PnL success
outputs.

`AccountingReplayMinimalEngine` now also supports `FX005_MISSING_FEE`,
returning `status=WARNING` with a non-blocking `MISSING_FEE` issue and no
normal cost, position, cash, or PnL success outputs. Missing fee is not treated
as `fee=0`.

`AccountingReplayMinimalEngine` now also supports `FX006_NEGATIVE_CASH`,
returning `status=ERROR` with a blocking `NEGATIVE_CASH` issue and no normal
position, cash, or PnL success outputs. Negative cash is not treated as an
implicit overdraft.

`AccountingReplayMinimalEngine` now also supports `FX007_MULTI_INSTRUMENT`,
returning separate 159509 and 518880 positions, `cashBalanceText=96998.00 CNY`,
and a non-blocking `MARKET_PRICE_MISSING` warning. It does not fabricate market
value or unrealized PnL.

`AccountingReplayMinimalEngine` now also supports `FX008_MULTI_ACCOUNT`,
returning separate account-scoped 159509 positions for `ACC-DEMO-001` and
`ACC-DEMO-002`, plus account cash summaries with `cashBalanceText=48999.00 CNY`
and `cashBalanceText=47999.00 CNY`. It does not mix accountId or portfolioId
dimensions, and does not fabricate market value or unrealized PnL. FX009-FX013
remain `NOT_IMPLEMENTED`.

`AccountingReplayMinimalEngine` now also supports `FX009_BASE_POSITION_LOCKED`,
returning readonly `basePositionRaw` for the 20% locked-base sample:
`targetBaseRatioText=20%`, `lockedBaseAmountText=20000.00 CNY`, and
`sellableAboveBaseAmountText=0.00 CNY`. The sellable amount is display-only and
is not a sell suggestion, TradeDraft, sell action, strategy command, or broker
order. FX010-FX013 remain `NOT_IMPLEMENTED`.
