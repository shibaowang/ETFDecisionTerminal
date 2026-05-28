# ShellAccounting QML Static Gate

## Document Purpose

This document defines the static gate that future ShellAccounting QML
integration must pass before merge.

This document does not modify QML and does not implement behavior outside
documentation and future static-test planning.

## Forbidden Direct Dependencies

Future QML must not contain direct references to:

- `DataServiceClient`
- `DataServiceApi`
- `SQLite`
- `AccountingEngine`
- `DataAccess`
- `trade_log`
- `cash_snapshot`
- `position_snapshot`
- `portfolio_summary`
- `TradeDraft`
- `brokerOrder`
- `strategyExecute`
- `data.audit.append`
- `writeEnabled: true`

## Allowed References

Future read-only QML may reference:

- `ShellAccountingPresenter`
- `accountingPresenter`
- `ShellAccountingStatusObject`
- `ShellAccountingIssueListModel`
- `ShellPositionListModel`
- `ReadOnlyStatusBadge`
- `ReadOnlyEmptyState`
- `ReadOnlyTable`
- `PrivacyAmountText`, if a later task creates it.

These references do not authorize write behavior or direct service access.

## Forbidden UI Behavior

- No buy button.
- No sell button.
- No order submission button.
- No draft generation button.
- No strategy execution button.
- No broker submission button.
- No write-enabled menu.
- No text that implies direct trading from current holdings.

## Static Scan Plan

Future static scans must:

- scan `apps/ETFDecisionShell/qml`.
- enforce denylist strings.
- check allowed binding strings.
- reject direct service strings.
- reject write action strings.
- reject broker strings.
- reject trade draft strings.
- reject strategy execution strings.
- fail on forbidden tokens.

## Go / No-Go Checklist

- [ ] Presenter has been implemented.
- [ ] StatusObject / IssueListModel / PositionListModel have been implemented.
- [ ] QML smoke plan has been merged.
- [ ] QML static gate has been merged.
- [ ] no direct DataServiceClient scan has been prepared.
- [ ] no trade button scan has been prepared.
- [ ] no write action scan has been prepared.
- [ ] privacy display plan has been prepared.
- [ ] User explicitly authorized QML integration.

## TASK-119 Automated CTest Gate

TASK-119 implements this static gate as CTest coverage under
`tests/ShellAccountingQmlStaticGate`.

Automated CTests:

- `shell_accounting_qml_static_gate_no_direct_service`
- `shell_accounting_qml_static_gate_no_accounting_binding_yet`
- `shell_accounting_qml_static_gate_no_write_tokens`
- `shell_accounting_qml_static_gate_accounting_scope_no_trade`
- `shell_accounting_qml_static_gate_docs_sync`

The global hard deny scans all QML / QML JavaScript content for direct service,
database, accounting-engine, adapter, concrete-port, write, SQL write, and
protected table tokens. Existing explanatory text that says QML does not
directly call DataServiceClient or access SQLite is allowed only as a negative
read-only statement.

The accounting-scoped no-trade gate is narrower. It scans files whose filename
or content indicates future accounting binding, such as Accounting,
PositionReadOnly, PositionList, CashSummary, PortfolioPnl, BasePosition,
SniperPool, ShellAccounting, accountingPresenter, or ShellPositionListModel.
Inside that scope it rejects buy / sell buttons, TradeDraft, brokerOrder,
strategyExecute, write action, and Chinese trading-button phrases.

This distinction prevents false positives from existing non-accounting
placeholder pages such as `TradeDraftPlaceholderPage` and
`TradeConfirmPlaceholderPage`. Those names are not treated as accounting QML
scope by themselves, and the gate scans file content rather than path names for
the global write-token checks.

Future accounting QML files will be covered by the accounting-scoped denylist
as soon as their filename or content identifies them as accounting UI.

Empty is not Unavailable. Future QML binding tasks must keep this display rule,
must pass TASK-119 static gates, and must still wait for User explicitly
authorized QML integration before modifying QML.

## TASK-120 ShellAccounting Read-only UI Readiness

The v0.6 readiness milestone in
`docs/61_shell_accounting_readonly_ui_readiness_milestone.md` treats this static
gate as a required precondition for any future QML binding. The milestone does
not modify QML and does not loosen this gate.
