# TASK-266 ShellAccounting Excel/VBA Import Persist Manual Entry Client Adapter

TASK-266 adds a typed DataServiceClient adapter for the TASK-265 persistence
write action `accounting.excel_vba_import.persist_manual_entry`.

## Scope

The adapter maps an accepted Excel/VBA import preview persistence request into
the existing DataService action payload. The typed request carries
`previewStatus`, `previewDigest`, `idempotencyKey`, `schemaVersion`, source,
accepted timestamp, import batch label, fact summary, and sanitized in-memory
import payload JSON.

The adapter parses the DataService response into a typed result covering
successful persistence, committed transaction flags, trade and audit write
flags, idempotent duplicate prevention, idempotency conflicts, and sanitized
issue codes.

## Client Boundary

The client adapter only calls the existing local DataService transport. It does
not access SQLite, DataAccess repositories, AccountingEngine, broker,
credentials, endpoints, production files, QML, Presenter, Controller, or
ShellServices UI wiring.

Protocol or transport failures remain client failures. Safe DataService
business outcomes such as duplicate imports, idempotency conflicts, or
non-accepted preview rejection are mapped from sanitized response payloads so
callers can inspect typed result flags.

## Forbidden Capability

TASK-266 does not modify apps, QML, startup, Presenter, Controller,
ShellServices production wiring, DataServiceApi action implementation,
DataAccess, migrations, AccountingEngine production code, historical fixtures,
DataService action policy, TradeDraft generation, strategy execution, broker
integration, network access beyond existing local DataService transport,
credentials, endpoints, real order placement, or automatic trading.
