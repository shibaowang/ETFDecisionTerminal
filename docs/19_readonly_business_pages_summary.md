# ReadOnly Business Pages Summary

## Page List

### 只读数据预览页

Purpose:

- Development-stage preview for Shell read-only DataService access.
- Verifies connection, refresh state, summary counts, and read-only list models.

Data source:

- `ShellReadOnlyDataController`.
- DataService read-only actions through the ShellServices boundary.

Not supported:

- Editing.
- Trading.
- Accounting entry.
- Direct `DataServiceClient` calls from QML.
- Direct SQLite access from QML.

### 账户与组合

Purpose:

- Display account and portfolio data as read-only business information.

Data source:

- `ShellAccountListModel`.
- `ShellPortfolioListModel`.
- Filtered read-only proxy models exposed by `ShellReadOnlyDataController`.

Not supported:

- Account editing.
- Portfolio editing.
- Deposit / withdraw.
- Cash calibration.
- Trading.
- Accounting entry.

### 标的与策略

Purpose:

- Display instruments, strategies, and OTC channels as read-only business information.

Data source:

- `ShellInstrumentListModel`.
- `ShellStrategyListModel`.
- `ShellOtcChannelListModel`.
- Filtered read-only proxy models exposed by `ShellReadOnlyDataController`.

Not supported:

- Instrument editing.
- Strategy editing.
- OTC channel editing.
- Strategy execution.
- Generate TradeDraft.
- Trading.

## Reusable Components

- `ReadOnlyTable`.
- `ReadOnlyFilterBar`.
- `ReadOnlyColumnChooser`.
- `ReadOnlyDensityToggle`.
- `ReadOnlyStatusBadge`.
- `ReadOnlyEmptyState`.
- `ReadOnlySectionHeader`.
- `ReadOnlyFieldLabel`.

## Current UI Functions

- Connect.
- Refresh.
- Search.
- Active / enabled filter.
- Sorting.
- Column visibility.
- Density.

These functions only affect read-only display and controller-owned read operations. They do not write backend data.

## Prohibited Functions

禁止 / Not supported:

- edit.
- delete.
- create.
- deposit / withdraw.
- trade.
- execute strategy.
- generate TradeDraft.
- write TradeLog.
- call `data.audit.append`.

## Next Page Suggestions

- Add more read-only business pages.
- Design accounting-derived boundaries before building a position page.
- Design MarketService read-only DTOs before connecting real market data.
- Keep write flows out of scope until a separate task authorizes service boundaries, transaction rules, and tests.
