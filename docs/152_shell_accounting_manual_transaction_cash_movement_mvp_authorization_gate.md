# TASK-177 ShellAccounting Manual Transaction and Cash Movement MVP Authorization Gate

## Document Purpose

TASK-177 defines the authorization gate for the next local assisted decision MVP
phase of ShellAccounting. The priority now switches from broker sandbox runtime
capability development back to local accounting usability and decision support:

1. Manual buy and sell records.
2. Deposit and withdraw records.
3. TradeLog, cash facts, and position replay stability.
4. Daily holdings, cash, and PnL page usability.
5. Index retracement golden-tower and sniper suggestions.
6. On-exchange and off-exchange buy and sell suggestions.
7. Local backup, export, verification, and package readiness.

This document is gate-only. It does not implement manual transaction entry,
cash movement entry, DataService actions, DataAccess repositories, production
QML, strategy suggestions, TradeDraft generation, broker access, network calls,
credentials, endpoints, real order placement, automatic trading, or schema
changes.

TASK-178 follows this gate by adding only an in-memory DTO / validation
scaffold. TASK-178 still does not add runtime writes, DataService actions,
DataAccess repositories, AccountingEngine replay integration, QML, startup
wiring, broker access, or schema changes.

## Current State

TASK-176 completed disabled/fail-closed sandbox runtime enablement-state wiring
for the broker dry-run path. That work keeps the broker runtime disabled and
keeps the disabled/null provider semantics intact.

Broker sandbox new capability development is paused for this MVP pivot. Existing
broker, real-broker, no-network, no-credentials, no-order-placement, and
disabled-runtime gates remain active and must keep passing.

Broker sandbox new capability development remains paused through TASK-178.

The application currently has ShellAccounting read-only views, accounting
replay, snapshot write, audit write, TradeDraft creation, TradeDraft
confirmation, and production trading UI for draft/confirmation workflows. This
TASK does not alter those production behaviors.

## MVP Authorization Boundary

Future implementation tasks may be proposed for local manual records and cash
movement workflows, but each implementation must be separately authorized. The
first implementation step must stay local and DataService-bound:

- Manual buy and manual sell records must be structured facts, not raw UI
  payloads.
- Deposit and withdraw records must be structured cash facts, not ad hoc SQL.
- DataService actions must be allowlisted and individually testable.
- DataAccess repositories must be explicit write repositories scoped to the
  authorized fact type.
- AccountingEngine replay must consume authorized facts through the existing
  replay path; UI and Presenter code must not call replay directly.
- Production QML must not directly access SQLite, DataAccess, DataServiceClient,
  broker SDKs, credentials, endpoints, or order placement APIs.

## Out of Scope for TASK-177

TASK-177 does not authorize:

- Production QML manual transaction UI.
- Production startup registration.
- Presenter or Controller behavior changes.
- `DataServiceActions.cpp` manual entry actions.
- DataAccess manual write repositories.
- AccountingEngine manual transaction replay behavior changes.
- StrategyEngine or MarketEngine implementation changes.
- TradeDraft or trading suggestion implementation.
- Broker SDK integration.
- Network clients, endpoints, credentials, tokens, keys, passwords, or usable
  credential examples.
- No network client, endpoint, host, port, or URL is authorized by TASK-177.
- Real broker order ids.
- Real order placement.
- Automatic trading.
- Schema or migration changes.

## Broker Pause Policy

Broker sandbox work is paused for new capability development while the local MVP
focuses on manual accounting input and assisted decision quality. The pause does
not delete, weaken, skip, or bypass existing broker safety gates. All existing
broker-related tests continue to enforce no broker SDK, no network, no
credentials, no real order id, no order placement, no strategy execution, no
automatic trading, and no schema change unless a later TASK explicitly
authorizes the exact scope.

The broker pause does not delete, weaken, skip, or bypass existing broker safety gates.
The broker pause preserves no broker SDK, no network, no credentials, no real order id, and no order placement policy.

## Future Manual Transaction Policy

Future manual buy/sell implementation must:

- Use DataService-only boundaries.
- Validate account, portfolio, instrument, side, quantity, price, fees, trade
  timestamp, and reason.
- Reject invalid side, invalid quantity, missing instrument, missing account,
  stale facts, and unsupported currency with explicit error mapping.
- Persist facts only through an explicitly authorized repository.
- Keep TradeLog/cash facts/position replay stable.
- Avoid generating TradeDraft, broker orders, strategy execution, or automatic
  trading unless separately authorized.

## Future Cash Movement Policy

Future deposit/withdraw implementation must:

- Use DataService-only boundaries.
- Validate account, portfolio, currency, amount, movement type, timestamp, and
  reason.
- Validate currency, amount, movement type, timestamp, and reason before any
  future write.
- Reject zero or negative amount, unsupported currency, missing account, stale
  facts, and inconsistent cash with explicit error mapping.
- Persist cash movement facts only through an explicitly authorized repository.
- Feed replay as facts without bypassing DataService or DataAccess boundaries.

## Suggestion Boundary

Index retracement golden-tower/sniper suggestions and on-exchange/off-exchange
buy/sell suggestions remain future assisted decision features. TASK-177 does not
implement suggestions and does not generate TradeDraft. Future suggestion tasks
must be read-only or draft-only as explicitly authorized, must distinguish
suggestion from execution, and must never place orders.

## Privacy and Secret Policy

Future manual transaction and cash movement tasks must not expose raw SQL,
complete TradeLog payloads, complete cash movement payloads, credentials,
tokens, keys, passwords, endpoints, broker account data, or internal exception
stacks to QML, logs, docs, tests, or API responses. Diagnostics must be
sanitized.

## Rollback / Disable Policy

Future manual record and cash movement features must be independently
disableable. Disabling them must keep read-only accounting, replay, snapshot,
audit, TradeDraft, confirmation, and existing broker-disabled behavior
available. Fallbacks must not fake facts, write unauthorized tables, generate
suggestions, call broker SDKs, place orders, or bypass DataService.

Future manual record and cash movement features must be independently disableable.

## Required Pre-Gates for Future Implementation

- TASK-119 through TASK-176.
- TASK-177 gate tests.
- Existing broker, real broker, no-network, no-credentials, and
  no-order-placement gates.
- Full CTest.
- `transport_local_socket_echo` 50 repeat.
- `git diff --check`.
- Clean `git status --short`.
- Explicit user authorization for each implementation scope.
