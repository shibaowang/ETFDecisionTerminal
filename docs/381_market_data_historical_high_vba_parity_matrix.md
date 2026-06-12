# EPIC-280 Market Data Historical High VBA Parity Matrix

Positive samples in `samples/market_data/`:

- `EPIC280_etf_quote_and_history_high.json`
- `EPIC280_index_quote_and_history_high.json`
- `EPIC280_otc_nav_quote.json`
- `EPIC280_etf_drawdown_from_high.json`
- `EPIC280_index_drawdown_from_high.json`
- `EPIC280_premium_discount_positive.json`
- `EPIC280_premium_discount_negative.json`
- `EPIC280_stale_quote_with_cached_history.json`
- `EPIC280_partial_quote_missing_history.json`
- `EPIC280_multi_instrument_refresh.json`
- `EPIC280_strategy_input_market_fields.json`
- `EPIC280_current_price_breaks_historical_high_display_only.json`

Negative and safety-policy samples:

- `EPIC280_negative_provider_disabled.json`
- `EPIC280_negative_missing_price.json`
- `EPIC280_negative_invalid_price.json`
- `EPIC280_negative_missing_history.json`
- `EPIC280_negative_invalid_high.json`
- `EPIC280_negative_unsupported_instrument.json`
- `EPIC280_negative_stale_beyond_limit.json`
- `EPIC280_negative_sensitive_token.json`
- `EPIC280_negative_broker_endpoint_token.json`
- `EPIC280_negative_credential_token.json`
- `EPIC280_negative_network_endpoint_not_allowlisted.json`
- `EPIC280_negative_live_provider_disabled_by_default.json`
- `EPIC280_negative_rate_limited_same_batch.json`
- `EPIC280_negative_history_failure_circuit_breaker.json`
- `EPIC280_negative_no_parallel_same_host.json`

Each positive sample asserts exact current price, previous close, historical
high, displayed high, high date, drawdown, premium/discount, data-quality
status, and provider source. Negative samples fail closed or produce explicit
safe partial/stale status with sanitized issue codes.

The matrix is read-only, has no broker or endpoint dependency, and does not add
default live auto-refresh or a 2-second polling loop.
