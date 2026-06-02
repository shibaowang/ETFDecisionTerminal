# TASK-184 ShellAccounting Manual Entry Repository Scaffold Authorization Gate

## Document Purpose

TASK-184 is a manual entry repository scaffold authorization gate only. It
defines the future authorization boundary for a DataAccess repository scaffold
that may later support manual buy / sell records and manual deposit /
withdrawal cash movement records.

This document does not implement a repository scaffold, does not add
repository headers or sources, does not modify DataAccess production code, does
not modify DataAccess CMake, does not modify DataService action code, does not
write SQLite, does not modify schema, does not modify QML or startup, and does
not enable broker, network, endpoint, credential, real order placement, or
automatic trading behavior.

For static gate wording, TASK-184 does not add repository headers or sources.

## Current State

TASK-178 provides in-memory validation for manual transaction and cash movement
DTOs. TASK-180 registered disabled DataService action scaffolds. TASK-182 wired
payload parsing and validation into the existing actions while keeping writes
unavailable. TASK-183 added the persistence authorization gate.

The current actions still return validation accepted plus write not implemented
for valid payloads. The response must keep `writeImplemented=false`, must not
return a persistent id, and must not write `trade_log`, cash facts, cash ledger,
audit, ledger, or any SQLite table.

## Current Non-Changes

TASK-184 does not modify:

- `libs/DataServiceApi/src/DataServiceActions.cpp`
- `libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h`
- `libs/DataServiceApi/src/DataServiceActionRegistrar.cpp`
- `libs/DataServiceApi/include/DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h`
- `libs/DataServiceApi/src/ShellAccountingManualTransactionCashMovementValidation.cpp`
- `libs/DataAccess/CMakeLists.txt`
- `migrations/001_initial_schema.sql`
- production QML, startup, Presenter, or Controller code

TASK-184 does not add a DataService action implementation, a DataAccess write
repository, a manual entry repository scaffold, repository header/source files,
SQL, SQLite writes, migrations, replay changes, read models, UI, broker SDKs,
network calls, credentials, endpoints, real broker order ids, order placement,
reconciliation, cancellation, correction, or automatic trading.

## Future Repository Scaffold Boundary

A future repository scaffold must be separately authorized before any
DataAccess repository interface, command, result, mapper, or adapter boundary is
introduced for manual entry persistence.

The first future scaffold task may define only interface / command / result
boundaries. It must not write a database and must not make valid payloads look
persistent. Repository implementation must be a separate task. Write
implementation must be another separate task.

Future manual transaction repository scaffold is not a broker order repository.
Future manual cash movement repository scaffold is not a broker transfer
repository. Neither scaffold may call broker, network, credential, endpoint,
real order placement, or automatic trading code.

For static gate wording, Future manual cash movement repository scaffold is not a broker transfer repository.

## DataService-Only Repository Policy

Future repository scaffold must be DataService-only. QML, Presenter,
Controller, ShellServices, and ShellCore must not directly use a manual entry
repository scaffold, DataAccess write repository, SQLite write path, or raw SQL
path.

Any future repository scaffold must be consumed only through an allowlisted
DataService action path. A generic write-enabled escape hatch is forbidden.

## Validation-First Policy

Future repository scaffold must preserve validation-first semantics. It must
consume the TASK-182 structured payload mapping and TASK-178 validation result,
or a separately authorized successor. It must not accept unvalidated raw JSON,
raw UI payloads, or forged facts.

Validation success is not persistence success. Valid payloads must continue to
return write not implemented until a later write implementation task explicitly
authorizes durable effects.

## Transaction / Rollback / Idempotency Policy

Future repository scaffold must document how later implementation will define
transaction boundaries, rollback behavior, idempotency keys, duplicate
detection, and duplicate response semantics.

Partial writes are forbidden. Duplicate manual transactions or cash movements
must not create repeated durable facts. Rollback must preserve the TASK-182
validation-only fallback.

## Audit / Privacy Policy

Future repository scaffold must define sanitized audit and privacy boundaries
for later implementation. It must not log or return raw SQL, full sensitive
memo values, credentials, endpoints, secret-store material, raw broker payloads,
internal stack traces, or full unredacted manual entry payloads.

Audit write behavior must remain separately authorized before use.

## Schema Policy

Future repository scaffold must first verify whether existing schema is
sufficient. If schema support is insufficient, a separate schema authorization
task is required. `migrations/001_initial_schema.sql` must not be modified by
TASK-184.

## No Broker / Network / Trading Policy

The first phase still does not connect to a real broker, does not add broker
SDKs, does not use networks, does not read credentials, does not add endpoints,
does not store real broker order ids, does not place real orders, does not
execute strategies, and does not add automatic trading.

Broker sandbox new capability remains paused. Pausing broker sandbox work does
not remove existing broker gates. Existing broker disabled, fail-closed,
real-broker, no-network, no-credentials, no-order-placement, and
no-automatic-trading gates must remain present and passing.

## Future Task Split

Future manual entry persistence must remain split into separately authorized
tasks:

1. Repository scaffold authorization and interface / command / result boundary.
2. Repository implementation without enabling writes.
3. Manual transaction write implementation.
4. Manual cash movement write implementation.
5. Replay / read model integration.
6. QML / UI exposure.

Each task must restate no broker, no network, no credentials, no endpoint, no
real order placement, no automatic trading, no schema drift, transaction,
rollback, idempotency, audit, and privacy boundaries.

## TASK-185 Authorized Scaffold Evolution

TASK-185 implements the first authorized disabled repository scaffold after this
gate. The allowed scaffold is limited to command DTOs, a result DTO, and
disabled `persistManualTransaction(...)` / `persistManualCashMovement(...)`
methods in `ShellAccountingManualEntryRepositoryScaffold`.

TASK-185 still does not authorize repository write implementation, SQL
execution, SQLite writes, `trade_log`, cash facts, cash ledger, audit, ledger,
persistent ids, DataServiceActions changes, QML/startup changes, replay,
broker, network, credentials, endpoints, real order placement, or automatic
trading.
