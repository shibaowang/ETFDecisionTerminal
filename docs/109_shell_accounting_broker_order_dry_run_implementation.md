# TASK-154 ShellAccounting Broker Order Dry-run Implementation

## Purpose

TASK-154 implements only a DataService broker order dry-run boundary. It is not
real broker order placement, does not call a broker SDK, does not place an
order, does not execute a strategy, and does not enable automatic trading.

TASK-153 authorization gate is complete. This task evolves that gate from
`no broker order yet` to only authorized dry-run broker order boundary.

## DataService Action

- Action: `accounting.broker_order.dry_run`.
- Authorization token: `TASK-154_BROKER_ORDER_DRY_RUN`.
- The action is allowlisted as a write action only because it can write a
  sanitized audit event.
- The action returns explicit `dryRun=true`, `brokerDisabled=true`,
  `dryRunOnly=true`, and `realOrderPlacement=false`.
- It never returns a real broker order id.

## Input Source

Input must reference an already confirmed TradeDraft / ledger fact:

- `source=confirmedTradeDraft` with a confirmed `draftId`, or
- `source=confirmedLedgerFact` with a confirmed `tradeLogId`.

The action validates that the referenced TradeDraft is confirmed and backed by
`trade_execution_group` and `trade_log`. It does not accept raw QML payload as
an order and does not accept UI raw order payload fields.

## Dry-run / Disabled Semantics

The boundary builds an internal order envelope preview from the confirmed
ledger fact and records that the broker remains disabled. This is deliberately
dry-run-only. `brokerOrderDisabled=true` rejects the request, and disabled mode
does not silently succeed.

## Writes

Allowed write table:

- `audit_log` only, for a sanitized broker dry-run audit event.

The allowlist is audit_log only.

Forbidden writes:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- any broker order real status table
- any unapproved table

## No Broker / Strategy / Automatic Trading

TASK-154 does not add broker SDK, broker adapter, real order placement,
StrategyEngine execution, automatic trading, or production QML changes. QML /
Presenter does not directly call broker code.

## Audit Payload Sanitization

The audit payload records action name, authorization token, draft id, ledger
ids, account / portfolio ids, instrument code, side, quantity, dry-run status,
and disabled broker status. It does not expose raw SQL, full trade_log payload,
full TradeDraft payload, full broker payload, sensitive raw payload, or internal
exception stack traces.

## Idempotency / Duplicate Handling

Duplicate dry-run requests for the same confirmed ledger fact reuse the
existing `SHELL_ACCOUNTING_BROKER_ORDER_DRY_RUN` audit event and return a
duplicate/idempotent response instead of writing another audit row.

## rollback / disable strategy

The dry-run action can disable dry-run without affecting existing accounting
features. TradeDraft create / confirm remain unaffected. Disabling dry-run does
not fake broker success, does not write fallback tables, does not call a broker,
and does not execute a strategy.

## Error Mapping

The implementation maps:

- broker disabled
- authorization missing
- confirmed trade missing
- missing confirmed ledger ids
- raw UI payload rejected
- duplicate dry-run
- audit write failure
- DB write failure
- transaction rollback
- no silent success

## Tests

The TASK-154 test suite covers:

- DataService-only boundary
- authorization required
- confirmed trade required
- raw UI payload rejected
- dry-run-only response
- broker disabled semantics
- no broker SDK call
- no real order placement
- no strategy execution
- no automatic trading
- audit required
- sanitized audit payload
- audit failure blocks success
- idempotency / duplicate handling
- no `trade_log`, `trade_execution_group`, or `trade_draft` writes
- rollback-ready documentation

Future real broker SDK / real order placement must be a separate TASK with its
own authorization gate and implementation tests.

## TASK-155 Real Broker Order Gate

TASK-155 adds the real broker order authorization gate. This dry-run boundary
remains unchanged: it still writes `audit_log` only, does not call broker SDK,
does not call sandbox or paper trading broker endpoints, does not place a real
order, does not write `trade_log`, `trade_execution_group`, or `trade_draft`,
does not execute strategy, and does not enable automatic trading.
