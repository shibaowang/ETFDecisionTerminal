# ShellAccounting Next Phase Review

## Document Purpose

This document reviews whether ShellAccounting can enter the next QML binding
phase after the read-only UI readiness milestone.

This document does not modify QML and does not implement a QML page.

## Readiness Summary

- Presenter / controller / adapter / concrete port chain is complete.
- ViewModel / Model skeleton is complete.
- QML static gate is complete.
- QML smoke plan is complete.
- QML remains unwired.
- Real accounting action remains unimplemented.

## Go / No-Go Checklist for QML Binding

- [ ] `ShellAccountingPresenter` has been merged.
- [ ] `ShellAccountingStatusObject` has been merged.
- [ ] `ShellAccountingIssueListModel` has been merged.
- [ ] `ShellPositionListModel` has been merged.
- [ ] Presenter all guard refresh has been merged.
- [ ] QML static gate has been merged.
- [ ] QML static gate full run passes.
- [ ] no TradeDraft / no trade suggestion gate passes.
- [ ] QML direct DataServiceClient denylist passes.
- [ ] QML direct SQLite / AccountingEngine denylist passes.
- [ ] QML binding task scope is limited to read-only behavior.
- [ ] 用户明确授权 QML binding.

## First QML Binding Recommended Scope

First-stage QML binding should only allow:

- read-only placeholder page or read-only status in an existing page.
- read-only status banner.
- read-only issue panel.
- Unavailable state display.
- privacy display smoke.
- no trading buttons.
- no write action.

Explicitly excluded:

- real positions table.
- real cash / PnL / base / sniper data.
- trading buttons.
- TradeDraft.
- strategy execution.
- broker order.
- write-enabled action.

## Real Accounting Action Remains Separate

QML binding is not real `position.list`. QML binding is not DataService calling
AccountingEngine. QML binding is not SQLite facts query integration. Real
accounting action still requires separate authorization and no-write tests.

## Risks

- QML directly calls DataServiceClient.
- QML directly displays DataService payload.
- QML treats Unavailable as Empty.
- QML hides warning or error state.
- QML displays trading buttons.
- QML triggers TradeDraft.
- QML bypasses privacy display.
- QML implies a write action.
- Guard payload is mistaken for real data.

## Recommended Next Tasks

- TASK-121: ShellAccounting QML binding smoke test scaffold.
- TASK-121: ShellAccounting QML read-only placeholder page contract docs.
- TASK-121: ShellAccounting QML type registration boundary docs.

Do not directly modify QML to show real data. Do not directly implement real
`position.list` in a QML task. Do not add trading buttons in QML.

## TASK-121 Smoke Scaffold Status

The QML binding smoke scaffold is now a precondition for QML binding. It locks
the object contract, state matrix, guard payload, issue / privacy, no-trade UI,
and static gate dependency before any QML file is modified.

QML binding still requires 用户明确授权 QML binding.
