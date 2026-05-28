# position.list Real Implementation Boundary

## Document Purpose

This document defines the future boundary for moving `position.list` from a
DataService guard to a real read-only action.

This document does not implement `position.list`, does not modify the guard,
does not connect SQLite, and does not call AccountingEngine. It is an
implementation review document for a later task.

## Current Status

- `position.list` guard is registered.
- `position.list` currently returns `implemented=false`.
- `DataServiceClient::positionList` wrapper exists.
- DataAccess read-only trade facts query skeleton exists.
- AccountingEngine DTO-only replay skeleton exists.
- DataService does not call the DataAccess trade facts reader.
- DataService does not call AccountingEngine.
- Cash facts source is not confirmed.
- Market price facts source is not implemented.
- FX rate facts source is not implemented.
- No-write harness exists.
- Real `position.list` is not implemented.

## Implementation Target Boundary

The future first-stage real `position.list` target must be:

- read-only
- no database writes
- read trade facts through DataAccess
- map DataAccess `TradeFactRow` to AccountingEngine `TradeFactDto`
- call AccountingEngine `replayReadOnly`
- return `PositionListResponse`
- return issues for missing cash facts, market price, or FX rate
- no TradeDraft generation
- no trade suggestion generation
- no direct QML call

The first stage can support a controlled subset only. It must not attempt to
implement every complex accounting scenario at once.

## Draft Data Flow

```text
Client / ShellServices
-> DataServiceClient::positionList
-> DataService action position.list
-> request validation
-> DataAccess read-only facts query
-> DTO mapping
-> AccountingEngine replayReadOnly
-> response mapping
-> ProtocolResponse
```

Boundary rules:

- QML does not directly call DataServiceClient.
- ShellServices only consumes DTO / ViewModel data.
- DataAccess does not calculate replay.
- AccountingEngine does not access SQLite.
- DataServiceApi does not carry complex replay algorithms.

## DataAccess TradeFactRow To AccountingEngine TradeFactDto Mapping

Future mapping draft:

- `factId` -> `factId`
- `tradeTime` -> `tradeTime`
- `accountId` -> `accountId`
- `portfolioId` -> `portfolioId`
- `instrumentCode` -> `instrumentCode`
- `action` -> `action`
- `quantityText` -> `quantityText`
- `priceText` -> `priceText`
- `amountText` -> `amountText`
- `feeText` -> `feeText`
- `cashFlowText` -> `cashFlowText`
- `currency` -> `currency`
- `source` -> `source`
- `note` -> `note`

`feeText` missing must not default to zero. The current
`trade_log.fee_cents NOT NULL DEFAULT 0` schema gap is known: it cannot
distinguish missing fee from an explicit zero fee. That gap can affect
`MISSING_FEE` semantics, so a real implementation must either handle it
explicitly or degrade with an issue.

`SELL_EXCEEDS_POSITION` is a replay-layer issue, not a query-layer issue.

## Cash Facts Gap Impact

Some AccountingEngine scenarios require initial cash or cash facts. The
CashFactDto source remains unconfirmed.

If `position.list` lacks cash facts, it should return
`DATA_SOURCE_MISSING`, `CASH_FACTS_SOURCE_UNAVAILABLE`, or
`REPLAY_NOT_AVAILABLE` instead of fabricating facts. It must not reverse-engineer
cash facts from `cash_snapshot` or `portfolio_summary`.

If an early `position.list` only returns holdings derived from trade facts, it
must make `dataQualityStatus` and issues explicit.

## Market Price Gap Impact

`marketValueText` and `unrealizedPnlText` require MarketPriceFactDto.

The production market price facts source is not implemented. Missing market
price must return `MARKET_PRICE_MISSING` or `UNAVAILABLE` valuation fields.
The action must not fetch market data from the network, must not fabricate
prices, and must not use the latest price to overwrite historical price needs.

## FX Rate Gap Impact

Multi-currency positions require FxRateFactDto.

The production FX rate facts source is not implemented. Missing FX rate must
return `FX_RATE_MISSING` or `MULTI_CURRENCY_UNSUPPORTED`. The action must not
treat USD as CNY, must not fabricate FX rates, and must not fetch FX data from
the network.

## Response Contract

Future real `PositionListResponse`:

- `action`
- `implemented=true`
- `readOnly=true`
- `writeEnabled=false`
- `dataQualityStatus`
- `positions[]`
- `issues[]`
- `metadata`
- `warnings`
- `errors`

Future `PositionSummaryDto`:

- `accountId`
- `portfolioId`
- `instrumentCode`
- `instrumentName`, optional
- `quantityText`
- `costAmountText`
- `costPriceText`
- `currency`
- `marketValueText`, possibly `UNAVAILABLE`
- `unrealizedPnlText`, possibly `UNAVAILABLE`
- `dataQualityStatus`

Metadata:

- `sourceFromTime`
- `sourceToTime`
- `calculationVersion`
- `replayEngineVersion`
- `factsQueryVersion`
- `noWriteCheck`
- `rowCounts`, optional

## ProtocolResponse Semantics

For a valid request where the real action successfully returns a payload,
`ProtocolResponse.success=true` can be used.

Domain warnings and errors belong in `payload.issues`. Invalid JSON and
malformed requests use protocol errors. `REPLAY_NOT_AVAILABLE` can be a domain
issue and does not necessarily require protocol failure. A blocking issue does
not equal protocol failure unless the action cannot construct a valid payload.

