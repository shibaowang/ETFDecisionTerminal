# ShellAccounting Production QML Binding Test Plan

TASK-130 owns this production QML binding test plan.

## Document Purpose

This document defines tests required by a future production QML binding task.
This document does not modify production QML and does not add a production
import, page, or component.

## Test Matrix

This Test matrix fixes the required checks for a future production QML binding
task.

### A. Production QML Import

- Import location explicitly authorized.
- Import appears only in an allowed production QML file.
- No scattered imports.
- No dynamic import.

### B. Production Page / Component

- Page exists only when authorized.
- Page is read-only.
- No trading buttons.
- No TradeDraft action.
- No write action.

### C. Presenter / Context

- Presenter lifecycle explicit.
- Context property explicit.
- No anonymous exposure.
- No `DataServiceClient` exposure.
- No adapter exposure.
- No write-enabled object exposure.

### D. Unavailable / Disabled State

- Unavailable state visible.
- No fake data fallback.
- No direct service fallback.
- No DB fallback.
- No write fallback.

### E. Forbidden Runtime Access

- No SQLite.
- No `AccountingEngine`.
- No `DataAccess`.
- No `DataServiceClient` in QML.
- No `trade_log` write.
- No `trade_draft` write.
- No audit append.
- No broker order.
- No strategy execution.

### F. Regression

- TASK-119 passes.
- TASK-121 passes.
- TASK-122 passes.
- TASK-125 passes.
- TASK-126 passes.
- TASK-127 passes.
- TASK-128 passes.
- TASK-129 passes.
- Full tests pass.
- transport 50 passes.

## Required Fakes / Probes

- Static source scan for QML import.
- Static scan for page/component route.
- Static scan for context property.
- QML smoke probe for Unavailable state.
- Forbidden token scan.
- No write action scan.
- No trade UI scan.

## Go / No-Go Checklist

- [ ] docs/72 merged.
- [ ] docs/73 merged.
- [ ] TASK-119 static gate passes.
- [ ] TASK-121 scaffold passes.
- [ ] TASK-122 runtime smoke passes.
- [ ] TASK-125 registration scaffold passes.
- [ ] TASK-126 implementation gate passes.
- [ ] TASK-127 helper implementation passes.
- [ ] TASK-128 startup gate passes.
- [ ] TASK-129 startup wiring passes.
- [ ] Rollback strategy documented.
- [ ] User explicitly authorized production QML binding.

## TASK-131 Post-implementation Test Matrix

TASK-131 adds production QML binding implementation tests for:

- the authorized `ETFDecisionTerminal.ShellAccounting 1.0` import.
- the authorized `ShellAccountingReadOnlyPage.qml` component.
- the `shell_accounting` navigation entry and ContentHost mapping.
- Unavailable / disabled / read-only default state.
- nullable `ShellAccountingPresenter accountingPresenter: null` only.
- no presenter creation and no accounting context exposure.
- no DataServiceClient, SQLite, DataAccess, or AccountingEngine access.
- no write path, TradeDraft generation, trade suggestion, broker action,
  strategy execution, or trading UI.
- rollback readiness for removing the page, ContentHost mapping, registry
  entry, QML file list entry, tests, and docs.
