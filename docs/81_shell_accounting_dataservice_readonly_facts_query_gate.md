# ShellAccounting DataService Read-only Facts Query Gate

## Document Purpose

TASK-136 defines the gate before any future ShellAccounting DataService read-only
facts query / summary query implementation. This document does not modify
production startup. This document does not modify production QML. This document
does not implement SQLite query. This document does not call AccountingEngine.
This document does not write DB.

## Current State

ShellAccountingPresenter is in the production lifecycle, and
`accountingPresenter` is exposed to QML. The read-only adapter is wired to
DataServiceClient through the adapter / protocol boundary. The authorized action
surface is limited to `position.list`, `cash.summary`,
`portfolio.pnl.summary`, `base_position.summary`, and `sniper_pool.summary`.

The current DataService facts query is not implemented. SQLite / DataAccess is
not connected to ShellAccounting actions. AccountingEngine replay is not
connected to ShellAccounting actions. There are no database writes, TradeDraft,
trade suggestions, or trading UI.

TASK-137 update: the authorized DataService-internal read-only facts query
implementation is now complete for `position.list`, `cash.summary`,
`portfolio.pnl.summary`, `base_position.summary`, and `sniper_pool.summary`.
The implementation remains SELECT-only, DataService-owned, no-write,
no-snapshot-rebuild, and no-AccountingEngine-replay. Shell / Presenter / QML
still do not directly access SQLite or DataAccess.

## DataService Facts Query Gate Summary

Only explicit user authorization can start DataService facts query
implementation. The facts query implementation must be a separate task. The
facts query can only live inside DataService. The presenter must not connect
directly to SQLite. QML must not connect directly to DataServiceClient. The
adapter must not connect directly to SQLite.

The facts query is not write path. The facts query is not AccountingEngine
replay. The facts query is not snapshot rebuild. The facts query must not write
DB, generate TradeDraft, execute strategy, or place orders. Facts query failure
must map to unavailable / issue state and must not crash the app.

facts query is not AccountingEngine replay.

TASK-137 implementation completed the positive boundary: facts query exists
only as an authorized read-only DataService implementation and remains outside
write path, TradeDraft, trading action, snapshot rebuild, and replay scope.

TASK-138 adds the replay snapshot rebuild gate. TASK-137 facts query remains
the read-only fallback. AccountingEngine replay and snapshot rebuild are still
not implemented, and snapshot write is still not authorized.

## Required Pre-gates

- TASK-119 QML static gate.
- TASK-120 ShellAccounting read-only shell boundary.
- TASK-121 QML binding smoke scaffold.
- TASK-122 runtime QML smoke CTest.
- TASK-123 production QML binding boundary.
- TASK-124 QML type registration boundary.
- TASK-125 QML type registration scaffold.
- TASK-126 QML type registration implementation gate.
- TASK-127 QML type registration helper implementation.
- TASK-128 startup registration gate.
- TASK-129 startup wiring implementation.
- TASK-130 production QML binding gate.
- TASK-131 production QML binding shell.
- TASK-132 presenter lifecycle gate.
- TASK-133 presenter lifecycle implementation.
- TASK-134 real data adapter gate.
- TASK-135 real data adapter implementation.
- full CTest.
- transport_local_socket_echo 50 repeat.
- git status clean.

## Future Allowed Read Source Policy

Future DataService facts query work may read only approved read-only facts or
derived cache sources:

- `trade_log` as fact source, read-only only.
- `position_snapshot` as derived cache, read-only only.
- `cash_snapshot` as derived cache, read-only only.
- `portfolio_summary` as derived cache, read-only only.
- `base_position_allocation` as base allocation fact, read-only only.
- `trade_tier_allocation` as tier allocation fact, read-only only.
- `grid_cycle` as read-only cycle context, if needed.
- strategy / portfolio / account metadata as read-only context, if needed.

Reading `trade_log` is not modifying `trade_log`. Reading snapshot is not
rebuild snapshot. Reading summary is not recalculation write. stale snapshot
must be reported, not silently rebuilt. If replay, rebuild, or correction is
needed, rebuild must be separate TASK.

Reading snapshot is not rebuild snapshot.

## Future Forbidden Write Policy

Future facts query work must forbid:

- INSERT.
- UPDATE.
- DELETE.
- REPLACE.
- DROP.
- ALTER.
- CREATE TABLE.
- transaction write.
- write repository.
- audit append.
- `trade_log write`.
- `trade_execution_group write`.
- `trade_draft write`.
- `cash_snapshot write`.
- `position_snapshot write`.
- `portfolio_summary write`.
- `audit_log write`.
- `data.audit.append`.
- correction / reversal / voided.
- `confirmTrade`.
- `manualEntry`.
- `cashAdjustment`.
- `createTradeDraft`.
- `brokerOrder`.
- `strategyExecute`.

## Query Result Policy

- empty account -> structured empty/unavailable.
- missing account -> issue.
- missing portfolio -> issue.
- missing instrument -> issue.
- stale snapshot -> stale status.
- data inconsistency -> issue and SAFE_READONLY-compatible response.
- query error -> unavailable issue.
- timeout -> unavailable issue.
- no silent success.
- no fake data.

## AccountingEngine / Replay Policy

TASK-136 does not authorize AccountingEngine replay. DataService facts query
must not call AccountingReplayEngine. DataService facts query must not call
`replayFromFacts`. If replay integration is needed, it must be a separate gate
and implementation task. A read-only query may consume existing derived cache
but must not rebuild it. A read-only query may report stale or missing derived
cache as issue.

## Privacy / Payload Policy

- no raw SQL in QML.
- no full trade_log payload in QML.
- no sensitive raw payload in issue text.
- no full account cash history in QML error.
- privacy mode must remain respected.
- logs must not dump raw facts payload by default.

## Rollback / Disable Strategy

The facts query implementation must be reversible. If query fails, disable facts
query and keep presenter unavailable. Fallback must not fake data. Fallback must
not write DB. Fallback must not generate TradeDraft. Fallback must not bypass
DataService with shell-side SQLite reads. Fallback must not call AccountingEngine
replay.

Fallback must not write DB.
Fallback must not bypass DataService with shell-side SQLite reads.
Fallback must not call AccountingEngine replay.

## Required Post-query Tests

- query success with controlled fixture.
- empty account.
- missing account.
- missing portfolio.
- missing instrument.
- stale snapshot.
- data inconsistency.
- query error.
- timeout.
- no write SQL scan.
- no AccountingEngine replay scan.
- privacy/no raw payload scan.
- full CTest.
- transport 50 repeat.

## PR Requirements For Future Implementation

A future DataService facts query PR must document implemented query methods,
allowed tables, SQL read-only evidence, no write path evidence, no
AccountingEngine replay evidence, response contract, stale/unavailable/error
mapping, privacy handling, rollback plan, and all gate test results.

## TASK-139 Update

TASK-139 adds an explicit DataService-internal read-only replay path. The
TASK-137 facts query remains SELECT-only fallback. Snapshot rebuild/write,
TradeDraft, trade suggestion, strategy execution, broker order, and trading UI
remain unauthorized.
