# TASK-144 ShellAccounting Snapshot Write Implementation

## Purpose

TASK-144 implements the first ShellAccounting snapshot write path. TASK-143
authorization gate is complete. This task persists the TASK-142
`snapshotRebuildPreview` payload through the DataService boundary only.

## Scope

The implementation is limited to DataService-coordinated snapshot writes. It
does not modify production QML, production startup, DataServiceClient transport,
ShellAccounting presenter APIs, TASK-142 preview semantics, AccountingEngine
replay behavior, or `migrations/001_initial_schema.sql`.
The authorized action name is `accounting.snapshot.write`.

## Snapshot Write Input

Snapshot write input comes only from TASK-142 `snapshotRebuildPreview`
(TASK-142 snapshotRebuildPreview). The
write action requires:

- `source`: `snapshotRebuildPreview`
- `calculationMode`: `readonlyReplay`
- `authorization`: `TASK-144_SNAPSHOT_WRITE`

If authorization is missing, if write is disabled, or if preview input is not
available, the action returns a clear error/status and must not silently
succeed.

## Allowlist Tables

TASK-144 writes only these allowlist tables:

- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`

The write implementation uses deterministic `uid` values and idempotent
`ON CONFLICT(uid) DO UPDATE` behavior for duplicate snapshot handling.

## Forbidden Tables

TASK-144 does not write:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `audit_log`

Audit write remains separate. Any future audit write, TradeDraft, strategy, or
broker integration must be a separate TASK.

## Transaction / Rollback Policy

The snapshot write repository writes all allowlist rows inside one
`TransactionRunner` transaction. Partial write is forbidden. If any allowlist
row fails, the transaction rolls back and no already-attempted snapshot row is
kept.

## Idempotency / Duplicate Policy

Duplicate snapshot writes are handled by deterministic `uid` and SQLite
upsert-by-uid. Replaying the same preview write updates the same snapshot rows
instead of inserting duplicates.

## No Trade Policy

TASK-144 does not generate TradeDraft, trade suggestion, strategy execution, or
broker order. It does not add trading UI and does not modify `trade_log`.

## Error Mapping

The implementation covers:

- write disabled
- authorization missing
- preview unavailable
- stale or blocked preview
- DB write failure
- transaction rollback
- schema mismatch
- duplicate snapshot handling
- partial write forbidden
- no silent success

## Privacy Policy

Responses do not expose raw SQL, full `trade_log` payloads, sensitive raw
payloads, or internal exception stacks. Snapshot write diagnostics are limited to
table counts, status flags, allowlist/forbidden table names, and sanitized issue
codes.

## Rollback / Disable Strategy

Rollback is limited to the DataService write action, the
`ShellAccountingSnapshotWriteRepository`, tests, and docs. Disabling the action
or omitting TASK-144 authorization leaves TASK-142 preview and TASK-139
read-only replay available as read-only fallbacks. Fallback must not fake data,
write DB, generate TradeDraft, execute strategy, or bypass DataService.

## Test Checklist

- `shell_accounting_snapshot_write_implementation`
- `shell_accounting_snapshot_write_dataservice_only_boundary`
- `shell_accounting_snapshot_write_requires_preview_input`
- `shell_accounting_snapshot_write_cash_snapshot_success`
- `shell_accounting_snapshot_write_position_snapshot_success`
- `shell_accounting_snapshot_write_portfolio_summary_success`
- `shell_accounting_snapshot_write_transaction_rollback`
- `shell_accounting_snapshot_write_idempotency`
- `shell_accounting_snapshot_write_duplicate_snapshot_handling`
- `shell_accounting_snapshot_write_no_trade_log_write`
- `shell_accounting_snapshot_write_no_trade_draft_write`
- `shell_accounting_snapshot_write_no_audit_write`
- `shell_accounting_snapshot_write_no_trade_or_strategy`
- `shell_accounting_snapshot_write_no_ui_trigger`
- `shell_accounting_snapshot_write_privacy`
- `shell_accounting_snapshot_write_rollback_ready`

## Unapproved Follow-Up Scope

The following remain unapproved and require separate tasks:

- audit write for snapshot write events
- TradeDraft generation
- trade suggestion
- strategy execution
- broker order
- production QML write controls
- schema migration

## TASK-145 Audit Write Authorization Gate

TASK-145 adds the audit write authorization gate after this implementation.
Audit write remains unimplemented, `audit_log` remains unwritten by
ShellAccounting snapshot write, and TASK-144 snapshot write behavior is
unchanged. Future audit write must be separately authorized, DataService-only,
sanitized, and rollback-ready.
