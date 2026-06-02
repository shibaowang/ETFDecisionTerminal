# ShellAccounting Manual Entry QML Presenter Authorization Gate

## Document Purpose

TASK-199 is a QML / Presenter authorization gate-only task for future
ShellAccounting manual entry UI integration. It defines the boundary for a
future task that may expose manual transaction and manual cash movement entry
through production UI and ShellServices.

TASK-199 does not implement UI, does not modify production QML, does not modify startup, does not modify Presenter or Controller code, does not modify DataServiceActions, does not modify repositories, does not write the database, does not trigger replay, does not write audit or ledger rows, and does not add broker, network, credentials, endpoint, real order placement, or automatic trading capability.

## Current State

TASK-192 implemented DataAccess-only manual transaction repository writes.
TASK-196 implemented DataAccess-only manual cash movement dual writes.
TASK-198 implemented validation-first DataService write wiring for:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

The current production QML, startup, Presenter, and Controller layers are not
connected to manual entry writes. AccountingEngine replay, read model refresh,
audit write, ledger policy, broker SDK integration, network access, credential
sources, endpoint configuration, real order placement, and automatic trading
remain out of scope.

Broker sandbox new capability development remains paused and existing broker /
real broker / no-network / no-credentials / no-order-placement gates remain
retained.

## Future Manual Transaction Form Boundary

A future separately authorized UI implementation may collect a structured
manual BUY / SELL payload with account id, portfolio id, security code or
instrument code, side, quantity, price, gross amount, fee, tax, occurred-at
timestamp, source memo, request id, and idempotency key.

The future form may only submit through the DataService action
`accounting.manual_transaction.create`. QML, Presenter, and Controller code
must not directly call DataAccess repositories, directly write SQLite, compute
ledger facts, generate a broker order, execute strategy logic, or enable
automatic trading.

## Future Manual Cash Movement Form Boundary

A future separately authorized UI implementation may collect a structured
Deposit / Withdrawal payload with account id, portfolio id, movement type,
amount, currency, occurred-at timestamp, source memo, source reference, request
id, and idempotency key.

The future form may only submit through the DataService action
`accounting.manual_cash_movement.create`. QML, Presenter, and Controller code
must not directly call DataAccess repositories, directly write SQLite, generate
`trade_log` or `cash_adjustment` rows, generate a broker order, execute strategy
logic, or enable automatic trading.

## DataService-Only UI Policy

Future manual entry UI must travel through the existing application boundary:
QML to Presenter or Controller, then adapter, then DataService action. The UI
must not access DataServiceClient directly, must not access SQLite or
DataAccess directly, and must not introduce a generic write-enabled escape
hatch.

Future UI implementation must be separately authorized. Future readback, refresh, or AccountingEngine replay integration must be separately authorized.
Future audit write or ledger policy changes must be separately authorized.

## UI Response And Privacy Boundary

Future UI may display write status, validation issues, sanitized repository issues, duplicate / idempotent result state, and safe DataService errors.
Future UI must not expose real order ids.

Future UI must not expose raw SQL, raw payloads, credentials, endpoints, tokens,
internal stack traces, broker payloads, real broker order ids, or real order
ids. Failure states must not be silent success.

## Forbidden Policy

TASK-199 and any future UI task without explicit extra authorization must not:

- modify production startup as a side effect of this gate.
- call DataAccess repositories from QML, Presenter, or Controller.
- write SQLite directly from QML, Presenter, or Controller.
- write `audit_log` or ledger rows.
- trigger AccountingEngine replay or read model refresh.
- generate TradeDraft or trade suggestions.
- connect broker SDKs.
- add network calls, credentials, endpoints, tokens, or secrets.
- place real orders or store real broker order ids.
- execute StrategyEngine or MarketEngine trading behavior.
- enable automatic trading.

## Required Future Implementation Gates

A future implementation PR must reference this document and
`docs/195_shell_accounting_manual_entry_qml_presenter_authorization_test_plan.md`.
It must prove DataService-only UI routing, structured validation, sanitized
response mapping, no direct DB access, no broker / network / credentials /
endpoint capability, no real order placement, no automatic trading, and
retained TASK-198 / TASK-196 / TASK-192 regression behavior.
