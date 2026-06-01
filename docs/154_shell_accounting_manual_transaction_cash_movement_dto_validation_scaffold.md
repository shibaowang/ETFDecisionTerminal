# TASK-178 ShellAccounting Manual Transaction and Cash Movement DTO Validation Scaffold

## Document Purpose

TASK-178 implements only an in-memory DTO and validation scaffold for manual
buy/sell records and deposit/withdraw records. It advances the TASK-177 local
assisted decision MVP line without adding a runtime write path.

TASK-178 is a DTO / validation scaffold only.

This task does not write SQLite, does not add a DataService action, does not add
a DataAccess repository, does not modify AccountingEngine replay, does not
modify production QML, does not modify production startup, does not modify
migrations, does not implement TradeDraft or strategy suggestions, does not
connect broker SDKs, does not add network calls, credentials, endpoints, real
broker order ids, real order placement, or automatic trading.

TASK-178 does not add a DataAccess repository.
TASK-178 does not modify production QML, does not modify production startup,
does not modify migrations, and does not modify AccountingEngine replay.

## Current State

TASK-177 added the authorization gate for manual transaction and cash movement
MVP work. Broker sandbox new capability development remains paused, and
existing broker / real broker / no-network / no-credentials /
no-order-placement gates remain active.

TASK-178 adds:

- `ShellAccountingManualTransactionEntry`
- `ShellAccountingManualCashMovementEntry`
- `ShellAccountingManualTradeSide`
- `ShellAccountingManualCashMovementType`
- `ShellAccountingManualEntryValidationIssue`
- `ShellAccountingManualEntryValidationResult`
- `validateManualTransactionEntry(...)`
- `validateManualCashMovement(...)`
- `containsSensitiveManualEntryMemoToken(...)`

## DTO Scope

Manual transaction DTO fields:

- `accountId`
- `portfolioId`
- `securityCode`
- `tradeSide`
- `quantityUnits`
- `priceAmountMinor`
- `grossAmountMinor`
- `feeAmountMinor`
- `taxAmountMinor`
- `occurredAt`
- `sourceMemo`

Cash movement DTO fields:

- `accountId`
- `portfolioId`
- `movementType`
- `amountMinor`
- `occurredAt`
- `sourceMemo`

Money and quantity fields use integer fixed-unit fields. This scaffold does not
use `double` for fact amounts.

## Validation Policy

Manual transaction validation checks:

- Buy / Sell side must be valid.
- `quantityUnits` must be positive.
- `priceAmountMinor` must be positive.
- `grossAmountMinor` must be positive.
- `feeAmountMinor` and `taxAmountMinor` must not be negative.
- `accountId`, `portfolioId`, and `securityCode` must be present.
- `occurredAt` must be present.
- `sourceMemo` must not contain sensitive credential-like tokens.

Cash movement validation checks:

- Deposit / Withdrawal type must be valid.
- `amountMinor` must be positive.
- `accountId` and `portfolioId` must be present.
- `occurredAt` must be present.
- `sourceMemo` must not contain sensitive credential-like tokens.

## Boundary Policy

The validation scaffold is pure in-memory code. It must not:

- Read or write SQLite.
- Write `trade_log`, cash facts, audit rows, ledger rows, or snapshots.
- Create or modify TradeDraft.
- Add or invoke DataService actions.
- Add DataAccess write repositories.
- Call AccountingEngine replay.
- Call StrategyEngine or MarketEngine.
- Call broker SDKs.
- Access network clients, endpoints, credentials, tokens, keys, passwords, or
  secret stores.
- Change runtime state.

## Future Task Split

Later tasks must separately authorize:

- DataService manual transaction actions.
- DataService cash movement actions.
- DataAccess manual write repositories.
- AccountingEngine replay consumption of authorized manual facts.
- Production QML / Presenter / Controller UI integration.
- Assisted suggestion generation.
- Backup/export/package implementation.

Real broker integration, automatic trading, and real order placement remain out
of scope and still require separate authorization.

## Privacy Policy

Validation issues return sanitized code, field, and message values only. The
validation scaffold must not log raw memo contents, raw SQL, credentials,
tokens, keys, passwords, endpoints, complete TradeLog payloads, complete cash
movement payloads, or internal exception stacks.

## Rollback / Disable Policy

Rollback is to remove the DTO/validation files and their tests. Removing this
scaffold must not affect existing read-only accounting, replay, snapshot,
audit, TradeDraft, confirmation, production trading UI, disabled broker dry-run,
or broker safety gates.
