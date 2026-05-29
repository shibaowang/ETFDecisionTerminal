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

## TASK-125 Scaffold Status

TASK-125 adds the test scaffold for this plan. The scaffold covers:

- `shell_accounting_qml_type_registration_allowlist_scaffold`.
- `shell_accounting_qml_type_registration_denylist_scaffold`.
- `shell_accounting_qml_type_registration_method_denylist_scaffold`.
- `shell_accounting_qml_type_registration_module_import_scaffold`.
- `shell_accounting_qml_type_registration_gate_dependency`.
- `shell_accounting_qml_type_registration_no_production_change`.

Future registration implementation must pass these tests or production-equivalent
tests. The scaffold is not production QML type registration and does not modify
production QML.

## TASK-126 Implementation Gate Tests

TASK-126 adds implementation gate tests:

- `shell_accounting_qml_type_registration_implementation_gate`.
- `shell_accounting_qml_type_registration_location_gate`.
- `shell_accounting_qml_type_registration_allowlist_gate`.
- `shell_accounting_qml_type_registration_forbidden_exposure_gate`.
- `shell_accounting_qml_type_registration_rollback_gate`.
- `shell_accounting_qml_type_registration_no_production_registration`.

Future registration implementation must pass these gates in addition to the
TASK-125 scaffold and the TASK-119 / TASK-121 / TASK-122 gates.

## TASK-127 Registration Implementation Tests

TASK-127 adds implementation tests:

- `shell_accounting_qml_type_registration_module_import`.
- `shell_accounting_qml_type_registration_allowlist_registered`.
- `shell_accounting_qml_type_registration_forbidden_types_absent`.
- `shell_accounting_qml_type_registration_forbidden_methods_absent`.
- `shell_accounting_qml_type_registration_uncreatable_readonly`.
- `shell_accounting_qml_type_registration_no_production_qml_binding`.
- `shell_accounting_qml_type_registration_gate_regression`.

These tests prove the module imports, the four allowlist types are registered as
read-only uncreatable types, forbidden types and methods are absent, and
production QML still does not bind the module.

## TASK-128 Startup Registration Gate Tests

TASK-128 adds startup registration gate tests covering required pre-gates,
startup location policy, current no-production-wiring state, forbidden exposure
scanning, and rollback policy. Future startup wiring implementation must pass
these tests or their production-equivalent successor.
