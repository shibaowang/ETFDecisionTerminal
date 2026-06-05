# TASK-244 AccountingEngine adequacy review phase closeout and handoff test plan

## Test Objective

Verify that TASK-244 closes the AccountingEngine adequacy review phase and records a safe handoff to the next authorization task without implementing a bridge, calling replay, modifying AccountingEngine code, writing runtime state, refreshing read models, or introducing broker/network behavior.

## Required Static Checks

The TASK-244 gate must check:

- docs/284 exists.
- docs/285 exists.
- README references TASK-244 and docs/284 / docs/285.
- docs/README references docs/284 / docs/285.
- docs/12 references TASK-244 and docs/284 / docs/285.
- tests/CMakeLists registers `ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate`.
- TASK-244 CMake registers `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout`.
- docs/284 contains all required phase closeout sections.
- docs/285 contains all required test plan sections.
- Formal conclusion lines are present.
- TASK-239 through TASK-243 docs and gate scripts remain present.
- critical CTest names remain registered.
- clean main may have an empty changed-path set.
- nonempty changed paths are exact allowlisted.
- unauthorized changed paths fail closed.
- no fixture JSON is changed.
- no production code is changed.
- no `libs/AccountingEngine/` path is changed.
- no recursive full CTest is invoked inside the gate.
- no AccountingEngine replay call is introduced.
- no bridge implementation is introduced.
- no runtime SQL / SQLite / audit / ledger / snapshot write is introduced.
- no read model refresh is introduced.
- no broker, network, credentials, endpoint, real order placement, or automatic trading drift is introduced.

## Required Regression Gates

Each targeted CTest pattern must be probed with `ctest -N -R` before execution.

Required CTests:

- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout`
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
- `transport_local_socket_echo`

Full CTest must pass after targeted tests pass.

## Changed-Path Allowlist

Clean main may have an empty changed-path set.

When changed paths are nonempty, TASK-244 allows only these exact paths unless a later explicit supplemental authorization adds more exact paths:

- `README.md`
- `docs/README.md`
- `docs/12_codex_prompt_template.md`
- `docs/284_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_gate.md`
- `docs/285_shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout_handoff_test_plan.md`
- `tests/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineAdequacyReviewPhaseCloseoutGate/manual_entry_replay_accountingengine_adequacy_review_phase_closeout_gate.py`

The allowlist must not use directory-level `docs/`, `tests/`, `apps/`, `libs/`, or `migrations/` entries.

The gate must not use directory-level `docs/`, `tests/`, `apps/`, `libs/`, or `migrations/` entries.

## Phase Closeout Evidence

Phase closeout evidence is documentation and test-only gate evidence only. It is not runtime evidence, replay evidence, ledger evidence, audit evidence, read model refresh evidence, broker evidence, or order placement evidence.

The phase closeout gate must produce at least 120 meaningful check lines and print every check result.

## Handoff Checks

The gate must verify that docs/284 and docs/285 state the recommended next task:

`Recommended next task: TASK-245 ShellAccounting manual entry replay AccountingEngine bridge authorization gate.`

The next task must explicitly authorize bridge behavior before any bridge, replay invocation, runtime read/write, or read model refresh can be introduced.

## Failure Modes

The gate must fail for:

- missing docs/284.
- missing docs/285.
- missing index references.
- missing TASK-244 CTest registration.
- missing prior TASK-239 through TASK-243 docs or gates.
- missing critical CTest registrations.
- unauthorized changed paths.
- production code drift.
- fixture JSON drift.
- `libs/AccountingEngine/` drift.
- replay invocation.
- bridge implementation.
- runtime SQL / SQLite / audit / ledger / snapshot write.
- read model refresh.
- broker, network, credentials, endpoint, real order placement, or automatic trading drift.
- recursive full CTest invocation inside the gate.
- a gate that depends on nonempty changed paths.

## Acceptance Criteria

Acceptance requires:

- configure succeeds.
- build succeeds.
- TASK-244 phase closeout gate passes.
- TASK-243 CI closeout gate passes.
- TASK-242 failure-mode hardening gate passes.
- TASK-241 regression matrix gate passes.
- TASK-240 implementation gate passes.
- TASK-239 authorization gate passes.
- broader replay implementation regression gates pass.
- `shell_diagnostics_facade` passes.
- `transport_local_socket_echo` passes 50 repeated runs.
- full CTest passes.
- `git diff --check` passes.
- final `git status --short` is clean after commit and push.

## Formal Conclusion

TASK-244 closes out the AccountingEngine adequacy review phase only.

TASK-244 does not call AccountingEngine replay.

TASK-244 does not implement AccountingEngine bridge.

TASK-244 does not implement replay.

TASK-244 does not modify AccountingEngine code.

TASK-244 does not authorize runtime SQL / SQLite read/write.

TASK-244 does not authorize audit / ledger / snapshot writes.

TASK-244 does not authorize read model refresh.

TASK-244 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-245 ShellAccounting manual entry replay AccountingEngine bridge authorization gate.
