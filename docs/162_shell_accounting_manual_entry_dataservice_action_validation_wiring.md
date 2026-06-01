# TASK-182 ShellAccounting Manual Entry DataService Action Validation Wiring

## Document Purpose

TASK-182 implements the first authorized manual entry DataService action stage:
payload parsing plus TASK-178 validation wiring for:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

This document does not authorize persistence. TASK-182 does not add a
DataAccess write repository, does not write SQLite, does not write `trade_log`,
does not write cash facts, does not write audit or ledger records, does not
modify production QML or startup, does not call AccountingEngine replay, does
not generate TradeDraft or trading suggestions, and does not call broker,
network, credential, endpoint, or order-placement code.

## Current State

TASK-180 registered the two DataService action names as disabled scaffolds.
TASK-181 authorized a future implementation split. TASK-182 now connects the
first implementation slice: handlers parse structured JSON payloads into the
TASK-178 DTOs and call:

- `validateManualTransactionEntry`
- `validateManualCashMovement`

The actions remain validation-only and fail closed. A valid payload means the
DTO passed validation; it does not mean a manual transaction or cash movement
was persisted.

## Payload Parsing Boundary

The manual transaction payload must be a JSON object and maps only these
fields:

- `accountId`
- `portfolioId`
- `securityCode`
- `tradeSide` (`Buy` or `Sell`)
- `quantityUnits`
- `priceAmountMinor`
- `grossAmountMinor`
- `feeAmountMinor`
- `taxAmountMinor`
- `occurredAt`
- `sourceMemo`

The manual cash movement payload must be a JSON object and maps only these
fields:

- `accountId`
- `portfolioId`
- `movementType` (`Deposit` or `Withdrawal`)
- `amountMinor`
- `occurredAt`
- `sourceMemo`

Raw JSON escape hatches, raw SQL, broker payloads, endpoint configuration,
credentials, tokens, keys, passwords, and secret-store material are not
accepted as runtime write sources.

## Validation Response Policy

Every response is explicitly marked `validationOnly=true`.

Invalid payloads return failure with `validationAccepted=false` and a stable
`issues` array. Each issue includes:

- `code`
- `field`
- `message`

Valid payloads still return failure / unavailable with:

- `validationAccepted=true`
- `writeImplemented=false`
- `databaseWritten=false`
- `tradeLogWritten=false`
- `cashFactsWritten=false`
- `auditWritten=false`
- `ledgerWritten=false`
- `status=VALIDATION_ACCEPTED_WRITE_NOT_IMPLEMENTED`

TASK-182 must not return persistence success, persistent ids, trade log ids,
cash fact ids, or any durable write marker.

## No Write Policy

TASK-182 forbids:

- SQLite writes;
- DataAccess write repository calls;
- `trade_log` writes;
- cash facts or cash ledger writes;
- audit writes;
- ledger writes;
- schema or migration changes;
- new migrations;
- replay, snapshot, read-model, or UI side effects.

## No Broker / Trading Policy

Manual entry validation wiring is not broker order placement. It must not call
broker SDKs, networks, credentials, endpoints, order placement, strategy
execution, automatic trading, TradeDraft creation, or trading suggestion code.

Existing broker disabled, dry-run, sandbox, no-network, no-credentials,
no-real-order, and no-order-placement gates remain required.

## Privacy Policy

Validation diagnostics must use sanitized issue codes and fields. Responses and
logs must not expose raw SQL, raw broker payloads, full sensitive memo values,
credentials, endpoints, secret material, or internal stack traces.

## Rollback / Disable Policy

Rollback is to disable manual entry validation wiring and preserve the previous
fail-closed DataService behavior. Rollback must not fake manual facts, must not
write a database, must not bypass DataService, must not call broker systems,
and must not create automatic trading behavior.

## Future Implementation Requirements

Future manual entry persistence requires a separate task and must explicitly
authorize repository design, transaction policy, rollback policy, idempotency,
duplicate handling, audit policy, schema compatibility, and write targets.
Manual transaction persistence, manual cash movement persistence, UI exposure,
replay, snapshot, broker, and automatic trading remain outside TASK-182.
