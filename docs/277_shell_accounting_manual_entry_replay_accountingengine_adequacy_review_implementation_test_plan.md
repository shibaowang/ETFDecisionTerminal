# TASK-240 AccountingEngine adequacy review implementation test plan

## Test Objective

TASK-240 verifies the ShellAccounting manual entry replay AccountingEngine adequacy review implementation gate. The gate is test-only, deterministic, no-write, and static. It implements the review gate only; it does not implement AccountingEngine bridge behavior, does not call AccountingEngine replay, and does not modify AccountingEngine code.

## Static Checks

- docs/276 exists and contains Scope, Preconditions, Review Inputs, Static Adequacy Matrix, Explicit Non-Goals, Forbidden Side Effects, Formal Conclusion, Go/No-Go Decision, and Next Phase Handoff sections.
- docs/277 exists and contains Test Objective, Static Checks, Changed-Path Allowlist, Regression Gates, Failure Modes, Acceptance Criteria, and Formal Conclusion sections.
- README.md references TASK-240 and docs/276 / docs/277.
- docs/README.md references docs/276 / docs/277.
- docs/12_codex_prompt_template.md references TASK-240 and docs/276 / docs/277.
- tests/CMakeLists.txt registers `ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewImplementationGate`.
- TASK-240 CMakeLists registers `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation`.
- TASK-239 authorization documents remain present.
- TASK-233 through TASK-238 replay implementation documents remain present.
- TASK-233 through TASK-239 critical CTests remain registered.
- TASK-240 gate prints each check result and enforces at least 40 checks.
- The future runtime review directory is not created.
- The future AccountingEngine bridge directory is not created.
- `libs/AccountingEngine/` has no changed paths.
- Fixture JSON has no changed paths.
- Production code has no changed paths.
- The gate does not recursively execute full CTest.

## Changed-Path Allowlist

The gate must collect changed paths from unstaged diffs, staged diffs, untracked files, and `git status --porcelain=v1 -uall`.

Clean main / post-merge behavior:

- Changed paths may be empty.
- The gate must not require current TASK-240 files to appear in changed paths.
- All document existence, index, CTest registration, fixture, production no-drift, no-replay, no-write, and no-broker checks still run.

Development branch behavior:

- If changed paths exist, every path must be exact allowlisted.
- The allowlist must not include directory-level `docs/`, `tests/`, `apps/`, `libs/`, or `migrations/` entries.
- Unauthorized changed paths fail closed.
- Allowed TASK-240 paths are docs/276, docs/277, README.md, docs/README.md, docs/12, tests/CMakeLists.txt, and the TASK-240 gate CMake / script files.
- Old gate self-consistency fixes are only allowed when separately authorized as exact paths.

## Regression Gates

Required external targeted CTest probes:

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

Each targeted probe must first run `ctest -N -R` and confirm at least one matching test. Full CTest and `git diff --check` remain external verification steps, not recursive gate behavior.

## Failure Modes

- Missing TASK-240 docs fails.
- Missing required section fails.
- Missing index reference fails.
- Missing CTest registration fails.
- Zero targeted CTest matches during external validation fails the workflow.
- Unauthorized changed path fails.
- Directory-level allowlist entry fails.
- Fixture JSON change fails.
- `apps/`, `libs/`, `migrations/`, or `libs/AccountingEngine/` change fails.
- AccountingEngine replay call token in TASK-240 gate fails.
- Runtime SQL / SQLite write token in TASK-240 gate fails.
- Audit / ledger / snapshot write token in TASK-240 gate fails.
- Read model refresh token in TASK-240 gate fails.
- Broker / network / credentials / endpoint / order placement token in TASK-240 gate fails.
- Recursive full CTest invocation inside TASK-240 gate fails.
- Gate check count below 40 fails.

## Acceptance Criteria

- TASK-240 implementation gate passes.
- TASK-239 authorization gate passes.
- TASK-238 phase closeout gate passes.
- TASK-237 through TASK-233 replay implementation gates pass.
- TASK-232 and TASK-231 dry-run harness gates pass.
- `shell_diagnostics_facade` passes.
- `transport_local_socket_echo` passes 50 times.
- External full CTest passes.
- `git diff --check` passes.
- Final git status is clean after commit and push.

## Formal Conclusion

TASK-240 implements the AccountingEngine adequacy review implementation gate only.

Future AccountingEngine bridge, runtime integration, replay implementation, read model refresh, production parser / loader / reader work, or AccountingEngine replay calls must be separately authorized by a later TASK.
