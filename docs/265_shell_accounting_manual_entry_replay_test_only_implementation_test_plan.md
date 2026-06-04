# ShellAccounting Manual Entry Replay Test-Only Implementation Test Plan

## Document Purpose

This document defines the TASK-234 test plan for the ShellAccounting manual
entry replay test-only implementation shell. The plan proves that the
implementation remains test-only, reads only sanitized dry-run harness output
and fixture metadata, emits a sanitized in-memory report, and preserves all
TASK-233 safety boundaries.

## Test Matrix

- TASK-234 implementation CTest exists and passes.
- TASK-233 authorization gate passes after the implementation path is
  authorized.
- TASK-231 dry-run harness summary JSON remains valid.
- TASK-232 dry-run harness regression matrix remains valid.
- TASK-230 authorization gate remains valid.
- TASK-229 / TASK-228 / TASK-227 / TASK-224 gates remain valid.
- Fixture JSON remains unchanged.
- Dry-run harness business semantics remain unchanged.
- Validator validation behavior remains unchanged.
- Production code remains unchanged.
- No AccountingEngine replay call is made.
- No runtime SQL / SQLite / audit / ledger / snapshot write is made.
- No real position / cash / PnL is calculated.
- No broker / network / credentials / endpoint / real order / automatic trading
  path is connected.

## Required Probes

- Run `shell_accounting_manual_entry_replay_implementation`.
- Run `shell_accounting_manual_entry_replay_implementation_authorization`.
- Run `shell_accounting_manual_entry_replay_test_only_dry_run_harness`.
- Run `shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix`.
- Run `shell_accounting_manual_entry_replay_test_only_dry_run_harness_authorization`.
- Run `shell_accounting_manual_entry_replay_next_phase_authorization_planning`.
- Run `shell_accounting_manual_entry_replay_negative_fixture_static_validator`.
- Run broker and real-broker static gates.
- Run full CTest.
- Run `transport_local_socket_echo` 50 times.
- Run `git diff --check`.

Each probe must preserve fixture immutability, sanitized output, no production
drift, no runtime writes, no AccountingEngine replay, and no broker/network
behavior.

## Go / No-Go Checklist

Go only if:

- TASK-234 implementation CTest passes.
- TASK-233 authorization gate passes.
- Fixture JSON is unchanged.
- Dry-run harness business semantics are unchanged.
- Validator validation behavior is unchanged.
- Production code is unchanged.
- No AccountingEngine / no runtime writes / no real position cash PnL is
  observed.
- No broker / network / credentials / endpoint / real order placement /
  automatic trading behavior is added.
- Full CTest passes.
- `git diff --check` passes.

No-Go if any test-only script writes files, creates DB-like artifacts, calls
AccountingEngine replay, imports production modules, modifies fixture JSON,
calculates real position / cash / PnL, or connects broker/network behavior.
