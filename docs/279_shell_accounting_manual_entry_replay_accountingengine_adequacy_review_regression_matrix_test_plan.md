# TASK-241 AccountingEngine adequacy review regression matrix test plan

## Test Objective

Verify that TASK-241 adds a deterministic, test-only, review-only regression matrix for the manual entry replay AccountingEngine adequacy review boundary.

The test plan does not authorize AccountingEngine replay calls, AccountingEngine bridge implementation, production replay, runtime SQL / SQLite read/write, audit / ledger / snapshot writes, read model refresh, broker access, real order placement, or automatic trading.

## Matrix Checks

The regression matrix gate must check:

- `docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md` exists.
- `docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md` exists.
- README, docs README, and prompt template reference TASK-241 and docs/278 / docs/279.
- TASK-241 CMake registration exists.
- TASK-240 docs and gate still exist.
- TASK-239 docs and gate still exist.
- Review input matrix covers implementation summary, dry-run summary, fixture index, and negative fixture index.
- Static adequacy matrix documents no AccountingEngine call, no bridge, no runtime integration, no write, and no production code modification.
- Go / No-Go matrix documents pass and fail boundaries.
- Gate checks are printed and the check count is at least 60.

The test plan retains these explicit matrix conclusions:

- No AccountingEngine call.
- No runtime integration.
- No fixture JSON mutation.
- No validator semantic change.

## Changed-Path Allowlist

The gate must collect staged, unstaged, and untracked paths.

The gate must support clean main after merge, where changed paths may be empty.

If changed paths are non-empty, every changed path must be exact allowlisted. Directory-level allowlists such as `docs/`, `tests/`, `apps/`, `libs/`, or `migrations/` are forbidden.

Allowed TASK-241 paths are:

- `README.md`
- `docs/README.md`
- `docs/12_codex_prompt_template.md`
- `docs/278_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.md`
- `docs/279_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix_test_plan.md`
- `tests/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewRegressionMatrixGate/manual_entry_replay_accountingengine_adequacy_review_regression_matrix_gate.py`

Any old gate exact-path self-consistency update must add only the specific script path that was actually modified.

## Regression Gates

Each targeted probe must first run `ctest -N -R` and confirm at least one matching test.

Required probes:

- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization`
- `shell_accounting_manual_entry_replay_implementation_phase_closeout`
- `shell_accounting_manual_entry_replay_implementation_ci_closeout`
- `shell_accounting_manual_entry_replay_implementation_failure_mode_hardening`
- `shell_accounting_manual_entry_replay_implementation_regression_matrix`
- `shell_accounting_manual_entry_replay_implementation`
- `shell_accounting_manual_entry_replay_implementation_authorization`
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix`
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness`
- `shell_diagnostics_facade`
- `transport_local_socket_echo`

Full CTest must also pass externally. The TASK-241 gate must not recursively execute full CTest.

## Failure Modes

The gate must fail if:

- TASK-241 docs or CTest registration are missing.
- TASK-239 or TASK-240 artifacts are missing.
- Review input matrix omits implementation summary, dry-run summary, fixture index, or negative fixture index.
- The matrix authorizes AccountingEngine replay, AccountingEngine bridge, runtime integration, runtime SQL / SQLite write, audit / ledger / snapshot write, read model refresh, production code modification, broker access, real order placement, or automatic trading.
- Changed paths use a directory-level allowlist.
- Changed paths omit staged, unstaged, or untracked files.
- Fixture JSON, `apps/`, `libs/`, `migrations/`, or `libs/AccountingEngine/` changes.
- The gate creates DB-like runtime artifacts.

## Acceptance Criteria

- TASK-241 regression matrix gate passes.
- TASK-240 implementation gate passes.
- TASK-239 authorization gate passes.
- Replay implementation and dry-run harness regression gates pass.
- `shell_diagnostics_facade` passes.
- `transport_local_socket_echo` passes 50 times externally.
- Full CTest passes externally.
- `git diff --check` passes.
- `git status --short` is clean after commit.

## Formal Conclusion

TASK-241 adds a review-only regression matrix for AccountingEngine adequacy review.

TASK-241 does not call AccountingEngine replay.

TASK-241 does not implement AccountingEngine bridge.

TASK-241 does not implement replay.

TASK-241 does not modify AccountingEngine code.

TASK-241 does not authorize runtime SQL / SQLite read/write.

TASK-241 does not authorize audit / ledger / snapshot writes.

TASK-241 does not authorize read model refresh.

TASK-241 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-242 ShellAccounting manual entry replay AccountingEngine adequacy review failure-mode hardening gate.
