# Shell Accounting ViewModel State Contract

## Document Purpose

This document defines accounting ViewModel / Model state expression. It does
not implement code and does not modify QML.

## TASK-110 Mapping Input

`ShellAccountingDataServiceClientPort` responses now feed
`ShellAccountingServiceResult` in tests. The state contract is unchanged:
`implemented=false` maps to `Unavailable`, protocol errors map to visible error
state, timeout / transport flags remain visible, warnings are not swallowed, and
no trade action state is exposed to QML.

## State Enum Draft

- `Idle`: no refresh has started. QML can show neutral placeholders. Data may
  be absent. User refresh is allowed.
- `Loading`: refresh is in progress. QML shows a loading indicator. Existing
  stale data may stay visible if marked. Duplicate refresh may be blocked.
- `Loaded`: the request succeeded and usable data is available. QML may show
  data normally.
- `Empty`: the request succeeded but returned no rows. QML may show an empty
  state. This is not an error.
- `Unavailable`: the feature is not implemented or the data source is not
  available. QML must show unavailable state, not normal empty data.
- `Warning`: usable data is available with non-blocking issues. QML must show
  warnings.
- `Error`: blocking issue or request failure. QML must show error state and
  avoid pretending data is complete.
- `Stale`: previous data is visible but no longer current. QML must mark stale
  state and must not use it as trading guidance.

## DataQualityStatus Mapping

- `OK` -> `Loaded`
- `WARNING` / `DEGRADED` -> `Warning`
- `ERROR` -> `Error`
- `UNAVAILABLE` -> `Unavailable`
- `STALE` -> `Stale`

## Guard Payload Mapping

- `implemented=false -> Unavailable`
- `POSITION_LIST_NOT_AVAILABLE` -> `Unavailable`
- `CASH_SUMMARY_NOT_AVAILABLE` -> `Unavailable`
- `PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE` -> `Unavailable`
- `BASE_POSITION_SUMMARY_NOT_AVAILABLE` -> `Unavailable`
- `SNIPER_POOL_SUMMARY_NOT_AVAILABLE` -> `Unavailable`

## Warning / Issue Mapping

- `MARKET_PRICE_MISSING` -> `Warning`
- `MISSING_FEE` -> `Warning`
- `NEGATIVE_CASH` -> `Error`
- `SELL_EXCEEDS_POSITION` -> `Error`
- `MULTI_CURRENCY_UNSUPPORTED` -> `Error`
- `FX_RATE_MISSING` -> `Error`
- `REPLAY_NOT_AVAILABLE` -> `Unavailable` / `Error`, depending on blocking

## Amount Display / Privacy State

State fields:

- `amountVisible`
- `privacyMode`
- `maskedText`
- `rawText`
- `displayText`

QML binds only `displayText`. QML does not calculate `maskedText`. The
controller / model provides `displayText`.

## Empty vs Unavailable

`Empty` means the action succeeded and returned no data. `Unavailable` means
the feature is not implemented or the source is unavailable. They must not be
confused.

Guard actions currently map to `Unavailable`, not `Empty`.

## Stale Data

Stale data may be displayed, but it must be labeled. `Stale` is not `OK`.
Stale data cannot be used as a trade suggestion source.

## No Trade Suggestion Display

ViewModel does not provide buy/sell suggestion fields.

- `basePosition.sellableAboveBaseAmountText` is not a sell suggestion.
- `sniperPool.remainingAmountText` is not a buy suggestion.
- QML must not show trade buttons from read-only accounting ViewModels.

This is the no trade suggestion boundary for accounting ViewModels.

## Testing Expectations

Future implementation must include:

- status mapping tests
- issue mapping tests
- privacy display tests
- unavailable vs empty tests
- stale display tests
- no trade suggestion tests

## TASK-100 Test Scaffolding

TASK-100 adds test scaffolding that locks the ViewModel state contract before
production controller implementation:

