# v0.3 Position Accounting Boundary

## Document Purpose

This document defines the boundary for the future v0.3 read-only position and
accounting-derived stage. It defines fact sources, derived data, read-only
DataService action drafts, Shell ViewModel drafts, error states, and explicit
prohibited scope.

This document is a design boundary only. It does not mean any position page,
accounting replay code, database write, schema change, service action, or QML
behavior has been implemented.

## Core Principles

- trade_log 是唯一事实账本.
- cash_snapshot 是派生数据.
- position_snapshot 是派生数据.
- portfolio_summary 是派生数据.
- cash_snapshot / position_snapshot / portfolio_summary 是派生结果.
- Derived results must never rewrite, replace, or backfill the fact ledger.
- All accounting derivation must be replayable, explainable, and testable.
- UI displays ViewModel data only and must not perform accounting calculation.
- QML 不访问 SQLite.
- QML 不直接调用 DataServiceClient.
- QML 不计算成本, 持仓, 本金, 现金, or收益.
- Read-only pages must not create any write action.

## Data Layers

### A. Fact Layer

The fact layer contains records that describe what happened. It is the source
for future replay and correction.

- trade_log
- trade_execution_group
- Manual accounting event, future authorization required
- Execution confirmation event, future authorization required

### B. Derived Layer

The derived layer contains calculated views or cached summaries. These are not
fact sources.

- cash_snapshot
- position_snapshot
- portfolio_summary
- cost basis
- realized PnL
- unrealized PnL
- principal
- cash balance
- base position summary

### C. Service Layer

The service layer will expose controlled read-only access and future replay
boundaries.

- DataService read-only actions
- Accounting replay service, future design
- Snapshot query service, future design

### D. UI Layer

The UI layer must remain display-only for this domain.

- ShellServices ViewModel
- Shell QML read-only page
- No business calculation in QML
- No direct SQLite access in QML
- No direct DataServiceClient access in QML

## TradeLog Fact Ledger Boundary

- trade_log writes require a separate authorized task.
- The current stage must not write trade_log.
- Future trade_log writes must pass through controlled transactions, audit, and
  validation.
- Buy, sell, cash flow, fee, principal, and cash balance fields must use a
  single documented accounting convention before implementation.
- A sell event must not delete or mutate the original buy fact.
- Corrections must be represented by reversal or adjustment records; historical
  facts must not be silently edited.
- audit_log is not the accounting fact ledger and cannot replace trade_log.

## Cash Derivation Boundary

- Cash balance should be replayed from initial cash, buys, sells, fees,
  deposits, withdrawals, and other authorized cash-flow facts.
- cash_snapshot can only be a derived cache or display accelerator.
- cash_snapshot cannot be treated as the fact source.
- Negative cash, missing fees, or unbalanced cash flow must produce visible
  error states.
- The current task does not implement cash replay code.

## Position Derivation Boundary

- Position quantity comes from replaying trade_log buy and sell facts.
- Position cost depends on a single cost-basis convention. It must not be
  implemented until that convention is defined.
- position_snapshot is only a derived result.
- Negative position must be treated as an accounting error.
- Account, portfolio, and instrument dimensions must be explicit for every
  derived row.
- The current task does not implement position calculation code.

## Principal And PnL Boundary

- Principal, cash, total assets, and return ratio must have explicit
  definitions.
- Principal must not be inferred by simply reversing from current market value.
- realized PnL and unrealized PnL must be separated.
- Return ratio must define its denominator: principal, net contribution,
  opening assets, or another documented convention.
- The current task does not implement PnL or principal calculation code.

## Base Position And Sniper Pool Boundary

- 20% 底仓 is the locked base position.
- Sell logic must not touch the locked base position unless a later task
  explicitly authorizes that behavior.
- 80% 狙击资金池 should be determined after the base position is completed,
  using real cash and a documented funding plan.
- The sniper pool must not automatically expand or shrink from floating PnL.
- T1..T6 future tiers use 1/2/4/8/16/32 weighting and must remain separated
  from accounting facts.
- Tier completion state must come from aggregating buy facts, not from current
  market value.
