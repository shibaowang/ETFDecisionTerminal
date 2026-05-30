# ShellAccounting TradeDraft Authorization Test Plan

## Document Purpose

TASK-147 defines the test plan for a future ShellAccounting TradeDraft
implementation. This document does not implement TradeDraft, does not write
`trade_draft`, does not write `trade_log`, does not write
`trade_execution_group`, does not generate trade suggestions, does not execute
strategy, and does not submit broker orders.

## Test Matrix

This Test matrix is the required baseline for any future TradeDraft task.

### A. No TradeDraft Yet

- no `createTradeDraft`.
- no `trade_draft` insert.
- no TradeDraft write implementation.
- no `trade_log` write.
- no `trade_execution_group` write.

### B. DataService-Only Draft Policy

- future TradeDraft only through DataService.
- no QML / Shell / Presenter direct draft write.
- no generic write escape hatch.
- authorization required.

### C. Explicit Authorization Required

- missing authorization fails.
- invalid authorization fails.
- disabled TradeDraft fails.
- no silent success.

### D. Allowed Input Source Policy

- authorized snapshot / replay / accounting summary source only.
- no raw UI payload.
- no fake facts.
- no unauthorized strategy output.

### E. Payload Sanitization

- account / portfolio / instrument / side / quantity / reason.
- source snapshot id / source replay id.
- risk flags.
- no raw SQL.
- no full snapshot payload.
- no full trade_log payload.
- no stack trace.

### F. Audit Policy

- future TradeDraft must emit sanitized audit event.
- audit failure policy documented.
- audit is not trade confirmation.
- audit is not broker order.

### G. Idempotency / Duplicate Handling

- deterministic idempotency key.
- duplicate draft handling.
- no duplicate order implication.
- no trade execution on duplicate.

### H. No Trade Execution

- no broker order.
- no place order.
- no execute order.
- no `trade_log` write.
- no `trade_execution_group` write.

### I. No Strategy / Broker

- no strategy execute.
- no StrategyEngine execution path.
- no broker adapter.
- no auto trading.

### J. No UI-Triggered Draft

- no TradeDraft production UI trigger.
- no create draft button.
- no buy / sell / order button.
- no direct DataService draft call from QML / Shell / Presenter.

### K. Error Mapping

- authorization missing.
- disabled.
- input unavailable.
- stale snapshot.
- replay unavailable.
- invalid instrument.
- invalid side.
- invalid quantity.
- risk check failed.
- audit unavailable.
- duplicate draft.
- rollback.
- no silent success.

### L. Privacy

- no raw SQL in QML.
- no full trade_log payload in QML.
- no full snapshot payload in QML.
- no sensitive raw payload in issue text.
- no internal stack trace.

### M. Rollback

- disable TradeDraft.
- fallback keeps snapshot/replay/audit features unaffected.
- no fake draft.
- no trade execution fallback.
- no strategy fallback.
- no broker fallback.

### N. Regression

- TASK-119 through TASK-146 pass.
- all ShellAccounting gates pass.
- full CTest passes.
- transport 50 repeat passes.

## Required Probes

- static source scan.
- no `trade_draft` write scan.
- no `trade_log` write scan.
- no `trade_execution_group` write scan.
- no broker / strategy scan.
- no UI-triggered draft scan.
- no generic write action scan.
- no raw payload scan.
- audit policy documentation probe.
- rollback documentation probe.

## Go / No-Go Checklist

- [ ] docs/98 merged.
- [ ] docs/99 merged.
- [ ] TASK-119 through TASK-146 pass.
- [ ] TradeDraft authorization gate passes.
- [ ] full CTest passes.
- [ ] transport 50 passes.
- [ ] user explicitly authorized TradeDraft implementation.
- [ ] authorization token approved.
- [ ] audit behavior approved.
- [ ] no trade execution tests ready.
- [ ] no strategy / broker tests ready.
