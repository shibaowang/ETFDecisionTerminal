# TASK-243 AccountingEngine adequacy review CI closeout test plan

## Test Objective

TASK-243 verifies that the TASK-239 through TASK-242 AccountingEngine adequacy
review gates are ready for CI closeout while remaining review-only, test-only,
deterministic, no-write, and clean-main safe.

## CI Checks

- docs/282 exists.
- docs/283 exists.
- README references TASK-243, docs/282, and docs/283.
- docs/README references docs/282 and docs/283.
- docs/12 references TASK-243, docs/282, and docs/283.
- tests/CMakeLists registers the TASK-243 gate directory.
- TASK-243 CMakeLists registers
  `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout`.
- TASK-239 through TASK-242 docs and gate scripts still exist.
- Critical CTest names are registered.
- CI closeout commands are documented.
- The gate does not recursively run full CTest.

## Clean-Main Checks

Clean main may have an empty changed-path set. Feature branch changed paths
must include untracked files and must be exact-path allowlisted. Unauthorized
changed paths fail closed. The gate must not require current TASK-243 files to
be present in changed paths after merge.

## Changed-Path Allowlist

The changed-path allowlist is exact-file only. TASK-243 may change README,
docs/README, docs/12, docs/282, docs/283, tests/CMakeLists, and the TASK-243
CTest directory. Minimal self-consistency changes to existing review gates are
allowed only as exact paths and only for changed-set allowlist maintenance.

## Regression Gates

Each targeted CTest pattern must first be probed with `ctest -N -R` and must
match at least one registered test. Required regression gates include the
TASK-243 CI closeout gate, TASK-242 failure-mode hardening, TASK-241
regression matrix, TASK-240 implementation gate, TASK-239 authorization gate,
TASK-238 phase closeout, TASK-237 CI closeout, TASK-236 failure-mode
hardening, TASK-235 regression matrix, TASK-234 implementation, TASK-232
dry-run harness regression matrix, TASK-231 dry-run harness, shell diagnostics
facade, full CTest, and transport local socket echo repeated 50 times.

## Failure Modes

- Missing docs/282 or docs/283 fails.
- Missing index references fail.
- Missing CTest registration fails.
- Unauthorized changed paths fail.
- Clean-main changed paths empty succeeds.
- Known clean-main-only changed-path assertions fail.
- Recursive full CTest inside the gate fails.
- Fixture JSON modifications fail.
- AccountingEngine code modifications fail.
- AccountingEngine replay, bridge, parser, loader, reader, runtime SQL,
  audit, ledger, snapshot, read model refresh, broker, network, credential,
  endpoint, real order, or automatic trading tokens fail.

## Acceptance Criteria

- TASK-243 CTest passes.
- TASK-242 through TASK-239 gates still pass.
- Replay implementation closeout and dry-run harness gates still pass.
- Critical CTest names are registered.
- Gate check count is at least 100.
- The gate supports clean main with empty changed paths.
- No production code, fixture JSON, migrations, AccountingEngine code, broker,
  network, credentials, endpoint, real order placement, or automatic trading
  code changes are introduced.

## Formal Conclusion

TASK-243 adds a review-only CI closeout gate for AccountingEngine adequacy review.
TASK-243 does not call AccountingEngine replay.
TASK-243 does not implement AccountingEngine bridge.
TASK-243 does not implement replay.
TASK-243 does not modify AccountingEngine code.
TASK-243 does not authorize runtime SQL / SQLite read/write.
TASK-243 does not authorize audit / ledger / snapshot writes.
TASK-243 does not authorize read model refresh.
TASK-243 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.
Recommended next task: TASK-244 ShellAccounting manual entry replay AccountingEngine adequacy review phase closeout and handoff gate.

## Go / No-Go Checklist

- Go: docs/282 and docs/283 are present and indexed.
- Go: CTest registration is present and nonempty under `ctest -N -R`.
- Go: clean-main empty changed paths are accepted.
- Go: nonempty changed paths are exact-file allowlisted and fail closed.
- Go: review, regression, failure-mode, closeout, dry-run harness, diagnostics,
  full CTest, and transport validations pass.
- No-Go: any AccountingEngine replay call, bridge implementation, runtime
  write, production drift, fixture JSON mutation, broker/network drift, real
  order placement, or automatic trading is present.
