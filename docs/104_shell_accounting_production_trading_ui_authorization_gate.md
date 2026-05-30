# ShellAccounting Production Trading UI Authorization Gate

## Document Purpose

TASK-151 defines the authorization gate before any ShellAccounting production trading UI can expose TradeDraft creation or TradeDraft confirmation to QML.

This document does not modify production QML. It does not add a trading page, trading component, buy button, sell button, order button, or confirm trade button. It does not bind TradeDraft create or confirm actions to QML. It does not call a broker, execute a strategy, place an order, or change TASK-148 / TASK-150 backend behavior.

## Current State

TASK-119 through TASK-150 are complete. ShellAccounting production QML has a read-only page and a production presenter context property. DataService internally supports read-only facts query, read-only AccountingEngine replay, snapshot rebuild preview, snapshot write, audit write, TradeDraft creation, and TradeDraft confirmation.

The backend can write the currently authorized tables through DataService-only paths: `cash_snapshot`, `position_snapshot`, `portfolio_summary`, `audit_log`, `trade_draft`, `trade_execution_group`, and `trade_log`.

Production trading UI is still not implemented. QML does not create TradeDraft records, does not confirm TradeDraft records, does not call broker order APIs, does not execute strategy, does not automatically trade, and does not place orders.

## Future Production Trading UI Boundary

Future production trading UI implementation must be a separate explicitly authorized TASK. The UI may only expose narrowly scoped TradeDraft creation / review / confirmation flows through the existing presenter/controller/adapter/DataService boundary. It must not write SQLite directly, bypass TradeDraft, bypass confirmation authorization, or transform raw UI payload into `trade_log` ledger facts.

Trading UI is not broker order placement. Trading UI is not automatic trading. Trading UI is not strategy execution. Trading UI is a guarded human workflow around draft creation, draft review, and explicit confirmation.

## UI Action Taxonomy

- Draft creation UI: may request a pending TradeDraft only after explicit authorization and validation.
- Draft review UI: may display sanitized draft-only state and risk flags.
- Confirmation UI: may request confirmation only after a distinct explicit user confirmation step.
- Ledger status UI: may display confirmed-ledger state as read-only status.
- No broker order UI: production trading UI must not place broker orders.
- No strategy execution UI: production trading UI must not execute StrategyEngine output.

## DataService-only Policy

Future UI can only call through ShellAccounting presenter, controller, adapter, and DataService boundary. QML must not directly access `DataServiceClient`. QML must not directly access SQLite or DataAccess. QML must not directly call `trade_draft`, `trade_log`, or `trade_execution_group` write paths.

DataServiceApi and DataServiceClient must not expose a generic write escape hatch for UI. TradeDraft creation and confirmation must remain allowlisted actions with explicit authorization and typed request mapping.

## Authorization Policy

TradeDraft create must continue to require `TASK-148_TRADEDRAFT_WRITE`. TradeDraft confirm must continue to require `TASK-150_TRADEDRAFT_CONFIRM`.

Authorization missing must fail. Disabled state must fail. Silent success is forbidden. UI-visible state must distinguish authorization failure, disabled state, draft creation failure, confirmation failure, and transport unavailable.

## User Confirmation Policy

Creation and confirmation are different actions. Creation produces a draft-only record; confirmation may write factual ledger records after explicit user confirmation.

Confirmation must require a clear second user step. UI must show draft-only and confirmed-ledger differences. UI must not default-confirm a draft, auto-confirm on creation, or silently infer confirmation from successful draft creation.

## Risk / Privacy Policy

Production UI must not show raw SQL. It must not show full `trade_log` payloads, full draft payloads, internal exception stacks, or sensitive raw payloads. Risk and issue text must be sanitized before reaching QML.

## Forbidden Policy

- no broker order
- no strategy execute
- no automatic trading
- no direct QML DB write
- no direct QML DataServiceClient call
- no direct QML TradeDraft write
- no direct QML TradeLog write
- no generic write escape hatch
- no raw UI payload promoted to ledger fact

## Error Mapping Policy

Future UI implementation must map these cases explicitly:

- authorization missing
- trading UI disabled
- draft creation failed
- confirmation failed
- stale draft
- duplicate confirmation
- audit unavailable
- ledger write failed
- transport unavailable
- no silent success

## Rollback / Disable Policy

Production trading UI must be disableable without disabling the existing read-only accounting page. Disabling production trading UI must preserve ShellAccounting read-only facts query, replay, snapshot, audit, TradeDraft backend, and confirmation backend behavior.

Fallback keeps the read-only accounting page. Fallback must not fake draft creation. Fallback must not fake confirmation. Fallback must not call broker order APIs. Fallback must not execute strategy. Fallback must not bypass DataService.

## Future Implementation PR Requirements

A future production trading UI PR must state:

- the authorized QML components and routes
- the presenter/controller methods exposed to QML
- the DataService actions used
- proof QML does not call DataServiceClient directly
- proof QML does not access SQLite or DataAccess
- authorization token handling for create and confirm
- user confirmation UX behavior
- error mapping and privacy behavior
- rollback / disable strategy
- no broker, no strategy, no automatic trading evidence
- full CTest, ShellAccounting gates, and transport 50-repeat results
