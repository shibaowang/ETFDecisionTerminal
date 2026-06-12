# EPIC-289 Real Daily Use Startup Auto Refresh Policy

## Default Behavior

Daily-use mode starts from the repo-local daily-use DB:

```text
.local/daily_use/etfdt_daily_use.sqlite
```

On page startup, the dashboard requests a read-only real daily-use snapshot.
The snapshot combines imported trade/cash facts with a controlled public market
refresh. The UI must not require a manual market refresh button for normal
daily viewing.

## Not Allowed

- no high-frequency refresh loop
- no page-switch-triggered network loop
- no hidden network call
- no broker SDK
- no credential, cookie, session, token, or private endpoint access
- no real order placement
- no automatic trading
- no strategy execution
- no TradeDraft generation
- no SQLite writes from refresh

## Runtime Controls

Startup auto refresh must enforce:

- exact host allowlist:
  `qt.gtimg.cn`, `push2.eastmoney.com`, `hq.sinajs.cn`,
  `push2his.eastmoney.com`
- 30 second provider/batch cooldown
- no same-host concurrency
- one historical high refresh per day per security id
- 10 minute failure circuit breaker
- daily high cache
- visible status, cache usage, failure reason, and last refresh time

The dashboard may show `cached_market_data` when cache is used. If no cache is
available, it must show unavailable state and the sanitized failure prompt.
