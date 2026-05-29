# ShellAccounting Real Data Adapter Gate

## Document Purpose

TASK-134 defines the gate before future ShellAccounting real data adapter and
DataService read-only action integration. This document does not modify
production startup, does not modify production QML, does not call
DataServiceClient, does not implement DataService action, does not access
SQLite, does not call AccountingEngine, and does not write database tables.

## Current State

- ShellAccountingPresenter is in the production lifecycle.
- `accountingPresenter` is exposed to QML as the only authorized presenter
  context property.
- The UI shell remains read-only and unavailable-safe.
- No real data adapter is configured.
- No real DataService action is implemented.
- SQLite and AccountingEngine are not connected to the presenter path.
- TradeDraft, trade suggestion, strategy execution, broker order, and trading
  UI remain absent.

## Real Data Adapter Gate Summary

Only explicit user authorization can start real data adapter implementation.
The implementation must be a separate task. DataService action integration must
remain read-only, and adapter calls must go through the protocol boundary.
Presenter code must not read SQLite directly. QML must not call
DataServiceClient directly. The adapter must not write database tables, must
not generate TradeDraft, and must not execute strategy. Adapter failure must
map to Unavailable or issue state and must not crash the app.

real adapter is not write path. real adapter 不等于 write path. read-only DataService action is not TradeDraft or trading action.

## Required Pre-gates

- TASK-119 QML static gate.
- TASK-120 ShellAccounting read-only UI readiness follow-up gate.
- TASK-121 QML binding smoke scaffold.
- TASK-122 runtime QML smoke CTest.
- TASK-123 production QML binding boundary docs.
- TASK-124 QML type registration boundary docs.
- TASK-125 QML type registration scaffold.
- TASK-126 QML type registration implementation gate.
- TASK-127 QML type registration helper implementation.
- TASK-128 startup registration gate.
- TASK-129 startup wiring implementation.
- TASK-130 production QML binding gate.
- TASK-131 production QML binding shell.
- TASK-132 presenter lifecycle gate.
- TASK-133 presenter lifecycle implementation.
- full CTest.
- `transport_local_socket_echo` 50 repeat.
- `git status` clean.

## Future Read-only Action Policy

Future first-stage real adapter work may only use read-only actions:

- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`

The following remain forbidden:

- `trade_log write`
- `trade_execution_group write`
- `trade_draft write`
- `cash_snapshot write`
- `position_snapshot write`
- `portfolio_summary write`
- `audit_log write`
- `data.audit.append`
- `confirmTrade`
- `manualEntry`
- `cashAdjustment`
- `createTradeDraft`
- `brokerOrder`
- `strategyExecute`
- `writeEnabled: true`

## Future Adapter Policy

The adapter must be explicit and test-covered. It must map transport errors,
protocol errors, unavailable service, timeout, stale data, and empty result
states. It must not fake data. It must not cache sensitive payloads in QML. It
must not expose raw trade facts to QML unless a later task separately
authorizes that surface.

## Error / Timeout / Privacy Policy

- timeout mapping: timeout maps to an unavailable issue.
- transport error mapping: transport error maps to the issue list.
- protocol error mapping: protocol error maps to the issue list.
- unavailable service mapping: unavailable service maps to Unavailable.
- stale data mapping: stale data maps to explicit status.
- no silent success on failure.
- no raw SQL in logs.
- no full trade_log payload in QML.
- no sensitive raw payload in issue text.
- privacy mode remains respected.

## Rollback / Disable Strategy

The real adapter must be reversible. If a DataService action fails, disable the
adapter and keep presenter unavailable. Fallback must not fake data, must not write DB, must not generate TradeDraft, must not show trading UI, and must not bypass DataService with SQLite direct reads.

## Required Post-integration Tests

- adapter fake success.
- adapter unavailable.
- transport error.
- protocol error.
- timeout.
- stale data.
- privacy and no raw payload exposure.
- no write action scan.
- no SQLite direct scan.
- no AccountingEngine direct scan.
- QML unavailable state.
- full CTest.
- transport 50 repeat.

## PR Requirements For Future Implementation

A future real data adapter PR must document implemented action names, adapter
class, request/response contract, error mapping, no write path evidence, no
SQLite direct access evidence, no AccountingEngine direct access evidence, QML
state behavior, rollback plan, and all gate test results.
