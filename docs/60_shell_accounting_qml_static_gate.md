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
