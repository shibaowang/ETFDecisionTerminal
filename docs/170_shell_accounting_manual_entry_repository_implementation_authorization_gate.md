# TASK-186 ShellAccounting Manual Entry Repository Implementation Authorization Gate

## Document Purpose

TASK-186 is a manual entry repository implementation authorization gate only.
It does not implement a repository write path, does not execute SQL, does not
write SQLite, and does not modify production DataService, QML, startup,
validation, replay, broker, strategy, or schema code.
TASK-186 does not write SQLite.

This gate defines the boundary for a future DataAccess repository
implementation that may persist manual buy / sell records and manual deposit /
withdrawal records after a separate implementation task is explicitly
authorized.

## Current State

TASK-185 added the disabled-only DataAccess scaffold:

- `ShellAccountingManualEntryRepositoryScaffold`
- `ShellAccountingManualTransactionPersistenceCommand`
- `ShellAccountingManualCashMovementPersistenceCommand`
- `ShellAccountingManualEntryPersistenceResult`

The TASK-185 scaffold continues to return disabled / write-not-implemented. It
does not write any database table and does not return persistent identifiers.
TASK-182 validation wiring continues to return validation accepted plus
`writeImplemented=false` for valid manual entry payloads.

TASK-186 does not modify the TASK-185 scaffold header or source, does not modify
`DataServiceActions.cpp`, `DataServiceActions.h`, or
`DataServiceActionRegistrar.cpp`, and does not modify TASK-178 validation
production code.

## Future Repository Implementation Boundary

A future manual entry repository implementation must be a separate authorized
TASK. It may only proceed after confirming that the existing schema is adequate
for the target write semantics. If new tables, columns, indexes, or constraints
are required, a separate schema authorization TASK must happen first.

Future implementation must use a DataAccess repository boundary. SQL must not
be scattered inside DataService action handlers. The future repository must
define transaction, rollback, idempotency, duplicate handling, failure
atomicity, audit, and privacy policies before any write path is enabled.
Future implementation must define failure atomicity.

Manual transaction repository implementation is not broker order implementation.
Manual cash movement repository implementation is not broker transfer implementation.

## No Write Policy For TASK-186

TASK-186 does not add:

- SQL execution.
- SQLite write calls.
- `INSERT`, `UPDATE`, `DELETE`, or `REPLACE` execution.
- `trade_log` writes.
- cash facts or cash ledger writes.
- audit or ledger writes.
- persistent ids such as `tradeLogId`, `cashFactId`, `auditLogId`, or
  `ledgerId`.
- repository implementation classes beyond the existing disabled scaffold.

`migrations/001_initial_schema.sql` must not be modified, and no migration may
be added in this task.

## DataService Boundary

TASK-186 does not modify manual entry DataService action behavior. The current
action wiring remains validation-only and unavailable for persistence. Future
write implementation must keep DataService-only coordination and must not
expose a generic write escape hatch.

## Privacy And Audit Policy

Future repository implementation must sanitize memo and source payloads before
logging or auditing. It must not expose raw SQL, raw manual entry payloads,
internal exception stacks, credentials, endpoints, or sensitive broker data.

Future audit behavior must be defined before write enablement. Audit failure
policy must be explicit, and partial facts must not be committed after any
write failure.

## Broker And Trading Policy

This task keeps broker sandbox work paused. Pausing sandbox work does not remove
or weaken existing broker gates.

TASK-186 does not add broker SDKs, network calls, credentials, endpoints, real
broker order ids, real order placement, reconciliation, cancellation,
correction, strategy execution, or automatic trading.

Existing broker disabled, fail-closed, no-real-order, no-network,
no-credentials, and no-order-placement gates must remain present and passing.

## Required Future Gates

Before any future repository implementation:

- TASK-177 through TASK-186 gates must remain passing.
- TASK-185 scaffold tests must remain passing.
- TASK-184 and TASK-183 authorization gates must remain passing.
- TASK-182 validation wiring must remain passing until write behavior is
  separately authorized.
- Full CTest must pass.
- `transport_local_socket_echo` must pass 50 repeats.
- `git status --short` must be clean.
- User authorization for repository implementation must be explicit.
