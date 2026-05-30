# ShellAccounting TradeDraft Authorization Gate

## Document Purpose

TASK-147 defines the gate for any future ShellAccounting TradeDraft generation
or `trade_draft` write implementation. This document does not implement
TradeDraft, does not write `trade_draft`, does not write `trade_log`, does not
write `trade_execution_group`, does not generate trade suggestions, does not
execute strategy, does not submit broker orders, and does not modify production
QML or production startup.

## Current State

TASK-119 through TASK-146 are complete. ShellAccounting production QML is still
read-only. DataService has read-only facts query, read-only replay, snapshot
rebuild preview, snapshot write, and sanitized audit write. The current
allowlisted write tables are `cash_snapshot`, `position_snapshot`,
`portfolio_summary`, and `audit_log`. TradeDraft generation, `trade_draft`
write, `trade_log` write, `trade_execution_group` write, trade suggestions,
strategy execution, broker order, order placement, and trading UI remain
unauthorized and unimplemented.

## Future TradeDraft Authorization Boundary

TradeDraft implementation must be a separate explicitly authorized TASK. Future
TradeDraft must be DataService-only, allowlisted, statically scannable, and
test-covered. QML / Shell / Presenter must not write `trade_draft` directly and
must not receive a generic write escape hatch.

## TradeDraft Is Not Execution Policy

- TradeDraft is not trade execution.
- TradeDraft is not strategy execution.
- TradeDraft is not broker order.
- TradeDraft does not write `trade_log`.
- TradeDraft does not write `trade_execution_group`.
- TradeDraft does not trigger broker order.
- TradeDraft does not represent user confirmation.
- TradeDraft must not bypass user confirmation.

## DataService-Only Policy

Future TradeDraft can only be created through the DataService boundary. QML,
ShellServices, ShellCore, and ShellAccountingPresenter must not directly write
`trade_draft`, must not call a draft repository, and must not expose a generic
write-enabled escape hatch. Any future DataService action must be named,
allowlisted, authorization-guarded, and covered by static source scans.

## Input Policy

Future candidate input may come from an authorized snapshot, replay, or
accounting summary. It must not come from unauthorized strategy output, raw UI
payload, or fake facts. Source snapshot id, source replay id, account,
portfolio, instrument, side, quantity, reason, and risk flags must be explicit
and validated.

## Authorization Policy

Future TradeDraft implementation must require an explicit authorization token.
Missing authorization must fail. Invalid authorization must fail. Disabled
TradeDraft must fail. No path may report silent success.

## Payload Policy

Future TradeDraft payloads must be sanitized and contain only approved fields:
account, portfolio, instrument, side, quantity, reason, source snapshot id,
source replay id, and risk flags. Payloads must not include raw SQL, full
`trade_log` payload, full snapshot payload, internal stack trace, sensitive raw
payload, or unvalidated UI data.

## Audit Policy

Future TradeDraft must emit a sanitized audit event. Audit failure policy must
be explicit before implementation. Audit must not be used as trade confirmation
and must not imply execution, broker order, strategy execution, or user
approval.

## Forbidden Policy

Future TradeDraft authorization does not allow broker order, strategy execute,
trade execution, trading UI, `trade_log` write, or `trade_execution_group`
write. It also does not authorize schema migration, direct shell-side database
access, or generic write actions.

## Error Mapping Policy

Future implementation must map authorization missing, TradeDraft disabled,
input unavailable, source snapshot stale, replay unavailable, invalid
instrument, invalid side, invalid quantity, risk check failed, audit
unavailable, DB write authorization missing, duplicate draft, rollback, and no
silent success.

## Idempotency / Duplicate Policy

Future TradeDraft must define idempotency keys before writing. Duplicate draft
handling must be deterministic, must not create accidental duplicate orders,
and must not treat a duplicate as trade execution.

## Privacy Policy

Future TradeDraft diagnostics and payloads must not expose raw SQL, full
`trade_log` payload, full snapshot payload, internal exception stack, broker
credentials, or sensitive raw payload. QML-facing errors must remain sanitized.

## Rollback / Disable Policy

TradeDraft must be disableable. Disabling TradeDraft must keep read-only facts,
read-only replay, snapshot preview, snapshot write, and audit write unaffected.
Fallback must not fake a draft, must not execute a trade, must not execute a
strategy, and must not submit a broker order.

## Future Implementation PR Requirements

A future TradeDraft implementation PR must list the action name, authorization
token, input source, payload contract, audit behavior, idempotency key,
duplicate handling, no-trade-execution evidence, no strategy/broker evidence,
rollback strategy, and all gate test results.

## TASK-148 Implementation Status

TASK-148 implements the authorized DataService-internal TradeDraft creation
path as `accounting.tradedraft.create` with `TASK-148_TRADEDRAFT_WRITE`.
The gate has evolved from no TradeDraft yet to authorized TradeDraft creation
only. TradeDraft remains draft-only: it is not trade execution, not strategy
execution, not broker order, and not user confirmation.

TASK-148 may write only `trade_draft` and a sanitized `audit_log` event.
`trade_log`, `trade_execution_group`, broker order, strategy execution,
trade execution, production trading UI, and schema migration remain
unauthorized.
