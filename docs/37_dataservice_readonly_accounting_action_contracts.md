# DataService Read-only Accounting Action Contracts

## Document Purpose

This document defines future DataService read-only accounting action contracts.
It does not implement any action, does not connect SQLite, does not connect
AccountingEngine, and does not change DataServiceApi or DataServiceClient code.

The document exists so future action implementation can be reviewed before any
server action, repository query, replay integration, or UI-facing controller is
added.

## Current Status

- `accounting.health` exists.
- `accounting.replay.preview` remains a guard action.
- `position.list` is not implemented.
- `cash.summary` is not implemented.
- `portfolio.pnl.summary` is not implemented.
- `base_position.summary` is not implemented.
- `sniper_pool.summary` is not implemented.
- AccountingEngine has a production-side DTO-only replay skeleton.
- DataService does not call AccountingEngine.
- SQLite facts query integration is not implemented.
- No-write integration tests for accounting actions are not implemented.

## Common Action Principles

- The first phase for every future accounting action is read-only.
- Do not write `trade_log`.
- Do not write `trade_execution_group`.
- Do not write `trade_draft`.
- Do not write `cash_snapshot`.
- Do not write `position_snapshot`.
- Do not write `portfolio_summary`.
- Do not write `audit_log` unless a separate diagnostic audit task explicitly
  authorizes it.
- Do not generate TradeDraft records.
- Do not generate trading advice.
- Do not execute strategy logic.
- Do not place broker orders.
- Do not use network access.
- Do not access external market data or FX rate services.
- Every action must return `ProtocolResponse`.
- Domain errors must be represented in the payload issue list.
- Protocol errors and accounting domain errors must remain distinct.

## Common Request Fields

Draft `CommonAccountingRequest`:

- `accountId`: optional account filter.
- `portfolioId`: optional portfolio filter.
- `asOfTime`: optional valuation or replay time.
- `sourceFromTime`: optional inclusive facts start time.
- `sourceToTime`: optional inclusive facts end time.
- `includeIssues`: optional, default `true`.
- `includeDebugMetadata`: optional, default `false`.
- `calculationVersion`: optional requested calculation version.
- `requestedOutputs`: optional output names.
- `strictReadOnly`: optional, default `true`.

When `strictReadOnly=true`, any write attempt must be rejected. This task only
defines the contract; it does not implement the request parser or enforcement.

## Common Response Fields

Draft `CommonAccountingResponse`:

- `action`
- `implemented`
- `readOnly`
- `writeEnabled`
- `calculationVersion`
- `dataQualityStatus`
- `metadata`
- `issues`
- `result`
- `warnings`
- `errors`

Draft `metadata` fields:

- `sourceOfTruth`
- `sourceFromTime`
- `sourceToTime`
- `generatedAt`
- `calculationVersion`
- `replayEngineVersion`
- `rowCounts`, optional
- `noWriteCheck`, optional

Draft `AccountingIssueDto` compatible fields:

- `level`
- `code`
- `message`
- `blocking`
- `field`
- `sourceId`

## Action: accounting.health

Status: implemented.

`accounting.health` should continue returning the accounting module boundary
state. It must not write database tables, run replay, or scan complex business
tables. It is a lightweight read-only health action.

## Action: accounting.replay.preview

Status: implemented as a guard.

`accounting.replay.preview` does not execute production replay today. If it is
later connected to AccountingEngine, that integration requires a separate task.
The future action must stay read-only, must not write snapshots, must not write
TradeLog rows, and must not generate TradeDraft records.

If replay is unavailable, the payload should return `REPLAY_NOT_AVAILABLE` or an
equivalent domain status instead of pretending replay succeeded.

## Action: position.list

Status: not implemented.

Purpose: return a read-only position list.

Future input:

- `CommonAccountingRequest`
- `accountId` filtering
- `portfolioId` filtering

Future output:

- `PositionListResponse`
- `positions[]`
- `issues[]`

Draft `PositionSummaryDto` fields:

- `accountId`
- `portfolioId`
- `instrumentCode`
- `instrumentName`, optional
- `quantityText`
- `costAmountText`
- `costPriceText`
- `currency`
- `marketValueText`, optional or `UNAVAILABLE`
- `unrealizedPnlText`, optional or `UNAVAILABLE`
- `dataQualityStatus`

Forbidden writes:

