# EPIC-289 Real Daily Use Acceptance Checklist

## Manual Acceptance

- Start from a clean repo-local daily-use workspace.
- Confirm DB path is `.local/daily_use/etfdt_daily_use.sqlite`.
- Launch DataService with the daily-use service mode:
  `scripts/local_trial/Start-ETFDTDailyUseDataService.ps1`.
- The DataService startup command must use `--serve-daily-use`, not
  `--serve-readonly`, so accepted Excel/VBA import persistence is registered
  without enabling broker, order, or automaticTrading actions.
- Confirm the DataService startup evidence includes `socketReady=true`.
- Confirm the DataService startup evidence includes
  `dailyUseWriteActionsEnabled=true`,
  `excelVbaImportPersistActionRegistered=true`,
  `serveMode=daily-use`, `readOnlyOnlyMode=false`, and
  `brokerOrderActionsRegistered=false`.
- If DataService readiness fails, inspect:
  `.local/daily_use/logs/dataservice.log`,
  `.local/daily_use/logs/dataservice.err.log`, and
  `.local/daily_use/logs/dataservice.pid`.
- Open ShellAccounting local page with the daily-use shell launcher:
  `scripts/local_trial/Start-ETFDTDailyUseShell.ps1`.
- The daily-use shell launcher must pass:
  `--daily-use --socket-name ETFDataServiceDailyUse --db .local/daily_use/etfdt_daily_use.sqlite --default-page shell-accounting-daily-use`.
- The daily-use Shell and DataService scripts must use the same normalized
  local socket name and must not require the user to guess Qt local socket
  details.
- Confirm Shell starts on the ShellAccounting daily-use dashboard instead of the
  mock home dashboard.
- Confirm the top status and right info panel show daily-use connection context,
  not `本地试用模拟模式` / `PLACEHOLDER` / `NO_DATA` as the daily-use default state.
- Do not use `scripts/local_trial/Start-ETFDTLocalShell.ps1` for this
  daily-use acceptance path; that script targets `.demo/local_trial_rc`.
- Confirm the daily-use panel title is `真实数据日常看板`.
- Without import data, confirm the prompt:
  `请先导入真实 VBA 脱敏导出文件。`
- Import a sanitized VBA export through the existing local workflow.
- After the preview is `ACCEPTED`, explicitly check the persist confirmation
  box and click the accepted-preview write button. The normal result must not
  be `DATASERVICE_CLIENT_CALL_FAILED`; if that status appears, restart
  DataService with `Start-ETFDTDailyUseDataService.ps1` so it uses
  `--serve-daily-use`.
- Confirm the persist result shows `transactionCommitted=true`, positive
  trade row counts, positive cash adjustment row counts when cash facts exist,
  and sanitized issue text only.
- Confirm current holdings are visible.
- Confirm remaining cash is visible.
- Confirm total market value is a concrete value when quotes are available.
- Confirm total assets is a concrete value when quotes are available.
- Confirm floating PnL is a concrete value when quotes are available.
- Confirm base position completion is visible or the missing-config prompt is
  explicit.
- Confirm missing ETF-to-index mapping is shown as an explicit issue when the
  mapping is absent.
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
- production live provider implementation exists and is not deferred-only
- startup scripts report `.local/daily_use/cache/market_cache.json`
- DataService startup writes `.local/daily_use/logs/dataservice.pid`
- DataService startup redirects stdout/stderr to
  `.local/daily_use/logs/dataservice.log` and
  `.local/daily_use/logs/dataservice.err.log`
- DataService startup waits for a local socket readiness probe and reports
  `socketReady`
- Shell startup passes and reports the normalized daily-use socket name
- the daily-use Shell script passes `--daily-use`, `--socket-name`,
  `--db`, and `--default-page shell-accounting-daily-use`
- Shell startup parses daily-use arguments and defaults to the ShellAccounting
  daily-use dashboard, not the mock home dashboard
- no real network is used in tests
- host allowlist, rate limit, daily cache, failure circuit breaker, and
  no-same-host-concurrency evidence is present
- broker, credentials, endpoints, real order placement, and automatic trading
  remain disabled
