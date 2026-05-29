# ShellAccounting Production QML Binding Boundary

## Document Purpose

This document defines the boundary for future ShellAccounting production QML
binding.

This document does not modify QML. This document does not register a QML type.
This document does not implement a QML page. This document is for review before
any production QML integration task.

## Current State

- `ShellAccountingPresenter` exists.
- `ShellAccountingStatusObject` exists.
- `ShellAccountingIssueListModel` exists.
- `ShellPositionListModel` exists.
- TASK-119 QML static gate exists.
- TASK-121 QML binding smoke scaffold exists.
- TASK-122 test-only QML smoke CTest exists.
- QML is not wired to accounting actions.
- Real accounting actions are not implemented.
- no TradeDraft and no trade suggestion remain active boundaries.

## Allowed Production QML Binding Objects

Future production QML may bind only read-only ShellServices objects:

- `ShellAccountingPresenter`
- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`
- future `ShellCashSummaryObject`
- future `ShellPortfolioPnlObject`
- future `ShellBasePositionObject`
- future `ShellSniperPoolObject`
- future `ShellSniperTierListModel`

The binding is read-only. QML must not bind raw DataService payloads. QML must
not call DataServiceClient directly. QML must not directly access SQLite,
DataAccess, or AccountingEngine.

## Forbidden Production QML Dependencies

Production accounting QML must not reference:

- `DataServiceClient`
- `DataServiceApi`
- `SQLite`
- `SQLiteConnection`
- `DataAccess`
- `AccountingEngine`
- `AccountingReplayEngine`
- `AccountingTradeFactReader`
- `ShellAccountingDataServiceAdapter`
- `ShellAccountingDataServiceClientPortAdapter`
- `trade_log`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `data.audit.append`
- `writeEnabled: true`

## First-stage Production QML Allowed Scope

The first production QML binding task should only allow:

- read-only placeholder.
- Unavailable state display.
- issue panel.
- status banner.
- privacy display smoke.
- no trading buttons.
- no write action.
- no real accounting data.

The first stage must not include:

- real position table.
- real cash summary.
- real portfolio PnL.
- real base position.
- real sniper pool.
- buy / sell buttons.
- TradeDraft.
- strategy execution.
- broker order.
- write-enabled action.

## QML Type Registration Boundary

QML type registration must be a separate task. Registered objects must be
read-only and must not expose write methods. Registered objects must not expose
buy, sell, `createTradeDraft`, `brokerOrder`, or `strategyExecute`.

After registration, TASK-119 QML static gate must pass. TASK-122 smoke CTest,
or a production-equivalent smoke test, must pass. QML type registration does
not mean real accounting data is available.

## QML Page / Component Boundary

A production QML page must be a separate task. The first stage can only use a
read-only placeholder. Components can only consume ShellServices ViewModel /
Model objects. Components must not import or reference DataServiceClient,
operate DataService actions directly, display trading buttons, or display a
create-draft button.

## State Display Rules

Production QML must distinguish:

- Idle.
- Loading.
- Loaded.
- Empty.
- Unavailable.
- Warning.
- Error.
- Stale.

Empty is not Unavailable. `implemented=false` maps to Unavailable. Warning must
not be hidden. Error must not be hidden. Stale must not be treated as OK.

## Privacy Display Rules

QML only displays `displayText`. QML does not calculate `maskedText`.
`privacyMode` is controlled by ShellServices. QML does not modify `rawText`.
Issues and warnings are not hidden by privacy mode. Unavailable and Empty do not
fabricate amounts.

## No-trade UI Rules

Production accounting QML must not display:

- buy buttons.
- sell buttons.
- `createTradeDraft`.
- `brokerOrder`.
- `strategyExecute`.
- write-enabled action.

`sellableAboveBaseAmountText` is not a sell suggestion.
`remainingAmountText` is not a buy suggestion. `completed` is not calculated by
QML.

## Required Gates Before Production QML Binding

Before any production QML binding task modifies `apps/ETFDecisionShell/qml`, it
must pass or explicitly require:

- TASK-119 QML static gate.
- TASK-121 QML binding smoke scaffold.
- TASK-122 QML runtime smoke CTest.
- DevDocs static checks.
- full CTest.
- `transport_local_socket_echo` repeat 50.
- no-write guard tests.
- no-trade tests.
- 用户明确授权 production QML binding.

## Rollback / Fallback

Production QML integration must be reversible. If the presenter is not
configured, the UI displays Unavailable. If a guard returns not available, the
UI displays Unavailable. QML must not fabricate data, must not swallow issues,
and must not fall back to direct DataServiceClient access.
