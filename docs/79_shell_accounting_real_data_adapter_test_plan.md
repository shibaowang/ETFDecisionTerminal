# ShellAccounting Real Data Adapter Test Plan

## Document Purpose

TASK-134 defines the tests required for any future real data adapter or
DataService read-only action integration task. This document does not implement
the real adapter and does not implement DataService action.

## Test Matrix

### A. Read-only Action Contract

- allowed action names: `position.list`, `cash.summary`,
  `portfolio.pnl.summary`, `base_position.summary`, `sniper_pool.summary`.
- no write actions.
- no TradeDraft actions.
- no strategy actions.
- no broker order actions.

### B. Adapter Behavior

- success mapping.
- unavailable mapping.
- transport error mapping.
- protocol error mapping.
- timeout mapping.
- stale data mapping.
- no fake data fallback.

### C. Boundary

- no QML direct DataServiceClient.
- no presenter direct SQLite.
- no presenter direct AccountingEngine.
- no DataAccess direct from QML or presenter.
- no write-enabled action.

### D. Privacy

- no raw SQL in logs.
- no full trade_log payload in QML.
- no sensitive raw payload in issue text.
- privacy mode unaffected.

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
- TASK-132 passes.
- TASK-133 passes.
- full tests pass.
- transport 50 passes.

## Required Fakes / Probes

- fake adapter success.
- fake adapter unavailable.
- fake transport client.
- protocol error probe.
- timeout probe.
- forbidden token scan.
- no write action scan.
- no SQLite direct scan.
- no AccountingEngine direct scan.

## Go / No-Go Checklist

- [ ] docs/78 merged.
- [ ] docs/79 merged.
- [ ] TASK-119 through TASK-133 gates pass.
- [ ] rollback strategy documented.
- [ ] user explicitly authorized real data adapter implementation.
- [ ] read-only action names approved.
- [ ] no write path tests ready.

## TASK-135 Post-implementation Matrix

TASK-135 adds implementation tests for authorized read-only actions, fake
success mapping, unavailable mapping, transport error mapping, protocol error
mapping, timeout mapping, privacy / no raw payload exposure, no write path after
integration, no SQLite direct access after integration, no AccountingEngine
direct access after integration, and rollback readiness after integration.
Rollback fallback must not fake data, must not write DB, must not generate TradeDraft,
and must not bypass DataService with SQLite direct reads.
