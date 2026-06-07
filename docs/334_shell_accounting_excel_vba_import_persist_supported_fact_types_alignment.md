# TASK-269 ShellAccounting Excel/VBA Import Persist Supported Fact Types Alignment

TASK-269 aligns accepted Excel/VBA import preview persistence across the QML
enablement rule, Presenter validation, DataService action, and DataAccess
composition repository.

## Supported Fact Types

- Trade facts persist through `ShellAccountingManualTransactionRepository`.
- Cash facts persist through `ShellAccountingManualCashMovementRepository`.
- Trade and cash facts are committed together with the sanitized audit marker
  inside one DataAccess-owned transaction boundary.
- Duplicate imports with the same idempotency key and digest do not duplicate
  trade rows, cash adjustment rows, or audit rows.

## Unsupported Fact Types

Market price and FX rate fact persistence remains unsupported in TASK-269.
The QML persist control must stay disabled when either unsupported count is
non-zero. The Presenter and DataService action must also fail closed with
sanitized unsupported issue codes.

## Transaction Boundary

The import composition repository owns the outer `TransactionRunner`.
Manual trade facts use the existing manual transaction repository active
transaction path. Cash facts use the manual cash movement repository active
transaction path. Any trade, cash, or audit failure rolls back the full write.

## Forbidden Scope

TASK-269 does not modify migrations, AccountingEngine, DataServiceClient,
startup, broker/network/credentials/endpoint code, TradeDraft, strategy,
real order placement, or automatic trading. Tests use only temporary SQLite
databases.
