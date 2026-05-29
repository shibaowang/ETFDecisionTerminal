# ShellAccounting QML Type Registration Test Plan

## Document Purpose

This document defines the tests required for a future ShellAccounting QML type
registration task. It does not implement registration, modify production QML, or
register any QML type.

## Test matrix

### A. Registration Construction

- Module loads.
- Allowed types are registered.
- No forbidden types are registered.
- App startup is not broken.

### B. Type allowlist

- `ShellAccountingPresenter` registered.
- `ShellAccountingStatusObject` registered.
- `ShellAccountingIssueListModel` registered.
- `ShellPositionListModel` registered.

### C. Type denylist

- `DataServiceClient` not registered.
- `DataServiceApi` not registered.
- `ShellAccountingDataServiceAdapter` not registered.
- `ShellAccountingDataServiceClientPortAdapter` not registered.
- `AccountingEngine` not registered.
- `SQLite` not registered.
- `DataAccess` not registered.

### D. Property / Method Denylist

- no `buy`.
- no `sell`.
- no `createTradeDraft`.
- no `brokerOrder`.
- no `strategyExecute`.
- no `writeEnabled true`.
- no write actions.

### E. Smoke Binding

- status readable.
- issues readable.
- positions readable.
- unavailable state readable.
- Empty is not Unavailable.
- privacy display readable.
- no trading buttons.

### F. Static Gates

- TASK-119 passes.
- TASK-121 passes.
- TASK-122 passes.
- full CTest passes.
- transport 50 passes.

## Required Test Helpers

Future registration tests should provide:

- QML engine smoke helper.
- module import helper.
- type availability probe.
- property scanner.
- method scanner.
- forbidden token scanner.
- production QML static scan.

## Go / No-Go Checklist

- [ ] `docs/65_shell_accounting_qml_type_registration_boundary.md` boundary has
  been merged.
- [ ] `docs/66_shell_accounting_qml_type_registration_test_plan.md` test plan has
  been merged.
- [ ] TASK-119 static gate passes.
- [ ] TASK-121 scaffold passes.
- [ ] TASK-122 runtime smoke passes.
- [ ] read-only type allowlist has been confirmed.
- [ ] forbidden type denylist has been confirmed.
- [ ] forbidden method denylist has been confirmed.
- [ ] 用户明确授权 QML type registration.
