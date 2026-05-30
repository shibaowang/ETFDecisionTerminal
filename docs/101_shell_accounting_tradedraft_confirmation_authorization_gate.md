# ShellAccounting TradeDraft Confirmation Authorization Gate

TASK-150 update: the authorization gate has been exercised by the
DataService-internal `accounting.tradedraft.confirm` implementation. The
allowed implementation path is now only authorized TradeDraft confirmation with
`TASK-150_TRADEDRAFT_CONFIRM`, atomic `trade_execution_group` / `trade_log`
ledger write, confirmed `trade_draft` status metadata, and sanitized
`audit_log`. Broker order, strategy execution, automatic trading, production
trading UI, schema changes, and Shell/QML direct ledger writes remain
unauthorized.

## Document Purpose

TASK-149 defines the authorization gate for any future ShellAccounting
TradeDraft confirmation / TradeLog write implementation. This document does
not implement TradeDraft confirmation, does not write `trade_log`, does not
write `trade_execution_group`, does not modify `trade_draft` behavior, does
not call broker code, does not execute strategies, and does not modify
production QML or production startup.

## Current Status

TASK-119 through TASK-148 are complete. DataService internally supports
read-only facts query, AccountingEngine read-only replay, snapshot rebuild
preview, snapshot write, audit write, and TradeDraft creation. Current allowed
writes are `cash_snapshot`, `position_snapshot`, `portfolio_summary`,
`audit_log`, and `trade_draft`.

TradeDraft confirmation, `trade_log` write, `trade_execution_group` write,
broker order, strategy execution, automatic trading, order placement, and
production trading UI remain unauthorized.

## Future Confirmation Boundary

TradeDraft confirmation implementation must be a separate explicitly
authorized TASK. Confirmation is the high-risk boundary from draft to ledger
and must not be implemented by this gate. Future confirmation must be
DataService-only, allowlisted, statically scannable, audited, transactional,
idempotent, and rollback-ready.

future confirmation implementation must be a separate implementation task.

## TradeDraft Confirmation Definition

- Confirmation is the first point where a draft may enter the factual ledger.
- Confirmation is not broker order.
- Confirmation is not automatic trading.
- Confirmation is not strategy execution.
- Confirmation must not bypass user confirmation.
- Confirmation must not be triggered directly by QML, Shell, or Presenter.

## Ledger Policy

`trade_log` is the factual ledger. Future `trade_execution_group` and
`trade_log` writes must be atomic. Partial ledger write is forbidden. Future
confirmation must not bypass draft status and must prevent duplicate
confirmation of the same draft.

trade_log is the factual ledger, and trade_execution_group and trade_log must
be written atomically by any future authorized confirmation implementation.

## DataService-Only Policy

Future confirmation can only be implemented through the DataService boundary.
QML, Shell, Presenter, ShellServices, and ShellCore must not directly write
`trade_log` or `trade_execution_group`. DataServiceApi and DataServiceClient
must not expose a generic write escape hatch. Future confirmation must require
explicit authorization.

## Input Policy

Future confirmation input must come from an existing unconfirmed TradeDraft.
The implementation must validate draft status, account, portfolio, instrument,
side, and quantity. UI raw payload must not directly become a ledger fact, and
facts must not be faked.

## Authorization Policy

Future implementation must require an explicit authorization token. Missing
authorization must fail. A disabled confirmation state must fail. No path may
report silent success.

## Audit Policy

Future confirmation must emit a sanitized audit event. Audit failure must
trigger rollback. Audit is not a replacement for trade execution and does not
stand in for ledger write.

## Forbidden Policy

Future confirmation authorization does not allow broker order, strategy
execute, automatic trading, production trading UI, direct QML write, schema
migration, or generic write actions.

## Error Mapping Policy

Future implementation must map authorization missing, confirmation disabled,
draft missing, draft already confirmed, draft canceled, stale draft, invalid
draft payload, ledger write authorization missing, audit unavailable,
duplicate confirmation, transaction rollback, DB write failure, and no silent
success.

## Idempotency / Duplicate Policy

Future confirmation must define a deterministic idempotency key before writing
ledger rows. Duplicate confirmation must not duplicate `trade_log` or
`trade_execution_group` rows and must not submit broker orders.

## Privacy Policy

Future confirmation diagnostics and payloads must not expose raw SQL, full
`trade_log` payloads, full TradeDraft payloads, internal exception stacks,
broker credentials, or sensitive raw payload. QML-facing errors must remain
sanitized.

## Rollback / Disable Policy

Confirmation must be disableable. Disabling confirmation must keep TradeDraft
creation, audit write, snapshot write, snapshot preview, replay, and read-only
facts query unaffected. Fallback must not fake confirmation, must not write
ledger rows, must not execute a strategy, and must not submit a broker order.

## Future Implementation PR Requirements

A future confirmation implementation PR must list the action name,
authorization token, draft input contract, ledger write contract, transaction
plan, audit behavior, idempotency key, duplicate handling, no-broker evidence,
no-strategy evidence, rollback strategy, schema impact evidence, and all gate
test results.
