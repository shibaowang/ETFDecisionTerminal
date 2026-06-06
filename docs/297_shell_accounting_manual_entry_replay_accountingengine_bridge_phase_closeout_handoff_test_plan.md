# TASK-250 AccountingEngine bridge phase closeout and handoff test plan

## Test Objective

Verify that the TASK-245 through TASK-249 test-only AccountingEngine bridge
phase is complete, documented, registered, deterministic, no-write, no-replay,
no-production, and ready for a separately authorized runtime integration gate.

## Required Static Checks

The TASK-250 gate must verify docs/296 and docs/297, README indexing,
docs/README indexing, docs/12 prompt guidance, tests/CMakeLists registration,
TASK-250 CMake registration, and the existence of all TASK-245 through TASK-249
bridge docs and gates.

Static source scans must verify that bridge and gate source do not import or
call `libs/AccountingEngine/`, `replayReadOnly`, runtime SQL / SQLite,
audit / ledger / snapshot writes, read model refresh, broker, network,
credentials, endpoint, real order placement, or automatic trading.

## Required Regression Gates

Required CTest names:

- `shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_failure_mode_hardening`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_implementation`.
- `shell_accounting_manual_entry_replay_accountingengine_bridge_authorization`.
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_phase_closeout`.
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_ci_closeout`.
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_failure_mode_hardening`.
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_regression_matrix`.
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_implementation`.
- `shell_accounting_manual_entry_replay_accountingengine_adequacy_review_authorization`.
- `shell_accounting_manual_entry_replay_implementation_phase_closeout`.
- `shell_accounting_manual_entry_replay_implementation_ci_closeout`.
- `shell_accounting_manual_entry_replay_implementation_failure_mode_hardening`.
- `shell_accounting_manual_entry_replay_implementation_regression_matrix`.
- `shell_accounting_manual_entry_replay_implementation`.
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix`.
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness`.
- `shell_diagnostics_facade`.
- `transport_local_socket_echo`.

Every required `ctest -R` validation outside the gate must be preceded by
`ctest -N -R` and must match at least one test.

## Changed-Path Allowlist

Allowed TASK-250 changed paths are exact paths only:

- `README.md`.
- `docs/README.md`.
- `docs/12_codex_prompt_template.md`.
- `docs/296_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_gate.md`.
- `docs/297_shell_accounting_manual_entry_replay_accountingengine_bridge_phase_closeout_handoff_test_plan.md`.
- `tests/CMakeLists.txt`.
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/CMakeLists.txt`.
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgePhaseCloseoutGate/manual_entry_replay_accountingengine_bridge_phase_closeout_gate.py`.

If older gates require exact-path self-consistency updates, only the exact old
gate script paths actually modified by authorization may be added. Directory
wildcards are not allowed.

## Phase Closeout Evidence

The gate must verify that TASK-245 through TASK-249 bridge docs, CMake files,
and scripts remain present.

The gate must verify the TASK-246 bridge stdout contract and TASK-248
failure-mode issue codes without writing repository files.

## Handoff Checks

The gate must verify that docs/296 and docs/297 state that runtime integration
requires a separate TASK-251 authorization gate.

The gate must verify that the next task does not inherit authorization for
production bridge, replay execution, runtime SQL / SQLite, audit / ledger /
snapshot writes, read model refresh, broker access, network access,
credentials, endpoint access, real order placement, or automatic trading.

## Failure Modes

The gate must fail if:

- docs/296 or docs/297 are missing or not indexed.
- TASK-245 through TASK-249 bridge docs or gates are missing.
- Required CTest names are missing.
- Bridge stdout no longer reports `task=TASK-246`.
- Bridge stdout no longer reports `mode=test-only-accountingengine-bridge-contract`.
- Bridge stdout no longer reports `accountingEngineCalled=false`.
- Bridge stdout no longer reports `replayExecuted=false`.
- Bridge stdout no longer reports `runtimeWrite=false`.
- Stable TASK-248 bridge failure issue codes are missing.
- `libs/AccountingEngine/`, production bridge, replay, runtime write, read
  model refresh, fixture JSON, broker, network, credentials, endpoint, real
  order placement, or automatic trading boundaries are crossed.

## Acceptance Criteria

The TASK-250 gate must produce at least 180 meaningful checks and print each
check result.

The gate must be deterministic, test-only, no-write, and must not recursively
run full CTest.

Targeted CTests, full CTest, transport 50-run repeat, `git diff --check`, and
final clean status must pass before commit and push.

## Formal Conclusion

TASK-250 closes out the test-only AccountingEngine bridge phase.

TASK-250 does not call AccountingEngine replay.

TASK-250 does not modify AccountingEngine code.

TASK-250 does not implement production bridge.

TASK-250 does not implement runtime integration.

TASK-250 does not modify fixture JSON.

TASK-250 does not authorize production integration.

TASK-250 does not authorize runtime SQL / SQLite read/write.

TASK-250 does not authorize audit / ledger / snapshot writes.

TASK-250 does not authorize read model refresh.

TASK-250 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-251 ShellAccounting manual entry replay AccountingEngine runtime integration authorization gate.
