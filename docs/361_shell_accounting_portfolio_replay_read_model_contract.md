# ShellAccounting Portfolio Replay Read Model Contract

EPIC-276 introduces a read-only replay summary contract. The contract is
designed for DataService and ShellServices consumption without adding persistent
read model writes.

## DataService Action

Action name:

```text
accounting.portfolio_replay.readonly_summary
```

The action accepts sanitized in-memory replay facts and returns a summary JSON.
It calls the AccountingEngine portfolio replay module in read-only mode and
does not use production write paths.

## Response Fields

The response includes:

- `task=EPIC-276`
- `mode=portfolio-replay-vba-parity-readonly-summary`
- `status`
- `accepted`
- `replayExecuted`
- `positionCount`
- `cashSummaryCount`
- `positions`
- `cashSummaries`
- `pnlSummary`
- `issues`
- `readOnlyReplayNoWrite=true`

The response also includes no-write evidence flags for trade_log,
cash_adjustment, audit_log, SQLite, read models, broker, network, credentials,
endpoints, and automatic trading.

## Read-only Boundary

This contract must not write SQLite, audit_log, ledger, snapshot, trade_log,
cash_adjustment, or persistent read model rows. It must not call broker,
network, credentials, endpoints, strategy, TradeDraft, real order placement, or
automatic trading paths.
