# ShellAccounting Snapshot Rebuild Write Gate

## Document Purpose

TASK-140 defines the gate before any future ShellAccounting snapshot rebuild /
snapshot write implementation. This document does not implement snapshot
rebuild, does not write database tables, does not modify QML, and does not
generate TradeDraft or trade suggestions.

## Current State

TASK-139 completed DataService-internal read-only replay. The five existing
read-only actions can use read-only replay to produce read-only accounting
state:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

Snapshot rebuild is not implemented. Snapshot write is not authorized.
`trade_log` remains the factual ledger and must not be modified by this stage.
TradeDraft, trade suggestion, strategy execution, broker order, order
placement, and trading UI remain unauthorized.

## Future Snapshot Rebuild Boundary

Snapshot rebuild can only be implemented in a separate task after explicit user
authorization. Snapshot rebuild must go through the DataService boundary.
Snapshot rebuild input must come from read-only replay / facts query results.

Snapshot rebuild is not read-only replay. Snapshot rebuild must not modify
`trade_log`, must not generate TradeDraft, must not generate trade suggestions,
must not trigger strategy execution, and must not submit broker orders or place
orders.

Snapshot write is not trade execution.

## Future Snapshot Write Boundary

Snapshot write, if ever needed, must be separately authorized. Snapshot write is
not trade execution. Snapshot write may only write explicitly allowlisted tables
in a future task. The future allowlist may only contain derived snapshot /
summary tables and must not contain `trade_log` or `trade_draft`.

Shell, QML, and Presenter must not directly trigger snapshot write. Snapshot
write must have a no-write fallback and a rollback / disable switch. Snapshot
write must not modify `migrations/001_initial_schema.sql` unless a separate
schema migration task explicitly authorizes it.

## DataService-Only Write Policy

Future snapshot rebuild / write may only be coordinated by DataService. QML,
ShellServices, and ShellCore must not directly depend on DataAccess or a SQLite
write path.

DataServiceApi and DataServiceClient must not expose a generic write-enabled
escape hatch. Every future write action must be allowlisted, statically
scannable, and test-covered.

Future rebuild/write code must not directly depend on DataAccess or a SQLite write path from Shell/QML.
DataServiceApi and DataServiceClient must not expose a generic write-enabled escape hatch.
All future rebuild/write actions must be allowlisted, statically scannable, and test-covered.

## No Trade Policy

The snapshot rebuild / write boundary forbids TradeDraft generation, broker
order, strategy execute, trading UI, order confirmation, buy / sell action,
cash adjustment, manual entry, and confirmTrade.

## Error Mapping Policy

Future snapshot rebuild / write implementation must map these states explicitly:
rebuild disabled, write authorization missing, replay unavailable, missing
facts, stale facts, inconsistent facts, DB write failure, transaction rollback,
schema mismatch, privacy redaction failure, and no silent success.

The required mapping list includes missing facts, stale facts, and inconsistent facts.

## Privacy Policy

QML must not receive raw `trade_log` payload, raw SQL, or internal exception
stacks. Logs must not print complete sensitive payloads by default. Rebuild
diagnostics must be redacted before they cross into UI or issue text.

## Rollback / Disable Policy

Future snapshot rebuild / write must be disableable. Disabling snapshot rebuild
should fall back to TASK-139 read-only replay. Disabling snapshot write must not
fake snapshot data. Fallback must not write DB, must not generate trade
suggestions, must not generate TradeDraft, must not add a trading fallback, and
must not bypass DataService.

Disable rebuild and disable write are required rollback states. No fake data and
no write fallback are allowed.

## Required Pre-Gates For Future Implementation

Future snapshot rebuild / write implementation requires TASK-119 through
TASK-139, full CTest, `transport_local_socket_echo` 50 repeat, clean git status,
TASK-140 gate tests, and explicit user authorization.

The full prerequisite sequence is TASK-119, TASK-120, TASK-121, TASK-122,
TASK-123, TASK-124, TASK-125, TASK-126, TASK-127, TASK-128, TASK-129,
TASK-130, TASK-131, TASK-132, TASK-133, TASK-134, TASK-135, TASK-136,
TASK-137, TASK-138, and TASK-139.
