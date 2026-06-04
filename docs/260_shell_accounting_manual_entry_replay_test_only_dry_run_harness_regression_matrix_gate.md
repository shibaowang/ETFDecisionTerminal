# ShellAccounting Manual Entry Replay Test-Only Dry-Run Harness Regression Matrix Gate

## Purpose

TASK-232 adds a regression matrix gate for the TASK-231 manual entry replay
test-only dry-run harness. The gate invokes the harness through CTest, validates
its sanitized summary JSON, and keeps the no-replay, no-write, no-production,
and no-broker boundaries fixed before any future replay implementation work.

TASK-232 is a gate only. It does not change dry-run harness business semantics,
does not modify fixture JSON, does not implement replay, does not call
AccountingEngine replay, and does not add runtime SQL / SQLite reads or writes.

## Relationship To TASK-231

TASK-231 implemented the test-only dry-run harness under:

```text
tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/
```

TASK-232 validates that implementation through a separate regression matrix
gate. The gate must call:

```text
tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarness/manual_entry_replay_test_only_dry_run_harness.py --source-root <root> --summary-json
```

The TASK-231 harness remains test-only. TASK-232 does not convert it into a
production parser, loader, reader, replay engine, or runtime data source.

## Regression Matrix Scope

The regression matrix covers:

- positive manual entry replay fixture metadata.
- negative manual entry replay fixture scaffold metadata.
- sanitized dry-run summary shape.
- per-entry dry-run status.
- no replay execution flags.
- no AccountingEngine call flags.
- no runtime write flags.
- fixture JSON immutability.
- no production path drift.
- no runtime SQL / SQLite / audit / ledger / snapshot writes.
- no broker / network / credentials / endpoint / real-order behavior.

## Harness Summary Contract

The gate requires summary JSON with:

```text
schemaVersion = manual-entry-replay-test-only-dry-run-summary/v1
dryRunStatus = ok
positiveFixtureCount = current positive fixture index count
negativeFixtureCount = 10
entries = positive and negative fixture dry-run rows
```

Each entry must contain only:

```text
fixtureId
source
dryRunStatus
plannedStepCount
replayExecuted
accountingEngineCalled
runtimeWrites
```

`plannedStepCount` must be a non-negative integer. `replayExecuted`,
`accountingEngineCalled`, and `runtimeWrites` must remain `false` for every
entry.

## Sanitized Output Boundary

The gate must reject raw fixture payloads, raw SQL, credentials, endpoints,
broker payloads, real broker order ids, stack traces, internal exception
payloads, production path names, runtime database state, or full manual entry
payloads in the summary JSON.

TASK-232 does not compute real positions, cash balances, PnL, market value,
cash facts, trade facts, ledger rows, audit rows, or snapshot rows.

## Fixture Immutability Boundary

The gate records positive and negative fixture JSON hashes before and after
the harness run. The hashes must remain unchanged.

TASK-232 does not modify:

```text
tests/fixtures/manual_entry_replay/*.json
tests/fixtures/manual_entry_replay/fixtures_index.json
tests/fixtures/manual_entry_replay_negative/*.json
tests/fixtures/manual_entry_replay_negative/negative_fixtures_index.json
```

## No-Write Boundary

The gate verifies that changed paths remain exact-path allowlisted. Clean-main
closeout is valid when changed paths are empty. If changed paths are non-empty,
every path must be an exact TASK-232-authorized file.

The gate must not create DB, WAL, SHM, SQLite, audit, ledger, snapshot, cash,
trade, broker, or generated replay files.

## No-Replay Boundary

TASK-232 does not implement replay and does not call AccountingEngine replay.
The regression matrix validates that each summary entry reports:

```text
replayExecuted = false
accountingEngineCalled = false
runtimeWrites = false
```

Production parser, loader, reader, replay, and AccountingEngine integration
remain future work requiring separate authorization.

## No-Production Boundary

TASK-232 does not modify apps, libs, migrations, production QML, startup,
DataServiceActions, repositories, ShellServices, Presenter, Controller,
AccountingEngine, StrategyEngine, MarketEngine, or broker-related code.

TASK-232 does not connect broker SDKs, network clients, credentials, endpoints,
real broker order ids, order placement, strategy execution, or automatic
trading.

## Gate Implementation Boundary

The CTest is:

```text
shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix
```

The gate implementation lives only under:

```text
tests/ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate/
```

The gate must not recursively run full CTest. Full CTest remains an external
validation step for the developer workflow.

## Formal Conclusion And Next Task

TASK-232 adds a test-only regression matrix gate around the TASK-231 dry-run
harness. It does not implement replay, production parser / loader / reader
behavior, AccountingEngine replay, runtime SQL / SQLite read/write, audit /
ledger / snapshot writes, real position / cash / PnL calculation, broker,
network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-233 manual entry replay implementation
authorization gate.
