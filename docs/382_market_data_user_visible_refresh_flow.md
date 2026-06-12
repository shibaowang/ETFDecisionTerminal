# EPIC-280 Market Data User-visible Refresh Flow

The user-visible flow is a manual read-only refresh:

1. A sanitized in-memory market data payload is submitted.
2. DataService calls the read-only market data action.
3. MarketEngine calculates quote, historical high, displayed high, drawdown,
   premium/discount, and data-quality status.
4. DataServiceClient maps the response.
5. ShellServices and Presenter expose status, issue codes, current price,
   previous close, historical high, displayed high, drawdown, premium/discount,
   provider source, and summary text.

Supported visible states:

- `OK` when quote and historical high are both usable.
- `STALE` when stale quote data is accepted with an issue code.
- `PARTIAL` when a quote is available but history is missing or partial.
- `REJECTED` / provider disabled / unavailable for fail-closed scenarios.

The flow is read-only. It does not write SQLite, production DBs, `trade_log`,
`cash_adjustment`, `audit_log`, ledger, snapshots, read models, or TradeDrafts.
It does not access broker, credentials, private endpoints, real order
placement, automatic trading, paid market data, or PushPlus.

No QML auto-refresh is added in EPIC-280. There is no default live
auto-refresh, no background refresh, and no 2-second polling loop.
