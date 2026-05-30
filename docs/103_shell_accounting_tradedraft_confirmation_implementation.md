# TASK-150 ShellAccounting TradeDraft Confirmation Implementation

## Purpose

TASK-150 implements ShellAccounting TradeDraft confirmation inside the DataService boundary. The implementation confirms an existing, unconfirmed TradeDraft into the factual ledger and keeps the boundary separate from broker order placement, strategy execution, automatic trading, and production trading UI.

TASK-149 authorization gate is complete. This document records the implementation boundary, tests, rollback policy, and non-goals.

TASK-151 update: production trading UI authorization is now gated separately.
This implementation remains backend-only; production QML still does not expose
TradeDraft creation, TradeDraft confirmation, broker order placement, strategy
execution, or order submission.

## Confirmation Definition

TradeDraft confirmation is a user-authorized ledger write from an existing draft. It is not a broker order, not automatic trading, not strategy execution, and not production trading UI. Confirmation writes the accounting ledger only after validating that the draft exists and remains confirmable.

`trade_log` is the factual ledger. `trade_execution_group` and `trade_log` are written atomically. `trade_draft` is updated only for the confirmed draft status and confirmation metadata.

## DataService-Only Boundary

The DataService action is `accounting.tradedraft.confirm`.

The required authorization token is `TASK-150_TRADEDRAFT_CONFIRM`.

Shell, QML, Presenter, and ShellServices do not write `trade_log`, do not write `trade_execution_group`, and do not trigger confirmation directly. The implementation uses the DataService action boundary and a DataAccess repository.

## Input Draft Policy

Confirmation input references an existing TradeDraft id. The persisted draft is loaded and validated before any ledger write.

Required checks:

- draft exists.
- draft status is `ACTIVE`.
- draft is not confirmed, canceled, expired, or stale.
- account, portfolio, strategy, instrument, side, quantity, and source identity match the persisted draft signature.
- UI raw payload cannot override persisted draft content.
- ledger facts are derived from the confirmed draft, not from fabricated facts.

## Ledger Write Policy

Allowed writes:

- `trade_execution_group`
- `trade_log`
- `audit_log`
- `trade_draft` status / confirmation metadata for the confirmed draft only

Forbidden behavior:

- no broker order.
- no strategy execute.
- no automatic trading.
- no production trading UI.
- no schema migration.
- no generic write escape hatch.

## Transaction / Rollback Policy

`trade_execution_group`, `trade_log`, `trade_draft` status update, and `audit_log` write occur in a single transaction. Partial ledger write is forbidden. If any step fails, including audit write, the full confirmation transaction rolls back.

Rollback keeps TradeDraft creation, snapshot write, audit write, and read-only accounting features unaffected. Disabling confirmation must not fake confirmation, must not call broker fallback, and must not trigger strategy fallback.

## Idempotency / Duplicate Policy

Duplicate confirmation is detected by existing `trade_execution_group` / `trade_log` rows for the draft id. Duplicate confirmation returns a structured duplicate result and does not insert another ledger row.

## Audit Event Policy

Confirmation writes a sanitized audit event with source action, authorization token, draft id, execution group id, trade log id, account, portfolio, instrument, side, quantity, and privacy flags. Audit failure rolls back the ledger write.

The audit event does not contain raw SQL, full `trade_log` payload, full draft payload, internal stack traces, broker payloads, or strategy output.

## Error Mapping Policy

Mapped failures include:

- authorization missing.
- confirmation disabled.
- draft missing.
- draft already confirmed.
- draft canceled.
- stale draft.
- invalid draft payload.
- ledger write authorization missing.
- audit unavailable.
- duplicate confirmation.
- transaction rollback.
- DB write failure.
- no silent success.

## Privacy Policy

Responses and audit payloads expose sanitized identifiers and status only. They do not expose raw SQL, full `trade_log` payload, full draft payload, internal exception stack traces, or sensitive raw payload.

## No Production QML / Startup Changes

TASK-150 does not modify production QML and does not modify production startup. No trading buttons, order controls, broker calls, or strategy execution surfaces are added.

## Tests

TASK-150 adds `tests/ShellAccountingTradeDraftConfirmationImplementation/` and validates:

- implementation/action/token wiring.
- DataService-only boundary.
- authorization required.
- disabled mapping.
- success path.
- draft missing.
- already confirmed duplicate handling.
- canceled draft.
- stale draft.
- invalid payload.
- atomic ledger write.
- audit event required.
- audit failure rollback.
- transaction rollback.
- idempotency.
- duplicate handling.
- writes `trade_log`.
- writes `trade_execution_group`.
- updates `trade_draft` status.
- no broker order.
- no strategy execution.
- no UI trigger.
- privacy.
- rollback readiness.

## Future Work

broker order / strategy execution / trading UI remain unauthorized. Broker order, strategy execution, automatic trading, and production trading UI require separate TASKs.
