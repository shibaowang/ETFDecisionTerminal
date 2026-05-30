# ShellAccounting Real Broker Order Authorization Test Plan

## Document Purpose

TASK-155 defines the test plan for future ShellAccounting real broker order /
real order placement implementation. This document is test-planning only. It
does not implement real broker order, broker adapters, broker SDK calls,
sandbox broker calls, paper trading broker calls, strategy execution, automatic
trading, or order placement.

## Test Matrix

### A. No Real Broker Order Yet

- no real broker order implementation
- no real order placement action
- no broker adapter
- no broker SDK include / import / link
- no sandbox broker call
- no paper trading broker call
- no real order placement token

### B. DataService-only Real Order Boundary

- future real broker order only through DataService
- no QML direct broker SDK
- no Presenter direct broker SDK
- no Shell direct broker SDK
- no DataAccess direct broker SDK
- no generic broker/write escape hatch

### C. Dry-run Preflight Required

- TASK-154 dry-run boundary remains present
- successful dry-run result required before real placement
- dry-run reference must be carried into audit
- missing dry-run must fail

### D. Confirmed Ledger Input Required

- future real order must reference confirmed TradeDraft / ledger fact
- no raw UI payload order
- no unconfirmed Draft order
- no stale Draft order
- no stale ledger order

### E. Final User Confirmation Required

- final user confirmation after dry-run
- final confirmation cannot be defaulted
- final confirmation missing must fail
- UI must not auto-submit real order

### F. Independent Authorization Required

- future real order must have an independent authorization token
- TASK-154 dry-run token is insufficient for real order
- authorization missing fails
- disabled state fails

### G. Broker Adapter Boundary

- broker adapter must be behind DataService action
- no QML direct broker
- no Presenter direct broker
- no DataAccess direct broker
- no hidden plugin registration
- no background order placement

### H. Risk Controls

- market / account / instrument validation
- side / quantity / price validation
- max quantity / max notional guard
- stale quote guard
- market hours guard
- duplicate order guard
- idempotency key
- kill switch

### I. Idempotency / Duplicate Prevention

- idempotency key required
- duplicate order attempt fails or returns idempotent status
- duplicate must not place a second order
- duplicate must be audited

### J. Audit

- sanitized order intent
- dry-run reference
- confirmed trade reference
- user confirmation reference
- no raw broker payload
- no credentials
- no raw SQL
- no full `trade_log` payload

### K. Reconciliation

- order accepted
- order rejected
- order pending
- order unknown
- broker order id storage separately authorized
- fill reconciliation separately authorized
- correction flow separately authorized

### L. No Strategy Execution

- no StrategyEngine execution
- no strategy-generated broker order
- no new trade suggestion

### M. No Automatic Trading

- no auto-trading trigger
- no background broker submission
- no order after confirmation without final user confirmation

### N. No UI Direct Broker

- no broker tokens in production QML
- no broker SDK includes in ShellServices or ShellCore
- no direct QML order placement
- no raw UI payload order

### O. Disabled / Kill Switch

- broker disabled
- kill switch enabled
- disabled state fails closed
- dry-run fallback remains available

### P. Error Mapping

- broker disabled
- authorization missing
- dry-run missing
- confirmed trade missing
- final confirmation missing
- stale quote
- market closed
- risk guard failed
- duplicate order
- broker unavailable
- broker rejected
- timeout
- unknown broker state
- audit failure
- reconciliation required
- rollback
- no silent success

### Q. Rollback / Incident Containment

- disable real broker order
- contain unknown broker state
- keep TradeDraft create / confirm unaffected
- keep TASK-154 dry-run unaffected
- no fake broker success
- no strategy fallback
- no automatic trading fallback

### R. Regression

- TASK-119 through TASK-154 gates pass
- broker order dry-run tests pass
- production trading UI tests pass
- full CTest passes
- transport 50 repeat passes

## Required Probes

- static source scan
- forbidden broker SDK token scan
- no network broker call scan
- no UI direct broker scan
- no strategy execution scan
- no automatic trading scan
- no real order placement scan
- dry-run preflight documentation probe
- rollback documentation probe

## Go / No-Go Checklist

- [ ] docs/110 merged.
- [ ] docs/111 merged.
- [ ] TASK-154 broker dry-run implementation passes.
- [ ] TASK-155 gate passes.
- [ ] full CTest passes.
- [ ] transport 50 repeat passes.
- [ ] user explicitly authorized real broker order implementation.
- [ ] independent real broker authorization token approved.
- [ ] broker adapter boundary approved.
- [ ] risk controls approved.
- [ ] reconciliation plan approved.
- [ ] rollback / incident containment approved.

## TASK-156 Test Update

TASK-156 adds `docs/112` / `docs/113` and
`tests/ShellAccountingRealBrokerOrderImplementationGate`. The new gate keeps
TASK-154 dry-run as the only allowed broker-order-related implementation while
checking no real broker order, no broker adapter, no broker SDK, no broker
network call, no credentials, no sandbox / paper broker call, no strategy
execution, no automatic trading, no UI direct broker access, and complete
documentation of dry-run preflight, confirmed-ledger input, final confirmation,
independent authorization, sandbox / paper / real isolation, risk preflight,
audit, reconciliation, kill switch, error mapping, and rollback policy.
