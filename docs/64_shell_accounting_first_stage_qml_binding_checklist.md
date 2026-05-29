# ShellAccounting First-stage QML Binding Checklist

## Document Purpose

This checklist determines whether the first production QML binding stage can
begin.

This document does not modify QML.

## Go / No-Go Checklist

- [ ] TASK-119 QML static gate has been merged and passes.
- [ ] TASK-121 QML binding smoke scaffold has been merged and passes.
- [ ] TASK-122 QML runtime smoke CTest has been merged and passes.
- [ ] `ShellAccountingPresenter` has been merged.
- [ ] `ShellAccountingStatusObject` has been merged.
- [ ] `ShellAccountingIssueListModel` has been merged.
- [ ] `ShellPositionListModel` has been merged.
- [ ] Presenter all guard refresh has been merged.
- [ ] no TradeDraft / no trade suggestion tests pass.
- [ ] no direct DataServiceClient static gate passes.
- [ ] no direct SQLite / AccountingEngine static gate passes.
- [ ] QML task scope is explicitly read-only.
- [ ] 用户明确授权 production QML binding.

## First-stage Allowed Content

- read-only placeholder page.
- status banner.
- issue panel.
- unavailable state display.
- privacy display smoke.
- no trading buttons static check.
- no write action static check.

## First-stage Forbidden Content

- real positions table.
- real cash summary.
- real PnL.
- real base position.
- real sniper pool.
- TradeDraft.
- buy / sell action.
- broker order.
- strategy execution.
- write-enabled action.
- direct DataServiceClient usage.
- direct SQLite usage.
- direct AccountingEngine usage.

## Required Validation

- CMake configure.
- build.
- full CTest.
- `shell_accounting_qml_static_gate`.
- `shell_accounting_qml_binding_smoke`.
- `shell_accounting_qml_smoke`.
- future production QML smoke.
- transport 50 repeat.
- `git status` clean.

## TASK-124 QML Type Registration Boundary

Before the first production QML binding stage registers any ShellAccounting type,
the registration boundary in
`docs/65_shell_accounting_qml_type_registration_boundary.md` and the test plan in
`docs/66_shell_accounting_qml_type_registration_test_plan.md` must be reviewed.
Registration still requires explicit authorization and must remain read-only,
with no DataServiceClient, SQLite, AccountingEngine, write action, TradeDraft,
trade suggestion, broker order, or strategy execution exposure.

## TASK-125 Registration Scaffold Gate

The first-stage Go / No-Go decision must include a passing
`shell_accounting_qml_type_registration` scaffold run. That scaffold proves the
allowed type list, forbidden type list, method/property denylist, module import
boundary, gate dependency, and no-production-change boundary remain intact.

## TASK-126 Implementation Gate

The first-stage Go / No-Go decision must also include the TASK-126
implementation gate and rollback strategy. Registration is still a separate
authorization step and must preserve read-only types, no forbidden exposure, and
rollback / disable behavior.

## TASK-127 Type Registration Implementation

The first-stage registration Go / No-Go item is now satisfied by the centralized
helper and module import tests. Production QML binding remains a separate
Go / No-Go decision and still excludes real positions, cash, PnL, base position,
sniper pool, TradeDraft, broker order, strategy execution, and write actions.

## TASK-128 Checklist Update

Before any production startup wiring, docs/69 and docs/70 must be merged and
the `shell_accounting_qml_startup_registration` gate must pass. Startup wiring
does not authorize production QML page binding or real accounting data display.