- `implemented=false -> Unavailable` is tested.
- `Empty vs Unavailable` is tested and must not be confused.
- `privacyMode` display mapping is tested.
- `no trade suggestion` behavior is tested.
- Warning, Error, Stale, and Loading states are preserved by test-only
  scenarios.

The scaffolding is not a production controller and does not call real
DataServiceClient, SQLite, or AccountingEngine.

## TASK-101 Production Skeleton

TASK-101 moves the state / issue / privacy / no-trade-action boundary into the
production ShellServices skeleton:

- `ShellAccountingViewState`
- `ShellAccountingIssue`
- `ShellAccountingDisplayText`
- `ShellAccountingReadOnlyController`

QML integration has not started. Shell models are still not implemented. The
skeleton accepts only in-memory state snapshots and does not call real
DataServiceClient, SQLite, or AccountingEngine.

## TASK-102 Fake Payload Boundary Tests

TASK-102 adds fake/client boundary tests for
`ShellAccountingReadOnlyController`.

The fake payload coverage locks these ViewModel state rules:

- guard payloads with `implemented=false` continue to map to `Unavailable`
- `Empty vs Unavailable` remains distinct
- fake warning, error, and stale payloads cannot be swallowed
- issue lists remain visible, including blocking issues
- `privacyMode` selects masked display text without mutating raw text
- privacy mode does not hide warnings or issues
- fake no-trade payloads do not enable TradeDraft, trade suggestion, strategy,
  or broker order behavior

The fake payload tests are not real DataServiceClient integration and do not
authorize QML integration.

## TASK-103 Service Adapter State Mapping

The future service adapter must map service results into this ViewModel state
contract.

- `implemented=false` still maps to `Unavailable`.
- Protocol error and domain issue must stay distinct.
- A protocol error may indicate malformed response, timeout, transport error,
  or `protocolSuccess=false`.
- A domain issue such as `MARKET_PRICE_MISSING`,
  `MULTI_CURRENCY_UNSUPPORTED`, or `FX_RATE_MISSING` must enter the visible
  issue list.
- Warning issues must not be swallowed.
- Blocking issues must remain visible.
- Adapter output must preserve `readOnly` and `writeEnabled`.

See [Shell Accounting Service Adapter Boundary](51_shell_accounting_service_adapter_boundary.md)
and [Shell Accounting Service Integration Readiness](52_shell_accounting_service_integration_readiness.md).

## TASK-104 Service Result Input

`ShellAccountingServiceResult` is the future service adapter result input for
ViewModel state mapping.

It must keep:

- protocol error separate from domain issue
- `implemented=false -> Unavailable`
- `readOnly=true`
- `writeEnabled=false`
- issue code / level / message / blocking visibility
- no TradeDraft / no trade suggestion / no strategy / no broker order flags

The interface skeleton does not change QML behavior and does not perform real
service calls.

## TASK-105 Fake Adapter Refresh Mapping

This fake adapter refresh coverage remains test-only.

TASK-105 covers the ViewModel state contract through controller refresh methods
fed by a test-only fake `ShellAccountingServiceAdapter`.

The tests cover:

- guard payloads with `implemented=false -> Unavailable`
- `*_NOT_AVAILABLE -> Unavailable`
- protocol error / timeout / transport error visibility
- warning and blocking domain issue preservation
- `OK + rows -> Loaded`
- `OK + no rows -> Empty`
- `WARNING` / `DEGRADED -> Warning`
- `ERROR -> Error`
- `STALE -> Stale`
- privacy display selection after refresh
- no TradeDraft, no trade suggestion, no strategy execution, and no broker
  order after refresh

Empty and Unavailable remain distinct. Fake adapter refresh does not imply real
DataServiceClient or QML integration.

## TASK-106 Real Adapter State Input Boundary

The future `ShellAccountingDataServiceAdapter` must map DataService
ProtocolResponse and payload data into `ShellAccountingServiceResult` without
losing ViewModel state semantics:

- protocol error remains separate from domain issue
- `implemented=false` remains `Unavailable`
- `*_NOT_AVAILABLE` remains visible as unavailable state
- warnings and blocking errors remain visible issues
- `readOnly` and `writeEnabled` are preserved
- raw payload can be retained for diagnostics

Real adapter implementation is still not started and does not imply QML
integration.

## TASK-107 Spy Response Mapping Boundary

TASK-107 adds test-only response and error mapping scaffolding for future
DataService ProtocolResponse conversion into `ShellAccountingServiceResult`.

The scaffolding keeps protocol error, timeout, transport error, domain issue,
`implemented=false`, `*_NOT_AVAILABLE`, warning, blocking issue, raw payload,
`readOnly`, and `writeEnabled` semantics visible for future ViewModel state
mapping.

## TASK-108 Adapter Not Connected State

The `ShellAccountingDataServiceAdapter` production skeleton returns
`SHELL_ACCOUNTING_DATASERVICE_ADAPTER_NOT_CONNECTED` with
`dataQualityStatus=UNAVAILABLE`, `implemented=false`, `readOnly=true`, and
`writeEnabled=false`.

Controller refresh with the skeleton maps this to `Unavailable`. The blocking
adapter-not-connected issue remains visible. This does not represent `Empty`,
does not hide the unavailable state, and does not imply real DataServiceClient
or QML integration.

## TASK-109 Live-call State Mapping Gate

Future live-call adapter results must continue to map protocol errors and
domain issues into Shell state / issue models without hiding them.

The live-call gate requires:

- protocol error remains distinct from domain issue
- `implemented=false` remains `Unavailable`
- `*_NOT_AVAILABLE` remains unavailable
- warnings remain visible
- blocking errors remain visible
- timeout and transport errors become visible Error / Unavailable states
- QML integration remains separate

## TASK-110 / TASK-111 Port State Mapping

TASK-110 introduced the abstract client port as a state input source for the
adapter. TASK-111 adds the concrete DataServiceClient port for guard wrappers.

The concrete port must preserve the same state semantics:

- `implemented=false` maps to `Unavailable`, not `Empty`;
- `*_NOT_AVAILABLE` remains visible as unavailable;
- protocol failure, malformed payload, timeout, and transport error remain
  visible error inputs;
- warning and blocking domain issues are preserved;
- `readOnly=true` and `writeEnabled=false` are retained.

This does not change privacy display rules and does not add TradeDraft or trade
suggestion state.

## TASK-112 Concrete Port Controller State

TASK-112 verifies the concrete-port controller path for all five guard
wrappers. Each guard payload remains a state input that maps to `Unavailable`:

- `POSITION_LIST_NOT_AVAILABLE`
- `CASH_SUMMARY_NOT_AVAILABLE`
- `PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE`
- `BASE_POSITION_SUMMARY_NOT_AVAILABLE`
- `SNIPER_POOL_SUMMARY_NOT_AVAILABLE`

`implemented=false` continues to mean `Unavailable`, not `Empty`. Visible issues
are retained, and `readOnly=true` / `writeEnabled=false` stay intact.

## TASK-113 ShellServices ViewModel / Model Boundary

TASK-113 moves the read-only ViewModel state contract into ShellServices
skeleton objects:

- `ShellAccountingStatusObject` maps in-memory snapshots to state flags.
- `ShellAccountingIssueListModel` exposes issue code / level / message /
  blocking / sourceId roles.
- `ShellPositionListModel` exposes test-loadable position display rows without
  real positions.

The state contract remains unchanged:

- `implemented=false -> Unavailable`
- `*_NOT_AVAILABLE -> Unavailable`
- `OK + rows -> Loaded`
- `OK + no rows -> Empty`
- `WARNING` / `DEGRADED -> Warning`
- `ERROR -> Error`
- stale snapshots -> `Stale`

`Empty` and `Unavailable` remain distinct. Privacy mode changes only
`displayText`; it does not mutate `rawText`. The models do not expose
TradeDraft, trade suggestion, strategy execution, broker order, or write action
interfaces. QML integration remains separate.