- UI can only display derived base-position and sniper-pool results.
- QML must not calculate base position, sniper pool, or tier completion.
- The current task does not implement base position or sniper-pool calculation
  code.

## Snapshot Boundary

- position_snapshot / cash_snapshot / portfolio_summary are derived snapshots.
- Snapshot generation must be designed by a dedicated future task.
- A snapshot must include source time, calculation version, and input range.
- Stale snapshots must be identifiable.
- Snapshots must not be treated as trade fact sources.
- The current task writes no snapshot tables.
- 不写快照表.

## Future Read-Only DataService Action Draft

These action names are drafts only. They are not implemented in this task and
future implementation requires separate authorization.

| Action | Read-only | Input | Output | Source tables | Forbidden writes | Error cases |
| --- | --- | --- | --- | --- | --- | --- |
| accounting.health | yes | none or portfolio scope | accounting derivation readiness | schema metadata, replay config draft | all writes | missing convention, unsupported schema |
| accounting.replay.preview | yes | accountId, portfolioId, source range | replay validation summary | trade_log, trade_execution_group | all writes | negative cash, negative position, invalid order |
| position.summary | yes | accountId, portfolioId | aggregate position summary | trade_log or future snapshots | all writes | stale data, undefined cost basis |
| position.list | yes | accountId, portfolioId, instrument filter | position rows | trade_log or position_snapshot | all writes | negative position, missing instrument |
| cash.summary | yes | accountId, portfolioId | cash balance summary | trade_log or cash_snapshot | all writes | negative cash, unsupported currency |
| portfolio.valuation.summary | yes | portfolioId | valuation summary | position data, future market data | all writes | missing market data, stale valuation |
| portfolio.pnl.summary | yes | portfolioId, period | PnL summary | trade_log, valuation inputs | all writes | undefined denominator, missing cost basis |
| base_position.summary | yes | portfolioId | base position derived summary | trade_log, future policy config | all writes | base policy missing, incomplete facts |
| sniper_pool.summary | yes | portfolioId | sniper-pool derived summary | cash facts, trade_log, future policy config | all writes | insufficient cash, incomplete tier facts |

## Future Shell ViewModel Draft

These ViewModels are drafts only. They are not implemented in this task.

- ShellPositionSummaryViewModel
- ShellPositionListModel
- ShellCashSummaryViewModel
- ShellPortfolioPnlViewModel
- ShellBasePositionViewModel
- ShellSniperPoolViewModel

Rules for future implementation:

- QML only binds these ViewModels and models.
- QML does not perform accounting calculation.
- QML does not directly call DataServiceClient.
- QML does not directly access SQLite.
- ViewModels must expose error states instead of silently returning partial or
  fake values.

## Exceptions And Error States

Future position and accounting pages must show these states visibly and must
not silently swallow them:

- Negative position
- Negative cash
- Missing fee
- Abnormal trade order
- Sell quantity exceeds holdings
- Stale snapshot
- Undefined FX rate for multi-currency data
- Undefined cost-basis convention
- Missing market data
- Data version mismatch

## Explicitly Prohibited Scope

- 不写 trade_log.
- 不写快照表.
- Do not modify schema.
- Do not implement write actions.
- Do not implement automatic trading.
- Do not implement broker API integration.
- Do not calculate position, cost, or PnL in QML.
- Do not execute strategy in QML.
- Do not generate TradeDraft.
- Do not place real orders.
- Do not write position_snapshot / cash_snapshot / portfolio_summary.
- Do not call data.audit.append.

## Suggested v0.3 Task Split

These are planning suggestions only and do not implement code:

- TASK-045: Position accounting data contract design
- TASK-046: Accounting replay test fixture design
- TASK-047: Read-only position DTO / ViewModel design
- TASK-048: DataService accounting health read-only action
- TASK-049: Position page mock UI
- TASK-050: Position replay implementation, separate authorization required

## Follow-up Contract References

- Detailed stable data contract: [Position Accounting Data Contract](22_position_accounting_data_contract.md).
- Future replay fixture design: [Position Accounting Test Fixture Design](23_position_accounting_test_fixture_design.md).
- The stable contract refines the earlier action and ViewModel drafts in this boundary document.
