# Shell Accounting Presenter Contract

## Document Purpose

This document defines the future `ShellAccountingPresenter` / facade boundary
before any QML integration work starts.

This document does not implement a presenter, does not modify QML, does not
register QML types, and does not call DataServiceClient. It is a review
contract for a separately authorized QML-facing Shell accounting task.

## Current Status

- `ShellAccountingReadOnlyController` exists.
- `ShellAccountingStatusObject` exists.
- `ShellAccountingIssueListModel` exists.
- `ShellPositionListModel` exists.
- `ShellAccountingDataServiceAdapter` plus the concrete port can consume
  DataServiceClient read-only accounting guard wrappers.
- QML is not wired to accounting actions.
- Real accounting actions are not implemented.
- TradeDraft, trade suggestion, strategy execution, and broker order behavior
  are not authorized.

## Presenter Layering Draft

Future layering:

```text
QML
-> ShellAccountingPresenter / Facade
-> ShellAccounting ViewModel / Model
-> ShellAccountingReadOnlyController
-> ShellAccountingServiceAdapter
-> ShellAccountingDataServiceAdapter
-> DataServiceClient read-only wrappers
-> DataService accounting actions
```

Forbidden directions:

- QML -> DataServiceClient
- QML -> ShellAccountingDataServiceAdapter
- QML -> SQLite
- QML -> AccountingEngine
- Presenter -> SQLite
- Presenter -> AccountingEngine
- Presenter -> DataAccess
- Presenter -> write action
- Presenter -> TradeDraft
- Presenter -> strategy execution
- Presenter -> broker order

## Candidate Presenter Responsibilities

A future presenter may:

- provide a QML-bindable read-only status object
- provide a QML-bindable read-only issue model
- provide a QML-bindable read-only position list model
- provide `refreshPositionList` / `refreshAllAccountingReadOnly` entry points
- expose loading / unavailable / warning / error / stale state
- expose last refresh time
- expose `privacyMode` / `displayText`
- forward user-triggered read-only refresh requests

Refresh is a read-only action. The presenter must not write database tables,
must not calculate accounting, must not generate trade suggestions, must not
generate TradeDraft, and must not execute strategies.

## Non-responsibilities

The presenter must not implement or own:

- replay algorithm
- SQLite query
- DataAccess facts loading
- DataService action implementation
- market price fetching
- FX rate fetching
- TradeLog write
- snapshot write
- TradeDraft generation
- broker order
- strategy execution
- QML rendering details
- trade suggestion generation

## Action-specific Presenter Mapping

### position.list

- `ShellPositionListModel` displays rows.
- `implemented=false -> Unavailable`.
- `POSITION_LIST_NOT_AVAILABLE -> Unavailable`.
- `MARKET_PRICE_MISSING -> Warning`.
- `MULTI_CURRENCY_UNSUPPORTED -> Error`.
- No trade suggestion is generated.

### cash.summary

- Future `ShellCashSummaryObject` displays the summary.
- `CASH_SUMMARY_NOT_AVAILABLE -> Unavailable`.
- The presenter must not infer cash from snapshots.

### portfolio.pnl.summary

- Future `ShellPortfolioPnlObject` displays the summary.
- `PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE -> Unavailable`.
- The presenter must not fake total assets or PnL.

### base_position.summary

- Future `ShellBasePositionObject` displays base-position status.
- `BASE_POSITION_SUMMARY_NOT_AVAILABLE -> Unavailable`.
- `sellableAboveBaseAmountText` is not a sell suggestion.
- The presenter must not generate TradeDraft.

### sniper_pool.summary

- Future `ShellSniperPoolObject` / `ShellSniperTierListModel` displays sniper
  pool status.
- `SNIPER_POOL_SUMMARY_NOT_AVAILABLE -> Unavailable`.
- `remainingAmountText` is not a buy suggestion.
- `completed` is not calculated by QML.
- The presenter must not generate TradeDraft.

## Refresh Lifecycle

- `Idle -> Loading`.
- `Loading -> Loaded / Empty / Unavailable / Warning / Error / Stale`.
- Refresh failure must not write database tables.
- Refresh failure must not generate TradeDraft.
- Refresh failure must not clear the previous successful data unless a later
  design explicitly authorizes that behavior.
- Timeout and transport error must remain visible.
- Blocking issues must remain visible.

## Privacy Boundary

- Presenter / model provides `displayText`.
- QML does not calculate `maskedText`.
- `privacyMode=true` hides amount display only.
- `rawText` does not change when privacy mode changes.
- Issues and warnings stay visible in privacy mode.
- Unavailable and Empty states do not fake amount text.

## No Trade Action Boundary

- Presenter does not expose buy.
- Presenter does not expose sell.
- Presenter does not expose `createTradeDraft`.
- Presenter does not expose `brokerOrder`.
- Presenter does not expose `strategyExecute`.
- Read-only accounting UI does not display trading buttons.
- `sellableAboveBaseAmountText` is not a sell suggestion.
- `remainingAmountText` is not a buy suggestion.

