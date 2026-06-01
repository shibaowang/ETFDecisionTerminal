# TASK-180 ShellAccounting Manual Entry DataService Action Scaffold

## Document Purpose

TASK-180 adds the first DataService action scaffold for local ShellAccounting
manual entry. It registers stable action names for future manual buy/sell
transaction records and deposit/withdraw cash movement records, but the
handlers only return unavailable / not implemented / disabled scaffold
responses.

TASK-180 does not implement writes, does not write a database, does not add a
repository, does not add a DataAccess repository, does not modify schema, does not modify production QML
or startup, does not change AccountingEngine replay, and does not connect
broker, network, credentials, endpoint, real order placement, or automatic
trading.

TASK-180 does not add a repository.

## Current State

TASK-177 authorized the local manual transaction / cash movement MVP boundary.
TASK-178 added pure in-memory DTO / validation scaffolding. TASK-179 added the
DataService action authorization gate. TASK-180 is the next step: action names
and dispatcher handlers are present, while implementation remains disabled.

Broker sandbox new capability work remains paused. Existing broker disabled,
real-broker, no-network, no-credentials, no-order-placement, disabled-runtime,
and sandbox-disabled gates remain required.

## Registered Scaffold Actions

The registered DataService action names are:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

Both actions are scaffold-only. The dispatcher can find them, but each handler
returns `success=false`, `manualEntryEnabled=false`, `writeEnabled=false`, and
an unavailable/not implemented reason.

## Response Semantics

The manual transaction action returns
`MANUAL_TRANSACTION_ENTRY_NOT_IMPLEMENTED`.

The manual cash movement action returns
`MANUAL_CASH_MOVEMENT_NOT_IMPLEMENTED`.

Both responses expose explicit negative policy flags:

- `implemented=false`
- `databaseWritten=false`
- `tradeLogWritten=false`
- `cashFactsWritten=false`
- `cashLedgerWritten=false`
- `auditWritten=false`
- `ledgerWritten=false`
- `repositoryCalled=false`
- `accountingReplayCalled=false`
- `tradeDraftGenerated=false`
- `tradeSuggestionGenerated=false`
- `brokerSdkCalled=false`
- `networkAccessed=false`
- `credentialsAccessed=false`
- `endpointAccessed=false`
- `realOrderPlacement=false`
- `automaticTrading=false`

## No Write Policy

TASK-180 does not authorize SQLite writes. It does not write `trade_log`, cash
facts, cash ledger, audit, derived ledger state, TradeDraft, broker state, or
any other table. It does not add a migration and does not modify
`migrations/001_initial_schema.sql`.

## No Repository Policy

TASK-180 does not add a DataAccess manual write repository. Future repository
work for manual transaction or cash movement persistence must be separately
authorized and must remain DataService-only.

## No Replay / Read Model Policy

TASK-180 does not modify AccountingEngine replay and does not trigger replay
from manual facts. It does not update read models, snapshots, summaries, or
cash ledgers.

## No UI / Startup Policy

TASK-180 does not modify production QML, Shell startup, Presenter, Controller,
or Shell adapters. Future UI work must be separately authorized and must not
write SQLite or DataAccess directly.

## No Broker / Strategy Policy

TASK-180 does not connect broker SDKs, network endpoints, credentials, usable
tokens, real broker order ids, order placement, reconciliation, cancellation,
correction, strategy execution, strategy suggestions, or automatic trading.

## Future Task Split

Separate tasks are required for:

- DataService manual entry implementation authorization gate;
- DataService manual entry implementation;
- DataAccess manual transaction write repository;
- DataAccess manual cash movement write repository;
- authorized AccountingEngine replay from manual facts;
- read model or snapshot updates;
- production QML / Presenter / Controller UI;
- trading suggestions;
- broker sandbox capability work;
- real broker order placement.

TASK-181 adds the future DataService action implementation authorization gate.
It still does not implement action writes, does not modify
`DataServiceActions.cpp`, `DataServiceActions.h`, or
`DataServiceActionRegistrar.cpp`, does not add repositories, and does not
change the TASK-180 disabled scaffold response semantics.

## Rollback Policy

Disabling this scaffold should remove only the manual entry action registration
and leave existing read-only accounting, TradeDraft, confirmation, broker
dry-run, and broker gate behavior unaffected. Rollback must not fake manual
facts, must not write a fallback record, and must not bypass DataService.
