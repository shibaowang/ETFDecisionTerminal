# ShellAccounting Snapshot Rebuild Implementation

## TASK-142 Purpose

TASK-142 implements ShellAccounting snapshot rebuild preview. TASK-141 gate is
complete, and this implementation keeps the preview inside the DataService
boundary.

## Scope

This task only implements snapshot rebuild preview. It does not implement
snapshot write, does not write any database table, does not modify production
QML, does not modify production startup, and does not add TradeDraft, trade
suggestion, strategy execution, broker order, or trading UI.

## Preview Input

The preview input comes from TASK-139 read-only replay. Shell, QML, Presenter,
and ShellServices still do not directly access SQLite, DataAccess, or
AccountingEngine. The existing `calculationMode=readonlyReplay` action path
continues to be the authorized input boundary.

## Preview Output

The preview output is an in-memory derived snapshot payload named
`snapshotRebuildPreview`. It is attached to the existing read-only action
response and contains:

- `previewOnly: true`
- `databaseWritten: false`
- `snapshotWritten: false`
- `tradeLogModified: false`
- `positionSnapshots`
- `cashSnapshots`
- `portfolioSummaries`
- `issues`
- privacy flags proving raw SQL, raw `trade_log`, raw replay payload, and
  internal stack traces are not exposed.

The existing response still reports `snapshotRebuilt: false`.

## Authorized Action Mapping

- `position.list` maps replay positions to in-memory `positionSnapshots`.
- `cash.summary` maps replay cash summaries to in-memory `cashSnapshots`.
- `portfolio.pnl.summary` maps replay portfolio PnL to in-memory
  `portfolioSummaries`.
- `base_position.summary` keeps the existing read-only base allocation response
  and may include replay-derived preview context when `readonlyReplay` is
  requested.
- `sniper_pool.summary` keeps the existing read-only sniper tier response and
  may include replay-derived preview context when `readonlyReplay` is requested.

## No Database Write

TASK-142 does not write `cash_snapshot`, `position_snapshot`,
`portfolio_summary`, `audit_log`, `trade_log`, `trade_execution_group`, or
`trade_draft`. It does not add `INSERT`, `UPDATE`, `DELETE`, `REPLACE`,
`CREATE`, `DROP`, `ALTER`, `data.audit.append`, write repository usage, or
`writeEnabled: true`.

## No Snapshot Write

Snapshot write remains unimplemented. Snapshot write must be a separate TASK
with explicit authorization before any derived snapshot table can be persisted.
TASK-142 produces only in-memory preview payloads.

## No Trade Policy

TASK-142 does not generate TradeDraft, trade suggestion, strategy execution,
broker order, order placement, order confirmation, buy / sell action, cash
adjustment, manual entry, confirmTrade behavior, or trading UI.

## Error Mapping

The preview reuses TASK-139 read-only replay issue mapping. Replay unavailable,
missing facts, stale facts, inconsistent facts, unsupported currency, missing
market price, missing FX, and rebuild disabled states remain structured issues
or unavailable preview states. There is no silent success and no fake preview
data.

## Privacy Policy

The preview payload does not expose raw SQL, complete raw `trade_log` payloads,
raw replay payloads, sensitive facts, or internal exception stacks. Preview
diagnostics stay redacted before crossing into ShellAccounting response JSON.

## Rollback / Fallback

Rollback can disable the preview mapping and fallback to TASK-139 read-only replay.
Fallback must not fake data, must not write database tables, must not generate
TradeDraft, must not generate trade suggestions, must not execute strategy, must
not submit broker orders, and must not bypass DataService.

snapshot write must be a separate TASK with explicit authorization before any
derived snapshot table can be persisted.

## Test Checklist

- `shell_accounting_snapshot_rebuild_preview_implementation`
- `shell_accounting_snapshot_rebuild_preview_dataservice_only_boundary`
- `shell_accounting_snapshot_rebuild_preview_from_readonly_replay`
- `shell_accounting_snapshot_rebuild_preview_position_snapshot_mapping`
- `shell_accounting_snapshot_rebuild_preview_cash_snapshot_mapping`
- `shell_accounting_snapshot_rebuild_preview_portfolio_summary_mapping`
- `shell_accounting_snapshot_rebuild_preview_stale_input_mapping`
- `shell_accounting_snapshot_rebuild_preview_inconsistent_facts_mapping`
- `shell_accounting_snapshot_rebuild_preview_no_db_write`
- `shell_accounting_snapshot_rebuild_preview_no_snapshot_write`
- `shell_accounting_snapshot_rebuild_preview_no_trade_or_strategy`
- `shell_accounting_snapshot_rebuild_preview_privacy`
- `shell_accounting_snapshot_rebuild_preview_rollback_ready`

## Unauthorized Follow-Up Scope

Snapshot write, schema migration, TradeDraft generation, trade suggestions,
strategy execution, broker orders, and trading UI require separate future TASKs.
