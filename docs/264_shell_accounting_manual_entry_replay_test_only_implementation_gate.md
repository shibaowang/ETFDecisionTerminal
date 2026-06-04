# ShellAccounting Manual Entry Replay Test-Only Implementation Gate

## Purpose

TASK-234 implements a test-only manual entry replay implementation shell. It is
limited to reading the TASK-231 dry-run harness sanitized summary and fixture
metadata, then producing an in-memory sanitized replay implementation report.

TASK-234 does not implement production replay.

## Relationship To TASK-233

TASK-233 authorized the boundary for a future manual entry replay
implementation. TASK-234 consumes that authorization only for a test-only
implementation shell under `tests/`.

TASK-234 keeps the TASK-233 boundaries in force: no production replay, no
runtime SQL / SQLite, no audit / ledger / snapshot writes, no read model
refresh, and no broker or network integration.

## Test-Only Implementation Scope

The implementation is located at
`tests/ShellAccountingManualEntryReplayImplementation/manual_entry_replay_implementation.py`.
It is registered as the CTest
`shell_accounting_manual_entry_replay_implementation`.

The implementation reads only sanitized dry-run harness output and fixture
metadata. It creates a sanitized report in memory and prints it as JSON when
`--summary-json` is requested.

## Input Boundary

Allowed input is the TASK-231 dry-run harness `--summary-json` output. The
implementation may verify fixture index metadata and fixture immutability, but
it must not expose raw fixture payloads in its report.

The implementation must not accept UI raw payloads, production payloads,
runtime database rows, broker payloads, or credentials.

## Output Contract

The output schema is
`manual-entry-replay-test-only-implementation-summary/v1`.

Each entry contains only sanitized fields:

- `fixtureId`
- `source`
- `replayPlanStatus`
- `plannedEventCount`
- `accountingEngineCalled`
- `runtimeWrites`
- `positionCashPnlCalculated`

The report must not contain raw SQL, full fixture payloads, credentials,
endpoints, broker payloads, real order ids, traceback text, or exception stack
details.

## AccountingEngine Boundary

TASK-234 does not call AccountingEngine replay.

The report marks `accountingEngineCalled` as `false` for every entry. Any
future AccountingEngine replay behavior must be separately authorized and must
remain test-only until a later production task explicitly changes that boundary.

## No-Write Boundary

TASK-234 does not write files, runtime SQL, SQLite, audit rows, ledger rows,
snapshot rows, cash facts, trade facts, or read models.

The implementation must not create DB, WAL, SHM, or SQLite artifacts.

## Runtime SQL And Ledger Boundary

TASK-234 does not write runtime SQL / SQLite / audit / ledger / snapshot.

The implementation must not run `INSERT`, `UPDATE`, `DELETE`, `REPLACE`, schema
migrations, or any ledger mutation. It must not refresh read models or persist
derived replay state.

## Production Integration Boundary

TASK-234 does not modify production code / QML / DataServiceActions /
repositories / migrations.

The implementation must not import `apps/` or `libs/` production modules. It is
a CTest-only script and has no production startup, Presenter, Controller,
ShellServices, DataServiceActions, repository, or migration integration.

## Broker And Network Boundary

TASK-234 does not connect broker / network / credentials / endpoint.

TASK-234 does not place real orders or enable automatic trading. It must not
call broker SDKs, network clients, credential stores, endpoints, real order
placement, strategy execution, or automatic trading paths.

## Fixture Immutability Boundary

TASK-234 does not modify fixture JSON.

Positive and negative fixture JSON hashes must remain unchanged before and
after the test-only implementation probe. The implementation may read fixture
metadata only to verify coverage and immutability.

## Formal Conclusion And Next Task

TASK-234 implements a test-only manual entry replay implementation shell.

TASK-234 does not implement production replay. TASK-234 does not call
AccountingEngine replay. TASK-234 does not modify fixture JSON. TASK-234 does
not modify production code / QML / DataServiceActions / repositories /
migrations. TASK-234 does not write runtime SQL / SQLite / audit / ledger /
snapshot. TASK-234 does not refresh read models. TASK-234 does not calculate
real position / cash / PnL. TASK-234 does not connect broker / network /
credentials / endpoint. TASK-234 does not place real orders or enable automatic
trading.

Recommended next task: TASK-235 manual entry replay test-only implementation
regression matrix gate.
