# ShellAccounting Real Broker Order Implementation Test Plan

## Document Purpose

TASK-156 defines the final test plan before future ShellAccounting real broker
order implementation. This document is test-planning only. It does not
implement real broker order, broker adapters, broker SDK calls, broker network
calls, sandbox broker calls, paper trading broker calls, strategy execution,
automatic trading, or real order placement.

## Test Matrix

### A. No Real Implementation Yet

- no real order action implementation
- no real order placement token
- no broker order status write path
- no real placement result

### B. DataService-only Implementation Boundary

- future implementation only through DataService
- broker adapter behind DataService
- no QML direct broker
- no Presenter direct broker
- no hidden background order placement

### C. Broker Adapter Boundary

- no broker adapter yet
- explicit interface required later
- no concrete SDK in QML / Shell / Presenter
- no direct network call in tests

### D. Dry-run Preflight Required

- TASK-154 dry-run boundary remains present
- dry-run success required
- dry-run reference required
- dry-run missing fails

### E. Confirmed Ledger Required

- confirmed TradeDraft / ledger fact required
- unconfirmed Draft rejected
- raw UI payload rejected
- stale ledger rejected

### F. Final Confirmation Required

- final user confirmation required
- final confirmation cannot be defaulted
- missing final confirmation fails
- no background submission

### G. Independent Authorization Required

- independent real-order token required
- dry-run token is not enough
- authorization missing fails
- disabled state fails

### H. Sandbox / Paper / Real Isolation

- distinct mode
- distinct credentials
- distinct audit marker
- no accidental real order in tests
- CI network calls disabled

### I. No CI Broker Network Call

- no broker network call in tests
- no sandbox broker call
- no paper trading broker call
- no real broker endpoint call

### J. Credentials Isolation

- no credentials in source
- no credentials in docs
- no credentials in tests
- no credentials in QML
- no credentials in logs

### K. Risk Preflight

- account allowlist
- instrument allowlist
- side / quantity / price validation
- max notional guard
- stale quote guard
- market hours guard
- duplicate guard
- idempotency key
- kill switch

### L. Idempotency / Duplicate Prevention

- idempotency key required
- duplicate order prevention
- duplicate audit behavior
- no second broker order for duplicate

### M. Audit

- sanitized order intent
- dry-run reference
- confirmed ledger reference
- user confirmation reference
- idempotency key
- broker mode marker
- no raw broker payload
- no credentials
- no raw SQL
- no full trade_log payload

### N. Reconciliation

- accepted
- rejected
- pending
- unknown broker state
- broker order id storage policy
- fill reconciliation separate authorization
- correction / cancel flow separate authorization

### O. Unknown Broker State Handling

- unknown broker state is not success
- reconciliation required
- incident containment required
- no silent success

### P. Kill Switch

- kill switch
- disabled switch
- default disabled
- no fake broker success
- dry-run fallback remains available

### Q. Incident Containment

- rollback policy
- incident containment
- no blind retry
- no strategy fallback
- no automatic trading fallback

### R. No Strategy Execution

- no StrategyEngine execution
- no strategy-generated broker order
- no new trade suggestion

### S. No Automatic Trading

- no auto trading
- no background broker submission
- no real order after confirmation without final user confirmation

### T. No UI Direct Broker

- no QML broker call
- no Presenter broker call
- no direct DataServiceClient broker call from QML
- no raw UI payload order

### U. Regression

- TASK-119 through TASK-155 gates pass
- TASK-154 broker dry-run tests pass
- production trading UI tests pass
- full CTest passes
- transport 50 repeat passes

## Required Probes

- static source scan
- broker SDK token scan
- network broker call scan
- credential token scan
- no QML broker scan
- no Presenter broker scan
- no strategy execution scan
- no automatic trading scan
- no real placement scan
- rollback / kill switch documentation probe
- incident containment documentation probe

## Go / No-Go Checklist

- [ ] docs/112 merged.
- [ ] docs/113 merged.
- [ ] TASK-154 broker dry-run tests pass.
- [ ] TASK-155 real broker authorization gate passes.
- [ ] TASK-156 implementation gate passes.
- [ ] full CTest passes.
- [ ] transport 50 repeat passes.
- [ ] user explicitly authorizes real broker order implementation.
- [ ] default-disabled behavior approved.
- [ ] sandbox / paper / real isolation approved.
- [ ] broker adapter interface approved.
- [ ] risk preflight approved.
- [ ] credential handling approved outside repo.
- [ ] reconciliation and incident containment approved.
