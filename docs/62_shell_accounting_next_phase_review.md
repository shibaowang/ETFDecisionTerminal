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
- [ ] User explicitly authorized QML binding.

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

QML binding still requires User explicitly authorized QML binding.
QML binding still requires 用户明确授权 QML binding.

## TASK-122 QML Smoke CTest Status

TASK-122 adds test-only runtime QML smoke CTests. The tests load inline QML with
fake read-only presenter/status/issue/position properties and verify the smoke
rules from docs/59 without modifying production QML.

This makes QML binding smoke a stronger precondition, but it is still not
production QML binding. A future QML task must still be explicitly authorized,
must pass TASK-119 static gate, TASK-121 scaffold, and TASK-122 smoke CTests,
and must remain read-only.

## TASK-123 Production QML Binding Boundary

TASK-123 adds docs/63 production QML binding boundary and docs/64 first-stage
QML binding checklist. Production QML binding still requires explicit
authorization. The first production stage must remain read-only placeholder /
Unavailable / issue / privacy smoke and must not include real accounting data,
trading buttons, TradeDraft, broker order, strategy execution, or write action.

## TASK-124 QML Type Registration Boundary

TASK-124 adds docs/65 QML type registration boundary and docs/66 registration
test plan. QML registration remains a separate explicitly authorized task and
must preserve TASK-119 static gate, TASK-121 scaffold, TASK-122 runtime smoke,
read-only type exposure, no-write, and no-trade boundaries.

## TASK-125 Registration Scaffold

TASK-125 adds automated scaffold tests for the future QML type registration
task. QML registration remains separately authorized and must pass the scaffold,
static gate, binding smoke scaffold, runtime smoke CTest, full CTest, and
transport repeat before it can be considered ready.

## TASK-126 Implementation Gate

TASK-126 adds implementation gate tests and rollback strategy docs. Future QML
type registration remains separately authorized and must pass the implementation
gate, location gate, allowlist gate, forbidden exposure gate, rollback gate, and
no-production-registration gate.

## TASK-127 Type Registration Implementation

TASK-127 implements the first-stage type registration, but the next phase for
production QML binding still requires explicit authorization. The allowed next
UI step remains a read-only placeholder / unavailable / issue / privacy smoke,
not real accounting data or trading UI.

## TASK-128 Review Update

The startup registration gate is now defined. A future startup wiring task must
pass TASK-119 / TASK-121 / TASK-122 / TASK-125 / TASK-126 / TASK-127 and the new
TASK-128 gate, must be explicitly authorized, and must not be treated as QML
page binding or real accounting action implementation.
