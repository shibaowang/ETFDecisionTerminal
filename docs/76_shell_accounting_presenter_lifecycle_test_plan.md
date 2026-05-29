# ShellAccounting Presenter Lifecycle Test Plan

TASK-132 defines the test plan for a future ShellAccountingPresenter lifecycle
and context exposure task. This document does not create presenter and does not
expose a context property.

## Document Purpose

This plan fixes the tests required before any future presenter lifecycle /
context exposure implementation can be accepted.

## Test Matrix

### A. Presenter Creation

- Creation location explicitly authorized.
- Only one production creation point.
- Ownership explicit.
- No hidden singleton.
- No QML-created presenter.

### B. Context Exposure

- Context property name explicit.
- No anonymous exposure.
- No DataServiceClient exposure.
- No adapter exposure.
- No write-enabled object exposure.
- QML tolerates null.

### C. Null / Unavailable State

- Page renders with `accountingPresenter` null.
- No fake data fallback.
- No direct service fallback.
- No DB fallback.
- No write fallback.
- No TradeDraft fallback.

### D. Forbidden Runtime Access

- No SQLite.
- No AccountingEngine.
- No DataAccess.
- No DataServiceClient.
- No trade_log write.
- No trade_draft write.
- No audit append.
- No broker order.
- No strategy execution.

### E. Regression

- TASK-119 passes.
- TASK-121 passes.
- TASK-122 passes.
- TASK-125 passes.
- TASK-126 passes.
- TASK-127 passes.
- TASK-128 passes.
- TASK-129 passes.
- TASK-130 passes.
- TASK-131 passes.
- Full tests pass.
- Transport 50 passes.

## Required Fakes / Probes

- Static source scan for presenter creation.
- Static scan for context property.
- QML smoke probe for null presenter.
- Forbidden token scan.
- No write action scan.
- No trade UI scan.
- Rollback scan.

## Go / No-Go Checklist

- [ ] docs/75 merged.
- [ ] docs/76 merged.
- [ ] TASK-119 static gate passes.
- [ ] TASK-121 scaffold passes.
- [ ] TASK-122 runtime smoke passes.
- [ ] TASK-125 registration scaffold passes.
- [ ] TASK-126 implementation gate passes.
- [ ] TASK-127 helper implementation passes.
- [ ] TASK-128 startup gate passes.
- [ ] TASK-129 startup wiring passes.
- [ ] TASK-130 production QML binding gate passes.
- [ ] TASK-131 production QML binding shell passes.
- [ ] Rollback strategy documented.
- [ ] User explicitly authorized presenter lifecycle implementation.

## TASK-133 Post-implementation Test Matrix

TASK-133 adds implementation tests for:

- authorized presenter creation in production startup.
- exactly one `accountingPresenter` context property.
- presenter ownership and destruction order.
- unavailable/read-only page state after context exposure.
- no DataServiceClient, SQLite, DataAccess, or AccountingEngine access.
- no database writes, TradeDraft generation, trade suggestion, strategy
  execution, broker order, or trading UI.
- rollback readiness for removing presenter construction, context exposure,
  minimal QML plumbing, tests, and docs.
