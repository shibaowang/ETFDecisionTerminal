# ShellAccounting Manual Entry Replay Test-Only Implementation CI Closeout Gate

## Purpose

TASK-237 is a CI closeout gate for the TASK-233 through TASK-236 test-only implementation chain.

The purpose is to make the manual entry replay test-only implementation chain verifiable by a static CTest gate before the next phase starts. The gate checks registration, required artifacts, clean-main safety, fixture immutability, and no-drift boundaries for the TASK-233 authorization gate, TASK-234 implementation, TASK-235 regression matrix, and TASK-236 failure-mode hardening gate.

## Scope

TASK-237 adds only a CI closeout gate and its documentation. It does not add replay capability, does not change the TASK-234 implementation success path, and does not change the dry-run harness or validator business behavior.

The closeout gate may inspect docs, CMake files, scripts, fixture metadata, and `ctest -N` output. It must not execute full CTest from inside the gate.

## Required CTest Set

The closeout command set includes:

- `shell_accounting_manual_entry_replay_implementation_ci_closeout`
- `shell_accounting_manual_entry_replay_implementation_failure_mode_hardening`
- `shell_accounting_manual_entry_replay_implementation_regression_matrix`
- `shell_accounting_manual_entry_replay_implementation`
- `shell_accounting_manual_entry_replay_implementation_authorization`
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix`
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness`
- `shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization`
- `shell_accounting_manual_entry_replay_next_phase_authorization_planning`
- `shell_accounting_manual_entry_replay_negative_fixture_static_validator`
- `shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix`
- `shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening`
- `shell_diagnostics_facade`

The full CTest suite remains an external verification step, not a recursive action performed by the closeout gate.

## Clean-Main Invariants

TASK-237 must support clean main where changed paths are empty. It must not rely on a currently dirty branch to prove that TASK files exist or that test-only parameters were added.

The closeout gate must reject known clean-main brittle assertions, including assertions that changed path detection must include current TASK files, test-only parameter extension changed, must include current TASK-, or implementation extension changed.

If changed paths exist during development, the gate must fail closed unless every path is explicitly allowlisted as an exact file path.

## No Recursive Full CTest Boundary

The closeout gate may use `ctest -N` to inspect registration. It must not invoke full `ctest --test-dir build --output-on-failure` from inside the gate.

Full CTest must be run externally by the task validation workflow.

## CI Closeout Commands

Required validation commands are:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_implementation_ci_closeout$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_implementation_failure_mode_hardening$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_implementation_regression_matrix$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_implementation$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_implementation_authorization$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_test_only_dry_run_harness$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_next_phase_authorization_planning$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_negative_fixture_static_validator$" --output-on-failure
ctest --test-dir build -R "^shell_diagnostics_facade$" --output-on-failure
ctest --test-dir build -R "^transport_local_socket_echo$" --repeat until-fail:50 --output-on-failure
ctest --test-dir build --output-on-failure
git diff --check
git status --short
```

## Implementation Behavior Boundary

TASK-237 does not modify implementation success semantics.

The TASK-234 implementation remains test-only. TASK-237 may verify that implementation artifacts exist and remain registered, but it does not authorize production replay, runtime database reads, runtime database writes, real position computation, real cash computation, or PnL computation.

## Dry-Run Harness Boundary

TASK-237 does not modify dry-run harness business semantics.

The TASK-231 harness remains a test-only planning surface. TASK-237 may check that the harness and regression matrix remain present, but it does not authorize harness conversion into production parser, loader, reader, or replay behavior.

## Validator Boundary

TASK-237 does not modify validator validation behavior.

The TASK-224 negative fixture static validator remains a test-only validator. TASK-237 may verify that the validator exists and is registered in the chain, but it does not authorize new fixture parsing, production validation, production loading, production reading, or AccountingEngine replay.

## Fixture Immutability Boundary

TASK-237 does not modify fixture JSON.

Positive fixtures under `tests/fixtures/manual_entry_replay/` and negative fixtures under `tests/fixtures/manual_entry_replay_negative/` must remain unchanged. Fixture JSON changes require a separate authorization task.

## AccountingEngine Boundary

TASK-237 does not implement production replay.

TASK-237 does not call AccountingEngine replay.

The closeout gate only verifies static boundaries and test registration. It must not instantiate or invoke AccountingEngine replay.

## Runtime SQL And Ledger Boundary

TASK-237 does not write runtime SQL / SQLite / audit / ledger / snapshot.

TASK-237 does not refresh read models.

TASK-237 does not calculate real position / cash / PnL.

No runtime write, audit write, ledger write, snapshot write, or read-model refresh is authorized by this closeout.

## Production Integration Boundary

TASK-237 does not modify production code / QML / DataServiceActions / repositories / migrations.

TASK-237 does not implement production parser / loader / reader.

Production integration remains out of scope and must be separately authorized.

## Broker And Network Boundary

TASK-237 does not connect broker / network / credentials / endpoint.

TASK-237 does not place real orders or enable automatic trading.

Broker access, network calls, credentials, endpoints, real order placement, strategy execution, and automatic trading remain forbidden.

## Formal Conclusion And Next Task

TASK-237 closes out the TASK-233 through TASK-236 test-only manual entry replay implementation chain for CI verification only.

Recommended next task: TASK-238 manual entry replay test-only implementation phase closeout and handoff gate.
