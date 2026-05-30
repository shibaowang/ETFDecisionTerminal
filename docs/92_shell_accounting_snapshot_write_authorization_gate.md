# ShellAccounting Snapshot Write Authorization Gate

## Document Purpose

TASK-143 defines the authorization gate before any future ShellAccounting
snapshot write implementation. This document does not implement snapshot write,
does not write database tables, does not modify production QML, does not modify
production startup, and does not generate TradeDraft, trade suggestions,
strategy execution, or broker orders.

## Current State

TASK-119 through TASK-142 are complete. DataService has read-only facts query,
AccountingEngine read-only replay, and TASK-142 snapshot rebuild preview. The
current snapshot rebuild remains an in-memory `snapshotRebuildPreview` payload.
Snapshot write is not implemented. DataService does not write `cash_snapshot`,
`position_snapshot`, `portfolio_summary`, or `audit_log` for ShellAccounting.
TradeDraft, trade suggestion, strategy execution, broker order, order
placement, and trading UI remain unauthorized.

## Future Snapshot Write Authorization Boundary

Snapshot write implementation must be a separate TASK after explicit user
authorization. Snapshot write input may only come from TASK-142 snapshot rebuild
preview. Snapshot write must go through the DataService boundary and must not be
triggered directly by Shell, QML, Presenter, ShellServices, or ShellCore.

snapshot write implementation must be a separate TASK.
snapshot write input may only come from TASK-142 snapshot rebuild preview.
snapshot write must go through the DataService boundary.

Snapshot write must not modify the `trade_log` factual ledger, must not write
`trade_draft`, must not write `trade_execution_group`, must not generate
TradeDraft, must not generate trade suggestions, must not execute strategy, and
must not submit broker orders or place orders.

Schema migration must be a separate TASK. Snapshot write must not modify
`migrations/001_initial_schema.sql`.

schema migration must be a separate TASK.

## DataService-Only Write Policy

Future snapshot write may only be coordinated by DataService. QML,
ShellServices, ShellCore, Presenter, and Shell must not directly trigger
snapshot write or directly depend on SQLite / DataAccess write paths.

future snapshot write may only be coordinated by DataService.
QML, ShellServices, ShellCore, Presenter, and Shell must not directly trigger snapshot write.

DataServiceApi and DataServiceClient must not expose a generic write-enabled
escape hatch. Every future snapshot write action must be allowlisted, statically
scannable, and test-covered. Snapshot write must not broaden the existing
DataServiceClient transport contract into generic write access.

must not expose a generic write-enabled escape hatch.

## Write Input Policy

Snapshot write input can only be the TASK-142 snapshot rebuild preview payload:
`snapshotRebuildPreview`. Snapshot write must not read directly from replay or
facts query when preview is unavailable. Preview input only is required so the
future write path cannot silently diverge from the reviewed derived snapshot
payload.

If the preview is unavailable, stale, inconsistent, or missing required
sections, the write path must return an authorization / unavailable issue and
must not write.

## Write Allowlist Policy

The future candidate allowlist is limited to derived snapshot / summary tables:

- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`

These tables are only candidate allowlist entries for a future implementation.
Current TASK-143 does not write these tables. A future implementation must
prove table-by-table authorization and must keep writes scoped to the approved
allowlist.

current TASK-143 does not write these tables.

## Forbidden Write Policy

The forbidden tables and actions remain:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `audit_log`, unless a future task separately authorizes audit writes
- any generic SQL write path
- any generic write-enabled DataService action
- `data.audit.append` as a side effect of snapshot write unless separately
  authorized

Snapshot write must not mutate the factual ledger, must not create drafts, and
must not record audit entries without a separate authorization boundary.

## No Trade Policy

Snapshot write does not authorize TradeDraft generation, trade suggestions,
broker orders, strategy execution, order confirmation, buy / sell action, cash
adjustment, manual entry, confirmTrade, or trading UI.

## Transaction / Rollback Policy

Future snapshot write must be transactional. Partial write forbidden is a hard
rule: either every authorized snapshot table write succeeds or the transaction
rolls back. Transaction rollback must be reported as an issue and must not be
treated as success.

Snapshot write must include an explicit authorization check and a disable
switch. Disable snapshot write must keep TASK-142 snapshot rebuild preview
available as the read-only fallback. No fake data, no write fallback, and no
trading fallback are allowed.

## Idempotency / Duplicate Write Policy

Future snapshot write must define snapshot identity, idempotency keys, and
duplicate snapshot behavior before implementation. Duplicate snapshot must map
to an explicit no-op or issue state. It must not silently overwrite data, must
not create duplicate derived rows, and must not mutate `trade_log`.

## Error Mapping Policy

Future snapshot write implementation must map these states explicitly:

- write disabled
- authorization missing
- preview unavailable
- stale preview
- DB write failure
- transaction rollback
- schema mismatch
- duplicate snapshot
- partial write forbidden
- no silent success

## Privacy Policy

Snapshot write diagnostics must not expose raw SQL, full `trade_log` payloads,
internal exception stacks, raw preview internals beyond the authorized derived
snapshot payload, or sensitive account history in QML. Logs must not dump full
sensitive payloads by default.

## Rollback / Disable Policy

The rollback policy must allow disabling snapshot write without disabling
TASK-142 snapshot rebuild preview. Fallback must keep the read-only preview
visible, must not fake data, must not write database tables, must not generate
TradeDraft, must not generate trade suggestions, must not execute strategy, and
must not submit broker orders.

rollback policy is required for every future snapshot write implementation.

## Future Implementation PR Requirements

A future snapshot write PR must document the authorized action name, candidate
allowlist tables actually used, preview input contract, transaction behavior,
idempotency key, duplicate snapshot behavior, error mapping, privacy handling,
rollback plan, no TradeDraft evidence, no strategy / broker order evidence, no
schema migration evidence, and all TASK-143 gate test results.

## Required Pre-Gates For Future Implementation

Future snapshot write implementation requires TASK-119, TASK-120, TASK-121,
TASK-122, TASK-123, TASK-124, TASK-125, TASK-126, TASK-127, TASK-128, TASK-129,
TASK-130, TASK-131, TASK-132, TASK-133, TASK-134, TASK-135, TASK-136, TASK-137,
TASK-138, TASK-139, TASK-140, TASK-141, TASK-142, full CTest,
`transport_local_socket_echo` 50 repeat, clean git status, TASK-143 gate tests,
and explicit user authorization.
