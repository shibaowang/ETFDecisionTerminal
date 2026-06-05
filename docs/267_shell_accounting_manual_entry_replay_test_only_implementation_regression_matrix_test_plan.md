# ShellAccounting Manual Entry Replay Test-Only Implementation Regression Matrix Test Plan

## Document Purpose

This document defines the TASK-235 test plan for the ShellAccounting manual
entry replay test-only implementation regression matrix gate. The plan proves
that the TASK-234 implementation summary remains sanitized, test-only,
fixture-complete, immutable, and free of production replay side effects.

## Test Matrix

- TASK-235 regression matrix CTest exists and passes.
- TASK-234 implementation CTest still passes.
- TASK-233 authorization gate still passes.
- TASK-232 dry-run harness regression matrix still passes.
- TASK-231 dry-run harness still passes.
- TASK-230 dry-run harness authorization gate still passes.
- TASK-224 negative fixture static validator still passes.
- Summary JSON uses schema
  `manual-entry-replay-test-only-implementation-summary/v1`.
- Summary `implementationMode` remains `test-only-in-memory`.
- Summary `implementationStatus` remains `ok`.
- `entries` is non-empty.
- `fixtureCount` equals the number of entries.
- Implementation entries cover the dry-run summary fixture ids.
- Every entry reports `accountingEngineCalled=false`.
- Every entry reports `runtimeWrites=false`.
- Every entry reports `positionCashPnlCalculated=false`.
- Planned event counts are non-negative integers.
- Output is sanitized and contains no raw fixture payload, credentials,
  endpoint, broker payload, real order id, raw SQL, stack trace, or production
  path disclosure.
- Fixture JSON remains unchanged.
- The implementation creates no DB / WAL / SHM / SQLite artifact.
- Production code remains unchanged.
- Full CTest and transport echo regression remain green.

## Required Probes

- Run `shell_accounting_manual_entry_replay_implementation_regression_matrix`.
- Run `shell_accounting_manual_entry_replay_implementation`.
- Run `shell_accounting_manual_entry_replay_implementation_authorization`.
- Run `shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix`.
- Run `shell_accounting_manual_entry_replay_test_only_dry_run_harness`.
- Run `shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization`.
- Run `shell_accounting_manual_entry_replay_next_phase_authorization_planning`.
- Run `shell_accounting_manual_entry_replay_negative_fixture_static_validator`.
- Run `shell_diagnostics_facade`.
- Run broker and real-broker static gates.
- Run `transport_local_socket_echo` 50 times.
- Run full CTest.
- Run `git diff --check`.

## Go / No-Go Checklist

Go only if:

- TASK-235 regression matrix CTest passes.
- TASK-234 implementation CTest passes.
- Fixture JSON is unchanged.
- Implementation business semantics are unchanged.
- Dry-run harness business semantics are unchanged.
- Validator validation behavior is unchanged.
- Production code is unchanged.
- No AccountingEngine / no runtime writes / no real position cash PnL is
  observed.
- No broker / network / credentials / endpoint / real order placement /
  automatic trading behavior is added.
- Full CTest passes.
- `git diff --check` passes.

No-Go if the matrix depends on non-empty changed paths, writes files, modifies
fixtures, changes implementation semantics, invokes production replay, calls
AccountingEngine replay, calculates real position / cash / PnL, or connects
broker/network behavior.
