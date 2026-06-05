# TASK-245 AccountingEngine bridge authorization test plan

## Test Objective

Verify that TASK-245 records an authorization-only AccountingEngine bridge policy boundary without implementing a bridge, calling AccountingEngine replay, modifying AccountingEngine code, changing fixture JSON, writing runtime state, refreshing read models, or introducing broker/network behavior.

## Required Static Checks

The TASK-245 gate must check:

- docs/286 exists.
- docs/287 exists.
- README references TASK-245 and docs/286 / docs/287.
- docs/README references docs/286 / docs/287.
- docs/12 references TASK-245 and docs/286 / docs/287.
- tests/CMakeLists registers `ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate`.
- TASK-245 CMake registers `shell_accounting_manual_entry_replay_accountingengine_bridge_authorization`.
- docs/286 contains all required authorization sections.
- docs/287 contains all required test plan sections.
- Formal conclusion lines are present.
- TASK-239 through TASK-244 docs and gate scripts remain present.
- critical CTest names remain registered.
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridge/` does not exist.
- clean main may have an empty changed-path set.
- nonempty changed paths are exact allowlisted.
- unauthorized changed paths fail closed.
- no fixture JSON is changed.
- no production code is changed.
- no `libs/AccountingEngine/` path is changed.
- no recursive full CTest is invoked inside the gate.
- no AccountingEngine replay call is introduced.
- no AccountingEngine bridge implementation is introduced.
- no runtime SQL / SQLite / audit / ledger / snapshot write is introduced.
- no read model refresh is introduced.
- no broker, network, credentials, endpoint, real order placement, or automatic trading drift is introduced.

## Required Regression Gates

Each targeted CTest pattern must be probed with `ctest -N -R` before execution.

Required CTests:

- `shell_accounting_manual_entry_replay_accountingengine_bridge_authorization`
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

When changed paths are nonempty, TASK-245 allows only these exact paths unless a later explicit supplemental authorization adds more exact paths:

- `README.md`
- `docs/README.md`
- `docs/12_codex_prompt_template.md`
- `docs/286_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_gate.md`
- `docs/287_shell_accounting_manual_entry_replay_accountingengine_bridge_authorization_test_plan.md`
- `tests/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgeAuthorizationGate/manual_entry_replay_accountingengine_bridge_authorization_gate.py`

The allowlist must not use directory-level `docs/`, `tests/`, `apps/`, `libs/`, or `migrations/` entries.

## Authorization Checks

The gate must prove TASK-245 authorizes bridge policy only.

The gate must prove TASK-245 does not authorize bridge implementation, AccountingEngine replay calls, replay implementation, runtime integration, production integration, runtime SQL / SQLite read/write, audit / ledger / snapshot writes, read model refresh, broker, network, credentials, endpoint, real order placement, or automatic trading.

## Future Bridge Contract Checks

The gate must verify docs/286 names the future bridge directory candidate:

```text
tests/ShellAccountingManualEntryReplayAccountingEngineBridge/
```

The gate must verify that the future directory does not exist in TASK-245.

The gate must verify future input and output contract text exists, while keeping bridge implementation absent.

## Failure Modes

The gate must fail for:

- missing docs/286.
- missing docs/287.
- missing index references.
- missing TASK-245 CTest registration.
- missing prior TASK-239 through TASK-244 docs or gates.
- missing critical CTest registrations.
- unauthorized changed paths.
- creation of `tests/ShellAccountingManualEntryReplayAccountingEngineBridge/`.
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
- TASK-245 bridge authorization gate passes.
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

TASK-245 authorizes AccountingEngine bridge policy only.

TASK-245 does not implement AccountingEngine bridge.

TASK-245 does not call AccountingEngine replay.

TASK-245 does not implement replay.

TASK-245 does not modify AccountingEngine code.

TASK-245 does not modify fixture JSON.

TASK-245 does not authorize production integration.

TASK-245 does not authorize runtime SQL / SQLite read/write.

TASK-245 does not authorize audit / ledger / snapshot writes.

TASK-245 does not authorize read model refresh.

TASK-245 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-246 ShellAccounting manual entry replay AccountingEngine bridge test-only implementation gate.
