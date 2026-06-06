# TASK-265 ShellAccounting Excel/VBA Import Accepted Preview Manual Entry Persistence Implementation

TASK-265 implements the minimal accepted-preview-to-manual-entry persistence
slice for sanitized Excel/VBA import previews. It registers the dedicated
DataService write action
`accounting.excel_vba_import.persist_manual_entry`.

## Scope

The action accepts only an `ACCEPTED` preview request with a `previewDigest`,
`idempotencyKey`, `schemaVersion`, source marker, fact summary, and sanitized
in-memory import payload. It reuses
`ShellAccountingExcelVbaImportReadOnlyParser`, verifies the digest and fact
summary, and rejects non-accepted or mismatched previews before any write.

The DataService action does not write `trade_log` directly. It calls the
DataAccess composition repository, which maps accepted trade facts to the
existing `ShellAccountingManualTransactionRepository` boundary.

## Supplemental DataAccess Transaction Boundary

TASK-265 adds an active-transaction-only manual transaction repository method so
manual trade facts can participate in a larger import persistence transaction.
The existing public `persistManualTransaction()` behavior remains a transaction
owning wrapper and continues to commit a standalone manual transaction.

The new composition repository owns the single outer `TransactionRunner`. In
that transaction it writes manual transaction facts through the repository
boundary and writes a sanitized import audit event. Audit failure rolls back
manual facts. Trade fact failure rolls back audit and import marker effects.
Nested transactions are not attempted.

## Idempotency

The persistence request requires an idempotency key. The import marker binds the
idempotency key and preview digest to the sanitized audit event.

- Same idempotency key and same digest returns an idempotent result without
  adding trade or audit rows.
- Same idempotency key and different digest returns a sanitized conflict.
- Non-accepted previews do not create trade rows, audit rows, or import markers.

## Forbidden Capability

TASK-265 does not add QML, startup, Presenter, Controller, ShellServices UI
wiring, DataServiceClient changes, migrations, AccountingEngine production
code, historical fixtures, TradeDraft generation, strategy execution, broker
integration, network access, credentials, endpoints, real order placement, or
automatic trading.

TASK-265 tests write only temporary test SQLite databases. Production database
paths are not touched.
