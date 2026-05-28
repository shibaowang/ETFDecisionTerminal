# Shell Accounting ViewModel State Contract

## Document Purpose

This document defines accounting ViewModel / Model state expression. It does
not implement code and does not modify QML.

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
