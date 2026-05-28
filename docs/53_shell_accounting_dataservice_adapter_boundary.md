# Shell Accounting DataService Adapter Boundary

## Document Purpose

This document defines how a future `ShellAccountingDataServiceAdapter` may wrap
DataServiceClient guard wrappers.

This document does not implement the adapter. It does not call
DataServiceClient, does not modify `ShellAccountingReadOnlyController`, does
not wire QML, and is only a review boundary for a later real adapter task.

## Current State

- `ShellAccountingServiceAdapter` interface already exists.
- `ShellAccountingReadOnlyController` supports fake adapter refresh.
- DataServiceClient guard wrappers already exist.
- DataService accounting guard suite already exists.
- `ShellAccountingDataServiceAdapter` is not implemented.
- QML is not wired to the accounting controller.
- Real `position.list` is not implemented.
- DataService does not call AccountingEngine.
- SQLite, AccountingEngine, and DataAccess are not connected to ShellServices.

## Design Goals

- The real adapter is an implementation of `ShellAccountingServiceAdapter`.
- The real adapter may only wrap DataServiceClient wrapper methods.
- The real adapter must not calculate accounting.
- The real adapter must not access SQLite.
- The real adapter must not call AccountingEngine.
- The real adapter must not generate trade suggestions.
- The real adapter must not generate TradeDraft.
- The real adapter must not execute strategy.
- The real adapter must not be exposed directly to QML.
- The real adapter must remain read-only.

## Layering

Allowed future flow:

```text
ShellAccountingReadOnlyController
-> ShellAccountingServiceAdapter interface
-> ShellAccountingDataServiceAdapter
-> DataServiceClient wrappers
-> DataService accounting actions
```

Forbidden flows:

- QML -> ShellAccountingDataServiceAdapter
- QML -> DataServiceClient
- Controller -> DataServiceClient direct hard dependency
- Adapter -> SQLite
- Adapter -> AccountingEngine
- Adapter -> DataAccess
- Adapter -> write action
- Adapter -> TradeDraft
- Adapter -> strategy execution

## Action Mapping

Future method mapping:

- `fetchPositionList` -> `DataServiceClient::positionList`
- `fetchCashSummary` -> `DataServiceClient::cashSummary`
- `fetchPortfolioPnlSummary` -> `DataServiceClient::portfolioPnlSummary`
- `fetchBasePositionSummary` -> `DataServiceClient::basePositionSummary`
- `fetchSniperPoolSummary` -> `DataServiceClient::sniperPoolSummary`

Current mapping is only to guard wrappers. After real DataService actions are
implemented, the adapter should still call the same DataServiceClient wrapper
and must not bypass DataServiceClient.

## Request Mapping

Future `ShellAccountingServiceRequest` -> DataServiceClient payload mapping:

- `actionName`
- `accountId`
- `portfolioId`
- `sourceFromTime`
- `sourceToTime`
- `includeIssues`
- `includeDebugMetadata`
- `calculationVersion`
- `requestedOutputs`
- `timeoutMs`

Rules:

- Request JSON must be an object.
- No write intent may be added.
- No TradeDraft field may be added.
- No trade suggestion field may be added.
- `timeoutMs` affects call timeout only; it must not trigger writes.
- Invalid request mapping must become a protocol or domain error and must not
  write database tables.

## Response Mapping

Future DataService ProtocolResponse / payload -> `ShellAccountingServiceResult`
mapping:

- `ProtocolResponse.success` -> `protocolSuccess`
- `action` -> `actionName`
- `implemented` -> `implemented`
- `readOnly` -> `readOnly`
- `writeEnabled` -> `writeEnabled`
- `status` -> `payloadStatus`
- `dataQualityStatus` -> `dataQualityStatus`
- `issues[]` -> `issues`
- `warnings[]` -> `warnings`
- `errors[]` -> `errors`
- raw payload -> `rawPayload`
- timeout -> `timeout`
- transport error -> `transportError`
- protocol error -> `protocolError`
- domain issue -> `domainError`

Rules:

- protocol error and domain issue must remain distinct.
- `implemented=false` is not a protocol failure.
- `*_NOT_AVAILABLE` must be preserved as `payloadStatus`.
- `readOnly` and `writeEnabled` must be preserved.
- issue code / level / message / blocking must be preserved.
- Unknown fields may be preserved in `rawPayload`.

## Error / Timeout Mapping

Future adapter tests must cover:

- malformed JSON response
- `ProtocolResponse.success=false`
- timeout
- transport failure
- server unavailable
- `implemented=false`
- `POSITION_LIST_NOT_AVAILABLE`
- `CASH_SUMMARY_NOT_AVAILABLE`
- `PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE`
- `BASE_POSITION_SUMMARY_NOT_AVAILABLE`
- `SNIPER_POOL_SUMMARY_NOT_AVAILABLE`
- `MARKET_PRICE_MISSING`
- `MULTI_CURRENCY_UNSUPPORTED`
- `FX_RATE_MISSING`
- `REPLAY_NOT_AVAILABLE`

Timeout, transport failure, malformed response, and domain issue paths must not
trigger writes. Domain issue must not be swallowed. Blocking issue must remain
visible.

## No-write Guarantee

- The adapter itself must not access the database, so it cannot directly write
  database tables.
- Each DataService action called by the adapter must have its own no-write test.
- The adapter must not call write actions.
- The adapter must not call `data.audit.append`.
- The adapter must not generate TradeDraft.
- The adapter must not generate trade suggestions.
- The adapter must not execute strategy.
- Adapter tests must confirm the wrapper allowlist only includes read-only
  accounting actions.

## Testing Requirements For Future Implementation

Future implementation must include:

- construction test
- no direct QML access test
- wrapper method mapping test
- request payload mapping test
- protocol success mapping test
- protocol failure mapping test
- domain issue mapping test
- timeout mapping test
- transport error mapping test
- `implemented=false` mapping test
- `readOnly` / `writeEnabled` preservation test
- no TradeDraft test
- no trade suggestion test
- no write action invocation test
- controller consumes real adapter guard payload test
- transport repeat regression

## Explicitly Forbidden

- The adapter must not access SQLite.
- The adapter must not call AccountingEngine.
- The adapter must not call DataAccess.
- The adapter must not write database tables.
- The adapter must not generate TradeDraft.
- The adapter must not generate trade suggestions.
- The adapter must not execute strategy.
- QML must not use the adapter directly.
- The adapter must not bypass DataServiceClient wrapper methods.

## TASK-107 Spy / Wrapper Scaffolding

TASK-107 adds test-only spy/wrapper scaffolding under
`tests/ShellAccountingDataServiceAdapterScaffolding`.

The scaffolding locks:

- read-only action allowlist
- method mapping descriptors
- request payload mapping contract
- response mapping contract
- protocol error / timeout / transport error / domain issue contract
- no-write / no-trade / no-TradeDraft contract
- no real dependency checks

The read-only action allowlist is covered by tests and contains only:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

Forbidden write, trade, draft, snapshot, strategy, and broker actions are
covered by denylist tests. The real adapter is still not implemented.
