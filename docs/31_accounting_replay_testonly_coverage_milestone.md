# Accounting Replay Test-only Coverage Milestone

## Milestone Name

Accounting Replay Test-only Coverage Milestone.

## Milestone Positioning

This milestone completes FX001-FX013 test-only minimal replay coverage under
`tests/AccountingFixtures`.

This is a fixture coverage closure milestone. It is not production accounting
replay, and production accounting replay is not implemented.

The milestone validates the fixture contract, result skeleton, assertion
skeleton, and incremental replay boundary before any production architecture is
authorized.

## Completed Coverage

- `FX001_EMPTY_LEDGER`: `OK`.
- `FX002_SINGLE_BUY`: `OK`.
- `FX003_BUY_SELL_PARTIAL`: `OK`.
- `FX004_SELL_EXCEEDS_POSITION`: `ERROR` / `SELL_EXCEEDS_POSITION`.
- `FX005_MISSING_FEE`: `WARNING` / `MISSING_FEE`.
- `FX006_NEGATIVE_CASH`: `ERROR` / `NEGATIVE_CASH`.
- `FX007_MULTI_INSTRUMENT`: `WARNING` or fixture-defined status.
- `FX008_MULTI_ACCOUNT`: `OK` or fixture-defined status.
- `FX009_BASE_POSITION_LOCKED`: `OK` / `basePositionRaw`.
- `FX010_SNIPER_TIER_COMPLETED`: `OK` / `sniperPoolRaw`.
- `FX011_STALE_SNAPSHOT`: `STALE` / `SNAPSHOT_STALE`.
- `FX012_MISSING_MARKET_PRICE`: `WARNING` / `MARKET_PRICE_MISSING`.
- `FX013_MULTI_CURRENCY_UNSUPPORTED`: `ERROR` /
  `MULTI_CURRENCY_UNSUPPORTED` / `FX_RATE_MISSING`.

## Completed Test Infrastructure

- `validate_accounting_replay_fixtures.py`.
- `AccountingFixtureLoader`.
- `AccountingReplayResult`.
- `AccountingReplayResultMapper`.
- `AccountingReplayStubEngine`.
- `AccountingReplayMinimalEngine`.
- `AccountingReplayTestHarness`.
- `AccountingExpectedOutputInspector`.
- `AccountingReplayAssertionSkeleton`.
- `accounting_replay_fixture_static_validation`.
- `accounting_replay_fixture_loader`.
- `accounting_replay_harness_skeleton`.
- `accounting_replay_result_skeleton`.
- `accounting_replay_expected_output_assertions`.
- `accounting_replay_minimal_fx001`.
- `accounting_replay_minimal_fx002`.
- `accounting_replay_minimal_fx003`.
- `accounting_replay_minimal_fx004`.
- `accounting_replay_minimal_fx005`.
- `accounting_replay_minimal_fx006`.
- `accounting_replay_minimal_fx007`.
- `accounting_replay_minimal_fx008`.
- `accounting_replay_minimal_fx009`.
- `accounting_replay_minimal_fx010`.
- `accounting_replay_minimal_fx011`.
- `accounting_replay_minimal_fx012`.
- `accounting_replay_minimal_fx013`.

## Not Production Replay

This milestone does not implement production accounting replay.

- No DataService action is connected.
- No SQLite access is performed.
- No `trade_log` writes are performed.
- No snapshot writes are performed.
- No `portfolio_summary` writes are performed.
- `position.list` is not implemented.
- `cash.summary` is not implemented.
- `portfolio.pnl.summary` is not implemented.
- `base_position.summary` is not implemented.
- `sniper_pool.summary` is not implemented.
- QML is not connected.
- Real market data is not connected.
- Real FX rates are not connected.
- Broker APIs are not connected.
- Automatic trading is not implemented.

## Test-only Minimal Engine Limits

`AccountingReplayMinimalEngine` only serves fixture-backed tests. It is compiled
only under `tests/AccountingFixtures` and is not part of production libraries.

It does not guarantee production-grade extensibility. It does not process a real
database, real large-scale trades, real market data, or real multi-currency FX
rates. It must not be directly copied into production.

Future production replay may reuse the fixture contract, expected outputs, and
test strategy. It must not directly reuse this test helper implementation as a
production engine.

## Quality Gates

- CMake configure passes.
- Build passes.
- Full CTest passes.
- `transport_local_socket_echo` passes 50 repeated runs.
- Fixture static validator passes.
- Fixture loader passes.
- FX001-FX013 minimal tests pass.
- `git status` is clean.

## Suggested Tag

After this PR is merged, the user may manually create the milestone tag:

```powershell
git tag -a v0.3.0-accounting-replay-testonly-coverage -m "v0.3 Accounting Replay Test-only Coverage"
git push origin v0.3.0-accounting-replay-testonly-coverage
```

The tag should only be created after PR merge. TASK-068 does not create a tag.

## Next Stage Recommendation

The next stage should start with production accounting replay architecture and
boundary design, not a direct DataService action.

Recommended decisions:

- Production accounting replay architecture design.
- Production replay interface boundary.
- Replay engine placement decision.
- DataService read-only action boundary.
- SQLite read-only source boundary.
- Snapshot write boundary, still requiring separate authorization.
- Do not directly move the test-only engine into production.
