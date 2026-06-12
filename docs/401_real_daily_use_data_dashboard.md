# EPIC-289 Real Daily Use Data Dashboard

## Goal

EPIC-289 replaces the trial-first dashboard default with a real daily-use
dashboard for local users. The first screen must show imported holdings, cash,
base-position progress, ETF current data, ETF historical high, index current
point, index historical high, and startup auto refresh status when real data is
available.

Default daily-use database path:

```text
.local/daily_use/etfdt_daily_use.sqlite
```

The UI must show this DB path and the data source status so users can
distinguish real imported data from unavailable data.

## EPIC-289-FIX Completion

The original EPIC-289 commit `ae1548e5` delivered the dashboard framework. The
follow-up completion closes the real daily-use path:

- `providerMode=live` uses the production `LivePublicMarketDataProvider`
  instead of a deferred-only boundary.
- tests keep using fixture providers and do not call public network hosts.
- holdings come from imported/manual daily-use rows.
- remaining cash includes the `cash_adjustment` path when the schema exposes it,
  or a sanitized unavailable flag when only linkage is available.
- market value, total assets, and floating PnL are concrete values when quotes
  are available.
- missing base-position target config and missing ETF-to-index mapping are
  explicit user-visible issues, not mocked defaults.

## Data States

Allowed visible data source states:

- `imported_from_vba_export`
- `replayed_from_trade_log`
- `startup_auto_market_refresh`
- `cached_market_data`
- `unavailable`

If no imported trade/cash rows exist, the dashboard must show:

```text
请先导入真实 VBA 脱敏导出文件。
```

It must not fabricate holdings, cash, base-position progress, ETF prices, index
points, or market highs.

If market refresh fails and no fresh quote is available, the dashboard must
show:

```text
行情自动刷新失败，正在使用缓存 / 暂无行情数据。
```

## UI Requirements

The ShellAccounting page must include Chinese-first visible fields:

- `真实数据日常看板`
- `当前持仓`
- `剩余现金`
- `底仓完成度`
- `ETF 当前价`
- `ETF 历史高点`
- `指数当前点位`
- `指数历史高点`
- `自动刷新状态`
- `最近刷新时间`
- `使用缓存`
- `刷新失败原因`

The completed daily-use panel also shows concrete total market value, total
assets, and floating PnL when market quotes are available.

Raw JSON / diagnostic payloads stay collapsed by default behind Chinese
developer-detail controls.

The daily-use UI must not add a "manual refresh market" workflow. Startup
refresh is automatic and bounded by the policy in
[404_real_daily_use_startup_auto_refresh_policy.md](404_real_daily_use_startup_auto_refresh_policy.md).

## Safety Boundary

EPIC-289 remains read-only for daily-use dashboard rendering:

- no broker integration
- no credential or private endpoint access
- no real order placement
- no automatic trading
- no strategy execution
- no TradeDraft generation
- no persistent read-model writes
- no audit / ledger / snapshot writes
- no production DB destructive mutation

Tests use fixture/fake providers only and must not access real network.
