# ShellAccounting Manual Entry Replay Test-Only Implementation Regression Matrix Gate

## Purpose

TASK-235 adds a regression matrix gate for the TASK-234 test-only manual entry
replay implementation. The gate invokes the TASK-234 implementation with
`--source-root <root> --summary-json`, validates the sanitized implementation
summary, and compares its fixture coverage with the TASK-231 dry-run harness
summary.

TASK-235 does not modify fixture JSON.

## Relationship To TASK-234

TASK-234 created the test-only replay implementation shell under
`tests/ShellAccountingManualEntryReplayImplementation`. TASK-235 does not
change that implementation business semantics. It adds a separate regression
matrix gate that proves the TASK-234 output contract remains stable and safe.

TASK-235 does not modify dry-run harness business semantics. TASK-235 does not
modify validator validation behavior.

## Matrix Scope

The TASK-235 matrix covers the sanitized implementation summary schema,
implementation mode, implementation status, fixture count, entry shape,
fixture id coverage, and no-side-effect flags.

The matrix is registered as the CTest
`shell_accounting_manual_entry_replay_implementation_regression_matrix`.

## Input Coverage

The only input source is the TASK-234 implementation `--summary-json` output
and the TASK-231 dry-run harness `--summary-json` output used for fixture id
comparison.

The matrix verifies that implementation entries cover the dry-run summary
fixture ids and that fixture JSON hashes remain unchanged before and after the
probe.

## Output Contract

The TASK-234 implementation summary must use schema
`manual-entry-replay-test-only-implementation-summary/v1`.

The summary must report `implementationMode` as `test-only-in-memory` and
`implementationStatus` as `ok`. `fixtureCount` must equal the number of
implementation entries. Each entry must contain only sanitized replay planning
fields and must not expose raw fixture payloads.

## AccountingEngine Boundary

TASK-235 does not call AccountingEngine replay.

The matrix requires every implementation entry to report
`accountingEngineCalled=false`. Future AccountingEngine replay behavior must be
separately authorized.

## No-Write Boundary

TASK-235 does not write runtime SQL / SQLite / audit / ledger / snapshot.

The matrix verifies that the implementation does not create DB, WAL, SHM, or
SQLite artifacts and that fixture JSON hashes remain stable.

## Runtime SQL And Ledger Boundary

TASK-235 does not run SQL and does not write audit, ledger, snapshot, cash fact,
or trade fact rows.

The matrix rejects summaries that contain raw SQL, stack traces, credentials,
broker payloads, real order ids, or production path disclosure.

## Production Integration Boundary

TASK-235 does not implement production parser / loader / reader. TASK-235 does
not implement production replay.

TASK-235 does not modify production code / QML / DataServiceActions /
repositories / migrations. The regression gate is a test-only CTest script and
does not add startup, Presenter, Controller, ShellServices, DataServiceActions,
repository, migration, or QML behavior.

## Fixture Immutability Boundary

TASK-235 does not modify fixture JSON.

Positive and negative manual entry replay fixture hashes must remain unchanged
before and after the implementation summary probe. Changed-path checks must
support clean main with an empty changed set and fail closed when non-allowlisted
paths are present.

## Formal Conclusion And Next Task

TASK-235 adds a regression matrix gate for the TASK-234 test-only
implementation.

TASK-235 does not modify fixture JSON. TASK-235 does not modify implementation
business semantics. TASK-235 does not modify dry-run harness business
semantics. TASK-235 does not modify validator validation behavior. TASK-235
does not implement production parser / loader / reader. TASK-235 does not
implement production replay. TASK-235 does not call AccountingEngine replay.
TASK-235 does not modify production code / QML / DataServiceActions /
repositories / migrations. TASK-235 does not write runtime SQL / SQLite /
audit / ledger / snapshot. TASK-235 does not refresh read models. TASK-235
does not calculate real position / cash / PnL. TASK-235 does not connect broker
/ network / credentials / endpoint. TASK-235 does not place real orders or
enable automatic trading.

Recommended next task: TASK-236 manual entry replay test-only implementation
failure-mode hardening gate.
