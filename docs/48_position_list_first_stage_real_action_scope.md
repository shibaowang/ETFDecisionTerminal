# position.list First-stage Real Action Scope

## Document Purpose

This document fixes the minimum scope for a first real `position.list`
implementation.

It does not implement code and does not modify the current guard.

## Allowed First-stage Scope

The first implementation PR may include only:

- request validation
- DataAccess trade facts read-only query
- `TradeFactRow` -> `TradeFactDto` mapping
- `ReplayRequestDto` mapping
- AccountingEngine `replayReadOnly`
- `PositionListResponse` mapping
- empty ledger
- single BUY
- missing market price warning
- multi-currency unsupported error
- no-write integration tests
- guard fallback

## Explicitly Excluded

The first implementation PR must not include:

- `cash.summary` implementation
- `portfolio.pnl.summary` implementation
- `base_position.summary` implementation
- `sniper_pool.summary` implementation
- snapshot write
- TradeLog write
- TradeDraft
- trading suggestion
- strategy execution
- QML integration
- market data service
- FX service
- multi-currency valuation
- basePosition calculation
- sniperPool calculation

## Data Quality Policy

- `OK`: request, facts, replay, and response mapping succeed for the supported
  subset without domain issues.
- `WARNING`: the action returns useful positions but non-blocking issues are
  present, such as `MARKET_PRICE_MISSING`.
- `DEGRADED`: the action returns a safe subset, such as trade-facts-derived
  holdings without cash facts or valuation, and clearly reports the gap.
- `ERROR`: blocking domain issues prevent a trustworthy position list, such as
  unsupported multi-currency without FX facts.
- `UNAVAILABLE`: a field or output cannot be produced and must not be
  fabricated.

## PositionListResponse Minimum Payload

Minimum payload fields:

- `action`
- `implemented`
- `readOnly`
- `writeEnabled`
- `dataQualityStatus`
- `positions`
- `issues`
- `metadata`
- `warnings`
- `errors`

Minimum `positions[]` fields:

- `accountId`
- `portfolioId`
- `instrumentCode`
- `quantityText`
- `costAmountText`
- `costPriceText`
- `currency`
- `marketValueText`
- `unrealizedPnlText`
- `dataQualityStatus`

`marketValueText` and `unrealizedPnlText` may be `UNAVAILABLE` when market
price facts are missing.

## First Implementation PR Requirements

The first implementation PR must state:

- that it changes `implemented=false` to `implemented=true`
- supported scenarios
- unsupported scenarios
- no-write results
- fallback behavior
- QML is not connected
- full CTest result
- `transport_local_socket_echo` 50-run result

## TASK-098 Scope Contract Tests

TASK-098 adds first-stage scope contract tests and locks the explicitly
excluded scope in CTest. These tests are scaffolding only and do not implement
the real action.

The scope contract tests confirm that QML integration, snapshot writes,
TradeLog writes, TradeDraft, trading suggestions, strategy execution, market
data service, FX service, multi-currency valuation, basePosition calculation,
and sniperPool calculation remain excluded from the first implementation PR.

## TASK-099 ShellServices Boundary

The first-stage real action is still not QML-facing unless a separate
ShellServices controller task is authorized. Future QML integration must follow
`docs/49_shellservices_accounting_controller_contract.md` and
`docs/50_shell_accounting_viewmodel_state_contract.md`.

TASK-100 adds test-only Shell accounting controller scaffolding. The first-stage
real action remains non-QML-facing until a production ShellServices controller
implementation task is separately authorized and passes the TASK-100 state,
issue, privacy, and no trade action contract tests.

## TASK-103 Shell Accounting Service Adapter Boundary

First-stage real `position.list` remains non-QML-facing unless a separate
ShellServices service adapter and controller integration task is authorized.

The intended consumer path is:

```text
DataServiceClient positionList wrapper
-> ShellAccountingServiceAdapter
-> ShellAccountingReadOnlyController
-> ShellServices ViewModel / Model
```

The first real `position.list` PR must not let QML call DataServiceClient
directly and must not skip the adapter boundary documented in
`docs/51_shell_accounting_service_adapter_boundary.md`.
