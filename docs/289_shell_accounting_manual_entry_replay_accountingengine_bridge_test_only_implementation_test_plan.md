# TASK-246 AccountingEngine bridge test-only implementation test plan

## Test Objective

Verify that TASK-246 creates a test-only AccountingEngine bridge contract shell that consumes existing test-only manual entry replay summaries and emits sanitized request / response contract JSON without calling AccountingEngine replay, modifying AccountingEngine code, changing fixture JSON, writing runtime state, refreshing read models, or introducing broker/network behavior.

## Required Static Checks

The TASK-246 gate must check:

- docs/288 exists.
- docs/289 exists.
- README references TASK-246 and docs/288 / docs/289.
- docs/README references docs/288 / docs/289.
- docs/12 references TASK-246 and docs/288 / docs/289.
- tests/CMakeLists registers `ShellAccountingManualEntryReplayAccountingEngineBridge`.
- tests/CMakeLists registers `ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate`.
- TASK-246 bridge CMake registers `shell_accounting_manual_entry_replay_accountingengine_bridge_implementation`.
- TASK-246 gate CMake registers the same CTest name.
- docs/288 contains all required implementation sections.
- docs/289 contains all required test plan sections.
- Formal conclusion lines are present.
- TASK-245 docs and gate remain present.
- TASK-239 through TASK-244 docs and gate scripts remain present.
- critical CTest names remain registered.
- changed paths are exact allowlisted.
- clean main may have an empty changed-path set.
- no fixture JSON is changed.
- no production code is changed.
- no `libs/AccountingEngine/` path is changed.
- the bridge script supports `--dry-run-summary-json`, `--summary-json`, and `--expect-no-accountingengine-call`.
- the bridge script does not import AccountingEngine code.
- the bridge script does not call AccountingEngine replay.
- the bridge script does not call `replayReadOnly`.
- the bridge script does not write runtime SQL / SQLite / audit / ledger / snapshot.
- the bridge script does not access broker, network, credentials, or endpoint values.
- the bridge output sets `accountingEngineCalled=false`.
- the bridge output sets `replayExecuted=false`.
- the bridge output sets `runtimeWrite=false`.
- sanitized failure output fails closed.
- the gate does not recursively run full CTest.
- the gate does not depend on nonempty changed paths.

## Required Regression Gates

Each targeted CTest pattern must be probed with `ctest -N -R` before execution.

Required CTests:

- `shell_accounting_manual_entry_replay_accountingengine_bridge_implementation`
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

When changed paths are nonempty, TASK-246 allows only these exact paths unless a later explicit supplemental authorization adds more exact paths:

- `README.md`
- `docs/README.md`
- `docs/12_codex_prompt_template.md`
- `docs/288_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_gate.md`
- `docs/289_shell_accounting_manual_entry_replay_accountingengine_bridge_test_only_implementation_test_plan.md`
- `tests/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridge/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridge/manual_entry_replay_accountingengine_bridge.py`
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/CMakeLists.txt`
- `tests/ShellAccountingManualEntryReplayAccountingEngineBridgeImplementationGate/manual_entry_replay_accountingengine_bridge_implementation_gate.py`

The allowlist must not use directory-level `docs/`, `tests/`, `apps/`, `libs/`, or `migrations/` entries.

## Bridge Contract Checks

The bridge must produce sanitized JSON with:

- `schemaVersion=1`.
- `task=TASK-246`.
- `mode=test-only-accountingengine-bridge-contract`.
- `accountingEngineCalled=false`.
- `replayExecuted=false`.
- `runtimeWrite=false`.
- `bridgeRequest.schemaVersion=1`.
- `bridgeRequest.source=manual-entry-replay-test-only`.
- `bridgeRequest.syntheticDataOnly=true`.
- `bridgeResponse.status=not_executed`.
- `bridgeResponse.reason=AccountingEngine replay is not authorized in TASK-246`.
- `issues=[]`.

The output must not contain raw fixture payloads, raw SQL, credentials, endpoint values, broker payloads, real order ids, internal exception stacks, or production paths.

## Failure Modes

The gate must fail for:

- missing docs/288.
- missing docs/289.
- missing index references.
- missing TASK-246 CTest registration.
- missing bridge script.
- missing bridge CLI flags.
- malformed dry-run summary input.
- malformed implementation summary input.
- unsanitized failure output.
- unauthorized changed paths.
- production code drift.
- fixture JSON drift.
- `libs/AccountingEngine/` drift.
- AccountingEngine replay invocation.
- production bridge implementation.
- runtime SQL / SQLite / audit / ledger / snapshot write.
- read model refresh.
- broker, network, credentials, endpoint, real order placement, or automatic trading drift.
- recursive full CTest invocation inside the gate.
- a gate that depends on nonempty changed paths.

## Acceptance Criteria

Acceptance requires:

- configure succeeds.
- build succeeds.
- TASK-246 bridge implementation gate passes.
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

TASK-246 implements a test-only AccountingEngine bridge contract shell.

TASK-246 does not call AccountingEngine replay.

TASK-246 does not modify AccountingEngine code.

TASK-246 does not implement production bridge.

TASK-246 does not modify fixture JSON.

TASK-246 does not authorize production integration.

TASK-246 does not authorize runtime SQL / SQLite read/write.

TASK-246 does not authorize audit / ledger / snapshot writes.

TASK-246 does not authorize read model refresh.

TASK-246 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

Recommended next task: TASK-247 ShellAccounting manual entry replay AccountingEngine bridge regression matrix gate.
