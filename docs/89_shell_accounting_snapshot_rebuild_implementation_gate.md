# ShellAccounting Snapshot Rebuild Implementation Gate

## Document Purpose

TASK-141 defines the gate before any future ShellAccounting snapshot rebuild
implementation. This document does not implement snapshot rebuild, does not
write database tables, does not implement snapshot write, does not modify
production QML, and does not generate TradeDraft, trade suggestions, strategy
actions, or broker orders.

## Current State

TASK-119 through TASK-140 are complete. DataService now owns the ShellAccounting
read-only facts query and read-only AccountingEngine replay boundary. The five
authorized read-only actions remain:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

Snapshot rebuild is still not implemented. Snapshot write is still not
implemented or authorized. TradeDraft, trade suggestion, strategy execution,
broker order, order placement, and trading UI remain unauthorized.

## Future Snapshot Rebuild Implementation Boundary

Snapshot rebuild implementation must be a separate task after explicit user
authorization. Explicit user authorization is required before implementation.
The required authorization phrase is explicit user authorization.
Snapshot rebuild may only compute derived snapshot payloads in memory.
Snapshot rebuild must not write database tables, must not modify
`trade_log`, and must not modify `migrations/001_initial_schema.sql`.

Snapshot rebuild is not snapshot write. Snapshot rebuild is not TradeDraft.
Snapshot rebuild is not trade suggestion. Snapshot rebuild is not strategy
execution, broker order, order placement, or trading UI.

QML, Shell, and Presenter must not directly trigger snapshot rebuild. Future
rebuild implementation must be coordinated through DataService.

## DataService-Only Rebuild Policy

Future snapshot rebuild may only run inside the DataService boundary. Shell,
QML, Presenter, ShellServices, and ShellCore must not directly depend on
SQLite, DataAccess, AccountingEngine rebuild helpers, or snapshot write paths.

Future rebuild actions must be allowlisted, statically scannable, and
test-covered. DataServiceApi and DataServiceClient must not expose a generic
write-enabled escape hatch.

Future rebuild implementation must not expose a generic write-enabled escape hatch.

## Rebuild Input Policy

Snapshot rebuild input may only come from TASK-139 read-only replay output and
its authorized read-only facts query inputs. Rebuild must not use fake trade facts,
hidden QML state, Shell-side SQLite reads, direct DataAccess reads from
ShellServices, or user-entered trading commands.

If read-only replay is unavailable, stale, incomplete, or inconsistent, rebuild
must return a structured unavailable / issue state. It must not silently succeed.

## Rebuild Output Policy

Snapshot rebuild output may only be an in-memory derived snapshot preview. The
preview may describe calculated cash, position, or portfolio summary state for a
future write task, but it must not write `cash_snapshot`, `position_snapshot`,
`portfolio_summary`, `audit_log`, `trade_log`, `trade_execution_group`, or
`trade_draft`.

The in-memory preview must not become TradeDraft, trade suggestion, strategy
signal, broker order, or a QML trading action.

## No Write Policy

TASK-141 does not authorize database writes. Future snapshot rebuild
implementation must not include `INSERT`, `UPDATE`, `DELETE`, `REPLACE`,
`CREATE`, `DROP`, `ALTER`, transaction write, write repository access,
`writeEnabled: true`, or `data.audit.append`.

Snapshot write remains separately gated by TASK-140 and must remain separately
authorized before any database table can be written.

## No Trade Policy

Future snapshot rebuild implementation must not generate TradeDraft, trade
suggestion, broker order, strategy execution, order confirmation, buy / sell
action, cash adjustment, manual entry, or confirmTrade behavior.

## Error Mapping Policy

Future rebuild implementation must explicitly map replay unavailable, missing
facts, stale facts, inconsistent facts, unsupported currency, missing market
price, missing FX, rebuild disabled, and no silent success. A rebuild error must
be represented as a structured issue and must not be hidden behind fake data.

## Privacy Policy

QML must not receive raw SQL, full raw `trade_log` payloads, internal exception
stacks, or sensitive raw facts payloads. Rebuild preview diagnostics must be
redacted before they cross the DataService boundary into ShellAccounting issue
text or QML-visible state.

## Rollback / Disable Policy

Future snapshot rebuild must be disableable. Disabling rebuild must fall back to TASK-139 read-only replay.
Fallback must not fake data, must not write database
tables, must not generate TradeDraft, must not generate trade suggestions, must
not execute strategy, and must not bypass DataService. Fallback must not execute strategy.

Rollback scope for the future implementation must be limited to rebuild
calculation code, tests, and documentation. Snapshot write remains separately
disabled.

## Required Pre-Gates For Future Implementation

The full prerequisite sequence is TASK-119, TASK-120, TASK-121, TASK-122,
TASK-123, TASK-124, TASK-125, TASK-126, TASK-127, TASK-128, TASK-129,
TASK-130, TASK-131, TASK-132, TASK-133, TASK-134, TASK-135, TASK-136,
TASK-137, TASK-138, TASK-139, and TASK-140.

## Future Implementation PR Requirements

A future snapshot rebuild implementation PR must state:

- The DataService rebuild entry point.
- The read-only replay input source.
- The in-memory preview output shape.
- The error mapping for replay unavailable, missing facts, stale facts,
  inconsistent facts, unsupported currency, missing market price, missing FX,
  rebuild disabled, and no silent success.
- Evidence that no database tables are written.
- Evidence that snapshot write remains disabled.
- Evidence that QML / Shell / Presenter do not directly trigger rebuild.
- Evidence that no TradeDraft, trade suggestion, strategy execution, broker
  order, order placement, or trading UI was added.
- Rollback / disable strategy and all gate test results.

## TASK-142 Implementation Status

TASK-142 implements the authorized preview-only rebuild. The implementation is
DataService-only, uses TASK-139 read-only replay input, returns an in-memory
`snapshotRebuildPreview` payload, keeps `snapshotRebuilt:false`, and does not
write database tables. Snapshot write remains unimplemented and separately
unauthorized.
