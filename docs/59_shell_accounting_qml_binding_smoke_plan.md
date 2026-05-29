# ShellAccounting QML Binding Smoke Plan

## Document Purpose

This document defines the future ShellAccounting QML read-only binding smoke
plan. It is a review artifact before QML integration.

This document does not modify QML, does not register QML types, does not
implement QML pages, and does not add production behavior.

## Current Status

- `ShellAccountingPresenter` exists.
- `ShellAccountingStatusObject` exists.
- `ShellAccountingIssueListModel` exists.
- `ShellPositionListModel` exists.
- The presenter supports five guard refresh methods.
- The five guard payloads currently map to `Unavailable`.
- QML has not been wired to accounting actions.
- Real accounting actions are not implemented.
- no TradeDraft / no trade suggestion behavior remains active.

## Future Read-only QML Binding Targets

Future QML may bind only ShellServices read-only objects:

- `ShellAccountingPresenter`
- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`
- future `ShellCashSummaryObject`
- future `ShellPortfolioPnlObject`
- future `ShellBasePositionObject`
- future `ShellSniperPoolObject`
- future `ShellSniperTierListModel`

The binding is read-only. QML must not calculate accounting values, directly
bind DataService payloads, directly call DataServiceClient, access SQLite, or
bypass ShellServices.

## QML Binding Smoke Matrix

### A. Object Availability

- Presenter object can be injected.
- Status object is readable.
- Issue model is readable.
- Position model is readable.

### B. State Display

- `Idle` can be displayed.
- `Loading` can be displayed.
- `Loaded` can be displayed.
- `Empty` can be displayed.
- `Unavailable` can be displayed.
- `Warning` can be displayed.
- `Error` can be displayed.
- `Stale` can be displayed.

### C. Guard Payload Display

- `POSITION_LIST_NOT_AVAILABLE -> Unavailable`.
- `CASH_SUMMARY_NOT_AVAILABLE -> Unavailable`.
- `PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE -> Unavailable`.
- `BASE_POSITION_SUMMARY_NOT_AVAILABLE -> Unavailable`.
- `SNIPER_POOL_SUMMARY_NOT_AVAILABLE -> Unavailable`.

### D. Issue Display

- issue code is visible.
- issue level is visible.
- issue message is visible.
- blocking flag is visible.
- warning is not hidden.

### E. Privacy Display

- `privacyMode=false` displays `displayText`.
- `privacyMode=true` displays masked text supplied by ShellServices.
- `rawText` is not calculated by QML.
- issue / warning display is not hidden by privacy mode.

### F. No-trade UI

- no buy button.
- no sell button.
- no generate TradeDraft button.
- no strategy execution button.
- no broker order button.

## QML State Display Rules

- Empty is a successful response with no data.
- Unavailable means the feature is not implemented or the data source is not
  available.
- Empty is not Unavailable.
- Warning can be displayed with data.
- Error must be visible.
- Stale must be marked as stale.
- `implemented=false` must display as Unavailable.
- `readOnly=true` / `writeEnabled=false` must remain preserved.

## QML Privacy Rules

- QML displays only `displayText`.
- QML does not calculate `maskedText`.
- QML does not mutate `rawText`.
- `privacyMode` is controlled by ShellServices.
- Hiding amounts does not hide issue / warning display.
- Unavailable / Empty states do not fake amount text.

## QML No-trade Rules

- Read-only accounting pages do not display trading buttons.
- Do not display buy.
- Do not display sell.
- Do not display `createTradeDraft`.
- Do not display `brokerOrder`.
- Do not display `strategyExecute`.
- `sellableAboveBaseAmountText` is not a sell suggestion.
- `remainingAmountText` is not a buy suggestion.
- `completed` is not calculated by QML.
- QML does not trigger any write operation.

## QML Smoke Test Technical Plan

Future tasks may add:

- QML smoke tests.
- static scans.
- `QQmlEngine` component load checks.
- object property binding smoke checks.
- role existence smoke checks.
- no direct DataServiceClient string scans.
- no trading button string scans.
- no write action string scans.

This task does not implement these QML tests. QML smoke tests require a
separate task.

## First QML Integration Recommended Scope

The first QML accounting integration should only include:

- read-only status banner.
- read-only issue panel.
- position unavailable placeholder.
- privacy amount display smoke.
- no trading buttons static check.

Explicitly excluded:

- real position data.
- real cash / PnL / base / sniper data.
- trading buttons.
- TradeDraft.
- strategy execution.
- broker order.
- write action.

## TASK-119 Static Gate Automation

TASK-119 adds QML static gate CTests for the static portion of this smoke plan.
The tests scan `apps/ETFDecisionShell/qml` for direct service / database /
engine references, write tokens, premature ShellAccounting bindings, and
trading tokens in accounting-scoped files.

The automated static checks cover the smoke plan's no direct DataServiceClient,
no SQLite, no AccountingEngine, no write action, and no-trade UI gates. QML
smoke UI tests, QML component load tests, and actual QML binding remain
unimplemented and require a separate authorized task.

## TASK-120 ShellAccounting Read-only UI Readiness

The v0.6 readiness milestone is documented in
`docs/61_shell_accounting_readonly_ui_readiness_milestone.md`. The milestone
confirms the smoke plan is ready as a pre-QML input, but it does not implement
QML binding, QML smoke UI tests, or real accounting actions.

## TASK-121 QML Binding Smoke Scaffold

TASK-121 adds QML binding smoke scaffold CTests. The scaffold automates the
object contract, state matrix, guard payload, issue / privacy, no-trade UI, and
static gate dependency portions of this smoke plan.

This is still not a real QML page and does not register QML types. The tests
only lock the future smoke expectations so a later QML binding task cannot
bypass read-only ShellServices objects, Empty vs Unavailable, guard
Unavailable mapping, issue visibility, privacy display, no-trade UI rules, or
the TASK-119 static gate.

## TASK-122 QML Smoke CTest Implementation

TASK-122 adds test-only runtime smoke CTest coverage under
`tests/ShellAccountingQmlBindingSmokeRuntime`.

Automated smoke coverage now includes:

- object availability through inline QML reading fake read-only presenter,
  status, issue, and position properties.
- state display for Idle, Loading, Loaded, Empty, Unavailable, Warning, Error,
  and Stale.
- five guard unavailable payloads mapped to Unavailable.
- issue visibility for code, level, message, blocking, sourceId, warning, and
  unknown issues.
- privacy display where QML reads only `displayText` and does not calculate
  `maskedText`.
- no-trade UI token checks on the test-only smoke QML.
- static gate regression confirming TASK-119 remains a prerequisite.

This is not production QML binding. It does not modify
`apps/ETFDecisionShell/qml`, does not register production QML types, and does
not implement an accounting QML page.

## TASK-123 Production QML Boundary

Production QML binding boundary is
`docs/63_shell_accounting_production_qml_binding_boundary.md`. The first-stage
QML checklist is
`docs/64_shell_accounting_first_stage_qml_binding_checklist.md`.

The smoke plan remains a prerequisite. Production QML binding must still pass
TASK-119 static gate, TASK-121 scaffold, and TASK-122 runtime smoke CTest before
any QML file is modified.

## TASK-124 QML Type Registration Boundary

QML type registration boundary is
`docs/65_shell_accounting_qml_type_registration_boundary.md`. The QML type
registration test plan is
`docs/66_shell_accounting_qml_type_registration_test_plan.md`. Registration must
preserve the smoke plan object contract, state matrix, guard payload,
issue/privacy, no-trade, and static gate dependency expectations.

## TASK-125 Registration Scaffold

QML type registration scaffold tests are now added under
`tests/ShellAccountingQmlTypeRegistrationScaffold`. They lock the future
registration allowlist, denylist, method/property denylist, module import
boundary, gate dependency, and no-production-change expectations. QML type
registration still requires separate authorization.

## TASK-126 Implementation Gate

The QML type registration implementation gate is now documented in docs/67, with
rollback strategy in docs/68. Future QML binding and registration work must keep
the smoke plan, static gate, scaffold, implementation gate, rollback strategy,
and no-production-registration checks passing.

## TASK-127 Type Registration Implementation

TASK-127 implements the first-stage read-only ShellAccounting QML type
registration through a centralized helper. The module import and allowlist
registration are now test-covered, but production QML page binding still
requires a separate authorized task and real accounting actions remain
unimplemented.
