# ShellAccounting Manual Entry Replay Test-Only Implementation Failure-Mode Hardening Test Plan

## Document Purpose

This document defines the TASK-236 test plan for hardening the TASK-234
test-only manual entry replay implementation failure modes. The test plan proves
that bad temporary dry-run summary inputs fail closed with sanitized failure JSON
and no side effects.

## Test Matrix

- TASK-236 failure-mode hardening CTest exists and passes.
- TASK-235 regression matrix CTest passes.
- TASK-234 implementation CTest passes.
- TASK-233 authorization gate passes.
- TASK-232 dry-run harness regression matrix passes.
- TASK-231 dry-run harness passes.
- TASK-224 negative fixture static validator passes.
- Invalid dry-run summary JSON returns non-zero.
- Wrong dry-run summary schemaVersion returns non-zero.
- Non-ok dryRunStatus returns non-zero.
- Missing entries returns non-zero.
- Non-array entries returns non-zero.
- Duplicate fixtureId returns non-zero.
- Missing fixtureId returns non-zero.
- Missing source returns non-zero.
- Missing dryRunStatus returns non-zero.
- Missing plannedStepCount returns non-zero.
- Negative plannedStepCount returns non-zero.
- replayExecuted=true returns non-zero.
- accountingEngineCalled=true returns non-zero.
- runtimeWrites=true returns non-zero.
- Raw SQL token input returns non-zero and sanitized output.
- Credential token input returns non-zero and sanitized output.
- Endpoint token input returns non-zero and sanitized output.
- Broker payload token input returns non-zero and sanitized output.
- Real order token input returns non-zero and sanitized output.
- Production path reference input returns non-zero and sanitized output.
- DB/WAL/SHM artifact reference input returns non-zero and sanitized output.
- Stack trace token input returns non-zero and sanitized output.
- Fixture JSON is unchanged.
- Implementation success semantics are unchanged.
- Dry-run harness business semantics are unchanged.
- Validator validation behavior is unchanged.
- Production code is unchanged.
- No AccountingEngine / no runtime writes / no real position cash PnL is
  observed.
- Broker and real-broker static gates remain green.

## Required Probes

- Run `shell_accounting_manual_entry_replay_implementation_failure_mode_hardening`.
- Run `shell_accounting_manual_entry_replay_implementation_regression_matrix`.
- Run `shell_accounting_manual_entry_replay_implementation`.
- Run `shell_accounting_manual_entry_replay_implementation_authorization`.
- Run `shell_accounting_manual_entry_replay_test_only_dry_run_harness_regression_matrix`.
- Run `shell_accounting_manual_entry_replay_test_only_dry_run_harness`.
- Run `shell_accounting_manual_entry_replay_next_phase_authorization_planning`.
- Run `shell_accounting_manual_entry_replay_negative_fixture_static_validator`.
- Run `shell_diagnostics_facade`.
- Run broker and real-broker static gates.
- Run `transport_local_socket_echo` 50 times.
- Run full CTest.
- Run `git diff --check`.

## Go / No-Go Checklist

Go only if:

- TASK-236 failure-mode hardening CTest passes.
- TASK-235 regression matrix CTest passes.
- TASK-234 implementation CTest passes.
- Fixture JSON is unchanged.
- Implementation success semantics are unchanged.
- Dry-run harness business semantics are unchanged.
- Validator validation behavior is unchanged.
- Production code is unchanged.
- No AccountingEngine / no runtime writes / no real position cash PnL is
  observed.
- No broker / network / credentials / endpoint / real order placement /
  automatic trading behavior is added.
- Full CTest passes.
- `git diff --check` passes.

No-Go if the hardening gate writes fixture JSON, accepts bad dry-run summaries,
prints raw payloads or stack traces, invokes production replay, calls
AccountingEngine replay, calculates real position / cash / PnL, writes runtime
SQL, or connects broker/network behavior.
