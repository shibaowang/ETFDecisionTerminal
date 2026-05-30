# ShellAccounting Audit Write Implementation

## TASK-146 Purpose

TASK-146 implements ShellAccounting audit write inside the DataService boundary.
TASK-145 authorization gate is complete. This task records sanitized audit
events for authorized TASK-144 snapshot write results only.

## Scope

The implemented action is `accounting.audit.write`. It is a DataService write
action and requires explicit authorization token `TASK-146_AUDIT_WRITE`.

Audit write input comes from TASK-144 snapshot write result:

- `source=snapshotWriteResult`
- `sourceAction=accounting.snapshot.write`
- `resultStatus=OK`
- `snapshotWritten=true`
- account / portfolio identifiers
- snapshot uid / batch id
- sanitized issue code
- row counts for `cash_snapshot`, `position_snapshot`, and `portfolio_summary`

## Allowlist Table

The only table this task allows audit write to persist is:

- `audit_log`

## Forbidden Tables

The implementation must not write:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- any other non-allowlisted table

## Sanitized Payload Policy

The audit payload is intentionally small:

- source action
- authorization token
- snapshot batch id
- result status
- sanitized issue code
- row counts
- privacy flags

The payload contains no raw SQL, no raw trade_log payload,
no full snapshot payload, and no internal stack trace.

## Transaction / Rollback Policy

Audit insert is wrapped in the DataAccess transaction boundary. A failed insert
must roll back and must not create partial audit rows. Authorization missing,
audit disabled, invalid source write result, invalid source payload, DB write
failure, schema mismatch, and transaction rollback must not be silent success.

## Idempotency / Duplicate Audit Policy

Duplicate audit events are detected by account, portfolio, entity type, action,
and snapshot batch reason. A duplicate event returns an idempotent duplicate
status and does not insert another audit row.

## No Trade Policy

TASK-146 does not generate TradeDraft, does not generate trade suggestion, does
not execute strategy, does not submit broker order, and does not add trading UI.

## Production QML / Startup

TASK-146 does not modify production QML and does not modify production startup.
Shell / QML / Presenter do not trigger `accounting.audit.write` directly.

## Error Mapping

The implementation maps:

- audit disabled
- authorization missing
- source write unavailable
- source write failed
- audit payload invalid
- DB write failure
- transaction rollback
- schema mismatch
- duplicate audit event
- no silent success

## Privacy Policy

Responses and stored payloads do not expose raw SQL, full `trade_log` payloads,
full snapshot payloads, sensitive raw payloads, or internal exception stacks.

## Rollback / Disable Strategy

Rollback is limited to `accounting.audit.write`,
`ShellAccountingAuditWriteRepository`, tests, and docs. Disabling audit write
must not disable TASK-144 snapshot write. Fallback must not fake audit success,
must not write trade_log, must not write trade_draft, must not generate
TradeDraft, must not execute strategy, and must not bypass DataService.

## Test Checklist

- `shell_accounting_audit_write_implementation`
- `shell_accounting_audit_write_dataservice_only_boundary`
- `shell_accounting_audit_write_requires_snapshot_write_result`
- `shell_accounting_audit_write_requires_authorization`
- `shell_accounting_audit_write_success`
- `shell_accounting_audit_write_payload_sanitized`
- `shell_accounting_audit_write_no_raw_sql`
- `shell_accounting_audit_write_no_raw_trade_log_payload`
- `shell_accounting_audit_write_no_full_snapshot_payload`
- `shell_accounting_audit_write_transaction_rollback`
- `shell_accounting_audit_write_idempotency`
- `shell_accounting_audit_write_duplicate_event_handling`
- `shell_accounting_audit_write_no_trade_log_write`
- `shell_accounting_audit_write_no_trade_draft_write`
- `shell_accounting_audit_write_no_trade_or_strategy`
- `shell_accounting_audit_write_no_ui_trigger`
- `shell_accounting_audit_write_privacy`
- `shell_accounting_audit_write_rollback_ready`

## Unapproved Follow-Up Scope

The following remain unapproved and require separate tasks:

- TradeDraft generation
- trade suggestion
- strategy execution
- broker order
- production QML write controls
- schema migration
