# ShellAccounting Broker Order Authorization Test Plan

## Document Purpose

TASK-153 defines the broker order authorization test plan for future
ShellAccounting broker order / order placement implementation. This document is
test-planning only. It does not implement broker order, broker adapters,
StrategyEngine execution, automatic trading, or order placement.

## Test Matrix

### A. No Broker Order Yet

- no broker order action implementation
- no broker adapter
- no broker SDK call
- no order placement
- no broker authorization token in production path

### B. DataService-only Broker Boundary

- future broker order only through DataService
- no QML direct broker SDK
- no Presenter direct broker SDK
- no Shell direct broker SDK
- no direct DataServiceClient broker call from QML
- no generic broker/write escape hatch

### C. Confirmed Trade Input Only

- future broker order must reference confirmed TradeDraft / ledger fact
- no raw UI payload order
- no unconfirmed Draft order
- no stale Draft order
- no stale ledger order
- no bypass of TASK-148 TradeDraft creation or TASK-150 confirmation

### D. Explicit User Confirmation

- additional user confirmation after ledger confirmation
- separate broker authorization token
- authorization missing fails
- broker disabled fails
- no silent success

### E. No Strategy Execution

- no StrategyEngine execution
- no strategyExecute path
- no strategy-generated broker order
- no new trade suggestion

### F. No Automatic Trading

- no auto-trading trigger
- no order after Draft creation
- no order after Draft confirmation
- no background broker submission

### G. No UI Direct Broker

- no broker tokens in production QML
- no broker SDK includes in ShellServices or ShellCore
- no buy / sell / order broker submission button
- no direct QML order placement

### H. Audit

- future broker order must record sanitized audit
- audit must not log raw broker payload
- audit must not log credentials
- audit failure policy must be documented

### I. Error Mapping

- broker disabled
- authorization missing
- confirmed trade missing
- duplicate order attempt
- stale draft / stale ledger
- broker unavailable
- broker rejected
- timeout
- rollback
- no silent success

### J. Rollback

- disable broker order
- dry-run broker order
- fallback keeps TASK-152 Draft UI
- fallback keeps TASK-150 ledger confirmation
- no fake broker success
- no strategy fallback
- no automatic trading fallback

### K. Regression

- TASK-119 through TASK-152 gates pass
- production trading UI tests pass
- TradeDraft confirmation tests pass
- full CTest passes
- transport 50 repeat passes

## Required Probes

- static source scan
- forbidden broker token scan
- no UI broker call scan
- no strategy execution scan
- no automatic trading scan
- confirmed-ledger input documentation probe
- user confirmation documentation probe
- rollback documentation probe

## Go / No-Go Checklist

- [ ] docs/107 merged.
- [ ] docs/108 merged.
- [ ] TASK-153 gate passes.
- [ ] TASK-152 production trading UI tests pass.
- [ ] TASK-150 confirmation tests pass.
- [ ] full CTest passes.
- [ ] transport 50 repeat passes.
- [ ] user explicitly authorized broker order implementation.
- [ ] independent broker authorization token approved.
- [ ] dry-run / disabled / rollback behavior approved.
- [ ] audit and privacy policy approved.

## TASK-154 Test Update

TASK-154 converts the no-broker-yet check into an authorized dry-run-only check.
The positive boundary must find `accounting.broker_order.dry_run` and
`TASK-154_BROKER_ORDER_DRY_RUN`, while still rejecting broker SDK calls, real
order placement, StrategyEngine execution, automatic trading, UI direct broker
calls, raw UI payload orders, and writes outside `audit_log`.

The implementation test matrix now includes:

- DataService-only dry-run boundary
- confirmed TradeDraft / ledger fact input only
- explicit authorization token
- explicit user confirmation
- dry-run / broker-disabled response semantics
- sanitized audit event
- idempotency and duplicate dry-run handling
- no `trade_log`, `trade_execution_group`, or `trade_draft` write
- no broker SDK / no real order placement
- rollback and disable readiness
