# EPIC-289 Real Daily Use Market Data VBA Parity

## Public Market Source Boundary

Market data is attached to the daily-use dashboard whose default DB path is:

```text
.local/daily_use/etfdt_daily_use.sqlite
```

Daily-use startup auto refresh may use only the following public quote/history
hosts:

- `qt.gtimg.cn`
- `push2.eastmoney.com`
- `hq.sinajs.cn`
- `push2his.eastmoney.com`

EPIC-289-FIX implements the production live provider behind this boundary. It
is no longer a deferred-only path: `providerMode=live` may make bounded public
market requests when `liveMarketDataEnabled=true`. CTests continue to use
fixture providers, so test runs do not touch the public network.

Documented source shapes:

- Tencent quote: `http://qt.gtimg.cn/q=...`
- Eastmoney quote:
  `https://push2.eastmoney.com/api/qt/ulist.np/get?...fields=f12,f2,f3,f43,f170`
- Sina OTC NAV: `http://hq.sinajs.cn/list=f_...`
- Eastmoney historical K:
  `https://push2his.eastmoney.com/api/qt/stock/kline/get`

Historical K policy:

- monthly K uses `klt=103`
- ETF default adjustment uses `fqt=1`
- index history uses `fqt=0`

## Startup Auto Refresh Policy

Startup refresh is one controlled daily-use refresh, not background polling.

Required controls:

- exact host allowlist
- 30 second provider/batch cooldown
- one historical-high request per day per security id
- no same-host concurrency
- current quote timeout about 2 seconds
- history timeout about 3 seconds
- 10 minute failure circuit breaker
- daily cache for historical highs
- visible refresh status, cache status, last refresh time, and failure reason
- traceable logs without raw private data
- no broker, order, credential, endpoint, PushPlus, or automatic trading path

When cache is valid, the UI may prefer it and label the result `使用缓存`.

If current price exceeds the cached high, the visible high is raised for the
dashboard evidence. If no cache exists and network fails, no price or high is
fabricated.

## Test Boundary

CTest must use fixture or disabled providers. Tests must not call public market
hosts, broker hosts, private endpoints, PushPlus, credential stores, cookies,
sessions, tokens, or paid provider SDKs. Market refresh is never automatic trading
and never submits broker orders.
