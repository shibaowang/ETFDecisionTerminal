# TASK-264 ShellAccounting Excel/VBA Import Preview To Manual Entry Persistence Authorization Gate

TASK-264 defines the production write authorization boundary for moving an
accepted ShellAccounting Excel/VBA import preview into manual entry persistence.
This task is authorization-only. It does not implement import persistence, does
not register a write action, does not add UI controls, and does not write any
database table.

## Current State

TASK-263 provides a read-only Excel/VBA import preview acceptance UX and compact
sanitized export format contract. The current application can parse sanitized
in-memory preview payloads, map diagnostics, and show preview acceptance status.
No accepted preview can currently be persisted by this task.

## Authorization Boundary

Future import persistence may proceed only from a preview whose normalized state
is `ACCEPTED`. `READY`, `FILE_SELECTED`, `PREVIEWING`, `REJECTED`, and
`INPUT_ERROR` previews must not enter any persistence write path.

The future production write entry point must be a dedicated DataService action.
QML, Presenter, Controller, ShellServices adapter, and DataServiceClient code
must not directly write SQLite, repositories, `trade_log`, manual transaction
facts, `audit_log`, idempotency records, import batch markers, read models, or
ledger rows.

The future DataService action must be separately authorized before
implementation. A suggested action name is
`accounting.excel_vba_import.persist_manual_entry`, but TASK-264 does not
register or implement that action.

## Minimal Persistence Request DTO

The future request DTO from accepted preview to persistence must be explicit and
small:

```json
{
  "previewStatus": "ACCEPTED",
  "previewId": "opaque-preview-id",
  "previewDigest": "sha256-of-sanitized-preview-facts",
  "idempotencyKey": "client-or-dataservice-generated-key",
  "schemaVersion": "excel-vba-export/v1",
  "source": "sanitized-excel-vba-export",
  "acceptedAt": "iso-8601",
  "importBatchLabel": "sanitized-display-label",
  "tradeFacts": [],
  "cashFacts": [],
  "diagnosticCodes": []
}
```

The DTO must not contain raw local file paths, raw payload text, SQL, stack
traces, credentials, endpoints, broker payloads, real order ids, or UI-only
state. The DataService action must revalidate the accepted preview digest and
fact summary instead of trusting QML or Presenter state alone.

## Transaction Boundary

Future persistence must write all authorized rows in one transaction:

- `trade_log` or manual transaction facts required by the accepted preview.
- `audit_log` with a sanitized import persistence event.
- idempotency record or import batch marker that binds the idempotency key,
  preview digest, and sanitized batch metadata.

If any required row fails, the whole transaction must roll back. Partial
imports, orphan audit rows, orphan idempotency rows, and partial trade facts are
forbidden. Audit failure must roll back trade/manual facts and the import batch
marker.

## Idempotency And Duplicate Policy

The future DataService action must require an idempotency key. The key may be
provided by the client or derived by DataService from the accepted preview
digest, account, portfolio, source, and normalized fact set.

The duplicate policy is:

- Same idempotency key and same preview digest returns the original successful
  result without adding rows.
- Same idempotency key and different preview digest returns a sanitized
  idempotency conflict error.
- Same preview digest under a different key returns a sanitized duplicate import
  response or requires an explicit duplicate override in a separate future
  authorization.
- Rejected or input-error previews must never create an idempotency record.

## Sanitized Error Mapping

Future errors must map to stable sanitized issue codes such as:

- `EXCEL_VBA_IMPORT_PREVIEW_NOT_ACCEPTED`
- `EXCEL_VBA_IMPORT_IDEMPOTENCY_REQUIRED`
- `EXCEL_VBA_IMPORT_DUPLICATE`
- `EXCEL_VBA_IMPORT_IDEMPOTENCY_CONFLICT`
- `EXCEL_VBA_IMPORT_TRANSACTION_ROLLBACK`
- `EXCEL_VBA_IMPORT_AUDIT_REQUIRED`

Error responses must not expose raw payloads, local paths, SQL text, exception
stacks, credentials, endpoints, broker payloads, or real order ids.

## Forbidden Capability

TASK-264 does not add an Import, Confirm, or Persist UI control. It does not
modify QML, startup, Presenter, Controller, ShellServices production C++,
DataServiceApi production action implementation, DataServiceClient, DataAccess,
migrations, AccountingEngine production code, or historical fixtures.

TASK-264 does not write SQLite, `audit_log`, ledger, snapshot, `trade_log`, or
persistent read models. It does not generate TradeDrafts, execute strategy,
submit broker orders, access network, read credentials, read endpoints, place
real orders, or enable automatic trading.

