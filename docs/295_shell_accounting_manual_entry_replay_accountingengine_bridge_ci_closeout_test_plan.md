# TASK-249 AccountingEngine bridge CI closeout test plan

## Test Objective

Verify that the TASK-245 through TASK-248 test-only AccountingEngine bridge
phase remains documented, registered, deterministic, no-write, no-replay,
no-production, and safe for CI closeout.

## Required Static Checks

The TASK-249 gate must verify docs/294 and docs/295, README indexing,
docs/README indexing, docs/12 prompt guidance, tests/CMakeLists registration,
TASK-249 CMake registration, and the existence of all TASK-245 through TASK-248
bridge docs and gates.

Static source scans must verify that bridge and gate source do not import or
call `libs/AccountingEngine/`, `replayReadOnly`, runtime SQL / SQLite,
audit / ledger / snapshot writes, read model refresh, broker, network,
credentials, endpoint, real order placement, or automatic trading.

## Required Regression Gates

Required CTest names:

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

Allowed TASK-249 changed paths are exact paths only:

- `README.md`.
- `docs/README.md`.
- `docs/12_codex_prompt_template.md`.
- `docs/294_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_gate.md`.
- `docs/295_shell_accounting_manual_entry_replay_accountingengine_bridge_ci_closeout_test_plan.md`.
- `tests/CMakeLists.txt`.
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/CMakeLists.txt`.
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgeCiCloseoutGate/manual_entry_replay_accountingengine_bridge_ci_closeout_gate.py`.

If older gates require exact-path self-consistency updates, only the exact old
gate script paths actually modified by authorization may be added. Directory
wildcards are not allowed.

## Clean-Main Checks

Clean main may have an empty changed-path set.

The gate must not require current TASK-249 files to be changed. It must rely on
file existence, docs content, CTest registration, bridge stdout contract, and
static source boundaries.

## Failure Modes

The gate must fail if:

- docs/294 or docs/295 are missing or not indexed.
- TASK-245 through TASK-248 bridge docs or gates are missing.
- Required CTest names are missing.
- Bridge stdout no longer reports `task=TASK-246`.
- Bridge stdout no longer reports `mode=test-only-accountingengine-bridge-contract`.
- Bridge stdout no longer reports `accountingEngineCalled=false`.
- Bridge stdout no longer reports `replayExecuted=false`.
- Bridge stdout no longer reports `runtimeWrite=false`.
- Stable TASK-248 bridge failure issue codes are missing.
- `libs/AccountingEngine/`, production bridge, replay, runtime write, fixture
  JSON, broker, network, credentials, endpoint, real order placement, or
  automatic trading boundaries are crossed.

## Acceptance Criteria

The TASK-249 gate must produce at least 180 meaningful checks.

The gate must be deterministic, test-only, no-write, and must not recursively
run full CTest.

Targeted CTests, full CTest, transport 50-run repeat, `git diff --check`, and
final clean status must pass before commit and push.

## Formal Conclusion

TASK-249 adds a CI closeout gate for the test-only AccountingEngine bridge phase.

TASK-249 does not call AccountingEngine replay.

TASK-249 does not modify AccountingEngine code.

TASK-249 does not implement production bridge.

TASK-249 does not implement runtime integration.

TASK-249 does not modify fixture JSON.

TASK-249 does not authorize production integration.

TASK-249 does not authorize runtime SQL / SQLite read/write.

TASK-249 does not authorize audit / ledger / snapshot writes.

TASK-249 does not authorize read model refresh.

TASK-249 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-250 ShellAccounting manual entry replay AccountingEngine bridge phase closeout and handoff gate.
