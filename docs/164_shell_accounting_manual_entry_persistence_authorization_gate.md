# TASK-183 ShellAccounting Manual Entry Persistence Authorization Gate

## Document Purpose

TASK-183 defines the authorization gate for future ShellAccounting manual entry
persistence. It is gate-only. It does not implement persistence, does not add a
repository, does not write SQLite, does not modify schema, does not modify
production QML or startup, and does not change the TASK-182 validation wiring.

This gate covers future persistence for manual buy / sell records and manual
cash deposit / withdrawal records. The current actions still return
validation-only responses with write not implemented.

## Current State

TASK-178 provides in-memory DTO validation for manual transaction and cash
movement payloads. TASK-180 registered the disabled DataService action
scaffolds. TASK-182 wired payload parsing and validation into:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

Those actions remain fail-closed. A valid payload can produce
`validationAccepted=true`, but it must still produce `writeImplemented=false`
and must not return persistence success or a durable id.

## Current Non-Changes

TASK-183 does not modify:

- `libs/DataServiceApi/src/DataServiceActions.cpp`
- `libs/DataServiceApi/include/DataServiceApi/DataServiceActions.h`
- `libs/DataServiceApi/src/DataServiceActionRegistrar.cpp`
- `libs/DataServiceApi/include/DataServiceApi/ShellAccountingManualTransactionCashMovementValidation.h`
- `libs/DataServiceApi/src/ShellAccountingManualTransactionCashMovementValidation.cpp`
- `migrations/001_initial_schema.sql`
- production QML, startup, Presenter, or Controller code

TASK-183 does not add DataService action implementation, DataAccess write
repositories, SQL, migrations, SQLite writes, replay, read models, UI, broker
SDKs, networks, credentials, endpoints, order placement, or automatic trading.

## Future Persistence Boundary

Future manual transaction persistence must be separately authorized before any
validated buy / sell payload can become a durable trade fact such as
`trade_log` or an equivalent transaction fact table.

Future manual cash movement persistence must be separately authorized before
any validated deposit / withdrawal payload can become a durable cash fact or
cash ledger entry.

Manual transaction persistence is not broker order placement. Manual cash
movement persistence is not a broker transfer. Neither path may call broker,
network, credential, endpoint, real order placement, or automatic trading code.

For static gate wording, manual transaction persistence is not broker order
placement, and manual cash movement persistence is not a broker transfer.
manual transaction persistence is not broker order placement.

## DataService-Only Policy

Future persistence must be DataService-only.

Future persistence must happen only inside the DataService boundary. QML,
Presenter, Controller, ShellServices, and ShellCore must not directly write
SQLite, call DataAccess write repositories, or convert raw UI payloads into
durable facts.

Future DataService action handlers must remain allowlisted and testable. They
must not expose a generic write-enabled escape hatch.

## Validation-First Policy

Future persistence must be validation-first.

Future persistence must consume the existing TASK-182 structured payload
mapping and the TASK-178 validation result, or a separately authorized
successor. Persistence must not accept unvalidated raw JSON, raw UI payloads,
or forged facts.

Validation failure must fail closed. Validation success must not imply a write
unless a later persistence implementation task explicitly authorizes the write.

## Repository Boundary Policy

Future write implementation must use an explicit repository boundary. SQL must
not be scattered inside DataService action handlers.

Repository scaffold / DTO mapping and repository implementation must be split
into separately reviewable work before any manual transaction or cash movement
write is enabled.

## Transaction / Rollback Policy

Future persistence must define transaction boundaries before writing any manual
trade or cash fact. Partial writes are forbidden. A failure after validation
must rollback all durable effects and return an explicit error.

Rollback must preserve the TASK-182 validation-only fallback and must not fake
manual facts or durable identifiers.

## Idempotency / Duplicate Policy

Future persistence must define idempotency keys, duplicate detection, and
duplicate response semantics before any write is enabled. Duplicate manual
transactions or cash movements must not create repeated facts.

## Audit Policy

Future persistence must define sanitized audit events before writes are
enabled. Audit events must not contain raw SQL, full sensitive memo values,
credentials, endpoints, raw broker payloads, internal stack traces, or full
unredacted manual entry payloads.

Audit write behavior and audit failure rollback policy must be separately
authorized before use.

## Privacy / Memo Sanitization Policy

Manual entry diagnostics, logs, and future audit payloads must use sanitized
issue codes and field names. Sensitive memo tokens, credentials, keys,
passwords, endpoints, and secret-store material must not be logged or returned.

## Schema Policy

Future persistence must first confirm whether the existing schema is sufficient.
If schema support is insufficient, a separate schema authorization task is
required. `migrations/001_initial_schema.sql` must not be modified by this gate
or by an implementation task that lacks explicit schema authorization.

## No Broker / Network / Trading Policy

Future manual entry persistence must keep no broker, no network, no
credentials, no endpoint, no real broker order id, no order placement, no
strategy execution, and no automatic trading boundaries unless a later task
explicitly authorizes those exact capabilities.

Existing broker disabled, real broker, no-network, no-credentials,
no-order-placement, and no-automatic-trading gates must remain present and
passing.

## Future Task Split

Future manual entry persistence must be split into separate tasks:

1. Repository scaffold / DTO mapping gate.
2. Repository implementation.
3. Manual transaction write implementation.
4. Manual cash movement write implementation.
5. Replay / read model integration.
6. QML / UI exposure.

Each task must restate no broker, no network, no credentials, no endpoint, no
real order placement, no automatic trading, no schema drift, transaction,
rollback, idempotency, audit, and privacy boundaries.

## TASK-184 Follow-Up Gate

TASK-184 adds the future manual entry repository scaffold authorization gate in
docs/166 and docs/167. TASK-183 remains persistence authorization only:
repository scaffold, repository header/source files, DataAccess production code
changes, DataAccess CMake changes, repository implementation, and write
implementation are still not implemented.

TASK-182 validation wiring remains the fallback and continues to return
validation accepted plus write not implemented with `writeImplemented=false`
and no persistent ids. Broker sandbox new capability remains paused, and
existing broker disabled, real broker, no-network, no-credentials,
no-order-placement, and no-automatic-trading gates must remain retained and
passing.