- `trade_log`
- `position_snapshot`
- `portfolio_summary`
- `trade_draft`

Failure behavior:

- `REPLAY_NOT_AVAILABLE`
- `MARKET_PRICE_MISSING`
- `MULTI_CURRENCY_UNSUPPORTED`
- `DATA_VERSION_MISMATCH`
- `INVALID_REQUEST`

## Action: cash.summary

Status: not implemented.

Purpose: return a read-only cash summary by account and portfolio.

Draft `CashSummaryDto` fields:

- `accountId`
- `portfolioId`
- `currency`
- `cashBalanceText`
- `initialCashText`
- `buyCashOutflowText`
- `sellCashInflowText`
- `feeTotalText`
- `dataQualityStatus`

The response must support `accountCashSummaries[]` so different account cash is
not merged.

Forbidden writes:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `portfolio_summary`

Failure behavior:

- `REPLAY_NOT_AVAILABLE`
- `MULTI_CURRENCY_UNSUPPORTED`
- `FX_RATE_MISSING`
- `DATA_VERSION_MISMATCH`
- `INVALID_REQUEST`

## Action: portfolio.pnl.summary

Status: not implemented.

Purpose: return a read-only portfolio PnL summary.

Draft `PortfolioPnlDto` fields:

- `portfolioId`
- `currency`
- `totalAssetsText`
- `totalCostText`
- `realizedPnlText`
- `unrealizedPnlText`
- `totalPnlText`
- `totalReturnRatioText`
- `dataQualityStatus`

When market prices are missing, the action must not fabricate market valuation,
`totalAssetsText`, or `unrealizedPnlText`.

Forbidden writes:

- `trade_log`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `trade_draft`

Failure behavior:

- `REPLAY_NOT_AVAILABLE`
- `MARKET_PRICE_MISSING`
- `MULTI_CURRENCY_UNSUPPORTED`
- `FX_RATE_MISSING`
- `DATA_VERSION_MISMATCH`
- `INVALID_REQUEST`

## Action: base_position.summary

Status: not implemented.

Purpose: return read-only base-position lock derivations.

Draft `BasePositionDto` fields:

- `portfolioId`
- `targetBaseRatioText`
- `targetBaseAmountText`
- `currentBaseRatioText`
- `currentBaseAmountText`
- `lockedBaseAmountText`
- `damagedBaseAmountText`
- `sellableAboveBaseAmountText`
- `basePositionStatus`
- `dataQualityStatus`

`sellableAboveBaseAmountText` is not a sell recommendation. This action must
not generate TradeDraft records.

Forbidden writes:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `position_snapshot`
- `portfolio_summary`

Failure behavior:

- `REPLAY_NOT_AVAILABLE`
- `MARKET_PRICE_MISSING`
- `UNSUPPORTED_ACCOUNTING_MODE`
- `DATA_VERSION_MISMATCH`
- `INVALID_REQUEST`

## Action: sniper_pool.summary

Status: not implemented.

Purpose: return read-only sniper pool and tier derivations.

Draft `SniperPoolDto` fields:

- `portfolioId`
- `poolAmountText`
- `usedAmountText`
- `remainingAmountText`
- `tierCount`
- `tierSummary[]`
- `calculationVersion`
- `dataQualityStatus`

Draft `SniperTierDto` fields:

- `tierName`
- `weight`
- `targetAmountText`
- `executedAmountText`
- `remainingAmountText`
- `completed`
- `dataQualityStatus`

`completed` must not be derived from current market value. `remainingAmountText`
is not a trading recommendation. This action must not generate TradeDraft
records.

