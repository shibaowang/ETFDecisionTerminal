# ShellAccounting Manual Entry Replay Test-Only Dry-Run Harness Regression Matrix Test Plan

## Document Purpose

This test plan defines TASK-232 validation for the manual entry replay
test-only dry-run harness regression matrix. It verifies that the TASK-231
harness emits stable sanitized summary JSON across positive and negative fixture
metadata without replay, AccountingEngine calls, runtime writes, fixture JSON
changes, production drift, or broker behavior.

## Test Matrix

- docs/260 exists and documents TASK-232.
- docs/261 exists and documents this test plan.
- README, docs/README, and docs/12 reference TASK-232, docs/260, and docs/261.
- tests/CMakeLists registers
  `ShellAccountingManualEntryReplayTestOnlyDryRunHarnessRegressionMatrixGate`.
- CTest `shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix` is registered.
- TASK-231 dry-run harness CTest remains registered.
- TASK-230 authorization gate remains registered.
- TASK-229 planning gate remains registered.
- TASK-228 phase closeout gate remains registered.
- TASK-224 negative fixture static validator remains registered.
- The regression gate invokes the TASK-231 harness with `--source-root` and
  `--summary-json`.
- Summary JSON parses as a JSON object.
- Summary schema is
  `manual-entry-replay-test-only-dry-run-summary/v1`.
- Summary `dryRunStatus` is `ok`.
- Positive fixture count matches the positive fixture index.
- Negative fixture count remains ten.
- Entries cover every positive fixture id.
- Entries cover every negative fixture id.
- Every entry contains only sanitized metadata fields.
- Every entry has `fixtureId`, `source`, `dryRunStatus`, `plannedStepCount`,
  `replayExecuted`, `accountingEngineCalled`, and `runtimeWrites`.
- Every `plannedStepCount` is a non-negative integer.
- Every `replayExecuted` value is `false`.
- Every `accountingEngineCalled` value is `false`.
- Every `runtimeWrites` value is `false`.
- Summary output excludes raw fixture payloads.
- Summary output excludes credentials, endpoints, broker payloads, real order
  ids, raw SQL, stack traces, apps, libs, migrations, and runtime database
  state.
- Positive fixture JSON hashes are unchanged.
- Negative fixture JSON hashes are unchanged.
- Changed paths support clean-main empty state.
- Non-empty changed paths must be exact allowlisted.
- Production apps, libs, and migrations remain unchanged.
- The TASK-231 harness behavior is unchanged except exact-path changed-set
  self-consistency.
- TASK-224 validator validation behavior is unchanged except exact-path
  changed-set self-consistency if required.
- No parser / loader / reader / replay implementation appears.
- No AccountingEngine replay call appears.
- No runtime SQL / SQLite / audit / ledger / snapshot write appears.
- No real position / cash / PnL calculation appears.
- No broker / network / credentials / endpoint behavior appears.
- No real order placement or automatic trading appears.
- The gate check count is at least 180.
- The gate does not recursively run full CTest.

## Required Probes

- Documentation existence probe.
- Documentation index probe.
- CTest directory registration probe.
- CTest name registration probe using `ctest -N`.
- Harness invocation probe using `--source-root` and `--summary-json`.
- Summary JSON parse probe.
- Summary schema probe.
- Summary status probe.
- Positive fixture index count probe.
- Negative fixture index count probe.
- Positive fixture id coverage probe.
- Negative fixture id coverage probe.
- Sanitized summary entry key probe.
- Planned step non-negative integer probe.
- Replay false flag probe.
- AccountingEngine false flag probe.
- Runtime writes false flag probe.
- Raw payload output probe.
- SQL / credential / endpoint / broker / real-order output probe.
- Apps / libs / migrations output probe.
- Fixture hash before / after probe.
- Changed-path exact allowlist probe.
- Clean-main empty changed-path probe.
- Production path unchanged probe.
- No parser / loader / reader / replay probe.
- No AccountingEngine replay probe.
- No runtime SQL / SQLite / audit / ledger / snapshot write probe.
- No real position / cash / PnL probe.
- No broker / network / credentials / endpoint probe.
- No recursive full CTest probe.
- External full CTest regression.
- External transport 50-repeat regression.
- `git diff --check` probe.

## Go / No-Go Checklist

Go only if:

- TASK-232 regression matrix gate passes.
- TASK-231 dry-run harness CTest passes.
- TASK-230 authorization gate passes.
- TASK-229 planning gate passes.
- TASK-228 phase closeout gate passes.
- TASK-224 negative fixture static validator passes.
- Fixture JSON remains unchanged.
- Summary JSON is sanitized.
- Replay, AccountingEngine, and runtime write flags remain false.
- Production code remains unchanged.
- No parser / loader / reader / replay implementation is added.
- No AccountingEngine replay call is added.
- No runtime SQL / SQLite / audit / ledger / snapshot write is added.
- No real position / cash / PnL calculation is added.
- No broker / network / credentials / endpoint behavior is added.
- Full CTest is run externally and passes.
- Transport local socket echo passes 50 repeats.
- `git diff --check` passes.

No-Go if:

- Any required CTest pattern matches zero tests.
- The regression gate depends on changed paths being non-empty.
- Fixture JSON changes.
- The harness emits raw fixture payload or sensitive output.
- The harness or gate performs replay or calls AccountingEngine replay.
- Runtime SQL / SQLite / audit / ledger / snapshot writes appear.
- Production code, QML, DataServiceActions, repositories, or migrations change.
- Broker, network, credentials, endpoint, real order, or automatic trading appears.
