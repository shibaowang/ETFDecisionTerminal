# Release Notes - v0.3 Accounting Replay Test-only Coverage

## Date

TBD.

## Highlights

- FX001-FX013 test-only minimal replay coverage complete.
- Fixture files and validator.
- Fixture loader.
- Replay result skeleton.
- Replay harness.
- Expected-output assertion skeleton.
- Minimal engine coverage for each fixture.
- Production readiness review.

## Added

- Accounting replay fixture files.
- Fixture static validator.
- `AccountingFixtureLoader`.
- `AccountingReplayTestHarness`.
- `AccountingReplayResult`.
- `AccountingReplayResultMapper`.
- `AccountingReplayStubEngine`.
- `AccountingReplayAssertionSkeleton`.
- `AccountingReplayMinimalEngine` coverage for FX001-FX013.
- Accounting replay test-only coverage milestone documentation.
- Production accounting replay readiness review documentation.

## Not Included

- Production accounting replay.
- DataService `position.list`.
- DataService `cash.summary`.
- DataService `portfolio.pnl.summary`.
- Snapshot writes.
- `trade_log` writes.
- TradeDraft lifecycle.
- Strategy calculation.
- Market feed.
- FX rate service.
- Broker API.
- Auto trading.

## Validation

- Full CTest passed.
- `transport_local_socket_echo` repeat passed.
- Fixture validation passed.

## Suggested Tag After Merge

`v0.3.0-accounting-replay-testonly-coverage`

TASK-068 does not create a tag. The tag should be created manually only after
the PR is merged.
