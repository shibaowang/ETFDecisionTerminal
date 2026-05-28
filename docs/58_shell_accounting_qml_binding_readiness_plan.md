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
