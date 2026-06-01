# TASK-181 ShellAccounting Manual Entry DataService Action Implementation Authorization Gate

## Document Purpose

TASK-181 is a gate-only document for future ShellAccounting manual entry
DataService action implementation. It defines the authorization boundary before
`accounting.manual_transaction.create` or `accounting.manual_cash_movement.create`
can parse payloads, validate them through production wiring, or persist manual
facts.

TASK-181 does not implement either action. It does not modify
`DataServiceActions.cpp`, `DataServiceActions.h`, or
`DataServiceActionRegistrar.cpp`. It does not modify TASK-178 validation
production code. The TASK-180 scaffold actions must continue to return
unavailable / not implemented / disabled scaffold responses.

## Current State

TASK-177 defined the local manual transaction / cash movement MVP authorization
boundary. TASK-178 added pure in-memory DTO and validation scaffolding. TASK-179
added the DataService action authorization gate. TASK-180 registered disabled
DataService action scaffold names and dispatcher handlers:

- `accounting.manual_transaction.create`
- `accounting.manual_cash_movement.create`

The current handlers remain disabled. They do not parse payloads for
persistence, do not call repositories, do not write SQLite, and do not generate
manual accounting facts.

Broker sandbox new capability work remains paused. Pausing broker sandbox work
does not delete or weaken existing broker disabled, fail-closed, no-real-order,
no-network, no-credentials, or no-order-placement gates.

## Future Implementation Split

Future implementation must be split into separately authorized tasks:

1. Payload parsing plus TASK-178 validation wiring. This first implementation
   stage may return validation results only and must not write a database.
2. Manual entry repository / persistence authorization gate.
3. Manual transaction write implementation, potentially writing `trade_log` or
   an equivalent factual table, only after repository, transaction, rollback,
   and audit policy are authorized.
4. Manual cash movement write implementation, potentially writing cash facts or
   cash ledger facts, only after repository, transaction, rollback, and audit
   policy are authorized.
5. AccountingEngine replay, read model, snapshot, and UI work as separate
   tasks.

TASK-181 authorizes none of these implementation stages.

## Validation Wiring Boundary

The first future implementation stage may only parse structured payloads and
wire them to the TASK-178 DTO / validation scaffold or an explicitly authorized
successor. Validation wiring must not persist data, must not create repositories,
must not write `trade_log`, cash facts, cash ledger, audit, or ledger records,
and must not trigger AccountingEngine replay.

Payload parsing must reject credentials, endpoints, broker payloads, raw broker
SDK input, network configuration, tokens, keys, passwords, and secret-store
material. Manual transaction implementation is not broker order placement.
Manual cash movement implementation is not broker transfer.

## DataService-Only Policy

Future manual entry write implementation must remain DataService-only. QML,
Presenter, Controller, ShellServices, and ShellCore may only reach it through
the existing controller / adapter / client service boundary. They must not write
SQLite or DataAccess directly.

Future write implementation must include explicit transaction, rollback,
idempotency, duplicate handling, and audit policy before any persistence is
authorized. Trade log writes, cash facts writes, cash ledger writes, repository
work, replay, read model updates, and UI binding all require separate tasks.

## No Write / No Runtime Side Effect Policy

TASK-181 forbids:

- schema or migration changes;
- new migrations;
- SQLite writes;
- `trade_log` writes;
- cash facts writes;
- cash ledger writes;
- audit writes;
- ledger writes;
- DataAccess production write repositories;
- payload-to-write paths;
- `success=true` implementation responses;
- AccountingEngine replay changes;
- StrategyEngine or MarketEngine implementation changes;
- TradeDraft or trading suggestion implementation;
- production QML, startup, Presenter, or Controller changes.

## No Broker / No Automatic Trading Policy

The first future manual entry stage still must not connect to a real broker,
must not use broker SDKs, must not access networks, endpoints, credentials, or
secret stores, must not generate or store real broker order ids, must not place
orders, must not reconcile, cancel, or correct broker orders, and must not add
automatic trading.

Existing broker disabled, fail-closed, no-real-order, no-network,
no-credentials, no-order-placement, sandbox-disabled, and real-broker gates must
remain present and passing.

## Error Mapping Policy

Future implementation must fail explicitly for:

- implementation disabled;
- authorization missing;
- payload parsing unavailable;
- invalid DTO;
- validation failed;
- repository authorization missing;
- write authorization missing;
- transaction policy missing;
- audit policy missing;
- broker payload rejected;
- credentials rejected;
- no silent success.

Until an authorized implementation task changes this, the TASK-180 scaffold
response remains unavailable / not implemented / disabled.

## Privacy Policy

Future payload parsing must not expose raw SQL, raw broker payloads, credentials,
endpoints, full sensitive memo values, internal stack traces, or secret material
to QML or logs. Diagnostics must use sanitized issue codes.

## Rollback / Disable Policy

Manual entry implementation must be independently disableable. Disabling future
implementation must preserve the TASK-180 scaffold fallback or an explicitly
documented unavailable response. Fallback must not fake manual facts, must not
write a database, must not bypass DataService, must not call broker systems, and
must not create automatic trading behavior.

## Future Implementation PR Requirements

A future implementation PR must:

- reference docs/160 and docs/161;
- keep TASK-180 scaffold regressions passing until explicitly evolved;
- keep TASK-178 validation scaffold regressions passing;
- include new tests for payload parsing and validation wiring;
- prove no database write in the validation-only stage;
- defer repository, trade log, cash facts, replay, read model, UI, broker, and
  automatic trading to separately authorized tasks.
