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
