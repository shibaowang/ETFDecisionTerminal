# TASK-242 ShellAccounting manual entry replay AccountingEngine adequacy review failure-mode hardening gate

## Purpose

TASK-242 hardens the failure-mode behavior for the review-only
ShellAccounting manual entry replay AccountingEngine adequacy gates created by
TASK-240 and TASK-241. The task verifies that bad documentation, missing matrix
content, unauthorized changed paths, and forbidden implementation tokens fail
closed with sanitized output.

## Relationship To TASK-240 And TASK-241

TASK-240 implemented the review gate for AccountingEngine adequacy review.
TASK-241 added the regression matrix for that review-only boundary. TASK-242
does not change the success semantics of those gates. It adds temporary bad
input probes and sanitized failure handling so the existing gates can be
exercised in both normal and intentionally broken states.

## Failure-Mode Scope

The failure-mode hardening scope is test-only. The gate creates temporary
source roots, injects controlled documentation or changed-path defects, runs
the existing review gates, and requires nonzero exit status. The probes cover
missing docs, missing formal conclusion, missing matrix sections, missing
index references, missing CTest registration text, unauthorized changed paths,
and forbidden token injection.

## Temporary Input Boundary

Temporary input roots are disposable git repositories created from copied
test-only and documentation artifacts. TASK-242 does not mutate the real
workspace fixture JSON, production source, migrations, build output, or
runtime database files. Empty changed paths remain valid for clean main.

## Failure Case Matrix

- Missing docs/276.
- Missing docs/277.
- Missing docs/278.
- Missing docs/279.
- docs/276 missing Formal Conclusion.
- docs/278 missing No-Bridge Boundary.
- docs/278 missing No-Write Boundary.
- docs/278 missing Go/No-Go Matrix.
- README missing TASK-240 reference.
- docs/README missing TASK-241 reference.
- tests/CMakeLists missing TASK-240 registration.
- tests/CMakeLists missing TASK-241 registration.
- Changed path includes libs/AccountingEngine.
- Changed path includes apps production path.
- Changed path includes fixture JSON.
- Changed path includes migrations.
- Gate source contains AccountingEngine replay call token.
- Gate source contains replayReadOnly token.
- Gate source contains SQLite write token.
- Gate source contains audit ledger snapshot write token.
- Gate source contains broker/network/order placement token.

## Sanitized Failure Output

The failure probes require sanitized failure summaries. Failure output must not
include stack traces, raw SQL, credentials, endpoint values, broker payloads,
real order text, production paths, library paths, or migration paths.

## No-Bridge Boundary

TASK-242 does not implement AccountingEngine bridge. It does not create a
bridge directory, bridge CTest, bridge adapter, bridge DTO mapper, or bridge
runtime path. Any bridge implementation remains a separately authorized future
TASK.

## No-Replay Boundary

TASK-242 does not call AccountingEngine replay. It does not call replayReadOnly,
AccountingEngine replay APIs, replay ledger APIs, parser, loader, reader, or
production replay integration. It only runs static gate scripts against
temporary documentation and changed-path probes.

## No-Write Boundary

TASK-242 does not authorize runtime SQL / SQLite read/write. It does not
authorize audit / ledger / snapshot writes, read model refresh, runtime
database artifacts, TradeDraft generation, broker order state, or any manual
entry runtime write.

## AccountingEngine Code Boundary

TASK-242 does not modify AccountingEngine code. The `libs/AccountingEngine`
tree must remain unchanged. Any future AccountingEngine integration or replay
bridge must be separately authorized.

## Production Integration Boundary

TASK-242 does not modify production code, QML, startup, Presenter, Controller,
ShellServices adapter, DataServiceActions, repositories, migrations, broker,
network, credentials, endpoint, real order placement, or automatic trading.

## Formal Conclusion

TASK-242 hardens failure-mode behavior for review-only AccountingEngine adequacy gates.
TASK-242 does not call AccountingEngine replay.
TASK-242 does not implement AccountingEngine bridge.
TASK-242 does not implement replay.
TASK-242 does not modify AccountingEngine code.
TASK-242 does not authorize runtime SQL / SQLite read/write.
TASK-242 does not authorize audit / ledger / snapshot writes.
TASK-242 does not authorize read model refresh.
TASK-242 does not authorize broker, network, credentials, endpoint, real order placement, or automatic trading.

## Next Phase Handoff

Recommended next task: TASK-243 ShellAccounting manual entry replay AccountingEngine adequacy review CI closeout gate.
