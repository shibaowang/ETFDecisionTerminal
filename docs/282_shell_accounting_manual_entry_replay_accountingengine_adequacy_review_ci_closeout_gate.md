# TASK-243 ShellAccounting manual entry replay AccountingEngine adequacy review CI closeout gate

## Purpose

TASK-243 adds a review-only CI closeout gate for the ShellAccounting manual
entry replay AccountingEngine adequacy review chain. The gate verifies that
TASK-239 through TASK-242 remain registered, deterministic, clean-main safe,
and scoped to documentation and test-only static review.

## Relationship To TASK-239 Through TASK-242

TASK-239 authorized the AccountingEngine adequacy review boundary. TASK-240
implemented the static review gate. TASK-241 added the regression matrix.
TASK-242 hardened failure modes. TASK-243 closes that review chain for CI by
checking that the review, regression, and failure-mode gates remain present and
that known clean-main failure regressions are absent.

## Scope

TASK-243 is documentation and CTest only. The CI closeout gate reads
documentation, CMake registration, gate scripts, and fixture metadata. It does
not run a recursive full CTest from inside the gate and does not mutate
fixtures, source, migrations, build output, or runtime data.

## Required CTest Set

- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation`
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization`
- `shell_accounting_manual_entry_replay_implementation_phase_closeout`
- `shell_accounting_manual_entry_replay_implementation_ci_closeout`
- `shell_accounting_manual_entry_replay_implementation_failure_mode_hardening`
- `shell_accounting_manual_entry_replay_implementation_regression_matrix`
- `shell_accounting_manual_entry_replay_implementation`
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix`
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness`
- `shell_diagnostics_facade`

## Clean-Main Invariants

The gate supports clean main after merge. Empty changed paths are valid in
clean-main closeout. If changed paths exist during feature-branch development,
they must be exact-path allowlisted and fail closed on unauthorized files.
The gate must not require current TASK-243 files to appear in changed paths.

## No Recursive Full CTest Boundary

The TASK-243 gate may inspect CTest registration text and may rely on external
validation commands, but it must not invoke `ctest --test-dir build
--output-on-failure` internally. Full CTest remains an external verification
step.

## CI Closeout Commands

Every targeted CTest command must first be probed with `ctest -N -R` and must
match at least one registered test before execution. The closeout command set
includes the TASK-243 gate, TASK-242 failure-mode hardening, TASK-241
regression matrix, TASK-240 implementation gate, TASK-239 authorization gate,
TASK-238 phase closeout, TASK-237 CI closeout, TASK-236 failure-mode
hardening, TASK-235 regression matrix, TASK-234 implementation, TASK-232
dry-run harness regression matrix, TASK-231 dry-run harness,
`shell_diagnostics_facade`, full CTest, transport local socket echo repeated
50 times, and `git diff --check`.
The CI closeout command set is an external validation requirement, not an
internal recursive test runner inside the TASK-243 gate.

## Review Gate Boundary

The review gate remains a static adequacy review. It does not become an
AccountingEngine bridge, parser, loader, reader, replay implementation, or
runtime integration path.

## Regression Matrix Boundary

The regression matrix remains review-only. It verifies static matrix coverage
and artifact retention without changing review success semantics, fixture JSON,
runtime data, or production code.

## Failure-Mode Boundary

Failure-mode hardening remains test-only. It may use temporary bad inputs to
prove fail-closed behavior and sanitized output, but it must not mutate the
real workspace fixture JSON or production source.

## No-Bridge Boundary

TASK-243 does not implement AccountingEngine bridge. It does not create bridge
directories, bridge CTests, bridge adapters, DTO mappers, or production bridge
wiring.

## No-Replay Boundary

TASK-243 does not call AccountingEngine replay. It does not call
`replayReadOnly`, production replay APIs, replay ledger APIs, parser, loader,
reader, or replay integration.

## No-Write Boundary

TASK-243 does not authorize runtime SQL / SQLite read/write. It does not
authorize audit / ledger / snapshot writes, read model refresh, runtime
database artifacts, TradeDraft generation, broker order state, or manual entry
runtime writes.

## AccountingEngine Code Boundary

TASK-243 does not modify AccountingEngine code. The `libs/AccountingEngine`
tree must remain unchanged. Any future AccountingEngine integration or replay
bridge must be separately authorized.

## Production Integration Boundary

TASK-243 does not modify production code, QML, startup, Presenter, Controller,
ShellServices adapter, DataServiceActions, repositories, migrations, or runtime
database integration.

## Broker And Network Boundary

TASK-243 does not add broker, network, credentials, endpoint, real order
placement, or automatic trading capability.

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

## Next Phase Handoff

Recommended next task: TASK-244 ShellAccounting manual entry replay
AccountingEngine adequacy review phase closeout and handoff gate.
