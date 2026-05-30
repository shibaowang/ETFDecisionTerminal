# ShellAccounting TradeDraft Confirmation Authorization Test Plan

## Document Purpose

TASK-149 defines the test plan for a future ShellAccounting TradeDraft
confirmation / TradeLog write implementation. This document does not implement
confirmation, does not write `trade_log`, does not write
`trade_execution_group`, does not call broker code, and does not execute
strategies.

## Test Matrix

### A. No Confirmation Yet

- no confirmTradeDraft implementation.
- no `accounting.tradedraft.confirm` action implementation.
- no TradeDraft to trade_log conversion.
- no direct ledger write from draft.

### B. No TradeLog Write Yet

- no `trade_log` write in ShellAccounting / DataService TradeDraft path.
- no `trade_log` insert from TradeDraft.
- no `trade_log` update from TradeDraft.

### C. No Execution Group Write Yet

- no `trade_execution_group` write in ShellAccounting / DataService TradeDraft path.
- no execution group insert from TradeDraft.
- no execution group update from TradeDraft.

### D. DataService-Only Confirmation Policy

- future confirmation only through DataService.
- no QML / Shell / Presenter direct ledger write.
- no generic write escape hatch.
- explicit authorization required.

### E. Draft Input Policy

- existing unconfirmed TradeDraft required.
- draft status validation required.
- account / portfolio / instrument / side / quantity validation required.
- no UI raw payload.
- no fake facts.

### F. Ledger Write Policy

- `trade_log` is factual ledger.
- `trade_execution_group` and `trade_log` must be atomic.
- no partial ledger write.
- no duplicate confirmation.

### G. Audit Policy

- future confirmation must emit sanitized audit event.
- audit failure rollback documented.
- audit is not trade execution replacement.

### H. Idempotency / Duplicate Handling

- duplicate draft confirmation is detected.
- duplicate does not insert another `trade_log` row.
- duplicate does not insert another `trade_execution_group` row.
- duplicate does not submit broker order.

### I. Transaction Rollback

- ledger write failure rolls back all ledger rows.
- audit failure rolls back confirmation.
- partial ledger write is forbidden.

### J. No Broker / No Strategy / No Automatic Trading

- no broker order.
- no StrategyEngine execution.
- no automatic trading.
- no order placement.

### K. No UI-Triggered Confirmation

- no confirm draft button.
- no execute trade button.
- no buy / sell / order button.
- no direct DataService confirmation call from UI.

### L. Error Mapping

- authorization missing.
- confirmation disabled.
- draft missing.
- draft already confirmed.
- draft canceled.
- stale draft.
- invalid draft payload.
- ledger write authorization missing.
- audit unavailable.
- duplicate confirmation.
- transaction rollback.
- DB write failure.
- no silent success.

### M. Privacy

- no raw SQL.
- no full `trade_log` payload.
- no full TradeDraft payload.
- no internal stack trace.
- no sensitive raw payload.

### N. Rollback

- disable confirmation.
- fallback keeps TradeDraft creation / audit / snapshot features unaffected.
- no fake confirmation.
- no broker fallback.
- no strategy fallback.

### O. Regression

- TASK-119 through TASK-148 pass.
- all ShellAccounting gates pass.
- full CTest passes.
- transport 50 repeat passes.

## Required Probes

- static source scan.
- no `trade_log` write scan.
- no `trade_execution_group` write scan.
- no broker / strategy scan.
- no UI-triggered confirmation scan.
- no generic write action scan.
- no raw payload scan.
- audit policy documentation probe.
- rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/101 merged.
- [ ] docs/102 merged.
- [ ] TASK-119 through TASK-148 pass.
- [ ] TradeDraft confirmation authorization gate passes.
- [ ] full CTest passes.
- [ ] transport 50 passes.
- [ ] user explicitly authorized TradeDraft confirmation implementation.
- [ ] ledger write authorization token approved.
- [ ] audit behavior approved.
- [ ] no broker / no strategy tests ready.
