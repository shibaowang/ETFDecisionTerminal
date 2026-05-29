# ShellAccounting QML Startup Registration Test Plan

## Document Purpose

This document defines tests required by a future production startup wiring task.
This document does not modify startup code.

## Test Matrix

### A. Startup Registration Location

- Centralized call exists.
- Only one production call exists.
- No scattered registration.

### B. Module Registration

- `ETFDecisionTerminal.ShellAccounting` 1.0 is registered.
- Allowlist types are registered.
- Forbidden types are absent.

### C. Startup Smoke

- App can start.
- QML engine can import the module.
- Startup does not need `DataServiceClient`.
- Startup does not need SQLite.
- Startup does not need `AccountingEngine`.

### D. No Production QML Page Binding

- No production accounting page unless separately authorized.
- No production QML import unless the task allows it.
- No QML trading buttons.

### E. Rollback

- Disabling the call makes the app start.
- No fallback to direct services.
- No fake data.
- No write path.

### F. Regression

- TASK-119 passes.
- TASK-121 passes.
- TASK-122 passes.
- TASK-125 passes.
- TASK-126 passes.
- TASK-127 passes.
- Full tests pass.
- transport 50 passes.

## Required Fakes / Probes

- Static source scan for registration call.
- App startup smoke.
- QQmlEngine import probe.
- Forbidden type probe.
- Forbidden token scan.
- No write action scan.

## Go / No-Go Checklist

- [ ] docs/69 merged.
- [ ] docs/70 merged.
- [ ] TASK-119 static gate passes.
- [ ] TASK-121 scaffold passes.
- [ ] TASK-122 runtime smoke passes.
- [ ] TASK-125 registration scaffold passes.
- [ ] TASK-126 implementation gate passes.
- [ ] TASK-127 helper implementation passes.
- [ ] Rollback strategy documented.
- [ ] User explicitly authorized startup wiring.
