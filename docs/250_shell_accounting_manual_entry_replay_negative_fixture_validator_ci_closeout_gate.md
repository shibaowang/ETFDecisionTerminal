# ShellAccounting Manual Entry Replay Negative Fixture Validator CI Closeout Gate

## Purpose

TASK-227 is a CI closeout gate for TASK-220 through TASK-226. It closes the
manual entry replay negative fixture validator phase by proving the critical
docs, gates, CTest registrations, clean-main behavior, fixture immutability,
validator boundaries, and production no-drift policy are all statically
verifiable.

TASK-227 does not modify validator validation behavior. TASK-227 only allows
exact-path changed-set self-consistency updates to the TASK-224 test-only
validator. TASK-227 does not modify fixture JSON. TASK-227 does not implement
parser / loader / reader / replay. TASK-227 does not call AccountingEngine
replay.

## Scope

TASK-227 adds a static CTest:

```text
shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout
```

The gate checks registration and closeout invariants for TASK-220 through
TASK-226. It may read docs, CMake files, gate scripts, and fixture metadata.
It does not execute replay, mutate fixtures, or call validator behavior beyond
registration and static file probes.

## Required CTest Set

The closeout gate requires the following CTest names to be registered:

```text
shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout
shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening
shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix
shell_accounting_manual_entry_replay_negative_fixture_static_validator
shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization
shell_accounting_manual_entry_replay_negative_fixture_scaffold_files
shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization
shell_accounting_manual_entry_replay_fixture_negative_fixtures_authorization
shell_accounting_manual_entry_replay_fixture_static_validator
shell_accounting_manual_entry_replay_fixture_static_validator_authorization
shell_diagnostics_facade
```

## Clean-Main Invariants

Clean main is allowed to have an empty changed-path set. If changed paths are
present on a development branch, the gate must require an exact allowlist and
fail closed on any unauthorized path.

The closeout gate also verifies that critical gates no longer contain known
clean-main-unsafe assertions such as requiring current task files to appear in
changed paths or using changed paths to prove a test-only parameter extension.

## No Recursive Full CTest Boundary

The closeout gate must stay static. It may use `ctest -N` to inspect CTest
registration, but it must not recursively run full CTest from inside the gate.
Full CTest remains an external verification command.

## CI Closeout Commands

The external closeout command set is:

```powershell
cmake -S . -B build -DETFDT_QT6_ROOT=C:\Qt\6.9.3\msvc2022_64
cmake --build build
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_negative_fixture_validator_ci_closeout$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_negative_fixture_static_validator_failure_mode_hardening$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_negative_fixture_static_validator_regression_matrix$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_negative_fixture_static_validator$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_negative_fixture_static_validator_authorization$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_negative_fixture_scaffold_files$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_negative_fixtures_scaffold_authorization$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_fixture_negative_fixtures_authorization$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_manual_entry_replay_fixture_static_validator$" --output-on-failure
ctest --test-dir build -R "^shell_diagnostics_facade$" --output-on-failure
ctest --test-dir build -R "^shell_accounting_broker_adapter_disabled" --output-on-failure
ctest --test-dir build -R "^shell_accounting_broker_order" --output-on-failure
ctest --test-dir build -R "^shell_accounting_real_broker_order_authorization" --output-on-failure
ctest --test-dir build -R "^shell_accounting_real_broker_order_implementation" --output-on-failure
ctest --test-dir build -R "^transport_local_socket_echo$" --repeat until-fail:50 --output-on-failure
ctest --test-dir build --output-on-failure
git diff --check
git status --short
```

## No-Production-Drift Boundary

TASK-227 does not modify production code / QML / DataServiceActions /
repositories / migrations. TASK-227 does not write runtime SQL / SQLite /
audit / ledger / snapshot. Any changed path under `apps/`, `libs/`, or
`migrations/` must fail closed.

## Fixture Immutability Boundary

TASK-227 does not modify fixture JSON. Negative fixture JSON under
`tests/fixtures/manual_entry_replay_negative/` and positive fixture JSON under
`tests/fixtures/manual_entry_replay/` must remain unchanged.

## Replay And AccountingEngine Boundary

TASK-227 does not implement parser / loader / reader / replay. TASK-227 does
not call AccountingEngine replay. The closeout gate only verifies static
registration and boundary invariants.

## Broker And Network Boundary

TASK-227 does not connect broker / network / credentials / endpoint.
TASK-227 does not place real orders or enable automatic trading.

## Formal Conclusion And Next Task

TASK-227 closes the CI-verifiable static gate chain for TASK-220 through
TASK-226. The phase is ready only when the closeout gate, all required
critical gates, transport repeat, and full CTest pass externally.

Recommended next task: TASK-228 manual entry replay negative fixture validator
phase closeout and handoff gate.
