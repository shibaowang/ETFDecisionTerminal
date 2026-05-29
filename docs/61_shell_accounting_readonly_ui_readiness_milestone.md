# ShellAccounting Read-only UI Readiness Milestone

## Milestone Name

ShellAccounting Read-only UI Readiness Milestone

## Milestone Positioning

This milestone closes the current ShellAccounting read-only UI readiness phase.
It is the readiness checkpoint for ShellServices, Presenter, ViewModel / Model,
and the QML static gate.

This milestone is not QML binding. QML 尚未接入. It is not a real accounting
action implementation. It does not add write capability and does not include
TradeDraft, trade suggestion, strategy execution, or broker order behavior.

## Completed Capability

### A. DataService Accounting Guard Suite

- `accounting.health`
- `accounting.replay.preview`
- `position.list`
- `cash.summary`
- `portfolio.pnl.summary`
- `base_position.summary`
- `sniper_pool.summary`
- `implemented=false` guard status remains valid.

### B. DataServiceClient Guard Wrappers

- `positionList`
- `cashSummary`
- `portfolioPnlSummary`
- `basePositionSummary`
- `sniperPoolSummary`

### C. ShellAccounting Service Chain

- `ShellAccountingReadOnlyController`
- `ShellAccountingServiceAdapter`
- `ShellAccountingDataServiceAdapter`
- `ShellAccountingDataServiceClientPortAdapter`
- read-only concrete-port guard-wrapper chain.

### D. ShellAccounting Presenter / ViewModel / Model

- `ShellAccountingPresenter`
- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`

### E. QML Readiness Guards

- docs/59 QML binding smoke plan.
- docs/60 QML static gate.
- TASK-119 QML static gate CTest.

## Current End-to-End Read-only Chain

```text
ShellAccountingPresenter
-> ShellAccountingReadOnlyController
-> ShellAccountingDataServiceAdapter
-> ShellAccountingDataServiceClientPortAdapter
-> DataServiceClient guard wrapper
-> DataService accounting guard action
```

The current chain consumes guard payloads only. All five guard payloads map to
Unavailable. `readOnly=true` and `writeEnabled=false` remain preserved. Issues
are visible, and Empty is not confused with Unavailable.

## Current QML Status

- QML 尚未接 accounting action.
- QML has not registered ShellAccounting types.
- QML has not bound `ShellAccountingPresenter`.
- QML has not bound `ShellAccountingStatusObject`.
- QML has not bound `ShellAccountingIssueListModel`.
- QML has not bound `ShellPositionListModel`.
- QML static gate blocks direct DataServiceClient, SQLite, AccountingEngine,
  write token, and trade token usage.

## No-write / No-trade Status

- No `trade_log` write.
- No `trade_execution_group` write.
- No `trade_draft` write.
- No `cash_snapshot` write.
- No `position_snapshot` write.
- No `portfolio_summary` write.
- No `audit_log` write.
- no TradeDraft.
- no trade suggestion.
- No strategy execution.
- No order submission.
- No `data.audit.append` call.
- No write-enabled action call.

## Explicitly Not Implemented

- Real `position.list` is not implemented.
- Real `cash.summary` is not implemented.
- Real `portfolio.pnl.summary` is not implemented.
- Real `base_position.summary` is not implemented.
- Real `sniper_pool.summary` is not implemented.
- DataService does not call AccountingEngine.
- SQLite facts query is not connected to DataService.
- QML is not connected.
- Snapshot writes are not implemented.
- TradeLog writes are not authorized.
- TradeDraft is not implemented.
- Strategy execution is not implemented.
- Broker API is not implemented.

Real accounting action 仍未实现.

## Quality Gates

- CMake configure passes.
- Build passes.
- Full CTest passes.
- `transport_local_socket_echo` repeat 50 passes.
- DevDocs static checks pass.
- QML static gate passes.
- No-write guard tests pass.
- `git status` is clean.

## Suggested Tag

Suggested tag after merge only:

```powershell
git tag -a v0.6.0-shell-accounting-readonly-ui-readiness -m "v0.6 ShellAccounting Read-only UI Readiness"
git push origin v0.6.0-shell-accounting-readonly-ui-readiness
```

The tag must be created manually by the user after the PR is merged. This task
does not create a Git tag.

## Next Phase Suggestions

- QML binding smoke test implementation.
- ShellAccounting QML read-only placeholder page.
- ShellAccountingPresenter QML registration task.
- Real `position.list` implementation remains separate.
- no-write, no-trade, and TASK-119 static gate remain mandatory.

## TASK-121 Follow-up

TASK-121 adds the first v0.6 follow-up: QML binding smoke scaffold CTests. The
scaffold verifies future object binding, state matrix, guard payload display,
issue / privacy, no-trade UI, and static gate dependency expectations.

QML is still not wired, and real accounting actions remain unimplemented.

## TASK-122 Follow-up

TASK-122 adds test-only QML smoke CTests as the next v0.6 follow-up. The tests
load inline QML with fake read-only objects and verify object availability,
state display, guard unavailable display, issue visibility, privacy display,
no-trade UI tokens, and TASK-119 static gate regression.

Production QML remains unwired. No ShellAccounting QML type is registered, no
production accounting page is implemented, real accounting actions remain
unimplemented, and no SQLite, AccountingEngine, write, TradeDraft, or trade
suggestion behavior is enabled.

## TASK-123 Production QML Boundary

TASK-123 adds the production QML binding boundary docs/63 and first-stage QML
binding checklist docs/64. Production QML binding still requires explicit
authorization and remains limited to read-only placeholder / Unavailable /
issue / privacy smoke for the first stage.

## TASK-124 QML Type Registration Boundary

TASK-124 adds the QML type registration boundary docs/65 and registration test
plan docs/66. QML registration still requires explicit authorization. The v0.6
readiness milestone remains read-only: production QML is unwired, no type is
registered, no real accounting action is implemented, and no write / TradeDraft
/ trade suggestion path is enabled.

## TASK-125 Registration Scaffold

TASK-125 adds the QML type registration scaffold as a post-v0.6 readiness gate.
It does not register QML types or modify production QML. It fixes the future
registration allowlist, denylist, module boundary, and no-production-change
checks before any registration task is authorized.

## TASK-126 Implementation Gate

TASK-126 adds the QML type registration implementation gate and rollback
strategy as the final pre-registration readiness layer. It still does not
register QML types, modify production QML, or authorize real accounting actions,
writes, TradeDraft, trade suggestions, broker orders, or strategy execution.

## TASK-127 Type Registration Implementation

TASK-127 completes the first-stage read-only QML type registration after the
v0.6 milestone. It registers only the ShellAccounting allowlist types through a
centralized helper. Production QML is still not bound, real accounting actions
remain unimplemented, and no-write / no-trade rules remain mandatory.