## Testing Expectations

Future presenter implementation must include:

- construction test
- no QML direct DataServiceClient static check
- `implemented=false -> Unavailable` mapping
- Empty vs Unavailable mapping
- warning / error / stale mapping
- issue visibility
- privacy display
- no trade actions
- no DataServiceClient direct dependency
- no SQLite / no AccountingEngine dependency
- refresh no-write behavior

## TASK-115 Production Skeleton

TASK-115 adds the production-side `ShellAccountingPresenter` skeleton. The
skeleton owns and exposes only read-only ShellServices presentation objects:

- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`
- an optional `ShellAccountingReadOnlyController`

The skeleton keeps `readOnly=true`, `writeEnabled=false`,
`tradeDraftGenerationEnabled=false`, `tradeSuggestionEnabled=false`,
`strategyExecutionEnabled=false`, and broker submission disabled. It does not
register a QML type, does not modify QML, and does not call DataServiceClient.

`refreshPositionList` and `refreshAllReadOnly` are presenter boundary methods.
When no controller is configured they return a controlled Unavailable state
with a visible `CONTROLLER_NOT_CONFIGURED` issue. When a controller is
configured, refresh is routed only through `ShellAccountingReadOnlyController`;
the presenter does not directly depend on adapters, concrete ports, or
DataServiceClient.

QML integration remains a separate authorization step. The presenter skeleton
does not implement accounting calculations, does not access SQLite, does not
call AccountingEngine, does not write database tables, and does not generate
TradeDraft or trade suggestions.

## TASK-116 Concrete Port Integration

TASK-116 hardens the presenter integration path:

`ShellAccountingPresenter -> ShellAccountingReadOnlyController ->
ShellAccountingDataServiceAdapter -> ShellAccountingDataServiceClientPortAdapter
-> DataServiceClient guard wrapper`.

The presenter still does not directly call or include DataServiceClient. It
uses only the controller boundary. The current skeleton exposes
`refreshPositionList`, and `refreshAllReadOnly` delegates to position list until
additional presenter actions are separately authorized.

The position-list guard payload must map to `Unavailable`, keep
`POSITION_LIST_NOT_AVAILABLE` visible, preserve `readOnly=true` and
`writeEnabled=false`, keep the position model empty, and avoid TradeDraft,
trade suggestion, strategy execution, and broker submission paths. QML remains
unwired.

## TASK-117 All Guard Refresh Wiring

TASK-117 extends the production presenter skeleton so every read-only
accounting guard action has a presenter refresh boundary:

- `refreshPositionList`
- `refreshCashSummary`
- `refreshPortfolioPnlSummary`
- `refreshBasePositionSummary`
- `refreshSniperPoolSummary`
- `refreshAllReadOnly`

The presenter still only talks to `ShellAccountingReadOnlyController`; it does
not include or call DataServiceClient, ShellAccountingDataServiceAdapter,
ShellAccountingDataServiceClientPortAdapter, SQLite, DataAccess, or
AccountingEngine. The concrete port guard wrappers remain reachable only
through the existing controller / adapter / port chain.

`refreshAllReadOnly` has a fixed order: `position.list`, `cash.summary`,
`portfolio.pnl.summary`, `base_position.summary`, and `sniper_pool.summary`.
It aggregates the guard issues so all five `*_NOT_AVAILABLE` codes remain
visible. The aggregate state remains `Unavailable` for current guard payloads,
Empty is not treated as Unavailable, and no TradeDraft, trade suggestion,
strategy execution, broker order, QML binding, or database write path is
enabled.

## TASK-118 QML Binding Smoke Readiness

Presenter QML binding readiness now references:

- [docs/59_shell_accounting_qml_binding_smoke_plan.md](59_shell_accounting_qml_binding_smoke_plan.md)
- [docs/60_shell_accounting_qml_static_gate.md](60_shell_accounting_qml_static_gate.md)

The presenter remains read-only and is not registered with QML by this task.
Future QML can only bind ShellServices read-only objects and must not call
DataServiceClient, SQLite, DataAccess, AccountingEngine, adapters, concrete
ports, write actions, TradeDraft, trade suggestions, strategies, or broker
orders.
## TASK-119 QML Static Gate CTest

TASK-119 adds automated static gates for future presenter QML binding. The
presenter remains the only allowed future QML-facing accounting boundary, but
QML is still not wired in this task.

The gate rejects direct QML references to DataServiceClient, DataServiceApi,
SQLite, DataAccess, AccountingEngine, ShellAccountingDataServiceAdapter,
ShellAccountingDataServiceClientPortAdapter, write actions, premature
ShellAccounting bindings, and accounting-scoped trading controls.
