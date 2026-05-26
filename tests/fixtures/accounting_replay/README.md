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
`cashBalanceText=98999.00 CNY`. FX003-FX013 remain `NOT_IMPLEMENTED`.
