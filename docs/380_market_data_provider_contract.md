# EPIC-280 Market Data Provider Contract

The provider contract separates read-only market data retrieval from summary
calculation. Providers return a `MarketProviderSnapshot` with quotes, history
bars, issue codes, and safety flags. The engine converts that snapshot into a
sanitized summary.

Providers:

- Disabled provider: fail-closed with provider-disabled evidence.
- Fixture provider: reads sanitized test fixture payloads, with no network.
- Live public provider boundary: present as a disabled/deferred boundary only.

Safety policy:

- Exact public hosts only: `qt.gtimg.cn`, `push2.eastmoney.com`,
  `hq.sinajs.cn`, and `push2his.eastmoney.com`.
- Exact quote/history paths only:
  `/q=`, `/api/qt/ulist.np/get`, `/list=`, and
  `/api/qt/stock/kline/get`.
- Batch requests only, no parallel same-host requests.
- Per-host rate limiting.
- One successful historical high result per secid per day can be cached.
- A history failure opens a 10-minute circuit breaker.
- Raw URLs, raw provider responses, broker tokens, credentials, endpoints, and
  PushPlus tokens must not be exposed.

The provider contract is read-only. It must not write production DBs, SQLite,
`trade_log`, `cash_adjustment`, `audit_log`, ledger, snapshots, persistent read
models, or TradeDrafts. It must not access broker, credentials, private
endpoints, real order placement, or automatic trading.

EPIC-280 also forbids default live auto-refresh and any 2-second polling loop.
