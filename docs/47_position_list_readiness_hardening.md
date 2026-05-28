# position.list Readiness Hardening

## Document Purpose

This document hardens readiness before a real read-only `position.list`
implementation can start.

This document does not implement the real action, does not modify the guard,
does not connect SQLite, does not call AccountingEngine, and is used only to
decide whether the next implementation task can be authorized.

## Current State

- `position.list` guard exists and remains `implemented=false`.
- `DataServiceClient::positionList` exists.
- DataAccess read-only trade facts query skeleton exists.
- PositionList mapping contract tests exist.
- PositionList real action no-write skeleton exists.
- AccountingEngine DTO-only replay skeleton exists.
- cash facts source still has a gap.
- market price facts source is not implemented.
- FX rate facts source is not implemented.
- DataService does not call DataAccess.
- DataService does not call AccountingEngine.
- QML is not connected.
- Real `position.list` is not implemented.

## Readiness Hardening Conclusion

The project may prepare a real implementation task, but it must not jump into
a broad implementation. The first real `position.list` stage must be tightly
scoped, no-write integration tests must be written or expanded first, guard
fallback must remain available, and any implementation still requires separate
explicit authorization.

This TASK-097 document task does not switch `implemented=true`.

## First-stage Real Implementation Minimum Scope

Recommended first-stage support:

- read-only behavior
- request validation
- DataAccess `TradeFactRow` -> AccountingEngine `TradeFactDto` mapping
- AccountingEngine `replayReadOnly` call
- empty ledger
- single BUY
- missing market price warning
- multi-currency unsupported error
- unsupported scenario guard
- no-write table count
- `ProtocolResponse.success=true` with domain issues where appropriate

Not recommended in the first stage:

- full `cash.summary`
- true market valuation
- true FX conversion
- basePosition
- sniperPool
- QML
- snapshot writes
- TradeDraft
- strategy execution

## DataService Orchestration Boundary

Future real `position.list` may:

- validate request
- call DataAccess read-only facts reader
- map `TradeFactRow` to `TradeFactDto`
- build `ReplayRequestDto`
- call AccountingEngine `replayReadOnly`
- map result to `PositionListResponse`
- wrap `ProtocolResponse`

DataService must not own:

- replay algorithm
- PnL algorithm
- SQLite write
- snapshot write
- TradeDraft generation
- trading suggestion
- QML rendering

## Cash Facts Gap Strategy

The cash facts gap remains open because current `CashFactDto` source is not
confirmed. If AccountingEngine needs
`CashFactDto` but DataService cannot provide it, `position.list` should return
`DATA_SOURCE_MISSING`, `CASH_FACTS_SOURCE_UNAVAILABLE`, or
`REPLAY_NOT_AVAILABLE`.

Do not derive cash facts from `cash_snapshot` or `portfolio_summary`. Do not
fabricate initial cash. Do not silently treat missing cash facts as zero. If a
first stage returns only trade-facts-derived holdings, it must explicitly state
the degradation in `dataQualityStatus` and `issues`.

## Market Price Gap Strategy

Market price facts source is not implemented. Missing market price must return
`MARKET_PRICE_MISSING`. `marketValueText` and `unrealizedPnlText` must be
`UNAVAILABLE` or an equivalent unavailable expression.

Do not fetch market data from the network, do not fabricate prices, and do not
replace historical prices with the latest price.

## FX Rate / Multi-currency Strategy

FX rate facts source is not implemented. Multi-currency input must return
`MULTI_CURRENCY_UNSUPPORTED` and/or `FX_RATE_MISSING`.

Do not treat USD as CNY, do not fabricate FX rates, and do not fetch FX rates
from the network. `supportsFxRate=false` and `supportsMultiCurrency=false`
remain valid.

## No-write Hardening Gate

Before real implementation, these must pass:

- `position.list` guard no-write regression
- `position.list` real action no-write skeleton
- valid request no-write
- invalid request no-write
- replay unavailable no-write
- facts query unavailable no-write
- mapping failure no-write
- missing market price no-write
- multi-currency unsupported no-write
- DataAccess trade facts reader no-write
- ForbiddenSqlScanner
- protected table row count unchanged

Protected tables:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `audit_log`

## Failure Behavior Matrix

| Status | ProtocolResponse.success | implemented | readOnly | writeEnabled | Blocking | Partial positions | No-write |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `INVALID_REQUEST` | false for protocol parse errors, true for domain validation payloads | false before real switch, true after real action can construct payload | true | false | yes | no | required |
| `REPLAY_NOT_AVAILABLE` | true when payload can be returned | false before real switch, true only after scoped real implementation | true | false | yes | no | required |
| `DATA_SOURCE_MISSING` | true | true after real action exists | true | false | yes | no | required |
| `CASH_FACTS_SOURCE_UNAVAILABLE` | true | true after real action exists | true | false | maybe | maybe, if clearly degraded | required |
| `POSITION_LIST_MAPPING_FAILED` | true when payload can be returned | true after real action exists | true | false | yes | no | required |
| `MARKET_PRICE_MISSING` | true | true after real action exists | true | false | no unless valuation is mandatory | yes, with unavailable valuation | required |
| `MULTI_CURRENCY_UNSUPPORTED` | true | true after real action exists | true | false | yes | maybe, without fabricated valuation | required |
| `FX_RATE_MISSING` | true | true after real action exists | true | false | yes for FX valuation | maybe, without fabricated FX | required |
| `SQLITE_READONLY_OPEN_FAILED` | true if payload can be returned, otherwise protocol failure | true after real action exists | true | false | yes | no | required |
| `SQLITE_QUERY_FAILED` | true if payload can be returned, otherwise protocol failure | true after real action exists | true | false | yes | no | required |
| `UNSUPPORTED_SCENARIO` | true | true after real action exists | true | false | maybe | maybe, if safe subset is explicit | required |

## Guard Fallback / Rollback Requirement

Early real implementation must keep a fast fallback to the current guard. If
facts query, mapping, or AccountingEngine integration fails in an uncontrolled
way, return a domain issue and do not write the database.

Do not write `audit_log` on exception paths unless separately authorized. Do
not use fallback to return fabricated positions, valuation, cash facts, or FX
rates.

## QML / ShellServices Boundary

QML must not directly call `DataServiceClient`. ShellServices should expose
read-only data through controller / model layers. QML must not calculate
accounting and must not generate trading suggestions from `position.list`
results.

Before real `position.list` is complete, do not connect QML.

## Go / No-Go Final Checklist

- [ ] `position.list` guard has been merged to `main`.
- [ ] DataAccess trade facts reader has been merged to `main`.
- [ ] Mapping contract tests have been merged to `main`.
- [ ] No-write skeleton has been merged to `main`.
- [ ] Cash facts gap degrade strategy has been confirmed.
- [ ] Market price gap degrade strategy has been confirmed.
- [ ] FX rate gap degrade strategy has been confirmed.
- [ ] Failure behavior matrix has been confirmed.
- [ ] Guard fallback strategy has been confirmed.
- [ ] No-write protected table list has been confirmed.
- [ ] QML not connected has been confirmed.
- [ ] User explicitly authorized real implementation.
