# TASK-267 ShellAccounting Excel/VBA Import Persist Manual Entry ShellServices Presenter Contract

TASK-267 connects the TASK-266 typed client adapter to the ShellServices,
Controller, and Presenter C++ contract for accepted Excel/VBA import preview
persistence.

## Scope

The contract exposes a production C++ path for
`accounting.excel_vba_import.persist_manual_entry` without adding QML, startup,
Presenter context wiring, Controller UI wiring, or an Import / Confirm /
Persist button.

The ShellServices path maps an accepted preview persistence request through:

- `ShellAccountingPresenter::persistExcelVbaImportManualEntry(...)`
- `ShellAccountingReadOnlyController::persistExcelVbaImportManualEntry(...)`
- `ShellAccountingDataServiceAdapter`
- `ShellAccountingDataServiceClientPort`
- `ShellAccountingDataServiceClientPortAdapter`
- `DataServiceClient::accountingExcelVbaImportPersistManualEntry(...)`

Presenter and Controller code must not directly include or call
`DataServiceClient`. The typed client adapter is reachable only through the
ShellAccounting client port boundary.

## Request Boundary

Only `previewStatus=ACCEPTED` may enter the persistence request mapping.
Missing `previewDigest`, missing `idempotencyKey`, missing schema/source
metadata, missing accepted timestamp, missing import batch label, malformed
sanitized payload JSON, or an empty fact summary must fail closed before the
client port is called.

The mapped request carries preview status, preview digest, idempotency key,
schema version, sanitized source, accepted timestamp, import batch label, fact
counts, and sanitized in-memory import payload JSON.

## Response Boundary

The Presenter view-model maps transaction commit, trade_log write,
audit_log write, duplicate import prevention, idempotency conflict rejection,
and sanitized issue codes. Duplicate idempotent responses remain safe
view-model outcomes. Idempotency conflicts and non-accepted previews remain
fail-closed outcomes.

## Forbidden Capability

TASK-267 does not modify apps, QML, startup, DataServiceApi action
implementation, DataServiceClient, DataAccess, migrations, AccountingEngine,
production file loading behavior, historical fixtures, TradeDraft generation,
strategy execution, broker integration, network access beyond existing local
DataService transport, credentials, endpoints, real order placement, or
automatic trading.
