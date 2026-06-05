# ShellAccounting Manual Entry Replay Test-Only Implementation Failure-Mode Hardening Gate

## Purpose

TASK-236 hardens failure-mode behavior for the TASK-234 test-only implementation.
The gate proves that bad dry-run summary inputs fail closed, return a non-zero
exit code, and emit sanitized failure JSON.

TASK-236 uses temporary bad dry-run summaries only.

## Relationship To TASK-234 And TASK-235

TASK-234 added the test-only manual entry replay implementation summary. TASK-235
added a regression matrix for the successful implementation summary contract.
TASK-236 covers failure behavior without changing the successful TASK-234
implementation summary semantics.

TASK-236 does not modify implementation success semantics. TASK-236 does not
modify dry-run harness business semantics. TASK-236 does not modify validator
validation behavior.

## Failure-Mode Scope

The TASK-236 gate generates invalid or unsafe dry-run summary inputs in a test
temporary directory. It verifies failures for invalid JSON, wrong schema,
non-ok dry-run status, malformed entries, duplicate fixture ids, negative
planned step counts, replay flags, runtime write flags, raw SQL tokens,
credential tokens, endpoint tokens, broker payload tokens, real order tokens,
production path references, DB artifact references, and stack trace tokens.

The CTest is
`shell_accounting_manual_entry_replay_implementation_failure_mode_hardening`.

## Temporary Input Boundary

The only bad inputs are temporary dry-run summary files created by the gate. They
are not committed fixture JSON files and are not production inputs.

TASK-236 does not modify fixture JSON.

## Sanitized Failure Output Contract

Failure output must use schema
`manual-entry-replay-test-only-implementation-failure/v1`.

Failure output must not expose raw fixture payloads, credentials, endpoint
values, broker payloads, real order ids, raw SQL, stack traces, production paths,
or complete dry-run summary payloads.

## AccountingEngine Boundary

TASK-236 does not call AccountingEngine replay.

Bad dry-run summary inputs must fail before replay can be considered. Future
AccountingEngine replay integration must be separately authorized.

## No-Write Boundary

TASK-236 does not write runtime SQL / SQLite / audit / ledger / snapshot.

The gate checks fixture hashes before and after failure probes and rejects DB,
WAL, SHM, and SQLite artifacts.

## Runtime SQL And Ledger Boundary

TASK-236 does not read runtime DB state, write runtime SQL, write audit rows,
write ledger rows, or create snapshots.

Raw SQL tokens in bad input are used only inside temporary files and must never
appear in implementation failure output.

## Production Integration Boundary

TASK-236 does not implement production parser / loader / reader. TASK-236 does
not implement production replay.

TASK-236 does not modify production code / QML / DataServiceActions /
repositories / migrations.

## Fixture Immutability Boundary

TASK-236 does not modify fixture JSON.

The failure-mode gate must support clean main with an empty changed set. When
changed paths exist, they must be exact-file allowlisted and fail closed for
fixture, production, migration, SQL, and JSON paths.

## Formal Conclusion And Next Task

TASK-236 adds a failure-mode hardening gate for the TASK-234 test-only
implementation.

TASK-236 does not modify fixture JSON. TASK-236 does not modify implementation
success semantics. TASK-236 does not modify dry-run harness business semantics.
TASK-236 does not modify validator validation behavior. TASK-236 does not
implement production parser / loader / reader. TASK-236 does not implement
production replay. TASK-236 does not call AccountingEngine replay. TASK-236
does not modify production code / QML / DataServiceActions / repositories /
migrations. TASK-236 does not write runtime SQL / SQLite / audit / ledger /
snapshot. TASK-236 does not refresh read models. TASK-236 does not calculate
real position / cash / PnL. TASK-236 does not connect broker / network /
credentials / endpoint. TASK-236 does not place real orders or enable automatic
trading.

Recommended next task: TASK-237 manual entry replay test-only implementation CI
closeout gate.
