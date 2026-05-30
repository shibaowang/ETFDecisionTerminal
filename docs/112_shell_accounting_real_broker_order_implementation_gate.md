# ShellAccounting Real Broker Order Implementation Gate

## Document Purpose

TASK-156 defines the final engineering gate before any ShellAccounting real
broker order implementation can be added.

This document is a gate only. It does not implement real broker order, does not
add a broker adapter, does not import or link a broker SDK, does not call a
broker, does not call sandbox or paper trading broker endpoints, does not place
a real order, does not execute strategy, does not enable automatic trading,
does not modify production QML, does not modify production startup, does not
change TASK-148, TASK-150, TASK-152, or TASK-154 behavior, and does not modify
schema.

## Current State

TASK-119 through TASK-155 are complete. Production ShellAccounting UI can
create, review, and confirm TradeDrafts through the DataService boundary.
TASK-154 provides only `accounting.broker_order.dry_run` with
`TASK-154_BROKER_ORDER_DRY_RUN`; it writes only sanitized `audit_log` and
returns dry-run / broker-disabled semantics. TASK-155 adds the real broker
order authorization gate.

Real broker order implementation, broker adapter, broker SDK, sandbox broker
call, paper trading broker call, real order placement, strategy execution, and
automatic trading remain unauthorized and unimplemented.

## Implementation Gate Summary

Future real broker order implementation must be a separate TASK. The future
implementation must default disabled, isolate sandbox / paper / real modes,
route only through DataService, require successful TASK-154 dry-run result,
require confirmed TradeDraft / ledger fact input, require final user
confirmation, require an independent authorization token, require a kill switch,
require risk preflight, require an idempotency key, write sanitized audit, and
define post-order reconciliation.

This TASK does not authorize any broker adapter, broker SDK, broker network
call, or real order placement.

## Required Pre-gates

Future real broker order implementation must prove:

- TASK-148 TradeDraft create is complete.
- TASK-150 TradeDraft confirm is complete.
- TASK-152 production trading UI is complete.
- TASK-154 broker order dry-run implementation is complete.
- TASK-155 real broker order authorization gate is complete.
- full CTest passes.
- transport 50 repeat passes.
- git status is clean.
- TASK-156 real broker order implementation gate passes.
- user explicitly authorizes real broker order implementation.

## Future Implementation Shape Policy

Future real broker order implementation must be DataService-only. The broker
adapter must sit behind DataService. QML must not directly call broker code.
Presenter must not directly call broker code. ShellServices and ShellCore must
not gain hidden background order placement. DataAccess must not directly call
broker code.

Any future action must be typed, allowlisted, authorization-gated,
default-disabled, kill-switchable, auditable, idempotent, and statically
scannable.

## Sandbox / Paper / Real Isolation Policy

Future implementation must define distinct mode, distinct credentials, distinct
audit marker, and no accidental real order in tests. CI network calls disabled
is mandatory. Tests must not call sandbox, paper, or real broker endpoints.
Sandbox / paper support must be isolated from real mode and must not share
credentials or audit markers.

TASK-156 does not implement sandbox or paper broker calls.

## Authorization Policy

Future real broker order requires:

- independent token
- final user confirmation
- dry-run success required
- confirmed ledger required
- disabled state fails closed
- authorization missing fails
- no silent success

TASK-154 dry-run authorization is not sufficient for real placement.

## Risk Preflight Policy

Future real broker order requires:

- account allowlist
- instrument allowlist
- side / quantity / price validation
- max notional guard
- stale quote guard
- market hours guard
- duplicate guard
- idempotency key
- kill switch

Risk preflight failure must fail closed and must not produce a fake broker
success.

## Broker Adapter Policy

Future broker adapter must use an explicit interface only. No concrete SDK in
QML, Shell, Presenter, ShellServices, ShellCore, or DataAccess. No direct
network call in tests. No credentials in source. No credentials in docs. No
credentials in tests. No credentials in QML. No credentials in logs.

Real broker SDK / adapter / network call must not appear in TASK-156.

## Audit Policy

Future real broker order audit must include sanitized order intent, dry-run
reference, confirmed ledger reference, user confirmation reference, idempotency
key, broker mode marker, authorization result, risk preflight result, and mapped
broker state. Audit must not expose raw broker payload, credentials, raw SQL,
full `trade_log` payload, full TradeDraft payload, full dry-run payload, or
internal exception stack traces.

## Reconciliation Policy

Future implementation must map accepted / rejected / pending / unknown broker
states. Broker order id storage requires explicit policy. Fill reconciliation
requires separate authorization. Correction / cancel flow requires separate
authorization. Unknown broker state must not be silent success and must require
reconciliation.

## Error Mapping Policy

Future implementation must map:

- disabled
- authorization missing
- dry-run missing
- confirmed ledger missing
- final confirmation missing
- risk guard failed
- duplicate order
- credentials unavailable
- broker unavailable
- broker rejected
- timeout
- unknown broker state
- audit failure
- reconciliation required
- rollback
- no silent success

## Rollback / Kill Switch / Incident Containment Policy

Future implementation must include rollback / kill switch / incident containment
policy. It must support disabling real placement without disabling TradeDraft
create, TradeDraft confirm, production trading UI, TASK-154 dry-run, or
sanitized audit. Incident containment must avoid blind retries, strategy
fallback, automatic trading fallback, fake broker success, and background order
submission.

## Forbidden Policy

TASK-156 forbids:

- no real order implementation in TASK-156
- no broker adapter implementation
- no broker SDK
- no broker network call
- no sandbox/paper broker call
- no strategy execution
- no automatic trading
- no schema migration
- no credentials or secrets
- no production QML changes
- no production startup changes
- no TASK-148 / TASK-150 / TASK-152 / TASK-154 behavior changes

## Future Implementation PR Requirements

A future real broker order implementation PR must include:

- explicit user authorization for implementation
- action name and independent real-order authorization token
- default-disabled behavior
- DataService-only boundary proof
- broker adapter interface and mode isolation proof
- TASK-154 dry-run success preflight
- confirmed ledger input validation
- final user confirmation requirement
- risk preflight implementation
- idempotency key / duplicate prevention
- sanitized audit behavior
- reconciliation policy and tests
- rollback / kill switch / incident containment tests
- proof that CI does not call broker network endpoints
- proof that credentials are not committed
- no direct QML / Presenter broker access
- no StrategyEngine execution
- no automatic trading
- full CTest, ShellAccounting gates, TASK-155 gate, TASK-156 gate, broker
  dry-run tests, and transport 50-repeat results

## TASK-157 Broker Adapter Interface Scaffold Update

TASK-157 adds only the abstract `ShellAccountingBrokerOrderPort` / DTO scaffold
inside DataServiceApi, plus a disabled/null implementation and test-only fake
adapter coverage. It does not implement a concrete real broker adapter, does
not import or link a broker SDK, does not call sandbox / paper / real broker
endpoints, does not place real orders, does not add credentials, and does not
change production QML/startup, TASK-148 / TASK-150 / TASK-152 / TASK-154
behavior, StrategyEngine execution, automatic trading, or schema.
# TASK-158 Update

TASK-158 adds a separate broker adapter implementation gate after the TASK-157
interface scaffold. Real broker order implementation remains unimplemented:
there is no concrete broker adapter, no SDK, no network call, no real order id
storage, no reconciliation implementation, no cancellation/correction flow, no
strategy execution, and no automatic trading.
