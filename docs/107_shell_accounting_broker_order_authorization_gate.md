# ShellAccounting Broker Order Authorization Gate

## Document Purpose

TASK-153 defines the authorization gate before any ShellAccounting broker order
or order placement implementation can be added.

This document does not implement broker order placement. It does not add a
broker adapter, does not call a broker SDK, does not place an order, does not
execute StrategyEngine, does not enable automatic trading, does not modify
production QML, does not modify production startup, and does not change
TASK-148, TASK-150, or TASK-152 behavior.

## Current State

TASK-119 through TASK-152 are complete. Production ShellAccounting UI can create
Drafts, review Draft-only state, and request explicit Draft confirmation through
the presenter/controller/adapter/DataService boundary.

TradeDraft creation uses `accounting.tradedraft.create`. TradeDraft confirmation
uses `accounting.tradedraft.confirm`. Confirmation can write
`trade_execution_group`, `trade_log`, `audit_log`, and confirmation metadata on
`trade_draft`. The UI still does not call DataServiceClient, SQLite, DataAccess,
broker SDKs, or StrategyEngine directly.

Broker order placement, broker adapters, strategy execution, automatic trading,
and real order submission remain unauthorized and unimplemented.

## Future Broker Order Authorization Boundary

Future broker order implementation must be a separate explicitly authorized
TASK. Broker order is a high-risk boundary after a user-visible Draft and
confirmed ledger fact already exist. It is not TradeDraft creation, not
TradeDraft confirmation, not a strategy execution side effect, and not automatic
trading.

Broker order must not accept raw QML payload as an order source. It must not
bypass DataService. It must not be triggered by Presenter, QML, or Shell code
calling a broker SDK directly. It must not auto-trigger after Draft creation or
Draft confirmation.

## DataService-only Broker Policy

Future broker order coordination can only happen inside the DataService
boundary. QML must route through ShellAccounting presenter/controller/adapter and
must not call a broker adapter, broker SDK, DataAccess, SQLite, or
DataServiceClient directly.

DataServiceApi / DataServiceClient must not expose a generic broker or
write-enabled escape hatch. Any future broker order action must be allowlisted,
typed, authorization-gated, privacy-reviewed, rollback-ready, and covered by
static scans and CTest.

## Confirmed TradeDraft / Ledger Input Policy

Broker order can only use an already confirmed TradeDraft / ledger fact as its
input. The future implementation must validate:

- confirmed TradeDraft id
- confirmed-ledger status
- account / portfolio / instrument
- side / quantity
- no stale Draft
- no stale ledger fact
- no duplicate broker order attempt

Broker order must not create a new TradeDraft, must not write raw UI payload to
`trade_log`, and must not bypass TASK-148 TradeDraft creation or TASK-150
TradeDraft confirmation.

## Explicit User Confirmation Policy

Future broker order requires an additional explicit user confirmation step after
ledger confirmation. TradeDraft creation is not broker confirmation.
TradeDraft confirmation is not broker confirmation. The UI must distinguish
Draft-only, confirmed-ledger, and broker-order-intent states.

Broker order must have an independent authorization token in its future
implementation. Authorization missing must fail. Disabled state must fail.
Silent success is forbidden.

## Broker Disabled / Dry-run / Rollback Policy

Future broker order must support:

- broker disabled mode
- dry-run mode
- timeout handling
- broker rejection handling
- duplicate order prevention
- rollback / disable strategy
- no fake broker success

Disabling broker order must keep TASK-152 Draft UI and TASK-150 ledger
confirmation usable. Fallback must not call a broker, execute strategy, place
orders, or fake order placement.

## Forbidden Direct Broker Access Policy

The following remain forbidden unless a future implementation task explicitly
authorizes them:

- QML direct broker SDK calls
- Presenter direct broker SDK calls
- ShellServices direct broker adapter calls outside DataService boundary
- direct QML DataServiceClient broker calls
- raw UI payload promoted to broker order
- generic write or broker escape hatch

## No Strategy / No Automatic Trading Policy

Broker order is not strategy execution. Broker order is not automatic trading.
Future broker order must not generate a new strategy signal, run
StrategyEngine, infer a recommendation, or auto-place an order based on strategy
output. Strategy execution requires a separate authorization gate and
implementation task.

## No UI Raw Payload Order Policy

QML may never submit raw fields directly as a broker order. Future UI must
reference a confirmed Draft / ledger fact and send a typed request through the
ShellAccounting boundary. The DataService side must revalidate the confirmed
input before any broker request is considered.

## Audit Policy

Future broker order must write a sanitized audit event for intent, submission,
failure, and final mapped result. Audit payload must not contain raw SQL, full
`trade_log` payload, full broker request/response payload, API secrets, account
secrets, internal exception stacks, or sensitive raw payloads.

Audit failure policy must be explicit in the future implementation. Audit must
not be used as a substitute for broker confirmation or ledger facts.

## Error Mapping Policy

Future broker order implementation must map these cases explicitly:

- broker disabled
- authorization missing
- confirmed trade missing
- duplicate order attempt
- stale draft / stale ledger
- broker unavailable
- broker rejected
- timeout
- rollback
- no silent success

## Privacy Policy

QML and logs must not expose raw SQL, raw broker payload, broker credentials,
full `trade_log` payloads, full Draft payloads, internal stack traces, or
sensitive raw payloads. Broker diagnostics must be sanitized before reaching UI
or default logs.

## Future Implementation PR Requirements

A future broker order PR must include:

- explicit user authorization for broker order implementation
- action name and independent authorization token
- DataService-only boundary proof
- confirmed TradeDraft / ledger input validation
- dry-run / disabled / rollback policy
- duplicate order prevention
- sanitized audit behavior
- no direct QML / Presenter broker access evidence
- no StrategyEngine execution evidence
- no automatic trading evidence
- no schema migration unless separately authorized
- full CTest, ShellAccounting gates, broker order gate, and transport 50-repeat
  results