Forbidden writes:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`

Failure behavior:

- `REPLAY_NOT_AVAILABLE`
- `MARKET_PRICE_MISSING`
- `UNSUPPORTED_ACCOUNTING_MODE`
- `DATA_VERSION_MISMATCH`
- `INVALID_REQUEST`

## ProtocolResponse Semantics

- `ProtocolResponse.success=true` can mean the action successfully returned a
  payload.
- The accounting domain status inside the payload can still be
  `implemented=false` or `REPLAY_NOT_AVAILABLE`.
- Malformed JSON and malformed protocol requests should use protocol errors.
- Replay warnings and accounting issues should use payload `issues`.
- A blocking issue is not automatically a protocol failure unless the action
  cannot return a valid payload.
- The behavior should remain compatible with the current
  `accounting.replay.preview` guard semantics.

## No-write Contract

Each future accounting action needs no-write proof:

- `trade_log` row count remains unchanged before and after the action.
- `trade_execution_group` row count remains unchanged.
- `trade_draft` row count remains unchanged.
- `cash_snapshot` row count remains unchanged.
- `position_snapshot` row count remains unchanged.
- `portfolio_summary` row count remains unchanged.
- `audit_log` row count remains unchanged unless a separate audit task
  authorizes diagnostic writes.
- Database files should not show WAL writes when the test environment can check
  that reliably.
- Read transactions and database handles must be explicitly read-only.

## Required Tests Before Implementation

- Unit contract tests.
- Request validation tests.
- `ProtocolResponse` shape tests.
- No-write integration tests.
- Invalid request tests.
- Replay unavailable tests.
- Missing market price tests.
- Multi-currency unsupported tests.
- Stale snapshot tests.
- Transport repeat tests.
- Regression tests for the existing read-only demo.

## QML / ShellServices Boundary

- QML must not directly call `DataServiceClient`.
- QML must not access SQLite.
- QML does not calculate accounting.
- ShellServices expose future read-only DTOs through controller and model
  boundaries.
- Action buttons must not write database tables.
- No TradeDraft generation is allowed.

## Explicitly Forbidden Items

- Do not call accounting actions directly from QML.
- Do not implement complex replay algorithms in DataServiceApi.
- Do not implement replay algorithms in DataAccess.
- Do not treat snapshots as the source of truth.
- Do not write TradeLog rows.
- Do not write snapshots.
- Do not generate TradeDraft records.
- Do not generate trading advice.
- Do not execute strategies.
- Do not place broker orders.

## TASK-084 SQLite Facts Query Boundary References

Future DataService accounting actions must source replay facts through the
read-only query boundary defined in
`docs/39_sqlite_readonly_facts_query_boundary.md`.

AccountingEngine DTO and facts source mapping is defined in
`docs/40_accounting_facts_source_mapping.md`.

This document still does not implement any action. DataService still does not
call AccountingEngine, SQLite facts queries are still unimplemented, and writes
remain forbidden.

## TASK-085 position.list Guard

TASK-085 registers `position.list` as a DataService read-only action guard.
Current guard payload fields include:

- `module="accounting"`
- `action="position.list"`
- `implemented=false`
- `readOnly=true`
- `writeEnabled=false`
- `replayExecuted=false`
- `dataSourceAccessed=false`
- `sqliteAccessed=false`
- `accountingEngineCalled=false`
- `status="POSITION_LIST_NOT_AVAILABLE"`
- `futureOutput.type="PositionListResponse"`
- `futureOutput.positions=[]`
- `issues[]` with blocking `POSITION_LIST_NOT_AVAILABLE`
- `forbiddenWrites[]`
- `requiredNextTasks[]`

This does not implement the real `PositionListResponse`. SQLite facts query
integration remains unimplemented. DataService still does not call
AccountingEngine for `position.list`.

## TASK-086 No-write Harness Skeleton

TASK-086 adds a reusable test-only no-write harness skeleton for future
DataService accounting actions and SQLite facts query work. The existing
`position.list` guard keeps its no-write coverage, and future real action
implementations must extend those tests before they can read facts or map real
DTOs.

The harness is not a DataService action, does not call AccountingEngine, and
does not change any guard payload.

## TASK-087 Trade Facts Reader Boundary

TASK-087 adds a DataAccess-side `trade_log` read-only facts reader, but
`position.list` still does not use it. The `position.list` DataService action
remains a guard with `implemented=false`.

Real `position.list` implementation, DataService-to-DataAccess wiring, and
AccountingEngine DTO mapping still require separate authorization and no-write
tests.

## TASK-088 Cash Facts Source Boundary

Future `cash.summary` work must wait until the cash facts source is explicit and
auditable. The boundary is documented in
`docs/41_cash_facts_source_boundary.md`, and the implementation decision is
documented in `docs/42_cash_facts_query_decision.md`.

The real `cash.summary` action is still not implemented. A future
`cash.summary` guard or implementation must reference those documents, must not
infer cash facts from `cash_snapshot` or `portfolio_summary`, and must keep the
first implementation read-only with no-write tests.
