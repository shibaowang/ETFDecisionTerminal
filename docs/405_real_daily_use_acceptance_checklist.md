# EPIC-289 Real Daily Use Acceptance Checklist

## Manual Acceptance

- Start from a clean repo-local daily-use workspace.
- Confirm DB path is `.local/daily_use/etfdt_daily_use.sqlite`.
- Launch DataService with daily-use read-only startup policy.
- Open ShellAccounting local page.
- Confirm the daily-use panel title is `真实数据日常看板`.
- Without import data, confirm the prompt:
  `请先导入真实 VBA 脱敏导出文件。`
- Import a sanitized VBA export through the existing local workflow.
- Confirm current holdings are visible.
- Confirm remaining cash is visible.
- Confirm base position completion is visible or the missing-config prompt is
  explicit.
- Confirm ETF current price / ETF historical high fields are visible.
- Confirm index current point / index historical high fields are visible.
- Confirm auto refresh status, last refresh time, cache status, and failure
  reason are visible.
- Confirm raw JSON stays collapsed by default.
- Confirm no daily-use "manual refresh market" button exists.

## Automated Acceptance

Required CTest:

```text
real_daily_use_data_dashboard
```

The test must prove:

- fixture import rows are read from a temp daily-use DB
- holdings and remaining cash are calculated from real rows
- no mock holdings/cash/market is presented as real data
- fixture market refresh supplies ETF/index current and historical high fields
- no real network is used in tests
- host allowlist, rate limit, daily cache, failure circuit breaker, and
  no-same-host-concurrency evidence is present
- broker, credentials, endpoints, real order placement, and automatic trading
  remain disabled
