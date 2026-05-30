# ShellAccounting Production Trading UI Authorization Test Plan

## Document Purpose

TASK-151 defines the test plan for a future production trading UI implementation. This document does not implement production trading UI, QML TradeDraft bindings, broker order placement, strategy execution, automatic trading, or order submission.

## Test Matrix

### A. No Production Trading UI Yet

- production QML has no trading page
- production QML has no trading component
- production QML has no buy button
- production QML has no sell button
- production QML has no order button
- production QML has no confirm trade button

### B. No QML TradeDraft Create Binding Yet

- no `accounting.tradedraft.create` binding in production QML
- no `createTradeDraft` binding in production QML
- no `TASK-148_TRADEDRAFT_WRITE` token in production QML
- no direct DataServiceClient draft call from QML

### C. No QML TradeDraft Confirm Binding Yet

- no `accounting.tradedraft.confirm` binding in production QML
- no `confirmTradeDraft` binding in production QML
- no `TASK-150_TRADEDRAFT_CONFIRM` token in production QML
- no direct DataServiceClient confirmation call from QML

### D. DataService-only UI Policy

- UI only through presenter/controller/adapter/DataService boundary
- no QML direct DataServiceClient
- no QML direct SQLite or DataAccess
- no QML direct `trade_draft`, `trade_log`, or `trade_execution_group` write
- no generic write escape hatch

### E. Explicit Authorization Required

- TradeDraft create still requires `TASK-148_TRADEDRAFT_WRITE`
- TradeDraft confirm still requires `TASK-150_TRADEDRAFT_CONFIRM`
- authorization missing fails
- disabled state fails
- no silent success

### F. User Confirmation Required

- creation and confirmation are separate actions
- confirmation needs explicit user step
- no default confirmation
- draft-only and confirmed-ledger states must be visible

### G. No Broker / No Strategy / No Automatic Trading

- no broker order call
- no order placement
- no StrategyEngine execution
- no automatic trading
- no order confirmation bypassing TradeDraft confirmation

### H. No Direct QML DB Write

- no SQL write tokens in QML
- no SQLite or DataAccess references in QML
- no table write tokens in QML
- no direct ledger write from QML

### I. Error Mapping

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

### J. Privacy

- no raw SQL in QML
- no full `trade_log` payload in QML
- no full draft payload in QML
- no internal stack trace in QML
- no sensitive raw payload in issue text

### K. Rollback

- disable production trading UI
- fallback keeps read-only accounting page
- no fake draft
- no fake confirmation
- no broker fallback
- no strategy fallback

### L. Regression

- TASK-119 through TASK-150 gates pass
- TradeDraft creation tests pass
- TradeDraft confirmation tests pass
- production QML binding tests pass
- full CTest passes
- transport 50 repeat passes

## Required Probes

- static QML scan
- no buy/sell/order/confirm trading button scan
- no direct DataServiceClient in QML scan
- no `trade_draft` / `trade_log` write token in QML scan
- no broker / strategy token scan
- authorization documentation probe
- user confirmation documentation probe
- rollback documentation probe

## Go / No-Go Checklist

- [ ] docs/104 merged.
- [ ] docs/105 merged.
- [ ] TASK-151 gate passes.
- [ ] TASK-150 confirmation tests pass.
- [ ] full CTest passes.
- [ ] transport 50 repeat passes.
- [ ] user explicitly authorized production trading UI implementation.
- [ ] user explicitly authorized QML TradeDraft create binding.
- [ ] user explicitly authorized QML TradeDraft confirm binding.
- [ ] user explicitly accepted no broker / no strategy / no automatic trading boundary.
