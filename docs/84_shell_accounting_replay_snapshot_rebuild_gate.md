# ShellAccounting Replay Snapshot Rebuild Gate

## Document Purpose

TASK-138 defines the gate before any future ShellAccounting AccountingEngine
replay / snapshot rebuild implementation. This document does not implement
replay, does not call AccountingEngine, does not rebuild snapshot, does not
write database tables, and does not modify QML.

## Current State

TASK-137 completed the DataService read-only facts query implementation.
The five read-only actions `position.list`, `cash.summary`,
`portfolio.pnl.summary`, `base_position.summary`, and `sniper_pool.summary`
can query facts through the DataService / DataAccess boundary.

Shell / QML still do not directly access SQLite or DataAccess.
AccountingEngine replay is not connected to ShellAccounting. Snapshot rebuild is
not implemented. Snapshot write is not authorized. TradeDraft, trade suggestion,
strategy execution, broker order, and order placement are not authorized.

## Future Replay Boundary

Replay can only be implemented in a separate task after explicit user
authorization. Replay must go through the DataService service boundary. Shell,
QML, and Presenter must not directly call AccountingEngine.

Replay input must come from authorized facts / read-only repository sources. The
output may only be used as an internal source for read-only accounting state or
summary data unless a later task authorizes a different use. Replay must not
generate TradeDraft, trade suggestion, strategy execution, broker order, or
order placement.

Replay is not production QML binding. Replay is not TradeDraft or trading
suggestion. Replay is not TradeDraft or trading suggestion. Replay is not
strategy execution.

Replay is not TradeDraft or trading suggestion.

## Future Snapshot Rebuild Boundary

Snapshot rebuild must be authorized in a separate task. Snapshot rebuild is not
read-only facts query. If snapshot rebuild requires table writes, that write
boundary must be separately authorized and test-covered.

Snapshot rebuild must not be triggered by QML, Presenter, or Shell code as a
direct database write path. Snapshot rebuild must not fake data, overwrite the
`trade_log` fact ledger, or modify `migrations/001_initial_schema.sql`.

Snapshot rebuild is not trade execution. Snapshot write, if ever needed, must be
explicitly authorized. Snapshot write, if ever needed, must be explicitly
authorized.

Snapshot write, if ever needed, must be explicitly authorized.

## DataService-Only Policy

Future replay / rebuild may only be coordinated by DataService. QML,
ShellServices, and ShellCore must not directly depend on AccountingEngine,
DataAccess, or SQLite.

They must not directly depend on AccountingEngine, DataAccess, or SQLite.

DataServiceApi and DataServiceClient must not expose a write-enabled generic
escape hatch. All new actions must be allowlisted, statically scannable, and
test-covered.

DataServiceApi and DataServiceClient must not expose a write-enabled generic escape hatch.
All new actions must be allowlisted, statically scannable, and test-covered.

## No Write / No Trade Policy

The replay / rebuild boundary forbids `INSERT`, `UPDATE`, `DELETE`, `REPLACE`,
`CREATE`, `DROP`, and `ALTER` unless a later write task explicitly authorizes a
specific write path.

The boundary also forbids trade_log write, trade_execution_group write,
trade_draft write, cash_snapshot write, position_snapshot write,
portfolio_summary write, audit_log write, TradeDraft generation, broker order,
strategy execute, and trading UI.

## Error Mapping Policy

Future replay / rebuild implementation must map these states explicitly:
missing facts, inconsistent facts, unsupported currency, missing market price,
missing FX rate, negative cash, sell exceeds position, stale facts, replay
error, snapshot rebuild disabled, write authorization missing, timeout /
unavailable, and no silent success.

The future mapping must also name replay disabled and rebuild disabled states
when those capabilities are switched off.

## Privacy Policy

QML must not receive raw `trade_log` payload, raw SQL, or internal exception
stacks. Logs must not print complete sensitive payloads by default. Replay
diagnostics must be redacted before they cross into UI or issue text.

## Rollback / Disable Policy

Future replay / rebuild must be disableable. Disabling replay should fall back
to the TASK-137 read-only facts query. Disabling rebuild must not fake snapshot
data. Fallback must not write DB, must not generate trade suggestions, must not
generate TradeDraft, must not add a trading fallback, and must not bypass
DataService.

Fallback must not generate TradeDraft.

## Required Pre-Gates For Future Implementation

Future replay / rebuild implementation requires TASK-119 through TASK-137,
full CTest, `transport_local_socket_echo` 50 repeat, clean git status,
TASK-138 gate tests, and explicit user authorization.

The full prerequisite sequence is TASK-119, TASK-120, TASK-121, TASK-122,
TASK-123, TASK-124, TASK-125, TASK-126, TASK-127, TASK-128, TASK-129,
TASK-130, TASK-131, TASK-132, TASK-133, TASK-134, TASK-135, TASK-136, and
TASK-137.

## TASK-139 Update

TASK-139 implements authorized DataService-internal read-only replay for the
five existing ShellAccounting read-only actions. Snapshot rebuild is still not
implemented, snapshot write is still not authorized, Shell/QML/Presenter still
must not call AccountingEngine directly, and the TASK-137 read-only facts query
remains the fallback if read-only replay is disabled.