## TASK-114 QML Binding Readiness State Rules

The QML binding readiness plan in
[docs/58_shell_accounting_qml_binding_readiness_plan.md](58_shell_accounting_qml_binding_readiness_plan.md)
must preserve this state contract:

- Empty is not Unavailable.
- Unavailable must not be displayed as normal empty data.
- Warning, Error, and Stale must remain visible.
- Privacy mode affects `displayText` only.
- Read-only accounting UI must not expose trading buttons, TradeDraft actions,
  trade suggestions, strategy execution, or broker order controls.

## TASK-115 Presenter Skeleton State Boundary

TASK-115 adds `ShellAccountingPresenter` as a production skeleton that exposes
the status object, issue model, and position list model. The presenter keeps
the same state contract:

- `implemented=false -> Unavailable`
- Empty is not Unavailable
- Warning / Error / Stale remain visible
- privacy mode affects display text only
- no TradeDraft or trade suggestion action is exposed

Without a controller, presenter refresh produces a controlled Unavailable
state with a visible `CONTROLLER_NOT_CONFIGURED` issue. With a controller,
refresh remains routed only through `ShellAccountingReadOnlyController`.

## TASK-116 Presenter Guard Wrapper State

TASK-116 verifies that a `position.list` guard payload flowing through the full
presenter path still maps to `Unavailable`. The issue list keeps
`POSITION_LIST_NOT_AVAILABLE` visible, `readOnly=true` and `writeEnabled=false`
remain intact, the position list model does not fabricate rows, and Empty is
still distinct from Unavailable.

## TASK-117 Presenter All Guard State

TASK-117 verifies the same presenter state mapping for all five read-only
guard actions:

- `POSITION_LIST_NOT_AVAILABLE -> Unavailable`
- `CASH_SUMMARY_NOT_AVAILABLE -> Unavailable`
- `PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE -> Unavailable`
- `BASE_POSITION_SUMMARY_NOT_AVAILABLE -> Unavailable`
- `SNIPER_POOL_SUMMARY_NOT_AVAILABLE -> Unavailable`

`refreshAllReadOnly` aggregates the five visible guard issues while current
guard actions remain unimplemented. The aggregate status remains
`Unavailable`, `readOnly=true` and `writeEnabled=false` are preserved, Empty is
not used as a substitute for Unavailable, and no TradeDraft, trade suggestion,
strategy, broker, QML, SQLite, or AccountingEngine path is enabled.

## TASK-118 QML Smoke State Rules

The QML binding smoke plan in
[docs/59_shell_accounting_qml_binding_smoke_plan.md](59_shell_accounting_qml_binding_smoke_plan.md)
must preserve this state contract. The static gate in
[docs/60_shell_accounting_qml_static_gate.md](60_shell_accounting_qml_static_gate.md)
must keep QML from bypassing ShellServices.

Future QML must display Empty as successful no-data, Unavailable as
unimplemented or unavailable data source, Warning / Error / Stale as explicit
states, and privacy display only through ShellServices-provided display text.
Issue visibility and no-trade behavior remain mandatory.
## TASK-119 QML Static Gate CTest

The QML static gate CTest enforces the ViewModel state boundary before QML is
connected. Future accounting QML must preserve Empty vs Unavailable, visible
warning / error / stale states, issue visibility, privacy display ownership in
ShellServices, and no-trade UI behavior.

The gate currently verifies that QML has no ShellAccounting binding yet and no
direct DataServiceClient, SQLite / DataAccess, AccountingEngine, write action,
TradeDraft, broker, or strategy tokens.

## TASK-120 ShellAccounting Read-only UI Readiness

The v0.6 readiness milestone preserves ViewModel state mapping and no-trade
rules. Five guard payloads still map to Unavailable, Empty remains distinct from
Unavailable, issues remain visible, privacy display remains owned by
ShellServices, and no TradeDraft or trade suggestion behavior is exposed.
