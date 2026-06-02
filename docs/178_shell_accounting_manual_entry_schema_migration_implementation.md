# ShellAccounting Manual Entry Schema Migration Implementation

## Document Purpose

TASK-190 implements the manual entry schema migration implementation that was
authorized by TASK-189. The implementation adds the independent migration file
`migrations/002_shell_accounting_manual_entry_schema.sql` and keeps
`migrations/001_initial_schema.sql` unchanged.

This task is schema-only. It does not implement a manual entry repository, does
not add a DataService write implementation, does not execute runtime SQL, does
not write SQLite at runtime, does not modify production QML or startup, and does
not change AccountingEngine replay.

TASK-190 does not add a DataService write implementation.
TASK-190 does not write SQLite at runtime.
TASK-190 does not modify production QML or startup.
TASK-190 does not add broker SDKs, network calls, credentials, endpoints, real orders, or automatic trading.

## Migration Scope

The migration is DDL-only and is intended to run after
`migrations/001_initial_schema.sql`. It adds nullable or safe-default columns to
existing tables so existing data can remain compatible.

The migration updates:

- `trade_log` with request tracing, idempotency, occurred-at mapping, tax,
  sanitized memo, and cash adjustment linkage columns.
- `cash_adjustment` with request tracing, idempotency, occurred-at mapping,
  sanitized memo, and trade log linkage columns.
- `audit_log` with request tracing, audit idempotency, payload classification,
  and redaction status columns.

The migration adds stable indexes for request tracing, idempotency, and linkage.
Manual idempotency uniqueness uses partial unique indexes that ignore null or
empty idempotency keys.

## No Initial Schema Edit

`migrations/001_initial_schema.sql` remains the historical source of the initial
schema and is not edited by TASK-190. Future changes must continue through
independent migration files unless a separately authorized schema task states
otherwise.

## No Runtime Write Path

TASK-190 does not add repository implementation, runtime SQL execution, SQLite
write calls, DataService action implementation, or any runtime
`trade_log` / cash facts / audit / ledger write path. Repository implementation
must remain a later separately authorized task.

## Boundary Preservation

TASK-190 does not modify:

- `libs/DataServiceApi/src/DataServiceActions.cpp`
- `libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h`
- `libs/DataServiceApi/src/DataServiceActionRegistrar.cpp`
- TASK-178 validation production code
- TASK-185 repository scaffold production code
- production QML / startup / Presenter / Controller
- AccountingEngine replay
- StrategyEngine / MarketEngine

## Broker And Trading Policy

Broker sandbox new capability development remains paused. Existing broker
disabled, no-real-order, no-network, no-credentials, no-endpoint,
no-order-placement, and no-automatic-trading gates remain retained.

TASK-190 does not add broker SDKs, network calls, credentials, endpoints, real
broker order ids, real orders, automatic trading, TradeDraft implementation, or
trade suggestions.

## Rollback And Forward-Fix Policy

The migration is forward-only. If a future issue is found after deployment, the
allowed repair path is a separately authorized forward-fix migration. TASK-190
does not provide a destructive rollback, does not drop tables, and does not
rename existing tables.

## Compatibility Policy

The migration must be compatible with an empty database after applying
`001_initial_schema.sql` and then `002_shell_accounting_manual_entry_schema.sql`.
It must also be compatible with existing databases that already have the initial
schema but have not received manual entry repository implementation.

Full CTest, read-only demo static checks, manual entry authorization gates, and
broker safety gates remain required.

## Future Scope Not Authorized

TASK-190 does not authorize:

- manual entry repository implementation
- DataService write implementation
- replay changes
- UI changes
- TradeDraft or trading suggestion implementation
- broker sandbox capability development
- broker SDK / network / credential / endpoint integration
- real order placement
- automatic trading

## TASK-191 Post-Migration Gate

TASK-191 adds the manual entry repository implementation post-migration
authorization gate after this TASK-190 migration is present. TASK-191 still
does not implement repository writes, does not modify
`migrations/001_initial_schema.sql`, does not modify
`migrations/002_shell_accounting_manual_entry_schema.sql`, does not change
DataServiceActions, and does not execute runtime SQL or write SQLite.

Future repository implementation must remain a separate TASK and must use the
TASK-190 schema fields through a DataAccess repository boundary. Future
DataService action write implementation, replay, read model, and UI work must
also remain separately authorized.