## No-write Requirements

Before real implementation is accepted, tests must prove:

- `trade_log` row count is unchanged.
- `trade_execution_group` row count is unchanged.
- `trade_draft` row count is unchanged.
- `cash_snapshot` row count is unchanged.
- `position_snapshot` row count is unchanged.
- `portfolio_summary` row count is unchanged.
- `audit_log` row count is unchanged unless separately authorized.
- DataService action failure paths are no-write.
- Invalid request paths are no-write.
- Replay unavailable paths are no-write.

## Test Plan For Future Implementation

- Guard regression tests.
- Request validation tests.
- DataAccess trade facts query tests.
- `TradeFactRow` -> `TradeFactDto` mapping tests.
- AccountingEngine integration tests.
- No-write table count tests.
- Invalid request no-write tests.
- Missing cash facts tests.
- Missing market price tests.
- Multi-currency unsupported tests.
- No QML direct call tests.
- Transport repeat tests.

## Guard -> Real Action Switch Conditions

All conditions must be met before switching from guard to real action:

- `docs/45` review passes.
- `docs/38` no-write plan covers the real action.
- DataAccess trade facts query is merged.
- Cash facts source decision is explicit.
- AccountingEngine integration mapping is explicit.
- No-write tests are written first or in the same implementation task.
- Failure behavior is fixed.
- The PR explicitly scopes the `implemented=false` to `implemented=true`
  transition.
- QML remains disconnected.
- The user explicitly authorizes the real implementation.

## Explicitly Forbidden

- Do not implement complex position algorithms directly in DataServiceApi.
- Do not derive facts from snapshots.
- Do not write `trade_log`.
- Do not write snapshots.
- Do not generate TradeDraft.
- Do not generate trade suggestions.
- Do not execute strategies.
- Do not place broker orders.
- Do not fetch market data or FX rates from the network.
- Do not call this action directly from QML.

## TASK-095 Mapping Contract Tests

TASK-095 adds test-only mapping contract tests for future real
`position.list` implementation:

- request payload -> AccountingEngine `ReplayRequestDto`
- DataAccess `TradeFactRow` -> AccountingEngine `TradeFactDto`
- AccountingEngine `AccountingReplayResult` -> future `PositionListResponse`

These tests do not implement the real action, do not modify the
`position.list` guard, do not call DataService action replay paths, do not
access SQLite, and do not write database tables.

## TASK-096 real action no-write skeleton

TASK-096 adds test-only no-write skeleton tests for future real `position.list`
action paths. The real action is still not implemented, the current
`position.list` guard remains `implemented=false`, and DataService still does
not call DataAccess or AccountingEngine.

The skeleton covers valid request, invalid request, replay unavailable, facts
query unavailable, mapping failure, missing market price, multi-currency
unsupported, missing FX rate, SQLite read-only open failure, and SQLite query
failure no-write paths. It uses fake callbacks and the protected table
row-count harness only; it does not access real SQLite facts queries and does
not write tables.

Before the guard -> real action switch, these skeleton tests must be replaced
or extended with real action no-write coverage while preserving the same
protected table guarantees.

## TASK-097 Readiness Hardening

TASK-097 adds the final readiness hardening document in
`docs/47_position_list_readiness_hardening.md` and the first-stage real action
scope in `docs/48_position_list_first_stage_real_action_scope.md`.

Those documents are prerequisites before a real implementation task can switch
the guard. The `position.list` guard remains `implemented=false`; this task
does not connect DataAccess to DataService, does not call AccountingEngine,
does not access SQLite, and does not write database tables.

## TASK-098 First-stage Implementation Scaffolding

TASK-098 adds test-only first-stage implementation scaffolding. It covers the
future first-stage scenarios, the guard transition contract, and the failure
matrix without implementing the real action.

The real `position.list` action is still not implemented. The scaffolding only
describes the future DataService request -> `ReplayRequestDto` ->
AccountingEngine -> `PositionListResponse` path and locks no-write, guard
fallback, excluded scope, and failure status expectations.

## TASK-099 ShellServices Controller Boundary

Future `position.list` output must flow through a ShellServices controller
before QML can bind it. The controller boundary is documented in
`docs/49_shellservices_accounting_controller_contract.md`.

There must be no direct QML -> DataServiceClient path. QML must not calculate
positions, valuation, or accounting issues from raw DataService payloads.

## TASK-100 ShellServices Test Scaffolding Link

TASK-100 adds Shell accounting controller scaffolding that reinforces this
boundary. First-stage `position.list` output must still flow through
ShellServices controller before QML, and there must be no direct QML ->
DataServiceClient path.

The scaffolding does not implement production controller code and does not make
`position.list` QML-facing.

## TASK-103 Shell Accounting Service Adapter Boundary

Future `position.list` output must flow through ShellServices service adapter
and controller boundaries before it reaches QML:

```text
position.list DataService action
-> DataServiceClient wrapper
-> ShellAccountingServiceAdapter
-> ShellAccountingReadOnlyController
-> ShellServices ViewModel / Model
-> QML
```

QML must not call DataServiceClient directly. QML must not bypass
`ShellAccountingServiceAdapter` or the controller to interpret
`PositionListResponse` payloads. The adapter boundary is documented in
`docs/51_shell_accounting_service_adapter_boundary.md`.
