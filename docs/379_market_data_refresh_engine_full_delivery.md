# EPIC-280 Market Data Refresh Engine Full Delivery

EPIC-280 delivers the read-only MarketEngine path for user-visible market data
summaries. The scope is manual refresh of sanitized in-memory payloads and
fixture-backed tests for quote, historical high, displayed high,
drawdown-from-high, premium/discount, stale data, partial data, and fail-closed
provider states.

Implemented production boundaries:

- `MarketDataProvider` contract with disabled and fixture providers.
- `MarketDataRefreshEngine` read-only summary calculation.
- Deferred live public provider boundary, disabled by default.
- DataService read-only actions:
  `marketdata.refresh.readonly_summary` and
  `marketdata.historical_high.readonly_summary`.
- DataServiceClient typed adapter and ShellServices / Presenter mapping.
- Strategy recommendation input can consume sanitized market fields as
  read-only values.
- Samples under `samples/market_data/` cover 12 positive and 15 negative or
  safety-policy scenarios.

The refresh path is not a trading path. It does not write production DBs,
SQLite, `trade_log`, `cash_adjustment`, `audit_log`, ledger, snapshots,
persistent read models, or TradeDrafts. It does not submit broker orders, read
credentials, touch endpoints, place real orders, or enable automatic trading.

Refresh is manual. EPIC-280 does not add default live auto-refresh, a 2-second
polling loop, background polling, paid market data, PushPlus, or private
endpoint access.
