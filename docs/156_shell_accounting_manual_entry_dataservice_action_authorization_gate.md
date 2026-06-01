# TASK-179 ShellAccounting Manual Entry DataService Action Authorization Gate

## Document Purpose

TASK-179 is a DataService action authorization gate-only task for future local
manual entry writes. It authorizes documentation and static gates for later
manual buy/sell transaction and deposit/withdraw cash movement DataService
actions.

This task does not implement any DataService action, does not modify
`DataServiceActions.cpp`, does not add an action name, does not register a
dispatcher handler, does not write a database, does not modify schema, does not
modify QML or startup, does not add a repository, and does not connect broker
code.

TASK-179 itself did not modify `DataServiceActions.cpp` and did not register a
dispatcher handler. TASK-180 now evolves this gate: it allows only DataService action scaffold registration for manual entry actions and still does not
authorize write implementation.

TASK-180 still does not authorize write implementation.

## Current State

TASK-177 established the local manual transaction and cash movement MVP
authorization gate. TASK-178 added only an in-memory DTO / validation scaffold
for manual buy/sell and deposit/withdraw inputs.

Broker sandbox new capability development remains paused through TASK-179.
Pausing broker sandbox capability work does not delete, weaken, skip, or bypass
existing broker disabled, fail-closed, no-real-order, no-network,
no-credentials, and no-order-placement gates.

TASK-179 does not delete, weaken, skip, or bypass existing broker gates.

## Future DataService Action Boundary

Future manual entry DataService actions must be implemented in a separate
implementation task. Future scope may include:

- manual buy record creation action;
- manual sell record creation action;
- deposit record creation action;
- withdrawal record creation action;
- payload validation based on the TASK-178 DTO / validation scaffold;
- DataService-only writes to authorized trade log, cash facts, or cash ledger
  boundaries.

TASK-179 did not add these action names and did not register these handlers.
TASK-179 does not add these action names.
TASK-180 may register disabled scaffold handlers for
`accounting.manual_transaction.create` and
`accounting.manual_cash_movement.create`, but those handlers must return
unavailable / not implemented / disabled scaffold responses and must not write
or call repositories.

## DataService-Only Write Policy

DataService is the only future write boundary for manual entries. QML,
Presenter, Shell, ShellServices, and ShellCore must not directly write SQLite
or directly depend on DataAccess write repositories.

Future QML / Presenter flows may only call through the controller / adapter /
client boundary into DataService. Future actions must validate first, then use
an explicitly authorized repository / accounting boundary for persistence.

Future QML / Presenter flows may only call through the controller / adapter / client boundary.

## Validation Policy

Future manual entry actions must reuse the TASK-178 DTO / validation scaffold
or an explicitly authorized successor. Raw UI payload must not become ledger
facts without structured validation. Invalid account, portfolio, instrument,
side, quantity, amount, timestamp, or sensitive memo values must fail with
explicit errors and no silent success.

## Future Write Boundary

Future manual transaction writes should eventually enter `trade_log`, but this
task does not implement that write. Future cash movement writes should
eventually enter cash facts / cash ledger, but this task does not implement
that write.

Repository implementation, AccountingEngine replay consumption, read model
updates, and production UI must be advanced by separate authorized tasks.

## Forbidden Policy

TASK-179 forbids, except for the TASK-180 scaffold-only evolution:

- modifying `migrations/001_initial_schema.sql` or adding migrations;
- modifying production QML or startup;
- modifying Presenter / Controller behavior;
- modifying `DataServiceActions.cpp` for write implementation;
- adding DataService action names or dispatcher handlers beyond the TASK-180
  disabled scaffold actions;
- changing ServiceRuntime action routing;
- adding DataAccess production write repositories;
- changing AccountingEngine production replay;
- changing StrategyEngine or MarketEngine;
- adding SQLite writes;
- adding audit, ledger, `trade_log`, cash facts, or cash ledger writes;
- adding TradeDraft or trading suggestion implementation;
- adding broker SDKs, network calls, credentials, endpoints, real broker order
  ids, real order placement, reconciliation, cancellation, correction, or
  automatic trading.

## Broker Gate Retention Policy

Existing broker disabled, real-broker, no-network, no-credentials,
no-real-order, no-order-placement, sandbox-disabled, and disabled-runtime gates
must remain present and passing. TASK-179 docs and tests may mention broker
policy terms, but those policy terms must not become production broker
implementation.

## Future Task Split

Separate authorization is required for:

- DataService manual entry action scaffold;
- DataService manual entry action implementation;
- DataAccess manual transaction write repository;
- DataAccess cash movement write repository;
- AccountingEngine replay from authorized manual facts;
- read model updates;
- production QML / Presenter / Controller UI;
- strategy suggestions;
- broker sandbox capability work;
- real broker order placement.

## Privacy Policy

Future actions must not expose raw SQL, complete trade log payloads, complete
cash movement payloads, credentials, tokens, keys, passwords, endpoints, or
internal exception stacks. Validation and action errors must be sanitized and
must not print sensitive raw payloads.

## Rollback / Disable Policy

Future manual entry actions must be independently disableable. Disabling future
manual write actions must keep the TASK-178 validation scaffold and existing
read-only accounting flows available. Fallbacks must not fake facts, bypass
DataService, write directly from UI, create broker orders, or generate
automatic trading.
