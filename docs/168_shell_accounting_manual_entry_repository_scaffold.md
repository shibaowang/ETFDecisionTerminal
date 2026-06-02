# TASK-185 ShellAccounting Manual Entry Repository Scaffold

## Document Purpose

TASK-185 adds the ShellAccounting manual entry DataAccess repository scaffold.
This is a scaffold only, not a repository implementation and not a write implementation.

The scaffold defines command DTOs, a result DTO, and a disabled repository
boundary for future manual buy / sell and manual deposit / withdrawal
persistence. It does not write a database, does not execute SQL, does not write
`trade_log`, cash facts, cash ledger, audit, or ledger rows, does not modify
DataServiceActions, does not modify AccountingEngine replay, does not modify
production QML or startup, and does not modify migrations.

## TASK-184 Gate

TASK-184 completed the repository scaffold authorization gate in docs/166 and
docs/167. TASK-185 is the first authorized scaffold step after that gate. The
scaffold remains disabled and write-not-implemented.

## Added Scaffold

TASK-185 adds:

- `ShellAccountingManualEntryRepositoryScaffold`
- `ShellAccountingManualTransactionPersistenceCommand`
- `ShellAccountingManualCashMovementPersistenceCommand`
- `ShellAccountingManualEntryPersistenceResult`

The files are:

- `libs/DataAccess/include/DataAccess/ShellAccountingManualEntryRepositoryScaffold.h`
- `libs/DataAccess/src/ShellAccountingManualEntryRepositoryScaffold.cpp`

The scaffold is registered in `libs/DataAccess/CMakeLists.txt` only so it can
compile as part of the DataAccess library and be tested directly.

TASK-182 validation wiring still returns validation accepted plus `writeImplemented=false`.

TASK-186 adds a future repository implementation authorization gate. It does
not change this scaffold, and the scaffold remains disabled /
write-not-implemented until a separate repository implementation TASK is
explicitly authorized.

## Command DTO Boundary

`ShellAccountingManualTransactionPersistenceCommand` carries:

- `accountId`
- `portfolioId`
- `securityCode`
- `tradeSide`
- `quantityUnits`
- `priceAmountMinor`
- `grossAmountMinor`
- `feeAmountMinor`
- `taxAmountMinor`
- `occurredAt`
- `sourceMemo`
- `requestId`
- `idempotencyKey`

`ShellAccountingManualCashMovementPersistenceCommand` carries:

- `accountId`
- `portfolioId`
- `movementType`
- `amountMinor`
- `occurredAt`
- `sourceMemo`
- `requestId`
- `idempotencyKey`

These DTOs are boundary types only. They do not imply a database write.

## Result DTO Boundary

`ShellAccountingManualEntryPersistenceResult` is disabled by default:

- `accepted=false`
- `success=false`
- `writeImplemented=false`
- `databaseWritten=false`
- `tradeLogWritten=false`
- `cashFactsWritten=false`
- `auditWritten=false`
- `ledgerWritten=false`
- `errorCode=E9001_SERVICE_UNAVAILABLE`
- `status=MANUAL_ENTRY_REPOSITORY_SCAFFOLD_DISABLED`
- `issues` contains `MANUAL_ENTRY_REPOSITORY_SCAFFOLD_WRITE_NOT_IMPLEMENTED`

The result does not contain `tradeLogId`, `cashFactId`, `auditLogId`,
`ledgerId`, or `persistentId`.

## Disabled Repository Boundary

The scaffold exposes:

- `persistManualTransaction(...)`
- `persistManualCashMovement(...)`

Both methods return the disabled / write-not-implemented result. They do not
open SQLite, execute SQL, call SQLite mutation APIs, write facts, call replay,
call broker code, read network/credential/endpoint sources, place orders, or
start automatic trading.

The scaffold is not called by `DataServiceActions.cpp`; TASK-182 validation
wiring still returns validation accepted plus `writeImplemented=false`.

## Non-Changes

TASK-185 does not modify:

- `migrations/001_initial_schema.sql`
- production QML
- production startup
- Presenter / Controller
- `DataServiceActions.cpp`
- `DataServiceActions.h`
- `DataServiceActionRegistrar.cpp`
- TASK-178 validation header/source
- manual entry DataService action behavior
- AccountingEngine replay
- StrategyEngine / MarketEngine

## Forbidden Writes

TASK-185 does not add:

- SQL execution
- SQLite writes
- `trade_log` writes
- cash facts / cash ledger writes
- audit / ledger writes
- persistent ids
- migration files
- reconciliation / cancellation / correction

## No Broker / Network / Trading Policy

TASK-185 does not add broker SDKs, network calls, credentials, endpoints, real
broker order ids, real order placement, strategy execution, or automatic
trading. Broker sandbox new capability remains paused, and existing broker
disabled, real broker, no-network, no-credentials, no-order-placement, and
no-automatic-trading gates remain retained and passing.

## Future Work

Future repository implementation must be separately authorized. Future
DataService action write implementation, replay/read-model integration, and UI
exposure must also be separately authorized.

Any future write task must restate transaction, rollback, idempotency,
duplicate handling, audit, privacy, no broker, no network, no credentials, no
endpoint, no real order placement, no automatic trading, and schema boundaries.
