# ShellAccounting Broker Sandbox Adapter Scaffold (TASK-161)

## Document Purpose

TASK-161 adds a scaffold-only ShellAccounting sandbox broker adapter class behind the existing DataServiceApi broker order port. The scaffold is intentionally not a usable broker adapter: it never places an order, never calls a broker network, never reads credentials, never writes a database table, and never returns a real broker order id.

TASK-160 is complete and remains the authorization gate for this work. TASK-161 implements only the minimum production class needed for future sandbox work to compile against a named port implementation while still failing closed.

## Current Status

- TASK-159 disabled/null provider remains the default provider.
- TASK-154 broker order action remains dry-run-only and broker-disabled.
- TASK-160 sandbox adapter authorization gate remains active.
- TASK-161 adds a DataServiceApi sandbox scaffold that returns unavailable / not configured.
- No production QML, startup, presenter, controller, adapter, schema, or runtime DataService action wiring is changed.

## Scaffold-Only Boundary

The sandbox scaffold is not a usable broker adapter. It returns `success=false`, `disabled=true`, `dryRunOnly=true`, and an unavailable error code. It does not create, submit, cancel, correct, reconcile, or track broker orders.

The default provider remains disabled and is not replaced by the sandbox scaffold. Future tasks may wire sandbox mode only after separate authorization, configuration, tests, and rollback policy.
In short, default remains disabled for TASK-161.

## DataService-Only Policy

The scaffold lives behind the DataServiceApi broker order port type. QML, Presenter, ShellServices, ShellCore, and production startup must not instantiate, configure, or call the sandbox scaffold directly. Future runtime wiring must remain inside the DataService boundary and must not expose a generic broker escape hatch.

## Credentials Isolation Policy

TASK-161 does not add credentials, secrets, tokens, keys, passwords, account secrets, endpoint handles, or usable examples. Documentation and tests may mention policy words such as credentials, token, key, password, and network only as forbidden behavior. The scaffold does not read environment variables, settings, files, secret stores, or command line values.

## CI No-Network Policy

CI no-network behavior remains mandatory for broker adapter coverage. TASK-161 tests use direct object checks and static scans only. The scaffold does not import network libraries, open sockets, call broker hosts, or require any reachable service.

## No Broker SDK / No Real Order Placement

TASK-161 imports no broker SDK, links no broker SDK, and calls no broker SDK. It performs no real order placement, returns no broker order id, and stores no external order state.

## No Database / Audit Write Policy

The scaffold performs no database write and no audit write. It does not write `audit_log`, `trade_log`, `trade_execution_group`, `trade_draft`, snapshot tables, or any broker status table. It does not create audit rows; future audit for sandbox runtime behavior must be authorized separately.

## No Reconciliation / Cancellation / Correction

TASK-161 does not implement broker reconciliation, order status polling, cancellation, correction, incident containment, or external order state tracking. Those flows remain separate future tasks.

## No Strategy / Automatic Trading

The scaffold does not execute StrategyEngine, generate a new recommendation, enable automatic trading, or submit orders in the background. This is a no strategy execution and no automatic trading boundary. Sandbox adapter work must not become a strategy execution or automatic trading path.

## Error Mapping

The scaffold maps every request to a sanitized unavailable response:

- sandbox adapter not configured
- success false
- disabled true
- dry-run only true
- no real broker order id
- no raw SQL, raw broker payload, raw trade payload, or internal stack trace

## Rollback / Disable Policy

Rollback is the current default: keep TASK-159 disabled/null provider active. Removing or disabling the sandbox scaffold must not affect TradeDraft creation, TradeDraft confirmation, audit write, snapshot write, read-only replay, or the TASK-154 dry-run boundary.

## Test Checklist

- Scaffold compiles and instantiates.
- Scaffold returns sandbox unavailable / not configured.
- Scaffold never reports success.
- Scaffold returns no broker order id.
- Static scans prove no order placement, no network, no credentials, no broker SDK, and no database write.
- Default disabled provider remains unchanged.
- TASK-160 authorization gate remains valid.
- Production QML, Presenter, ShellServices, ShellCore, and startup do not expose the scaffold.

## Unauthorized Follow-Up Scope

Real sandbox runtime wiring, credential injection, endpoint configuration, broker SDK integration, order placement, broker order id storage, reconciliation, cancellation, correction, strategy execution, automatic trading, and real broker order implementation must each be separately authorized.
