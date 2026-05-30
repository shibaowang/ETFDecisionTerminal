# ShellAccounting Real Broker Order Authorization Gate

## Document Purpose

TASK-155 defines the authorization gate before any ShellAccounting real broker
order / real order placement implementation can be added.

This document is a gate only. It does not implement real broker order, does not
add a broker adapter, does not import or link a broker SDK, does not call a
sandbox broker, does not call a paper trading broker, does not place an order,
does not execute strategy, does not enable automatic trading, does not modify
production QML, does not modify production startup, and does not change
TASK-148, TASK-150, TASK-152, or TASK-154 behavior.

## Current State

TASK-119 through TASK-154 are complete. Production ShellAccounting UI supports
TradeDraft create / review / confirm through the
presenter/controller/adapter/DataService boundary. TradeDraft confirmation can
write `trade_execution_group`, `trade_log`, `audit_log`, and confirmation
metadata on `trade_draft`.

TASK-154 broker order dry-run implementation is complete. The only
broker-order-related DataService action is `accounting.broker_order.dry_run`,
authorized by `TASK-154_BROKER_ORDER_DRY_RUN`. It writes only a sanitized
`audit_log` event and returns dry-run / broker-disabled semantics.

Real broker SDK integration, real broker adapters, sandbox broker calls, paper
trading broker calls, real order placement, strategy execution, and automatic
trading remain unauthorized and unimplemented.

## Real Broker Order Implementation Gate Summary

Future real broker order implementation must be a separate TASK with explicit
user authorization; real broker order implementation must be a separate TASK.
Real broker order must use a successful TASK-154 dry-run result
as a pre-gate, must be based on a confirmed TradeDraft / ledger fact, must not
accept raw QML payload as an order, must stay inside the DataService boundary,
and must require an independent authorization token. final user confirmation required before any future placement.
The future boundary has dry-run result required and confirmed ledger input.

This TASK does not authorize real order placement.

## Required Pre-gates

Future real broker order implementation must prove:

- TASK-148 TradeDraft create is complete.
- TASK-150 TradeDraft confirm is complete.
- TASK-152 production trading UI is complete.
- TASK-153 broker order authorization gate is complete.
- TASK-154 broker order dry-run implementation is complete.
- full CTest passes.
- transport 50 repeat passes.
- git status is clean.
- TASK-155 real broker order authorization gate passes.
- user explicitly authorizes real broker order implementation.

## Real Order Boundary

Future real broker order must be DataService-only. It can only use confirmed
ledger input and must require a dry-run result. It must require final user
confirmation and an independent authorization token. It must not use QML raw
payload as order input, must not bypass TASK-148 TradeDraft creation or
TASK-150 TradeDraft confirmation, and must not auto-trigger from the UI or a
background worker.

## Broker Adapter Policy

Future broker adapter work must not expose broker SDK calls to QML, Presenter,
ShellServices, ShellCore, or DataAccess. The policy is no QML direct broker access,
no Presenter direct broker access, no DataAccess direct broker access, no hidden
plugin registration, and no background order placement.

Any future broker adapter must be owned behind a DataService action with typed
input, static scans, explicit authorization, kill switch behavior, sanitized
audit, and rollback / incident containment.

## Risk Control Policy

Future real broker order must validate:

- market / account / instrument state
- side / quantity / price
- max quantity / max notional guard
- stale quote guard
- market hours guard
- duplicate order guard
- idempotency key
- kill switch
- disabled switch

Risk guard failure must fail closed. It must not produce silent success or a
fake broker order result.

## Audit Policy

Future real broker order must write sanitized audit containing only safe order
intent, dry-run reference, confirmed trade reference, user confirmation
reference, authorization result, and mapped broker outcome. Audit must not log
raw broker payload, credentials, raw SQL, full `trade_log` payload, full
TradeDraft payload, full dry-run payload, or internal exception stack traces.

Audit failure policy must be explicit before any real broker placement is
allowed.

## Reconciliation Policy

Future real broker order must define accepted / rejected / pending / unknown
broker states. Broker order id storage must be separately authorized. Fill
reconciliation must be separately authorized. Correction flow must be separately
authorized. Unknown broker state must require reconciliation and must not be
reported as success.

## Forbidden Policy

The following remain forbidden in TASK-155:

- no strategy execution
- no automatic trading
- no broker order in this TASK
- no broker adapter in this TASK
- no broker SDK in this TASK
- no sandbox broker call
- no paper trading broker call
- no real order placement
- no schema migration in this TASK
- no write to broker real order status tables

## Error Mapping Policy

Future real broker order implementation must map:

- broker disabled
- authorization missing
- dry-run missing
- confirmed trade missing
- final confirmation missing
- stale quote
- market closed
- risk guard failed
- duplicate order
- broker unavailable
- broker rejected
- timeout
- unknown broker state
- audit failure
- reconciliation required
- rollback
- no silent success

## Rollback / Incident Containment Policy

Future real broker order must include a kill switch, disabled switch, dry-run
fallback, incident containment plan, duplicate prevention, and rollback policy.
Disabling real broker order must keep TradeDraft create, TradeDraft confirm,
production trading UI, and TASK-154 dry-run usable. Rollback must not fake
broker success, retry blindly, execute strategy, or place background orders.

## Future Implementation PR Requirements

A future real broker order PR must include:

- explicit user authorization for real broker implementation
- action name and independent authorization token
- DataService-only boundary proof
- successful dry-run preflight requirement
- confirmed ledger input validation
- final user confirmation requirement
- broker adapter boundary proof
- risk controls and idempotency key
- sanitized audit behavior
- reconciliation plan
- kill switch / disabled switch / rollback evidence
- no direct QML / Presenter broker access evidence
- no StrategyEngine execution evidence
- no automatic trading evidence
- no schema migration unless separately authorized
- full CTest, ShellAccounting gates, real broker order gate, broker dry-run
  tests, and transport 50-repeat results

## TASK-156 Implementation Gate Update

TASK-156 adds a dedicated real broker order implementation gate and test plan.
TASK-155 remains the authorization boundary, while TASK-156 adds final static
checks for no real order implementation, no broker adapter, no broker SDK, no
broker network call, no sandbox / paper broker call, no credentials, no
StrategyEngine execution, no automatic trading, and no QML / Presenter direct
broker access. Real broker order placement remains unimplemented and requires a
separate user-authorized implementation TASK.
