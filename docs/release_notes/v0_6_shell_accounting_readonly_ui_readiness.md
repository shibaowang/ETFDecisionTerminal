# Release Notes - v0.6 ShellAccounting Read-only UI Readiness

## Date

TBD

## Highlights

- `ShellAccountingReadOnlyController`
- `ShellAccountingServiceAdapter`
- `ShellAccountingDataServiceAdapter`
- `ShellAccountingDataServiceClientPortAdapter`
- `ShellAccountingPresenter`
- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`
- Presenter all guard refresh
- QML static gate
- QML binding smoke plan
- no-write / no-trade boundaries

## Not Included

- QML binding
- QML page implementation
- Real `position.list`
- Real `cash.summary`
- Real `portfolio.pnl.summary`
- Real `base_position.summary`
- Real `sniper_pool.summary`
- SQLite facts query integration
- AccountingEngine integration into DataService
- Snapshot writes
- TradeLog writes
- TradeDraft
- Strategy execution
- Broker API
- Auto trading

## Validation

- Full CTest passed.
- Transport repeat passed.
- DevDocs static checks passed.
- QML static gate passed.
- No-write guard tests passed.

## Suggested Tag After Merge

`v0.6.0-shell-accounting-readonly-ui-readiness`

This task does not create a Git tag.
