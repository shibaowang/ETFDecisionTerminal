# Shell Accounting QML Binding Readiness Plan

## Document Purpose

This document defines the readiness plan for future QML integration with Shell
accounting ViewModel / Model objects.

This document does not modify QML, does not register QML types, and does not
implement a QML page.

## Current QML Integration Status

- QML is not wired to accounting actions.
- QML does not bind `ShellAccountingStatusObject`.
- QML does not bind `ShellPositionListModel`.
- QML does not directly call DataServiceClient.
- QML does not access SQLite.
- QML does not calculate accounting.

## QML-allowed Binding Candidates

Future QML may bind only read-only ShellServices objects such as:

- `ShellAccountingPresenter` or facade
- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`
- future `ShellCashSummaryObject`
- future `ShellPortfolioPnlObject`
- future `ShellBasePositionObject`
- future `ShellSniperPoolObject`
- future `ShellSniperTierListModel`

QML must bind these read-only objects only. QML must not bind DataService
payload directly and must not bind DataServiceClient directly.

## QML State Display Rules

- `Idle`: show pending refresh.
- `Loading`: show loading state.
- `Loaded`: show data.
- `Empty`: show empty result.
- `Unavailable`: show feature unavailable / data source unavailable.
- `Warning`: show data with warning.
- `Error`: show error.
- `Stale`: show stale data marker.

Empty is not Unavailable. Unavailable must not be displayed as normal empty
data. Warning must not be hidden. Error must not be hidden. Stale must not be
treated as OK.

## QML Privacy Display Rules

- QML displays only `displayText`.
- QML does not access `rawText` unless a later task explicitly adds a read-only
  role controlled by privacy policy.
- `privacyMode` is controlled by ShellServices.
- Masking is not calculated in QML.
- Issues and warnings are not hidden by privacy mode.

## QML Forbidden Behaviors

- Do not call DataServiceClient.
- Do not access SQLite.
- Do not call AccountingEngine.
- Do not calculate position / cash / PnL / basePosition / sniperPool.
- Do not generate TradeDraft.
- Do not generate trade suggestions.
- Do not execute strategies.
- Do not submit broker orders.
- Do not call `data.audit.append`.
- Do not call write actions.
- Do not bypass ShellServices.

## UI Component Candidates

Future read-only UI components may include:

- `AccountingStatusBanner`
- `AccountingIssuePanel`
- `PositionListReadOnlyTable`
- `CashSummaryReadOnlyCard`
- `PortfolioPnlReadOnlyCard`
- `BasePositionReadOnlyCard`
- `SniperPoolReadOnlyCard`
- `PrivacyAmountText`

This task does not create these components. Components can only display
read-only data and must not display trading buttons.

## QML Integration Go / No-Go Checklist

- [ ] Presenter / facade has been implemented.
- [ ] StatusObject has been implemented.
- [ ] IssueListModel has been implemented.
- [ ] PositionListModel has been implemented.
- [ ] QML direct DataServiceClient static check has passed.
- [ ] no TradeDraft / no trade suggestion tests have passed.
- [ ] privacy display tests have passed.
- [ ] Empty vs Unavailable tests have passed.
- [ ] QML smoke test plan has been defined.
- [ ] 用户明确授权 QML 接入.

## First QML Integration Recommended Scope

The first QML accounting integration should only include:

- read-only status banner
- read-only issue panel
- position list placeholder / unavailable display
- privacy display smoke
- no trading buttons
- no service write calls

Explicitly excluded:

- real `position.list` data display
- real cash / pnl / base / sniper data
- trading buttons
- TradeDraft
- strategy execution
- broker order

## TASK-115 Presenter Skeleton Status

TASK-115 adds a production `ShellAccountingPresenter` skeleton that can hold
the read-only status object, issue model, position list model, and optional
controller boundary. This satisfies one prerequisite for future QML binding,
but QML binding has not started.

The presenter skeleton is not a QML type and is not registered with QML.
Future QML work still requires a separate task and explicit authorization. QML
must continue to bind only ShellServices read-only objects, must preserve Empty
vs Unavailable, and must not add trading buttons, TradeDraft, trade suggestion,
strategy, broker, SQLite, AccountingEngine, or DataServiceClient access.

## TASK-116 Presenter Integration Status

TASK-116 verifies that the presenter can consume the concrete-port guard wrapper
path through the controller boundary. This is not QML binding. No QML file is
modified, no QML type is registered, and the UI still has no accounting
binding.

Future QML work must remain separately authorized and must treat the presenter
integration result as read-only state only: `Unavailable` is not `Empty`,
`POSITION_LIST_NOT_AVAILABLE` remains visible, and no trading buttons,
TradeDraft, trade suggestion, strategy, or broker controls are allowed.

## TASK-117 Presenter All Guard Refresh Status

TASK-117 wires the presenter to expose all five read-only guard refresh
methods plus `refreshAllReadOnly`. This does not start QML binding. No QML file
is modified and no QML type is registered.

Future QML must still bind only ShellServices read-only presentation objects.
The five guard actions currently surface as `Unavailable` with visible
`*_NOT_AVAILABLE` issues. Empty must remain distinct from Unavailable, warning
and error issues must not be hidden, and accounting UI must still avoid
trading buttons, TradeDraft, trade suggestion, strategy, broker, SQLite,
AccountingEngine, and DataServiceClient access.

## TASK-118 Smoke Plan and Static Gate

TASK-118 adds the QML smoke readiness plan and static gate:

- [docs/59_shell_accounting_qml_binding_smoke_plan.md](59_shell_accounting_qml_binding_smoke_plan.md)
- [docs/60_shell_accounting_qml_static_gate.md](60_shell_accounting_qml_static_gate.md)

These documents define the future QML object availability smoke matrix, state
display checks, guard payload display checks, privacy display checks,
no-trading-button checks, and denylist scans for direct service / write /
TradeDraft / strategy / broker references.

QML binding still requires separate explicit authorization. This task does not
modify QML, register QML types, create QML pages, call DataServiceClient, access
SQLite, call AccountingEngine, write database tables, generate TradeDraft, or
generate trade suggestions.
## TASK-119 QML Static Gate CTest

The QML static gate from docs/60 is now automated as CTests. This completes the
static denylist portion of the readiness plan: no direct service dependencies,
no database / AccountingEngine access, no write tokens, no premature
ShellAccounting binding, and no accounting-scope trade controls.

QML binding still requires separate authorization. TASK-119 does not register
QML types, create QML components, or bind ShellAccountingPresenter.

## TASK-120 ShellAccounting Read-only UI Readiness

The v0.6 readiness milestone references this plan as one readiness input.
ShellAccountingPresenter, status object, issue model, position model, presenter
all guard refresh, QML smoke plan, and QML static gate are in place, but QML
binding is still not implemented and still requires explicit user
authorization.
