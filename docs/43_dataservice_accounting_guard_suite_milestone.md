# DataService Accounting Guard Suite Milestone

## Milestone Name

DataService Accounting Guard Suite Milestone

## Milestone Positioning

This milestone completes the DataService accounting read-only guard suite. It
is an action contract guard layer, not a real accounting action
implementation.

The guard suite proves that action names, stable payload shape, read-only
metadata, no-write boundaries, and client wrappers are available for the next
review step. It does not read SQLite, does not call AccountingEngine, does not
connect QML, and does not produce real accounting outputs.

## Covered Guard Actions

### A. accounting.health

- Status: implemented read-only health action.
- Input boundary: health-style request only.
- Output: accounting module boundary status.
- Not done: no replay, no SQLite facts query, no database writes.

### B. accounting.replay.preview

- Status: guard action.
- Output: `replayExecuted=false`.
- Not done: no production replay, no AccountingEngine call, no snapshot write.

### C. position.list

- Status: `implemented=false`.
- Guard status: `POSITION_LIST_NOT_AVAILABLE`.
- Future output: `futureOutput.type=PositionListResponse`.
- Not done: no real positions, no SQLite facts query, no AccountingEngine call.

### D. cash.summary

- Status: `implemented=false`.
- Guard status: `CASH_SUMMARY_NOT_AVAILABLE`.
- Future output: `futureOutput.type=CashSummaryResponse`.
- Not done: no real `cashBalance`, no cash facts query, no snapshot read.

### E. portfolio.pnl.summary

- Status: `implemented=false`.
- Guard status: `PORTFOLIO_PNL_SUMMARY_NOT_AVAILABLE`.
- Future output: `futureOutput.type=PortfolioPnlSummaryResponse`.
- Not done: no real `totalAssets`, realized PnL, unrealized PnL, total PnL,
  or total return ratio.

### F. base_position.summary

- Status: `implemented=false`.
- Guard status: `BASE_POSITION_SUMMARY_NOT_AVAILABLE`.
- Future output: `futureOutput.type=BasePositionSummaryResponse`.
- Not done: no real base position calculation, no trade suggestion, no
  TradeDraft generation.

### G. sniper_pool.summary

- Status: `implemented=false`.
- Guard status: `SNIPER_POOL_SUMMARY_NOT_AVAILABLE`.
- Future output: `futureOutput.type=SniperPoolSummaryResponse`.
- Not done: no real tierSummary, no T1-T6 calculation, no trade suggestion,
  no TradeDraft generation.

## Unified Guard Payload Contract

Each guard should include or equivalently express:

- `module=accounting`
- `action`
- `implemented=false`
- `readOnly=true`
- `writeEnabled=false`
- `replayExecuted=false`
- `dataSourceAccessed=false`
- `sqliteAccessed=false`
- `accountingEngineCalled=false`
- `status`
- `futureOutput`
- `forbiddenWrites`
- `issues`
- `warnings`

The guard payload is a domain-level unavailable response. For valid JSON
objects, ProtocolResponse success can remain true because the guard itself
returned its payload successfully.

## No-write Coverage

The guard suite has no-write table count coverage for:

- `position.list` no-write
- `cash.summary` no-write
- `portfolio.pnl.summary` no-write
- `base_position.summary` no-write
- `sniper_pool.summary` no-write

Protected tables:

- `trade_log`
- `trade_execution_group`
- `trade_draft`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `audit_log`

The current no-write coverage proves guard calls do not mutate these protected
tables. It does not prove that future real implementations are safe; those
must extend the no-write tests before implementation is accepted.

## DataServiceClient Wrappers

Current guard wrappers:

- `positionList`
- `cashSummary`
- `portfolioPnlSummary`
- `basePositionSummary`
- `sniperPoolSummary`

Each wrapper only calls the matching guard action. The wrappers do not
calculate accounting, do not connect QML, and do not access SQLite.

## Explicitly Not Implemented

- Real `position.list` is not implemented.
- Real `cash.summary` is not implemented.
- Real `portfolio.pnl.summary` is not implemented.
- Real `base_position.summary` is not implemented.
- Real `sniper_pool.summary` is not implemented.
- DataService does not call AccountingEngine.
- SQLite facts query is not connected to DataService.
- AccountingEngine result mapping is not connected to DataService.
- ShellServices and QML are not connected to these actions.
- Snapshot writes are not implemented.
- TradeLog writes are not authorized.

The guard suite milestone does not make these actions production-ready.

## Current Quality Gate

The milestone quality gate is:

- CMake configure passes.
- Build passes.
- Full CTest passes.
- `transport_local_socket_echo` passes 50 repeated runs.
- DevDocs static checks pass.
- No-write table count tests pass.
- `git status` is clean before merge.

## Suggested Tag

After this PR is merged to `main`, the user may create the milestone tag:

```powershell
git tag -a v0.5.0-dataservice-accounting-guard-suite -m "v0.5 DataService Accounting Guard Suite"
git push origin v0.5.0-dataservice-accounting-guard-suite
```

The tag must be created manually by the user after merge. This task does not
create a Git tag.

## Next Phase Suggestions

- DataService accounting guard suite readiness review.
- SQLite facts query to DataService integration boundary.
- AccountingEngine to DataService DTO mapping boundary.
- True `position.list` integration plan.
- ShellServices read-only accounting controller contract.
- No-write integration hardening.

The next phase must choose one boundary at a time. It must not bypass guard,
no-write, or facts query boundaries.
