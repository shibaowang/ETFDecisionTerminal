# TASK-251 AccountingEngine runtime integration authorization test plan

## Test Objective

Verify that TASK-251 records a deterministic, test-only, no-write authorization
gate for future AccountingEngine runtime integration without implementing
runtime integration or calling AccountingEngine replay.

## Required Static Checks

The gate must verify docs/298 and docs/299, README indexing, docs/README
indexing, docs/12 prompt guidance, tests/CMakeLists registration, TASK-251
CMake registration, and the existence of TASK-245 through TASK-250 bridge docs
and gates.

Static checks must prove that TASK-251 does not modify production code,
AccountingEngine code, fixture JSON, migrations, DataServiceActions,
repositories, QML, Presenter, Controller, startup, broker, network,
credentials, or endpoints.

## Required Regression Gates

Required CTest names:

- `shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization`.
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

Every required `ctest -R` validation outside the gate must first run
`ctest -N -R` and must match at least one test.

## Changed-Path Allowlist

Allowed TASK-251 changed paths are exact paths only:

- `README.md`.
- `docs/README.md`.
- `docs/12_codex_prompt_template.md`.
- `docs/298_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_gate.md`.
- `docs/299_shell_accounting_manual_entry_replay_accountingengine_runtime_integration_authorization_test_plan.md`.
- `tests/CMakeLists.txt`.
- `tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/CMakeLists.txt`.
- `tests/ShellAccountingManualEntryReplayAccountingEngineRuntimeIntegrationAuthorizationGate/manual_entry_replay_accountingengine_runtime_integration_authorization_gate.py`.

If older gates require exact-path self-consistency updates, only the exact old
gate script paths actually modified by authorization may be added. Directory
wildcards are not allowed.

## Authorization Checks

The gate must verify that TASK-251 authorizes AccountingEngine runtime
integration policy only.

The gate must verify that TASK-251 does not authorize implementation, runtime
replay, production bridge, runtime SQL / SQLite, audit writes, ledger writes,
snapshot writes, read model refresh, broker access, network access,
credentials, endpoint access, real order placement, or automatic trading.

## Runtime Integration Boundary Checks

The gate must fail if future runtime integration directories, production bridge
directories, AccountingEngine imports, `replayReadOnly`, runtime write tokens,
read model refresh tokens, broker tokens, network tokens, credential tokens, or
endpoint tokens appear in unauthorized changed paths or executable source.

The gate must support clean main after merge, where changed paths are empty.
The gate must not depend on non-empty changed paths.

## Failure Modes

The gate must fail if:

- docs/298 or docs/299 are missing or not indexed.
- TASK-245 through TASK-250 bridge docs or gates are missing.
- Required CTest names are missing.
- Changed paths are not exact allowlisted.
- apps, libs, migrations, AccountingEngine, fixture JSON, DataServiceActions,
  repositories, QML, Presenter, Controller, or startup paths are changed.
- Runtime integration, production bridge, AccountingEngine replay, runtime
  write, audit, ledger, snapshot, read model refresh, broker, network,
  credentials, endpoint, real order placement, or automatic trading boundaries
  are crossed.

## Acceptance Criteria

The TASK-251 gate must produce at least 180 meaningful checks and print each
check result.

The gate must be deterministic, authorization-only, test-only, no-write, and
must not recursively run full CTest.

Targeted CTests, full CTest, transport 50-run repeat, `git diff --check`, and
final clean status must pass before commit and push.

## Formal Conclusion

TASK-251 authorizes AccountingEngine runtime integration policy only.

TASK-251 does not implement runtime integration.

TASK-251 does not call AccountingEngine replay.

TASK-251 does not modify AccountingEngine code.

TASK-251 does not implement production bridge.

TASK-251 does not modify fixture JSON.

TASK-251 does not modify production code.

TASK-251 does not authorize runtime SQL / SQLite read/write.

TASK-251 does not authorize audit / ledger / snapshot writes.

TASK-251 does not authorize read model refresh.

TASK-251 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-252 ShellAccounting manual entry replay AccountingEngine runtime integration implementation authorization gate.
