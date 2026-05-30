# ShellAccounting TradeDraft Implementation

TASK-150 update: TradeDraft creation remains unchanged. Confirmation is now
implemented separately through `accounting.tradedraft.confirm`, and creation
still does not write `trade_log` or `trade_execution_group`.

## TASK-148 Purpose

TASK-148 implements ShellAccounting TradeDraft creation inside the DataService
boundary. TASK-147 authorization gate is complete. This task creates a
sanitized, audited draft record for later user confirmation only.

## TradeDraft Definition

TradeDraft is draft only. It is not execution, not broker order, not strategy
execution, and not a user-confirmed trade. TradeDraft creation does not write
`trade_log`, does not write `trade_execution_group`, does not submit an order,
and does not create production trading UI.

## DataService-Only Boundary

The action is `accounting.tradedraft.create`. It is allowlisted as a write
action and is handled only inside DataService. Shell, QML, Presenter, and
ShellServices do not directly write `trade_draft` and do not call broker,
strategy, or order code.

The required authorization token is `TASK-148_TRADEDRAFT_WRITE`. Missing
authorization, disabled draft creation, invalid input, unavailable source data,
or audit failure must fail with an explicit error. No path may report silent
success.

## Input Source Policy

Input must be structured DTO data, not a raw JSON escape hatch. The source must
come from an authorized accounting source such as `snapshotRebuildPreview` or
`readonlyReplay`. UI raw payloads are not accepted as facts, facts must not be
faked, and unauthorized strategy output is not an allowed source.

Required fields are account id, portfolio id, strategy id, strategy code,
instrument id, instrument code, side, quantity, reason, source snapshot id or
source replay id, risk flags, and authorization.

## Input Validation Policy

The implementation validates account, portfolio, strategy, and instrument
identity. `side` is allowlisted to `BUY` or `SELL`. Quantity must be positive.
Reason, strategy code, instrument code, authorization, source, and risk flags
must be present. Risk-blocked inputs fail without writing a draft.

Invalid account, portfolio, instrument, side, quantity, missing source,
disabled draft, missing authorization, stale source, unavailable replay, audit
failure, duplicate draft, and rollback paths are mapped explicitly.

## Payload Sanitization Policy

TradeDraft response and audit payloads are sanitized. They must not include raw
SQL, full `trade_log` payload, full snapshot payload, internal stack trace,
broker credentials, or sensitive raw payload. QML-facing errors remain
sanitized through the existing presenter/adapter boundary.

## Allowed Writes

TASK-148 may write only:

- `trade_draft`
- `audit_log`

`audit_log` is written only for a sanitized TradeDraft creation audit event.

## Forbidden Writes

TASK-148 does not write:

- `trade_log`
- `trade_execution_group`
- any broker/order table
- any unapproved table

Schema migrations remain out of scope. `migrations/001_initial_schema.sql` is
unchanged.

## Audit Event Policy

Successful TradeDraft creation must emit a sanitized audit event through the
TASK-146 audit boundary. The audit event records action name, source ids,
draft uid, status, sanitized issue code, timestamp, and redaction flags. Audit
is not trade confirmation and does not authorize execution.

Audit failure rolls back TradeDraft creation. A draft without a matching
sanitized audit event is not committed.

## Transaction / Rollback Policy

The draft write and audit write run in one transaction. If either insert fails,
the transaction rolls back. Partial write is forbidden. rollback behavior must
preserve TASK-144 snapshot write and TASK-146 audit write behavior outside the
failed TradeDraft request.

## Idempotency / Duplicate Policy

Draft creation uses a deterministic draft signature derived from source,
account, portfolio, strategy, instrument, side, quantity, and reason. Duplicate
requests return the existing draft and do not insert another row or another
audit event.

## Error Mapping Policy

The implementation maps authorization missing, TradeDraft disabled, input
unavailable, source snapshot stale, replay unavailable, invalid account,
invalid portfolio, invalid instrument, invalid side, invalid quantity, missing
source, risk check failed, audit unavailable, duplicate draft, transaction
rollback, and no silent success.

## Privacy Policy

No raw SQL, full `trade_log` payload, full snapshot payload, internal exception
stack, broker credentials, or sensitive raw payload is exposed in response,
audit payload, logs, or QML-facing issue text.

## No Production UI Or Startup Changes

TASK-148 does not modify production QML or production startup. It does not add
buy, sell, order, broker, strategy, confirmation, or trading controls.

## No Strategy / Broker / Trading UI

TradeDraft creation does not generate trade suggestions, does not execute
StrategyEngine, does not call broker adapters, does not submit orders, and does
not confirm trades.

## Test Checklist

- `shell_accounting_tradedraft_implementation`
- `shell_accounting_tradedraft_dataservice_only_boundary`
- `shell_accounting_tradedraft_requires_authorization`
- `shell_accounting_tradedraft_disabled_mapping`
- `shell_accounting_tradedraft_success`
- `shell_accounting_tradedraft_input_validation`
- `shell_accounting_tradedraft_invalid_instrument`
- `shell_accounting_tradedraft_invalid_side`
- `shell_accounting_tradedraft_invalid_quantity`
- `shell_accounting_tradedraft_source_snapshot_required`
- `shell_accounting_tradedraft_payload_sanitized`
- `shell_accounting_tradedraft_audit_event_required`
- `shell_accounting_tradedraft_audit_failure_rolls_back`
- `shell_accounting_tradedraft_transaction_rollback`
- `shell_accounting_tradedraft_idempotency`
- `shell_accounting_tradedraft_duplicate_handling`
- `shell_accounting_tradedraft_no_trade_log_write`
- `shell_accounting_tradedraft_no_trade_execution_group_write`
- `shell_accounting_tradedraft_no_broker_order`
- `shell_accounting_tradedraft_no_strategy_execution`
- `shell_accounting_tradedraft_no_ui_trigger`
- `shell_accounting_tradedraft_privacy`
- `shell_accounting_tradedraft_rollback_ready`

## Unauthorized Future Scope

TradeDraft confirmation, trade execution, trade_log writes,
trade_execution_group writes, broker order, strategy-generated recommendation,
production trading UI, and schema migration all require separate future TASK
authorization.

## TASK-149 Confirmation Gate Status

TASK-149 adds the TradeDraft confirmation / TradeLog write authorization gate.
TASK-148 TradeDraft creation behavior is unchanged. Confirmation,
`trade_log` write, `trade_execution_group` write, broker order, strategy
execution, automatic trading, production trading UI, and schema migration
remain unimplemented and unauthorized.
