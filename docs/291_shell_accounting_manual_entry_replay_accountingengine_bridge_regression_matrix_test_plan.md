# TASK-247 AccountingEngine bridge regression matrix test plan

## Test Objective

Verify that the TASK-246 test-only AccountingEngine bridge contract remains deterministic, sanitized, no-write, no-replay, and isolated from production code.

## Required Static Checks

The gate must verify:

- docs/290 and docs/291 exist
- README, docs/README, and docs/12 reference TASK-247 and docs/290 / docs/291
- tests/CMakeLists registers the TASK-247 gate directory
- the TASK-247 CMakeLists registers `shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix`
- TASK-246 bridge script exists
- TASK-246 implementation gate exists
- TASK-245 authorization gate exists
- TASK-239 through TASK-246 docs and gates still exist
- bridge source does not call AccountingEngine replay
- bridge source does not write SQL / SQLite / audit / ledger / snapshot
- bridge source does not access broker / network / credentials / endpoint
- fixture JSON remains unchanged
- apps, libs, migrations, and `libs/AccountingEngine/` remain unchanged

## Required Regression Gates

The validation run must include:

- `shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix`
- `shell_accounting_manual_entry_replay_accountingengine_bridge_implementation`
- `shell_accounting_manual_entry_replay_accountingengine_bridge_authorization`
- TASK-244 / TASK-243 / TASK-242 / TASK-241 / TASK-240 / TASK-239 gates
- replay implementation and dry-run harness regression gates
- `shell_diagnostics_facade`
- `transport_local_socket_echo`
- full CTest

## Changed-Path Allowlist

The gate must inspect staged, unstaged, and untracked paths.

Allowed TASK-247 paths are:

- `README.md`
- `docs/README.md`
- `docs/12_codex_prompt_template.md`
- `docs/290_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_gate.md`
- `docs/291_shell_accounting_manual_entry_replay_accountingengine_bridge_regression_matrix_test_plan.md`
- `tests/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgeRegressionMatrixGate/manual_entry_replay_accountingengine_bridge_regression_matrix_gate.py`

If self-consistency changes are authorized, each old gate script path must be added exactly and individually. Directory-level allowlists are forbidden.

Clean main may have an empty changed-path set.

## Bridge Matrix Cases

Positive matrix cases must verify:

- stdout summary output
- file summary output
- `--expect-no-accountingengine-call`
- `schemaVersion=1`
- `task=TASK-246`
- `mode=test-only-accountingengine-bridge-contract`
- `accountingEngineCalled=false`
- `replayExecuted=false`
- `runtimeWrite=false`
- `bridgeRequest.syntheticDataOnly=true`
- `bridgeResponse.status=not_executed`

## Failure Modes

Failure cases must include:

- missing input summary
- malformed JSON
- missing required fields
- non-synthetic flag
- `replayExecuted=true`
- `accountingEngineCalled=true`
- `runtimeWrite=true`

Every failure must return sanitized output and must not mutate fixtures, create database files, call replay, or write runtime state.

## Acceptance Criteria

Accept only if:

- the regression matrix CTest passes
- the TASK-246 bridge implementation gate still passes
- prior AccountingEngine adequacy and replay gates still pass
- transport repeat passes 50 times
- full CTest passes
- `git diff --check` passes
- final working tree is clean after commit

## Formal Conclusion

TASK-247 adds a test-only regression matrix gate for the AccountingEngine bridge contract.

TASK-247 does not call AccountingEngine replay.

TASK-247 does not modify AccountingEngine code.

TASK-247 does not implement production bridge.

TASK-247 does not modify fixture JSON.

TASK-247 does not authorize production integration.

TASK-247 does not authorize runtime SQL / SQLite read/write.

TASK-247 does not authorize audit / ledger / snapshot writes.

TASK-247 does not authorize read model refresh.

TASK-247 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-248 ShellAccounting manual entry replay AccountingEngine bridge failure-mode hardening gate.
