# ShellAccounting Audit Write Authorization Gate

## Document Purpose

TASK-145 defines the authorization gate before any future ShellAccounting audit
write implementation. This document does not implement audit write, does not
write `audit_log`, does not modify production QML, does not modify production
startup, does not modify TASK-144 snapshot write behavior, and does not generate
TradeDraft, trade suggestions, strategy execution, or broker orders.

## Current State

TASK-119 through TASK-144 are complete. DataService has read-only facts query,
AccountingEngine read-only replay, snapshot rebuild preview, and authorized
snapshot write. Current ShellAccounting snapshot write may write only
`cash_snapshot`, `position_snapshot`, and `portfolio_summary`. It must not write
`trade_log`, `trade_execution_group`, `trade_draft`, or `audit_log`. Audit write
for ShellAccounting remains unimplemented and unauthorized. TradeDraft, trade
suggestion, strategy execution, broker order, order placement, and trading UI
remain unauthorized.

## Future Audit Write Authorization Boundary

audit write implementation must be a separate TASK after explicit user
authorization. explicit user authorization is required before any audit write
code is added. Audit write can only record audit events for already-authorized
DataService write actions. The current future candidate source is limited to
TASK-144 `accounting.snapshot.write` result. audit write must go through the DataService boundary.
It must not be triggered directly by Shell, QML, Presenter, ShellServices, or ShellCore.

Audit write must not become a generic write escape hatch. It must not write
`trade_log`, `trade_execution_group`, or `trade_draft`; must not generate
TradeDraft; must not generate trade suggestions; must not execute strategy; and
must not submit broker orders or place orders.

Schema migration must be a separate TASK. Audit write must not modify
`migrations/001_initial_schema.sql`.

## DataService-Only Audit Policy

Future ShellAccounting audit write may only be coordinated by DataService.
QML, ShellServices, ShellCore, Presenter, and Shell must not directly trigger audit write
or directly depend on SQLite / DataAccess write paths.

DataServiceApi and DataServiceClient must not expose a generic write-enabled escape hatch.
Every future ShellAccounting audit write action must be
allowlisted, statically scannable, and test-covered.

## Audit Input Policy

Audit input can only come from an authorized DataService write action result.
The current future candidate source is the TASK-144 snapshot write result. Audit
write must not read raw facts, raw SQL, or raw snapshot payloads directly when a
source write result is unavailable.

Required source result context:

- action name
- authorization token
- snapshot uid / batch id
- result status
- sanitized issue code
- timestamp

## Audit Payload Policy

Future audit payload must be sanitized and minimal:

- action name
- authorization token
- snapshot uid / batch id
- result status
- sanitized issue code
- timestamp
- no raw SQL
- no raw trade_log payload
- no full snapshot payload
- no internal stack trace

## Forbidden Audit Policy

Audit write must not include or trigger:

- TradeDraft
- trade suggestion
- strategy execution
- broker order
- generic write
- raw SQL
- raw trade_log payload
- full snapshot payload
- internal stack trace

Audit write must not mutate the factual ledger, must not create drafts, and
must not act as a side channel for any non-audit business write.

## Transaction / Rollback Policy

Future audit write must be transactional with the authorized source write policy
defined by that future task. If audit is best-effort in a later task, that task
must explicitly document the consistency boundary. Transaction rollback must be
reported as an issue and must not be treated as success.

Audit write must include an explicit authorization check and a disable switch.
Disabling audit write must not disable TASK-144 snapshot write. No fake audit
data, no generic write fallback, and no trading fallback are allowed.

## Idempotency / Duplicate Audit Policy

Future audit write must define audit event identity, idempotency keys, and
duplicate audit behavior before implementation. Duplicate audit event must map
to an explicit no-op or issue state. It must not silently overwrite unrelated
audit records, create duplicate audit rows for the same source write, or mutate
`trade_log`.

## Error Mapping Policy

Future audit write implementation must map these states explicitly:

- audit disabled
- authorization missing
- source write unavailable
- source write failed
- audit payload invalid
- DB write failure
- transaction rollback
- schema mismatch
- duplicate audit event
- no silent success

## Privacy Policy

Audit diagnostics must not expose raw SQL, full `trade_log` payloads, full
snapshot payloads, internal exception stacks, raw preview internals, or sensitive
account history in QML. Logs must not dump full sensitive payloads by default.

## Rollback / Disable Policy

audit write must be reversible. Future implementation must be able to disable
audit write independently from the authorized source write.

The rollback policy must allow disabling audit write without disabling TASK-144
snapshot write. Fallback must keep snapshot write authorization behavior intact,
snapshot write must remain valid, must not fake audit success, must not fake
audit data, must not write audit_log, must not write trade_log, must not write trade_draft,
must not write database tables through a fallback, must not generate TradeDraft,
must not generate trade suggestions, must not execute
strategy, and must not submit broker orders.

rollback policy is required for every future audit write implementation.

## Future Implementation PR Requirements

A future audit write PR must document the authorized action name, source write
result contract, audit payload contract, transaction behavior, idempotency key,
duplicate audit behavior, error mapping, privacy handling, rollback plan, no
TradeDraft evidence, no strategy / broker order evidence, no schema migration
evidence, and all TASK-145 gate test results.

## Required Pre-Gates For Future Implementation

Future audit write implementation requires TASK-119, TASK-120, TASK-121,
TASK-122, TASK-123, TASK-124, TASK-125, TASK-126, TASK-127, TASK-128, TASK-129,
TASK-130, TASK-131, TASK-132, TASK-133, TASK-134, TASK-135, TASK-136, TASK-137,
TASK-138, TASK-139, TASK-140, TASK-141, TASK-142, TASK-143, TASK-144, full
CTest, `transport_local_socket_echo` 50 repeat, clean git status, TASK-145 gate
tests, and explicit user authorization.
